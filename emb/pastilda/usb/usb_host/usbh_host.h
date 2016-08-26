/*
 * This file is part of the pastilda project.
 * hosted at http://github.com/thirdpin/pastilda
 *
 * Copyright (C) 2016  Third Pin LLC
 *
 * Written by:
 *  Anastasiia Lazareva <a.lazareva@thirdpin.ru>
 *	Dmitrii Lisin <mrlisdim@ya.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef USBH_HOST_H
#define USBH_HOST_H

#include <string.h>
#include "timer_ext.h"
#include "gpio_ext.h"
#include "systick_ext.h"
BEGIN_DECLS
#include "usbh_hubbed.h"
#include "usbh_driver_hid_kbd.h"
#include "usbh_lld_stm32f4.h"
END_DECLS

using namespace TIMER_CPP_Extension;
using namespace GPIO_CPP_Extension;

constexpr uint8_t  USB_HOST_TIMER_NUMBER     = 6;
constexpr uint16_t USB_HOST_TIMER_PRESCALER  = (8400 - 1);
constexpr uint16_t USB_HOST_TIMER_PERIOD     = (65535);

typedef void (*callback_func)(uint8_t *data, uint8_t len);

static callback_func callback;

class USB_host
{
public:
	USB_host(callback_func callback);
	void poll();

	static void kbd_in_message_handler(uint8_t data_len, const uint8_t *data);

	static constexpr hid_kbd_config_t kbd_config = { &kbd_in_message_handler };
	static constexpr usbh_dev_driver_t *device_drivers[] =
	{
		(usbh_dev_driver_t *)&usbh_hid_kbd_driver
	};

private:
	TIMER_ext *_timer;
	void timer_setup();
	uint32_t get_time_us();
	void oth_hs_setup();
};
#endif
