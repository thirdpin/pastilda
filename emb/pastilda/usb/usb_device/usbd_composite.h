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

#ifndef USB_MSC_H
#define USB_MSC_H

#include <fs/file_system.h>
#include "usbd_composite_desc.h"
#include "systick_ext.h"
#include "gpio_ext.h"
#include "usb_deque.h"

using namespace UsbPackages;

#define USB_OTG_IRQ                  otg_fs_isr
extern "C" void USB_OTG_IRQ();

int USB_control_callback(usbd_device *usbd_dev, struct usb_setup_data *req,
		                 uint8_t **buf, uint16_t *len, usbd_control_complete_callback *complete);

void USB_set_config_callback(usbd_device *usbd_dev, uint16_t wValue);

static uint8_t keyboard_protocol = 1;
static uint8_t keyboard_idle = 0;
static uint8_t keyboard_leds = 0;

class USB_composite
{
public:
	uint8_t usbd_control_buffer[500];
	UsbCompositeDescriptors *descriptors;
	volatile uint32_t last_usb_request_time;

	usbd_device *my_usb_device;

	USB_composite(UsbMemoryControlParams params);

	void usb_send_packet(const void *buf, int len);
	uint16_t usb_send_packet_nonblock(const void *buf, int len);

	static void device_keybord_interrupt(usbd_device*, unsigned char);

	void poll() {
		usbd_poll(my_usb_device);
		last_usb_request_time = get_counter_ms();
	}

	int hid_control_request(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
			void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req));

	void hid_set_config(usbd_device *usbd_dev, uint16_t wValue)
	{
		(void)wValue;
		(void)usbd_dev;

		usbd_ep_setup(usbd_dev, Endpoint::E_KEYBOARD, USB_ENDPOINT_ATTR_INTERRUPT, 8, device_keybord_interrupt);
		usbd_register_control_callback(usbd_dev, USB_REQ_TYPE_INTERFACE, USB_REQ_TYPE_RECIPIENT, USB_control_callback );
	}

	UsbDequeStandart* get_usb_deque() {
		return &_usbDeque;
	}

	void init_hid_interrupt();
	void send_zero_package();

private:
	UsbDequeStandart _usbDeque;
};
#endif
