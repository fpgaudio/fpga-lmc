/**
 * This file contains the API used to control a serial device.
 */

#ifndef SERIAL_DEVICE_H
#define SERIAL_DEVICE_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct serial_device* serial_device_t;

/**
 * Begins communicating with a COM device.
 * 
 * @param path The COM port (eg. "COM3")
 * @param baud_rate The baud rate in Hz (eg. 9600)
 * @param verbose Whether the driver will spit out lots of data.
 */
serial_device_t serial_device_open(const char* path,
	                             unsigned long baud_rate,
	                             bool verbose);

/**
 * Writes to the current serial device.
 * 
 * @param device The instance of the serial device.
 * @param data The data to write.
 * @param data_len The number of bytes to write.
 */
bool serial_device_write(serial_device_t device, const unsigned char* data,
	                     size_t data_len);

/**
 * Closes the given serial device. Performs all cleanup.
 * 
 * @param device The instance of the serial device.
 */
void serial_device_close(serial_device_t device);

#endif // SERIAL_DEVICE_H