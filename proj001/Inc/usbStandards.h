

#ifndef USBSTANDARDS_H_
#define USBSTANDARDS_H_

#include "stm32f429xx.h"

typedef enum {
		USB_EPTYPE_CONTROL,
		USB_EPTYPE_ISOCHRONOUS,
		USB_EPTYPE_BULK,
		USB_EPTYPE_INTERRUPT
}USBEndPointType;

typedef struct{
	void (*on_usb_reset)(void);
	void (*on_setup_packet_received)(uint8_t endpoint_no,uint16_t bcnt);
	void (*on_out_data_received)(uint8_t endpoint_no,uint16_t bcnt);
	void (*on_in_transfer_complete)(uint8_t endpoint_no);
	void (*on_out_transfer_complete)(uint8_t endpoint_no);
	void (*on_usb_polled)();
}usbevent;

typedef enum {
	USB_DS_DEFAULT,
	USB_DS_ADDRESS,
	USB_DS_CONFIGURED,
	USB_DS_SUSPENDED
}usbdevicestate;

typedef enum  {
	USB_CS_SETUP,
	USB_CS_DATA_OUT,
	USB_CS_DATA_IN,
	USB_CS_DATA_IN_IDLE,
	USB_CS_DATA_IN_ZERO,
	USB_CS_STATUS_OUT,
	USB_CS_STATUS_IN
}USBTransferControlStage;

typedef struct
{
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
}USBRequest;

#define USB_BM_REQUEST_TYPE_DIRECTION_MASK (1 << 7)
#define USB_BM_REQUEST_TYPE_DIRECTION_TO_HOST (1 << 7)
#define USB_BM_REQUEST_TYPE_DIRECTION_TO_DEVICE (0 << 7)

#define USB_BM_REQUEST_TYPE_TYPE_MASK (3 << 5)
#define USB_BM_REQUEST_TYPE_TYPE_STANDARD (0 << 5)
#define USB_BM_REQUEST_TYPE_TYPE_CLASS (1 << 5)
#define USB_BM_REQUEST_TYPE_TYPE_VENDOR (2 << 5)

#define USB_BM_REQUEST_TYPE_RECEPIENT_MASK (3 << 0)
#define USB_BM_REQUEST_TYPE_RECEPIENT_DEVICE (0 << 0)
#define USB_BM_REQUEST_TYPE_RECEPIENT_INTERFACE (1 << 0)
#define USB_BM_REQUEST_TYPE_RECEPIENT_ENDPOINT (2 << 0)
#define USB_BM_REQUEST_TYPE_RECEPIENT_OTHER (3 << 0)


#endif /* USBSTANDARDS_H_ */
