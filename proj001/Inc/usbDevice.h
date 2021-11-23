/*
 * usbDevice.h
 *
 *  Created on: Nov. 23, 2021
 *      Author: ashwin
 */

#ifndef USBDEVICE_H_
#define USBDEVICE_H_

#include "usbStandards.h"

typedef struct {
	usbdevicestate ds;
	USBTransferControlStage tcs;
	uint8_t configuration_value;
	void const *out_buffer;
	uint32_t out_data_size;
	void const *in_buffer;
	uint32_t in_data_size;
}USBDevice;

#endif /* USBDEVICE_H_ */
