/*
 * This file is part of the pastilda project.
 * hosted at http://github.com/thirdpin/pastilda
 *
 * Copyright (C) 2016  Third Pin LLC
 * Written by Anastasiia Lazareva <a.lazareva@thirdpin.ru>
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

#ifndef USB_DISPATCHER_H
#define USB_DISPATCHER_H

#include "usbh_host.h"
#include "usbd_composite.h"
extern "C"
{
#include "keyboard.h"
}

class USB_dispatcher
{
public:
	USB_composite *usb_composite;
	USB_host *usb_host;
	uint8_t key[8];

	USB_dispatcher();

	static void redirect(uint8_t *data, uint8_t len);
	static void control_interception();
	void do_work();
	void process()
	{
		usb_host->poll();
	}
private:
	bool _interception_enabled;


};

#endif
