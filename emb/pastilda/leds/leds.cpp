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

#include "leds.h"

using namespace LEDS_API;

LEDS_api::LEDS_api()
: _timer_leds_toggle(TimerMode::CYCLE, LEDS_TOGGLE_PERIOD_MS), _leds_state(LEDS_INI_STATE)
{
	_leds[0].init(LED_R);
	_leds[1].init(LED_G);
	_leds[2].init(LED_B);
	_leds[0].mode_setup(Mode::OUTPUT, PullMode::NO_PULL);
	_leds[1].mode_setup(Mode::OUTPUT, PullMode::NO_PULL);
	_leds[2].mode_setup(Mode::OUTPUT, PullMode::NO_PULL);

	_leds_state = LEDS_INI_STATE;
}

void LEDS_api::toggle()
{
	if (_timer_leds_toggle.timeout()) {
		leds_toggle();
	}
}

void LEDS_api::leds_toggle()
{
	uint8_t mask = LEDS_INI_STATE;
	for(int i = 0; i < LEDS_COUNT; i++) {
		if (!(_leds_state & mask)) {
			_leds[i].set();
	    }
	    else {
	    	_leds[i].clear();
	    }

	    mask = mask << 1;
	}

	if( ++_leds_state > LEDS_MAX_STATE ) {
		_leds_state = LEDS_INI_STATE;
	}
}
