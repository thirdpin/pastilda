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

#ifndef USBH_HUBBED_
#define USBH_HUBBED_

#include "usbh_config.h"

#include <stdint.h>
#include <stdbool.h>

/* This must be placed around external function declaration for C++
 * support. */
#ifdef __cplusplus
# define BEGIN_DECLS extern "C" {
# define END_DECLS }
#else
# define BEGIN_DECLS
# define END_DECLS
#endif

BEGIN_DECLS

// set to -1 to unused items
struct _usbh_dev_driver_info {
	int32_t deviceClass;
	int32_t deviceSubClass;
	int32_t deviceProtocol;
	int32_t idVendor;
	int32_t idProduct;
	int32_t ifaceClass;
	int32_t ifaceSubClass;
	int32_t ifaceProtocol;
};
typedef struct _usbh_dev_driver_info usbh_dev_driver_info_t;

struct _usbh_dev_driver {
	void *(*init)(void *usbh_dev);
	bool (*analyze_descriptor)(void *drv, void *descriptor);
	void (*poll)(void *drvdata, uint32_t time_curr_us);
	void (*remove)(void *drvdata);
	const usbh_dev_driver_info_t * const info;
};
typedef struct _usbh_dev_driver usbh_dev_driver_t;

void usbh_init(const void *drivers[], const usbh_dev_driver_t * const device_drivers[]);

/**
 * \brief usbh_poll
 * \param time_curr_us - use monotically rising time
 *
 *	time_curr_us:
 *		* can overflow, in time of this writing, after 1s)
 *		* unit is microseconds
 */
void usbh_poll(uint32_t time_curr_us);

END_DECLS

#endif
