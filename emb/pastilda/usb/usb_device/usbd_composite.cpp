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

#include "menu/UsbPackageFactory.h"
#include "keys/Key.h"
#include "stdio.h"

#include "usbd_composite.h"

using namespace GPIO_CPP_Extension;

USB_composite *usb_pointer;

void USB_composite::device_keybord_interrupt(usbd_device*, unsigned char)
{
	static UsbPackage lastPackage = UsbPackages::ZERO_PACKAGE;

	UsbDequeStandart* usbDeque = usb_pointer->get_usb_deque();

	if (usbDeque->isPopOnly() && !usbDeque->empty()) {
		UsbPackage& package = usbDeque->front();

		lastPackage = package;

		size_t result = usb_pointer->usb_send_packet_nonblock(
				package.data(), package.length()
			);

		bool packageSended = (result != 0);
		if (packageSended) {
			usbDeque->pop_front();
		}
	}
	else {
		usb_pointer->usb_send_packet_nonblock(
				lastPackage.data(), lastPackage.length()
			);
	}
}


USB_composite::USB_composite(UsbMemoryControlParams memoryParams)
{
	usb_pointer = this;
	descriptors = new UsbCompositeDescriptors();

	GPIO_ext uf_p(PA11);
	GPIO_ext uf_m(PA12);

	uf_p.mode_setup(Mode::ALTERNATE_FUNCTION, PullMode::NO_PULL);
	uf_m.mode_setup(Mode::ALTERNATE_FUNCTION, PullMode::NO_PULL);

	uf_p.set_af(AF_Number::AF10);
	uf_m.set_af(AF_Number::AF10);

	my_usb_device = usbd_init(&otgfs_usb_driver, &(descriptors->dev),
			                  &(descriptors->config_descr), (const char**)descriptors->usb_strings, 3,
							  usbd_control_buffer, sizeof(usbd_control_buffer));

	usbd_register_set_config_callback(my_usb_device, USB_set_config_callback);

	usb_msc_init(my_usb_device,
			     Endpoint::E_MASS_STORAGE_IN, 64, Endpoint::E_MASS_STORAGE_OUT, 64,
			     "ThirdPin", "Pastilda", "0.00",
				 memoryParams.block_count, memoryParams.read_block_func, memoryParams.write_block_func);

	nvic_set_priority(NVIC_OTG_FS_IRQ, 0x01<<7);
	nvic_enable_irq(NVIC_OTG_FS_IRQ);
}

void USB_composite::init_hid_interrupt()
{
	send_zero_package();
}

void USB_composite::send_zero_package()
{
	usb_send_packet_nonblock(
			UsbPackages::ZERO_PACKAGE.data(),
			UsbPackages::ZERO_PACKAGE.length()
		);
}

void USB_composite::usb_send_packet(const void *buf, int len)
{
    while(usbd_ep_write_packet(my_usb_device, 0x81, buf, len) == 0);
}

uint16_t USB_composite::usb_send_packet_nonblock(const void *buf, int len)
{
    return usbd_ep_write_packet(my_usb_device, 0x81, buf, len);
}

void USB_OTG_IRQ()
{
	usbd_poll(usb_pointer->my_usb_device);
	usb_pointer->last_usb_request_time = get_counter_ms();
}

int USB_composite::hid_control_request(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
					                    void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req))
{
	(void)complete;
	(void)usbd_dev;

	if ((req->bmRequestType & USB_REQ_TYPE_DIRECTION) == USB_REQ_TYPE_IN)
	{
		if ((req->bmRequestType & USB_REQ_TYPE_TYPE) == USB_REQ_TYPE_STANDARD)
		{
			if (req->bRequest == USB_REQ_GET_DESCRIPTOR)
			{
				if (req->wValue == 0x2200)
				{
					*buf = (uint8_t *)descriptors->keyboard_report_descriptor;
					*len = sizeof(descriptors->keyboard_report_descriptor);
					return (USBD_REQ_HANDLED);
				}
				else if (req->wValue == 0x2100)
				{
					*buf = (uint8_t *)&descriptors->keyboard_hid_function;
					*len = sizeof(descriptors->keyboard_hid_function);
					return (USBD_REQ_HANDLED);
				}
				return (USBD_REQ_NOTSUPP);
			}
		}
		else if ((req->bmRequestType & USB_REQ_TYPE_TYPE) == USB_REQ_TYPE_CLASS)
		{
			if (req->bRequest == HidRequest::GET_REPORT)
			{
				static UsbPackage package = UsbPackages::ZERO_PACKAGE;
				*buf = package.data();
				*len = package.length();
				return (USBD_REQ_HANDLED);
			}
			else if (req->bRequest == HidRequest::GET_IDLE)
			{
				*buf = &keyboard_idle;
				*len = sizeof(keyboard_idle);
				return (USBD_REQ_HANDLED);
			}
			else if (req->bRequest == HidRequest::GET_PROTOCOL)
			{
				*buf = &keyboard_protocol;
				*len = sizeof(keyboard_protocol);
				return (USBD_REQ_HANDLED);
			}
			return (USBD_REQ_NOTSUPP);
		}
	}

	else
	{
		if ((req->bmRequestType & USB_REQ_TYPE_TYPE) == USB_REQ_TYPE_CLASS)
		{
			if (req->bRequest == HidRequest::SET_REPORT)
			{
				if (*len == 1)
				{
					keyboard_leds = (*buf)[0];
				}
				return (USBD_REQ_HANDLED);
			}
			else if (req->bRequest == HidRequest::SET_IDLE)
			{
				keyboard_idle = req->wValue >> 8;
				return (USBD_REQ_HANDLED);
			}
			else if (req->bRequest == HidRequest::SET_PROTOCOL)
			{
				keyboard_protocol = req->wValue;
				return (USBD_REQ_HANDLED);
			}
		}
		return (USBD_REQ_NOTSUPP);
	}

	return (USBD_REQ_NEXT_CALLBACK);
}

int USB_control_callback(usbd_device *usbd_dev,
		struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
		usbd_control_complete_callback *complete)
{
	return( usb_pointer->hid_control_request(usbd_dev, req, buf, len, complete));
}


void USB_set_config_callback(usbd_device *usbd_dev,
		 uint16_t wValue)
{
	usb_pointer->hid_set_config(usbd_dev, wValue) ;
}
