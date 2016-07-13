#include "usbd_composite.h"

using namespace GPIO_CPP_Extension;

USB_composite *usb_pointer;
USB_composite::USB_composite()
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
	nvic_enable_irq(NVIC_OTG_FS_IRQ);

	ramdisk_init();

	usb_msc_init(my_usb_device, Endpoint::E_MASS_STORAGE_IN, 64, Endpoint::E_MASS_STORAGE_OUT, 64,
			"ThirdPin", "Pastilda", "0.00", ramdisk_blocks(), ramdisk_read, ramdisk_write);
}

void USB_composite::usb_send_packet(const void *buf, int len)
{
    while(usbd_ep_write_packet(my_usb_device, 0x81, buf, len) == 0);
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
				*buf = (uint8_t*)&boot_key_report;
				*len = sizeof(boot_key_report);
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
