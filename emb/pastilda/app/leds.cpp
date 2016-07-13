#include "leds.h"

using namespace LEDS_API;

LEDS_api::LEDS_api()
{
	_leds[0] = new GPIO_ext(LED_A);
	_leds[1] = new GPIO_ext(LED_B);
	_leds[0]->mode_setup(Mode::OUTPUT, PullMode::NO_PULL);
	_leds[1]->mode_setup(Mode::OUTPUT, PullMode::NO_PULL);

	_leds_state = LEDS_INI_STATE;
	_timer_leds_toggle = new TimerMs(TimerMode::CYCLE, LEDS_TOGGLE_PERIOD_MS);
}

void LEDS_api::toggle()
{
	if (_timer_leds_toggle->timeout()) {
		leds_toggle();
	}
}

void LEDS_api::leds_toggle()
{
	uint8_t mask = LEDS_INI_STATE;
	for(int i = 0; i < LEDS_COUNT; i++) {
		if (!(_leds_state & mask)) {
			_leds[i]->set();
	    }
	    else {
	    	_leds[i]->clear();
	    }

	    mask = mask << 1;
	}

	if( ++_leds_state > LEDS_MAX_STATE ) {
		_leds_state = LEDS_INI_STATE;
	}
}
