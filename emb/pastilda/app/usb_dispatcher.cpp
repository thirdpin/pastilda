#include "usb_dispatcher.h"

USB_dispatcher *dispatcher_pointer;

USB_dispatcher::USB_dispatcher()
{
	dispatcher_pointer = this;
	usb_composite = new USB_composite();
	usb_host = new USB_host(redirect, control_interception);
	_interception_enabled = false;
}

void USB_dispatcher::redirect(uint8_t *data, uint8_t len)
{
	dispatcher_pointer->usb_composite->usb_send_packet(data, len);
}

void USB_dispatcher::control_interception()
{
	memset(dispatcher_pointer->key, 0, 8);
	dispatcher_pointer->key[2] = KEY_W;
	dispatcher_pointer->key[3] = KEY_O;
	dispatcher_pointer->key[4] = KEY_N;
	dispatcher_pointer->key[5] = KEY_D;
	dispatcher_pointer->key[6] = KEY_E;
	dispatcher_pointer->key[7] = KEY_R;
	dispatcher_pointer->usb_composite->usb_send_packet(dispatcher_pointer->key, 8);

	dispatcher_pointer->key[2] = 0;
	dispatcher_pointer->key[3] = 0;
	dispatcher_pointer->key[4] = 0;
	dispatcher_pointer->key[5] = 0;
	dispatcher_pointer->key[6] = 0;
	dispatcher_pointer->key[7] = 0;
	dispatcher_pointer->usb_composite->usb_send_packet(dispatcher_pointer->key, 8);

	dispatcher_pointer->key[2] = KEY_SPACEBAR;
	dispatcher_pointer->key[3] = KEY_W;
	dispatcher_pointer->key[4] = KEY_O;
	dispatcher_pointer->key[5] = KEY_M;
	dispatcher_pointer->key[6] = KEY_A;
	dispatcher_pointer->key[7] = KEY_N;
	dispatcher_pointer->usb_composite->usb_send_packet(dispatcher_pointer->key, 8);

	dispatcher_pointer->key[2] = 0;
	dispatcher_pointer->key[3] = 0;
	dispatcher_pointer->key[4] = 0;
	dispatcher_pointer->key[5] = 0;
	dispatcher_pointer->key[6] = 0;
	dispatcher_pointer->key[7] = 0;
	dispatcher_pointer->usb_composite->usb_send_packet(dispatcher_pointer->key, 8);
}
