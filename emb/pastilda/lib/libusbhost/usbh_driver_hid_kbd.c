#include "usbh_hubbed.h"
#include "usbh_device_driver.h"
#include "usbh_driver_hid_kbd.h"
//#include "usart_helpers.h"

#include <libopencm3/usb/usbstd.h>

enum STATES {
	STATE_INACTIVE,
	STATE_READING_COMPLETE,
	STATE_READING_REQUEST,
	STATE_SET_CONFIGURATION_REQUEST,
	STATE_SET_CONFIGURATION_EMPTY_READ,
	STATE_SET_CONFIGURATION_COMPLETE
};

struct _hid_kbd_device {
	usbh_device_t *usbh_device;
	uint8_t buffer[USBH_HID_MOUSE_BUFFER];
	uint16_t endpoint_in_maxpacketsize;
	uint8_t endpoint_in_address;
	enum STATES state_next;
	uint8_t endpoint_in_toggle;
	uint8_t device_id;
	uint8_t configuration_value;
};
typedef struct _hid_kbd_device hid_kbd_device_t;

static hid_kbd_device_t kbd_device[USBH_HID_MOUSE_MAX_DEVICES];
static const hid_kbd_config_t *kbd_config;

#include <stdint.h>

static bool initialized = false;

void hid_kbd_driver_init(const hid_kbd_config_t *config)
{
	uint32_t i;
	initialized = true;

	kbd_config = config;
	for (i = 0; i < USBH_HID_MOUSE_MAX_DEVICES; i++) {
		kbd_device[i].state_next = STATE_INACTIVE;
	}
}

static void *init(void *usbh_dev)
{
	if (!initialized) {
		return (0);
	}

	uint32_t i;
	hid_kbd_device_t *drvdata = 0;

	// find free data space for mouse device
	for (i = 0; i < USBH_HID_MOUSE_MAX_DEVICES; i++) {
		if (kbd_device[i].state_next == STATE_INACTIVE) {
			drvdata = &kbd_device[i];
			drvdata->device_id = i;
			drvdata->endpoint_in_address = 0;
			drvdata->endpoint_in_toggle = 0;
			drvdata->usbh_device = (usbh_device_t *)usbh_dev;
			break;
		}
	}

	return (drvdata);
}

static bool analyze_descriptor(void *drvdata, void *descriptor)
{
	hid_kbd_device_t *kbd = (hid_kbd_device_t *)drvdata;
	uint8_t desc_type = ((uint8_t *)descriptor)[1];
	switch (desc_type) {
	case USB_DT_CONFIGURATION:
		{
			struct usb_config_descriptor *cfg = (struct usb_config_descriptor*)descriptor;
			kbd->configuration_value = cfg->bConfigurationValue;
		}
		break;
	case USB_DT_DEVICE:
		break;
	case USB_DT_INTERFACE:
		break;
	case USB_DT_ENDPOINT:
		{
			struct usb_endpoint_descriptor *ep = (struct usb_endpoint_descriptor*)descriptor;
			if ((ep->bmAttributes&0x03) == USB_ENDPOINT_ATTR_INTERRUPT) {
				uint8_t epaddr = ep->bEndpointAddress;
				if (epaddr & (1<<7)) {
					kbd->endpoint_in_address = epaddr&0x7f;
					if (ep->wMaxPacketSize < USBH_HID_MOUSE_BUFFER) {
						kbd->endpoint_in_maxpacketsize = ep->wMaxPacketSize;
					} else {
						kbd->endpoint_in_maxpacketsize = USBH_HID_MOUSE_BUFFER;
					}
				}

				if (kbd->endpoint_in_address) {
					kbd->state_next = STATE_SET_CONFIGURATION_REQUEST;
					return (true);
				}
			}
		}
		break;
	// TODO Class Specific descriptors
	default:
		break;
	}
	return (false);
}

