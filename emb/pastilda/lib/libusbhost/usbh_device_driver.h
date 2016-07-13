/*
 * This file is part of the libusbhost library
 * hosted at http://github.com/libusbhost/libusbhost
 *
 * Copyright (C) 2015 Amir Hammad <amir.hammad@hotmail.com>
 *
 *
 * libusbhost is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef USBH_DEVICE_DRIVER_
#define USBH_DEVICE_DRIVER_

#include "usbh_config.h"
#include "usbh_hubbed.h"

#include <stdint.h>

BEGIN_DECLS

#define USBH_EPTYP_CONTROL				(0)
#define USBH_EPTYP_ISOCHRONOUS 			(1)
#define USBH_EPTYP_BULK					(2)
#define USBH_EPTYP_INTERRUPT 			(3)

#define USBH_SPEED_FULL					(0)
#define USBH_SPEED_LOW					(1)
#define USBH_SPEED_HIGH					(2)


enum USBH_PACKET_CALLBACK_STATUS {
	USBH_PACKET_CALLBACK_STATUS_OK = 0,
	USBH_PACKET_CALLBACK_STATUS_ERRSIZ = 1,
	USBH_PACKET_CALLBACK_STATUS_EAGAIN = 2, // -- TODO: automatic handling of transmit errors 3xTXERR->FATAL
	USBH_PACKET_CALLBACK_STATUS_EFATAL = 3
};

enum USBH_POLL_STATUS {
	USBH_POLL_STATUS_NONE,
	USBH_POLL_STATUS_DEVICE_CONNECTED,
	USBH_POLL_STATUS_DEVICE_DISCONNECTED
};

struct _usbh_device {
	uint16_t packet_size_max0;
	int8_t address;
	uint8_t speed;		// (USBH_SPEED_*)
	uint8_t state; // for enumeration purposes
	uint8_t toggle0;
	const usbh_dev_driver_t *drv;
	void *drvdata;
	const void *lld;
};
typedef struct _usbh_device usbh_device_t;

struct _usbh_packet_callback_data {
	enum USBH_PACKET_CALLBACK_STATUS status;
	uint32_t transferred_length;
};
typedef struct _usbh_packet_callback_data usbh_packet_callback_data_t;

typedef void (*usbh_packet_callback_t)(usbh_device_t *dev, usbh_packet_callback_data_t status);

struct _usbh_packet {
	void *data;		// Pointer to data
	uint16_t datalen;	// Data length 0..1023
	int8_t address;	// Device address
	uint8_t endpoint_type;		// Endpoint type (see USBH_EPTYP_*)
	uint8_t endpoint_address;		// Endpoint number 0..15
	uint16_t endpoint_size_max;	// Max packet size for an endpoint
	uint8_t speed;		// (USBH_SPEED_*)
	uint8_t *toggle;
	usbh_packet_callback_t callback;
	void *callback_arg;
};
typedef struct _usbh_packet usbh_packet_t;

struct _usbh_driver {
	void (*init)(void *drvdata);
	void (*write)(void *drvdata, const usbh_packet_t *packet);
	void (*read)(void *drvdata, usbh_packet_t *packet);
	enum USBH_POLL_STATUS (*poll)(void *drvdata, uint32_t time_curr_us);
	uint8_t (*root_speed)(void *drvdata);

	// Pointer to Low-level driver data
	void *driver_data;
};
typedef struct _usbh_driver usbh_driver_t;

struct _usbh_generic_data {
	usbh_device_t usbh_device[USBH_MAX_DEVICES];
	uint8_t usbh_buffer[BUFFER_ONE_BYTES];
};
typedef struct _usbh_generic_data usbh_generic_data_t;


#define ERROR(arg) LOG_PRINTF("UNHANDLED_ERROR %d: file: %s, line: %d",\
							arg, __FILE__, __LINE__)


/// Hub related functions

usbh_device_t *usbh_get_free_device(const usbh_device_t *dev);
bool usbh_enum_available(void);
void device_enumeration_start(usbh_device_t *dev);

/// All devices functions

///
void usbh_read(usbh_device_t *dev, usbh_packet_t *packet);
void usbh_write(usbh_device_t *dev, const usbh_packet_t *packet);
/// Helper functions
void device_xfer_control_read(void *data, uint16_t datalen, usbh_packet_callback_t callback, usbh_device_t *dev);
void device_xfer_control_write(void *data, uint16_t datalen, usbh_packet_callback_t callback, usbh_device_t *dev);


END_DECLS

#endif
