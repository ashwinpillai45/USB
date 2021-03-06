#include "usbFramework.h"

static USBDevice *usbdevice_Handler;

void usb_init(USBDevice *usbdevice)
{
	usbdevice_Handler = usbdevice;
	usbdriver.usb_gpio_init();
	usbdriver.usb_core_init();
	usbdriver.device_core_init();
	usbdriver.connect_usb();
}

void polling_FW(void)
{
	usbdriver.polling();
}

static void usb_rst_handler(void)
{
	usbdevice_Handler->in_data_size = 0;
	usbdevice_Handler->out_data_size = 0;
	usbdevice_Handler->configuration_value = 0;
	usbdevice_Handler->ds = USB_DS_DEFAULT;
	usbdevice_Handler->tcs = USB_CS_SETUP;
	usbdriver.setAddress(0);
}

static void process_request(void)
{

}

static void setup_data_received_handler(uint8_t endpoint_no, uint16_t bcnt)
{
	usbdriver.read_Packet(bcnt,usbdevice_Handler->out_buffer);
    process_request();
}

usbevent usbe =
{
		.on_usb_reset = &usb_rst_handler,
		.on_setup_packet_received = &setup_data_received_handler
};
