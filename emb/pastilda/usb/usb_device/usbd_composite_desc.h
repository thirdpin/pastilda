/*
 * This file is part of the pastilda project.
 * hosted at http://github.com/thirdpin/pastilda
 *
 * Copyright (C) 2016  Third Pin LLC
 * Written by Anastasiia Lazareva <a.lazareva@thirdpin.ru>
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

#ifndef USB_COMPOSITE_DESCRIPTORS
#define USB_COMPOSITE_DESCRIPTORS

extern "C"
{
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/hid.h>
#include <libopencm3/usb/msc.h>
}

typedef enum {
	I_KEYBOARD     = 0,
	I_MASS_STORAGE = 1
} Interface;

typedef enum : uint8_t {
	E_KEYBOARD = 0x81,
	E_MASS_STORAGE_IN = 0x83,
	E_MASS_STORAGE_OUT = 0x03
} Endpoint;

typedef enum {
	GET_REPORT = 1,
	GET_IDLE = 2,
	GET_PROTOCOL = 3,
	SET_REPORT = 9,
	SET_IDLE = 10,
	SET_PROTOCOL = 11,
} HidRequest;

class UsbCompositeDescriptors
{
public:
	static constexpr uint8_t keyboard_report_descriptor[]  =
	{
			0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x05, 0x07, 0x19, 0xE0, 0x29, 0xE7, 0x15, 0x00, 0x25, 0x01,
			0x75, 0x01, 0x95, 0x08, 0x81, 0x02, 0x95, 0x01, 0x75, 0x08, 0x81, 0x01, 0x95, 0x03, 0x75, 0x01,
			0x05, 0x08, 0x19, 0x01, 0x29, 0x03, 0x91, 0x02, 0x95, 0x05, 0x75, 0x01, 0x91, 0x01, 0x95, 0x06,
			0x75, 0x08, 0x15, 0x00, 0x26, 0xFF, 0x00, 0x05, 0x07, 0x19, 0x00, 0x2A, 0xFF, 0x00, 0x81, 0x00,
			0xC0
	};

	static constexpr char  usb_strings[][30] =
	{
			"Third Pin",
			"Composite Device",
			"Pastilda"
	};

	static constexpr struct usb_device_descriptor dev =
	{
			USB_DT_DEVICE_SIZE, //bLength
			USB_DT_DEVICE,      //bDescriptorType
			0x0110,             //bcdUSB
			0x0,                //bDeviceClass
			0x00,               //bDeviceSubClass
			0x00,               //bDeviceProtocol
			64,                 //bMaxPacketSize0
			0x0483,             //idVendor
			0x5741,             //idProduct
			0x0200,             //bcdDevice
			1,                  //iManufacturer
			2,                  //iProduct
			3,                  //iSerialNumber
			1                   //bNumConfigurations
	};

	typedef struct __attribute__((packed))
	{
		struct usb_hid_descriptor hid_descriptor;
		struct
		{
			uint8_t bReportDescriptorType;
			uint16_t wDescriptorLength;
		} __attribute__((packed)) hid_report;
	} type_hid_function;

	static constexpr type_hid_function  keyboard_hid_function =
	{
		{
			9,          //bLength
			USB_DT_HID, //bDescriptorType
			0x0111,     //bcdHID
			0,          //bCountryCode
			1           //bNumDescriptors
		},

		{
			USB_DT_REPORT,
			sizeof(keyboard_report_descriptor)
		}
	};

	static constexpr struct usb_endpoint_descriptor hid_endpoint =
	{
		USB_DT_ENDPOINT_SIZE,        //bLength
		USB_DT_ENDPOINT,             //bDescriptorType
		Endpoint::E_KEYBOARD,        //bEndpointAddress
		USB_ENDPOINT_ATTR_INTERRUPT, //bmAttributes
		64,                          //wMaxPacketSize
		0x20                         //bInterval
	};

	static constexpr struct usb_endpoint_descriptor msc_endpoint[] =
	{
		{
			USB_DT_ENDPOINT_SIZE,        //bLength
			USB_DT_ENDPOINT,             //bDescriptorType
			Endpoint::E_MASS_STORAGE_IN, //bEndpointAddress
			USB_ENDPOINT_ATTR_BULK,      //bmAttributes
			64,                          //wMaxPacketSize
			0                            //bInterval
		},

		{
			USB_DT_ENDPOINT_SIZE,         //bLength
			USB_DT_ENDPOINT,              //bDescriptorType
			Endpoint::E_MASS_STORAGE_OUT, //bEndpointAddress
			USB_ENDPOINT_ATTR_BULK,       //bmAttributes
			64,                           //wMaxPacketSize
			0                             //bInterval
		}
	};

	static constexpr struct usb_interface_descriptor iface[] =
	{
		{
			USB_DT_INTERFACE_SIZE,   //bLength
			USB_DT_INTERFACE,        //bDescriptorType
			Interface::I_KEYBOARD,   //bInterfaceNumber
			0,                       //bAlternateSetting
			1,                       //bNumEndpoints
			USB_CLASS_HID,           //bInterfaceClass
			1,                       //bInterfaceSubClass
			1,                       //bInterfaceProtocol
			0,                       //iInterface
			&hid_endpoint, &keyboard_hid_function,
			sizeof(keyboard_hid_function)
		},

		{
			USB_DT_INTERFACE_SIZE,     //bLength
			USB_DT_INTERFACE,          //bDescriptorType
			Interface::I_MASS_STORAGE, //bInterfaceNumber
			0,                         //bAlternateSetting
			2,                         //bNumEndpoints
			USB_CLASS_MSC,             //bInterfaceClass
			USB_MSC_SUBCLASS_SCSI,     //bInterfaceSubClass
			USB_MSC_PROTOCOL_BBB,      //bInterfaceProtocol
			0x00,                      //iInterface
			msc_endpoint, 0, 0
		},
	};

	static constexpr struct usb_config_descriptor::usb_interface ifaces[]
	{
		{
			(uint8_t *)0,                   //cur_altsetting
			1,                              //num_altsetting
			(usb_iface_assoc_descriptor*)0, //iface_assoc
			&iface[Interface::I_KEYBOARD]   //altsetting
		},

		{
			(uint8_t *)0,                     //cur_altsetting
			1,                                //num_altsetting
			(usb_iface_assoc_descriptor*)0,   //iface_assoc
			&iface[Interface::I_MASS_STORAGE] //altsetting
		},
	};

	static constexpr struct usb_config_descriptor config_descr =
	{
		USB_DT_CONFIGURATION_SIZE, //bLength
		USB_DT_CONFIGURATION,      //bDescriptorType
		0,                         //wTotalLength
		2,                         //bNumInterfaces
		1,                         //bConfigurationValue
		0,                         //iConfiguration
		0x80,                      //bmAttributes
		0x50,                      //bMaxPower
		ifaces
	};

	UsbCompositeDescriptors() {}
};
#endif
