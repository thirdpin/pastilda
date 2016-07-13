#include "usbh_host.h"

constexpr hid_kbd_config_t USB_host::kbd_config;
constexpr usbh_dev_driver_t* USB_host::device_drivers[];

USB_host *usb_host_pointer;
USB_host::USB_host(redirect _redirect_callback, control_interception _control_interception_callback)
{
	usb_host_pointer = this;
	redirect_callback = _redirect_callback;
	control_interception_callback = _control_interception_callback;

	timer_setup();
	oth_hs_setup();
	usart_setup();

	hid_kbd_driver_init(&kbd_config);
	usbh_init(usbh_lld_stm32f4_drivers, device_drivers);

	LOG_PRINTF("USB init complete\n");
	LOG_FLUSH();
}

void USB_host::poll()
{
	usbh_poll(get_time_us());
	LOG_FLUSH();
	delay_ms(1);
}

void USB_host::kbd_in_message_handler(uint8_t data_len, const uint8_t *data)
{
	if ((data[0] == 0x03) && data[2] == 0x35) {
		control_interception_callback();
	}
	else {
		redirect_callback((uint8_t*)data, data_len);
	}
}

void USB_host::timer_setup()
{
	_timer = new TIMER_ext(USB_HOST_TIMER_NUMBER);
	_timer->set_prescaler_value(USB_HOST_TIMER_PRESCALER);
	_timer->set_autoreload_value(USB_HOST_TIMER_PERIOD);
	_timer->enable_counter();
}

void USB_host::oth_hs_setup()
{
	GPIO_ext uf_p(PB15);
	GPIO_ext uf_m(PB14);

	uf_p.mode_setup(Mode::ALTERNATE_FUNCTION, PullMode::NO_PULL);
	uf_m.mode_setup(Mode::ALTERNATE_FUNCTION, PullMode::NO_PULL);

	uf_p.set_af(AF_Number::AF12);
	uf_m.set_af(AF_Number::AF12);
}

void USB_host::usart_setup()
{
	GPIO_ext uart_rx(PC6);
	GPIO_ext uart_tx(PC7);

	uart_rx.mode_setup(Mode::ALTERNATE_FUNCTION, PullMode::NO_PULL);
	uart_tx.mode_setup(Mode::ALTERNATE_FUNCTION, PullMode::NO_PULL);

	uart_rx.set_af(AF_Number::AF8);
	uart_tx.set_af(AF_Number::AF8);

#ifdef USART_DEBUG
	usart_init(USART6, 921600);
#endif
}

//convert to 1MHz less precise timer value
//units: microseconds
uint32_t USB_host::get_time_us()
{
	return ((_timer->get_counter_value()) * 100);
}
