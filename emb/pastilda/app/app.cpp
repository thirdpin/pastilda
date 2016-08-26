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

#include <cstdio>

#include <FastDelegate.h>
#include <libopencmsis/core_cm3.h>

#include "app.h"

using namespace std::placeholders;

using namespace Application;
using namespace Keys;

namespace fastdeligate = fd;

App *app_pointer;

App::App()
{
	app_pointer = this;

	scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP2_SUB8);

	_fs = new FileSystem();

	_usb_composite = new USB_composite(UsbMemoryControlParams {	_fs->msd_blocks(),
																_fs->msd_read,
																_fs->msd_write });

	Logic::TildaLogic::SpecialPoints specialMenuPoints {
		{
			"Format flash",
			std::strlen("Format flash\0"),
			fd::MakeDelegate(_fs, &FileSystem::format_to_FAT12)
		}
	};
	_tildaLogic = new Logic::TildaLogic(_usb_composite->get_usb_deque(),
								 	 	specialMenuPoints);

	_usb_host = new USB_host(host_keyboard_callback);
	// TODO: fix it
	delay_ms(6000);  // wait usb device initializing
	_usb_composite->init_hid_interrupt();
}

void App::process()
{
	_leds_api.toggle();
	_usb_host->poll();
}

void App::host_keyboard_callback(uint8_t *data, uint8_t len)
{
	app_pointer->_tildaLogic->process(data, len);
}
