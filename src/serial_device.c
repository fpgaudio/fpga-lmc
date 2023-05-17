#include "serial_device.h"
#include <stdlib.h>
#include <Windows.h>
#include <intsafe.h>
#include <stdbool.h>
#include <stdio.h>
#include "win_ex.h"
#include <time.h>

struct serial_device {
	const char* path;
	HANDLE hComm;
	bool verbosity;
	clock_t last_sample_time;
};

serial_device_t serial_device_open(const char* path, unsigned long baud_rate, bool verbose) {
	TCHAR full_path[MAX_PATH];
	if (!QueryDosDevice(path, (LPSTR)full_path, sizeof(full_path))) {
		if (verbose) {
			fprintf(stderr, "%s is not a known COM port.\n", path);
		}
		return NULL;
	}

	struct serial_device m_dev = {
		.path = path,
		.hComm = CreateFileA(
			path,
			GENERIC_READ | GENERIC_WRITE,
			0, NULL, OPEN_EXISTING,
			0, NULL
		),
		.verbosity = verbose,
		.last_sample_time = clock()
	};

	if (m_dev.hComm == INVALID_HANDLE_VALUE) {
		if (m_dev.verbosity) {
			fprintf(stderr, "Could not open the COM port: %s\n", path);
		}
		return NULL;
	}

	// Flush old data.
	if (!FlushFileBuffers(m_dev.hComm)) {
		if (m_dev.verbosity) {
			fprintf(stderr, "Could not flush pre-data.");
		}
		CloseHandle(m_dev.hComm);
		return NULL;
	}

	// Configure timeouts
	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 0;
	timeouts.ReadTotalTimeoutConstant = 100;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 100;
	timeouts.WriteTotalTimeoutMultiplier = 0;

	if (!SetCommTimeouts(m_dev.hComm, &timeouts)) {
		CloseHandle(m_dev.hComm);
		if (m_dev.verbosity) {
			fprintf(stderr, "Could not setup the timeout configuration: %s",
				    win32_err_str());
		}
		return NULL;
	}

	// Configure serial mode.
	DCB serialConf = { 0 };
	serialConf.DCBlength = sizeof(DCB);
	serialConf.BaudRate = baud_rate;
	serialConf.ByteSize = 8;
	serialConf.Parity = NOPARITY;
	serialConf.StopBits = ONESTOPBIT;

	if (!SetCommState(m_dev.hComm, &serialConf)) {
		CloseHandle(m_dev.hComm);
		if (m_dev.verbosity) {
			fprintf(stderr, "Could not configure the serial device: %s",
				    win32_err_str());
		}
		return NULL;
	}

	// Move the structure to the heap.
	serial_device_t out = (serial_device_t)malloc(sizeof(struct serial_device));
	if (out == NULL) {
		fprintf(stderr, "Critical Error: OOM\n");
		CloseHandle(m_dev.hComm);
		return NULL;
	}
	memcpy(out, &m_dev, sizeof(struct serial_device));

	return out;
}

bool serial_device_write(serial_device_t device, const unsigned char* data,
	                     size_t data_len) {
	DWORD data_len_dw, data_written;
	(void)SIZETToDWord(data_len, &data_len_dw);

	const clock_t sample_time = clock();
	if (device->verbosity) {
		const double sample_period =
			((double)sample_time - (double)device->last_sample_time)
			/ CLOCKS_PER_SEC;
		const double sample_frequency = 1.0 / sample_period;
		fprintf(stderr, "USART Write Rate: %.2fHz\n", sample_frequency);
	}

	if (!WriteFile(device->hComm, data, data_len_dw, &data_written, NULL)
		|| data_written != data_len_dw) {
		if (device->verbosity) {
			fprintf(stderr, "Could not write to the COM port (%lu != %lu): %s\n",
				    data_len_dw, data_written, win32_err_str());
		}
		return false;
	}

	if (device->verbosity) {
		fprintf(stderr, "Successfully written %lu bytes to COM port.\n",
			    data_written);
	}

	return true;
}

void serial_device_close(serial_device_t device) {
	if (device->hComm != INVALID_HANDLE_VALUE) {
		CloseHandle(device->hComm);
	}
	free(device);
}