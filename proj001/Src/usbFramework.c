#include "usbFramework.h"

void usb_init(void)
{
	usb_gpio_init();
	usb_core_init();
	device_core_init();
	connect_usb();
}