#include "app.h"
#include "stdio.h"
using namespace Application;

#ifdef DEBUG
    #define DEBUG_PRINT(x) printf(x)
#else
    #define DEBUG_PRINT(x) do {} while (0)
#endif

App *app_pointer;

App::App()
{
	app_pointer = this;
	clock_setup();
	systick_init();

	_leds_api = new LEDS_api();
	delay_ms(1000);

	usb_dispatcher = new USB_dispatcher();
}
void App::process()
{
	_leds_api->toggle();
	usb_dispatcher->process();
}
