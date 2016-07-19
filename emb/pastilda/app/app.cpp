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

#include "app.h"
#include "stdio.h"
using namespace Application;

App *app_pointer;

App::App()
{
	app_pointer = this;

	clock_setup();
	systick_init();

	_leds_api = new LEDS_api();
	_flash = new FlashMemory();
	usb_host = new USB_host(redirect, control_interception);
	usb_composite = new USB_composite(_flash->flash_blocks(), _flash->flash_read, _flash->flash_write);
}
void App::process()
{
	_leds_api->toggle();
	usb_host->poll();
}

void App::redirect(uint8_t *data, uint8_t len)
{
	app_pointer->usb_composite->usb_send_packet(data, len);
}

void App::control_interception()
{
	memset(app_pointer->key, 0, 8);
	app_pointer->key[2] = KEY_W;
	app_pointer->key[3] = KEY_O;
	app_pointer->key[4] = KEY_N;
	app_pointer->key[5] = KEY_D;
	app_pointer->key[6] = KEY_E;
	app_pointer->key[7] = KEY_R;
	app_pointer->usb_composite->usb_send_packet(app_pointer->key, 8);

	app_pointer->key[2] = 0;
	app_pointer->key[3] = 0;
	app_pointer->key[4] = 0;
	app_pointer->key[5] = 0;
	app_pointer->key[6] = 0;
	app_pointer->key[7] = 0;
	app_pointer->usb_composite->usb_send_packet(app_pointer->key, 8);

	app_pointer->key[2] = KEY_SPACEBAR;
	app_pointer->key[3] = KEY_W;
	app_pointer->key[4] = KEY_O;
	app_pointer->key[5] = KEY_M;
	app_pointer->key[6] = KEY_A;
	app_pointer->key[7] = KEY_N;
	app_pointer->usb_composite->usb_send_packet(app_pointer->key, 8);

	app_pointer->key[2] = 0;
	app_pointer->key[3] = 0;
	app_pointer->key[4] = 0;
	app_pointer->key[5] = 0;
	app_pointer->key[6] = 0;
	app_pointer->key[7] = 0;
	app_pointer->usb_composite->usb_send_packet(app_pointer->key, 8);
}