static void event(usbh_device_t *dev, usbh_packet_callback_data_t cb_data)
{
	hid_kbd_device_t *kbd = (hid_kbd_device_t *)dev->drvdata;
	switch (kbd->state_next) {
	case STATE_READING_COMPLETE:
		{
			switch (cb_data.status) {
			case USBH_PACKET_CALLBACK_STATUS_OK:
			case USBH_PACKET_CALLBACK_STATUS_ERRSIZ:
				kbd->state_next = STATE_READING_REQUEST;
				break;

			case USBH_PACKET_CALLBACK_STATUS_EFATAL:
			case USBH_PACKET_CALLBACK_STATUS_EAGAIN:
				kbd->state_next = STATE_INACTIVE;
				break;
			}
		}
		break;

	case STATE_SET_CONFIGURATION_EMPTY_READ:
		{
			switch (cb_data.status) {
			case USBH_PACKET_CALLBACK_STATUS_OK:
				kbd->state_next = STATE_SET_CONFIGURATION_COMPLETE;
				device_xfer_control_read(0, 0, event, dev);
				break;

			case USBH_PACKET_CALLBACK_STATUS_ERRSIZ:
			case USBH_PACKET_CALLBACK_STATUS_EFATAL:
			case USBH_PACKET_CALLBACK_STATUS_EAGAIN:
				kbd->state_next = STATE_INACTIVE;
				break;
			}
		}
		break;
	case STATE_SET_CONFIGURATION_COMPLETE: // Configured
		{
			switch (cb_data.status) {
			case USBH_PACKET_CALLBACK_STATUS_OK:
				kbd->state_next = STATE_READING_REQUEST;
				kbd->endpoint_in_toggle = 0;
				break;

			case USBH_PACKET_CALLBACK_STATUS_ERRSIZ:
			case USBH_PACKET_CALLBACK_STATUS_EFATAL:
			case USBH_PACKET_CALLBACK_STATUS_EAGAIN:
				kbd->state_next = STATE_INACTIVE;
				break;
			}
		}
		break;
	default:
		break;
	}
}

static void read_kbd_in(void *drvdata)
{
	hid_kbd_device_t *kbd = (hid_kbd_device_t *)drvdata;
	usbh_packet_t packet;

	packet.address = kbd->usbh_device->address;
	packet.data = &kbd->buffer[0];
	packet.datalen = kbd->endpoint_in_maxpacketsize;
	packet.endpoint_address = kbd->endpoint_in_address;
	packet.endpoint_size_max = kbd->endpoint_in_maxpacketsize;
	packet.endpoint_type = USBH_EPTYP_INTERRUPT;
	packet.speed = kbd->usbh_device->speed;
	packet.callback = event;
	packet.callback_arg = kbd->usbh_device;
	packet.toggle = &kbd->endpoint_in_toggle;

	kbd->state_next = STATE_READING_COMPLETE;
	usbh_read(kbd->usbh_device, &packet);
	kbd_config->kbd_in_message_handler(packet.datalen, packet.data);
}

static void poll(void *drvdata, uint32_t time_curr_us)
{
	(void)time_curr_us;

	hid_kbd_device_t *kbd = (hid_kbd_device_t *)drvdata;
	usbh_device_t *dev = kbd->usbh_device;
	switch (kbd->state_next) {
	case STATE_READING_REQUEST:
		{
			read_kbd_in(drvdata);
		}
		break;

	case STATE_SET_CONFIGURATION_REQUEST:
		{
			struct usb_setup_data setup_data;

			setup_data.bmRequestType = 0b00000000;
			setup_data.bRequest = USB_REQ_SET_CONFIGURATION;
			setup_data.wValue = kbd->configuration_value;
			setup_data.wIndex = 0;
			setup_data.wLength = 0;

			kbd->state_next = STATE_SET_CONFIGURATION_EMPTY_READ;

			device_xfer_control_write(&setup_data, sizeof(setup_data), event, dev);
		}
		break;

	default:
		// do nothing - probably transfer is in progress
		break;
	}
}

static void remove(void *drvdata)
{
	hid_kbd_device_t *kbd = (hid_kbd_device_t *)drvdata;
	kbd->state_next = STATE_INACTIVE;
	kbd->endpoint_in_address = 0;
}

static const usbh_dev_driver_info_t driver_info = {
	.deviceClass = -1,
	.deviceSubClass = -1,
	.deviceProtocol = -1,
	.idVendor = -1,
	.idProduct = -1,
	.ifaceClass = 0x03,
	.ifaceSubClass = 1, //-1
	.ifaceProtocol = 0x01 //0x02
};

const usbh_dev_driver_t usbh_hid_kbd_driver = {
	.init = init,
	.analyze_descriptor = analyze_descriptor,
	.poll = poll,
	.remove = remove,
	.info = &driver_info
};
