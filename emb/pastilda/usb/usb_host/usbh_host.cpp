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

#include "usbh_host.h"

constexpr hid_kbd_config_t USB_host::kbd_config;
constexpr usbh_dev_driver_t* USB_host::device_drivers[];

USB_host *usb_host_pointer;
USB_host::USB_host(callback_func _callback)
{
	usb_host_pointer = this;
	callback = _callback;

	timer_setup();
	oth_hs_setup();
	hid_kbd_driver_init(&kbd_config);
	usbh_init(usbh_lld_stm32f4_drivers, device_drivers);
}

void USB_host::poll()
{
	usbh_poll(get_time_us());
	delay_ms(1);
}

void USB_host::kbd_in_message_handler(uint8_t data_len, const uint8_t *data)
{
	callback((uint8_t*)data, data_len);
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

//convert to 1MHz less precise timer value
//units: microseconds
uint32_t USB_host::get_time_us()
{
	return ((_timer->get_counter_value()) * 100);
}
