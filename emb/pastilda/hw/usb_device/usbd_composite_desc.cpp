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
