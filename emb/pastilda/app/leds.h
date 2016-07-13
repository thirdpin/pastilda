#ifndef LEDS_API_H
#define LEDS_API_H

#include "gpio_ext.h"
#include "systick_ext.h"

using namespace GPIO_CPP_Extension;

namespace LEDS_API
{
	constexpr Pinout LED_A                       = PD12;
	constexpr Pinout LED_B                       = PD13;
	constexpr uint8_t LEDS_COUNT                 = 0x02;
	constexpr uint8_t LEDS_MAX_STATE             = 0x02;
	constexpr uint8_t LEDS_INI_STATE     		 = 0x01;
	constexpr uint32_t LEDS_TOGGLE_PERIOD_MS     = 500;

	class LEDS_api
	{
	public:
		LEDS_api();
		void toggle();

	private:
		TimerMs *_timer_leds_toggle;
		GPIO_ext *_leds[LEDS_COUNT];

		uint8_t _leds_state;

		void leds_toggle();
	};
}

#endif
