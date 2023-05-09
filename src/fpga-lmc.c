#include "lmc.h"
#include "serial_device.h"
#include "cli.h"
#include <Windows.h>
#include <stdio.h>

static serial_device_t s_com;
static cli_args_t s_args;
static lmc_t s_lmc;

void on_hand_evt(const lmc_t lmc, const lmc_hand_evt_min_t evt) {
    unsigned char* serialized_evt;
    size_t serialized_evt_len;
    lmc_serialize_min_evt(lmc, evt, &serialized_evt, &serialized_evt_len);
    serial_device_write(s_com, serialized_evt, serialized_evt_len);
}

BOOL WINAPI on_signal(DWORD signal) {
    (void)signal;
    if (s_args->verbose) {
        fprintf(stderr, "Received Exit Signal.\n");
    }

    lmc_stop_listening(s_lmc);
    return true;
}

int main(int argc, char** argv) {
    s_args = cli_parse_args(argc, argv);
    if (!s_args) {
        return 22;
    }

    // Hoop into the CTRL+C signal.
    if (!SetConsoleCtrlHandler(on_signal, TRUE)) {
        fprintf(stderr, "Couldn't register signals.\n");
        return 1;
    }

    // Open the serial port.
    if ((s_com = serial_device_open(s_args->com_port, s_args->baud_rate,
                                    s_args->verbose)) == NULL) {
        return 1;
    }

    // Begin LMC loop
    if ((s_lmc = lmc_connect(s_args->verbose)) == NULL) {
        return 1;
    }
    lmc_listen_for_hand(s_lmc, NULL, on_hand_evt);

    // Cleanup prior to exit.
    lmc_close(s_lmc);
    serial_device_close(s_com);
    free(s_args);
    return 0;
}