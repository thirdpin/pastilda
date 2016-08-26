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

#ifndef APP_H
#define APP_H

#include <string.h>

#include "clock.h"
#include "leds.h"
#include "usbd_composite.h"
#include "usbh_host.h"
#include "file_system.h"
#include "menu/TildaLogic.h"
#include "keepass_reader.h"

using namespace LEDS_API;
using namespace KeepAss;

namespace Application
{
	class App
	{
	public:
		App();
		void process();

		static void host_keyboard_callback(uint8_t *data, uint8_t len);

	private:
		LEDS_api _leds_api;
		FileSystem *_fs;
		USB_composite *_usb_composite;
		USB_host *_usb_host;
		Logic::TildaLogic* _tildaLogic;
	};
}
#endif
