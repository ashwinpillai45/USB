/*
 * usbFramework.h
 *
 *  Created on: Nov. 18, 2021
 *      Author: ashwin
 */

#ifndef USBFRAMEWORK_H_
#define USBFRAMEWORK_H_

#include "usbDriver.h"
#include "usbDevice.h"
#include "usbStandards.h"

void usb_init(USBDevice *usbdevice);
void polling_FW(void);

#endif /* USBFRAMEWORK_H_ */
