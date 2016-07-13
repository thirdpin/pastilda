#ifndef USBH_DRIVER_HID_KBD
#define USBH_DRIVER_HID_KBD

#include "usbh_hubbed.h"
#include <stdint.h>

BEGIN_DECLS

typedef void (*t_read_kbd)(void* data, uint8_t data_len);

struct _hid_kbd_config {
	void (*kbd_in_message_handler)(uint8_t device_id, const uint8_t *data);
};
typedef struct _hid_kbd_config hid_kbd_config_t;

void hid_kbd_driver_init(const hid_kbd_config_t *config);

extern const usbh_dev_driver_t usbh_hid_kbd_driver;

END_DECLS

#endif
