#ifndef USB_MSC_H
#define USB_MSC_H

extern "C"
{
#include "ramdisk.h"
}
#include "usbd_composite_desc.h"
#include "systick_ext.h"
#include "gpio_ext.h"

#define USB_OTG_IRQ                  otg_fs_isr
extern "C" void USB_OTG_IRQ();

int USB_control_callback(usbd_device *usbd_dev, struct usb_setup_data *req,
		                 uint8_t **buf, uint16_t *len, usbd_control_complete_callback *complete);

void USB_set_config_callback(usbd_device *usbd_dev, uint16_t wValue);

static uint8_t keyboard_protocol = 1;
static uint8_t keyboard_idle = 0;
static uint8_t keyboard_leds = 0;

static struct {
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t keys[6];
} boot_key_report;

class USB_composite
{
public:
	uint8_t usbd_control_buffer[500];
	UsbCompositeDescriptors *descriptors;
	uint8_t usb_ready = 0;
	volatile uint32_t last_usb_request_time;
	usbd_device *my_usb_device;

	USB_composite();

	void usb_send_packet(const void *buf, int len);

	void usb_poll()
	{
		usbd_poll(my_usb_device);
	}

	int hid_control_request(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
			void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req));

	void hid_set_config(usbd_device *usbd_dev, uint16_t wValue)
	{
		(void)wValue;
		(void)usbd_dev;

		usbd_ep_setup(usbd_dev, Endpoint::E_KEYBOARD, USB_ENDPOINT_ATTR_INTERRUPT, 8, 0);
		usbd_register_control_callback(usbd_dev, USB_REQ_TYPE_INTERFACE, USB_REQ_TYPE_RECIPIENT, USB_control_callback );
	}
};
#endif
