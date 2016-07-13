#ifndef USBH_HOST_H
#define USBH_HOST_H

#include <string.h>
#include <libopencm3/cm3/common.h>
#include <libopencm3/stm32/usart.h>
#include "timer_ext.h"
#include "gpio_ext.h"
#include "systick_ext.h"
BEGIN_DECLS
#include "usart_helpers.h"
#include "usbh_hubbed.h"
#include "usbh_driver_hid_kbd.h"
#include "usbh_lld_stm32f4.h"
END_DECLS

using namespace TIMER_CPP_Extension;
using namespace GPIO_CPP_Extension;

constexpr uint8_t  USB_HOST_TIMER_NUMBER     = 6;
constexpr uint16_t USB_HOST_TIMER_PRESCALER  = (8400 - 1);
constexpr uint16_t USB_HOST_TIMER_PERIOD     = (65535);

typedef void (*redirect)(uint8_t *data, uint8_t len);
typedef void (*control_interception)();

static redirect redirect_callback;
static control_interception control_interception_callback;

class USB_host
{
public:
	USB_host(redirect redirect_callback, control_interception control_interception_callback);
	void poll();

	static void kbd_in_message_handler(uint8_t data_len, const uint8_t *data);

	static constexpr hid_kbd_config_t kbd_config = { &kbd_in_message_handler };
	static constexpr usbh_dev_driver_t *device_drivers[] =
	{
		(usbh_dev_driver_t *)&usbh_hid_kbd_driver
	};

private:
	TIMER_ext *_timer;
	void timer_setup();
	uint32_t get_time_us();
	void oth_hs_setup();
	void usart_setup();
};
#endif
