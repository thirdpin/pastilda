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

#ifndef APP_H
#define APP_H

#include <string.h>
#include "clock.h"
#include "gpio_ext.h"
#include "systick_ext.h"
#include "leds.h"
#include "usb_dispatcher.h"
#include "spi_ext.h"

using namespace LEDS_API;
using namespace GPIO_CPP_Extension;
using namespace SPI_CPP_Extension;

namespace Application
{
	class App
	{
	public:
		App();
		void process();

	private:
		LEDS_api *_leds_api;
		USB_dispatcher *usb_dispatcher;
	};
}
#endif
