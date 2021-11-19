

#ifndef USBDRIVER_H_
#define USBDRIVER_H_

#include "stm32f429xx.h"
#include "stm32f4xx.h"


#define USB_OTG_HS_GLOBAL		((USB_OTG_GlobalTypeDef *)(USB_OTG_HS_PERIPH_BASE + USB_OTG_GLOBAL_BASE)) //Address of usb global registers
#define USB_OTG_HS_DEVICE		((USB_OTG_DeviceTypeDef *)(USB_OTG_HS_PERIPH_BASE + USB_OTG_DEVICE_BASE)) //Address of usb device register
#define USB_OTG_HS_PCGCCTL      ((uint32_t *)(USB_OTG_HS_PERIPH_BASE + USB_OTG_PCGCCTL_BASE)) //Address of power and clock gating control register

void usb_gpio_init(void);
void usb_core_init(void);
void device_core_init(void);
void connect_usb(void);
void disconnect_usb(void);

#endif /* USBDRIVER_H_ */
