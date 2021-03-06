

#ifndef USBDRIVER_H_
#define USBDRIVER_H_

#include "stm32f429xx.h"
#include "stm32f4xx.h"
#include "usbStandards.h"
#include "Helper/logger.h"


#define USB_OTG_HS_GLOBAL		((USB_OTG_GlobalTypeDef *)(USB_OTG_HS_PERIPH_BASE + USB_OTG_GLOBAL_BASE)) //Address of usb global registers
#define USB_OTG_HS_DEVICE		((USB_OTG_DeviceTypeDef *)(USB_OTG_HS_PERIPH_BASE + USB_OTG_DEVICE_BASE)) //Address of usb device register
#define USB_OTG_HS_PCGCCTL      ((uint32_t *)(USB_OTG_HS_PERIPH_BASE + USB_OTG_PCGCCTL_BASE)) //Address of power and clock gating control register
#define MAXENDPOINT 			6                   //Maximum number of endpoints possible in STM32F429ZI

/*
 * @brief : returns the address of the register DIEPCTL(x) where x is the in-endpoint number
 * @return : returns the address of the control register of individual end point
 * @parameters : endpoint_number - The in-endpoint number of whose address you want
 */
inline static USB_OTG_INEndpointTypeDef* IEPNT (uint8_t endpoint_number)
{
 return ((USB_OTG_INEndpointTypeDef*) ( USB_OTG_IN_ENDPOINT_BASE + ( 0x20 * endpoint_number )));
}

/*
 * @brief : returns the address of the register DIEPCTL(x) where x is the out-endpoint number
 * @return : returns the address of the control register of individual endpoint
 * @parameters : endpoint_number - The out-endpoint number of whose address you want
 */
inline static USB_OTG_OUTEndpointTypeDef* OEPNT (uint8_t endpoint_number)
{
 return ((USB_OTG_OUTEndpointTypeDef*) ( USB_OTG_OUT_ENDPOINT_BASE + ( 0x20 * endpoint_number )));
}
/*
 * @brief : returns the address of the FIFO registers in order to access them
 * @param : the end point number whose address has to be calculated
 * @retval : the address of the end point mentioned in the argument
 */
inline static __IO uint32_t * FIFO(uint8_t endpoint_no)
{
	return ((__IO uint32_t *)(USB_OTG_HS_PERIPH_BASE + USB_OTG_FIFO_BASE + (endpoint_no * 1000UL)));
}

typedef struct {
	void (*usb_gpio_init)(void);
	void (*usb_core_init)(void);
	void (*device_core_init)(void);
	void (*connect_usb)(void);
	void (*disconnect_usb)(void);
	void (*setAddress)(uint8_t add);
	void (*configure_in_endpoint)(uint8_t endpoint_no , USBEndPointType eptype , uint8_t endpoint_size);
	void (*read_Packet)(uint16_t size,void* buffer);
	void (*write_packet)(uint16_t size, uint8_t endpoint_no , void const *buffer);
	void (*flushRXFIFO)(void);
	void (*flushTXFIFO)(uint8_t endpoint_no);
	void (*polling)(void);
}UsbDriver;

extern const UsbDriver usbdriver;
extern usbevent usbe;

#endif /* USBDRIVER_H_ */
