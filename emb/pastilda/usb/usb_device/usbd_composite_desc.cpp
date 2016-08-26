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

#include "usbd_composite_desc.h"

constexpr uint8_t UsbCompositeDescriptors::keyboard_report_descriptor[];
constexpr UsbCompositeDescriptors::type_hid_function  UsbCompositeDescriptors::keyboard_hid_function;
constexpr struct usb_device_descriptor UsbCompositeDescriptors::dev;
constexpr struct usb_endpoint_descriptor UsbCompositeDescriptors::hid_endpoint;
constexpr struct usb_endpoint_descriptor UsbCompositeDescriptors::msc_endpoint[];
constexpr struct usb_interface_descriptor UsbCompositeDescriptors::iface[];
constexpr struct usb_config_descriptor::usb_interface UsbCompositeDescriptors::ifaces[];
constexpr struct usb_config_descriptor UsbCompositeDescriptors::config_descr;
constexpr char UsbCompositeDescriptors::usb_strings[][30];
