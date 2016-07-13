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
