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

#ifndef LEDS_API_H
#define LEDS_API_H

#include "gpio_ext.h"
#include "systick_ext.h"

using namespace GPIO_CPP_Extension;

namespace LEDS_API
{
	constexpr Pinout LED_R                       = PB0;
	constexpr Pinout LED_G                       = PB1;
	constexpr Pinout LED_B                       = PB3;
	constexpr uint8_t LEDS_COUNT                 = 0x03;
	constexpr uint8_t LEDS_MAX_STATE             = 0x07;
	constexpr uint8_t LEDS_INI_STATE     		 = 0x01;
	constexpr uint32_t LEDS_TOGGLE_PERIOD_MS     = 1000;

	class LEDS_api
	{
	public:
		LEDS_api();
		void toggle();

	private:
		TimerMs _timer_leds_toggle;
		GPIO_ext _leds[LEDS_COUNT];
		uint8_t _leds_state;

		void leds_toggle();
	};
}

#endif
