#ifndef USB_DISPATCHER_H
#define USB_DISPATCHER_H

#include "usbh_host.h"
#include "usbd_composite.h"
extern "C"
{
#include "keyboard.h"
}

class USB_dispatcher
{
public:
	USB_composite *usb_composite;
	USB_host *usb_host;
	uint8_t key[8];

	USB_dispatcher();

	static void redirect(uint8_t *data, uint8_t len);
	static void control_interception();
	void do_work();
	void process()
	{
		usb_host->poll();
	}
private:
	bool _interception_enabled;


};

#endif
