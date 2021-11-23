/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Ashwin Pillai
 * @brief          : Main program body (Application Layer)
 ******************************************************************************

 *
 ******************************************************************************
 */

#include <stdint.h>
#include "main.h"
#include "Helper/logger.h"
#include "usbFramework.h"
#include "usbDevice.h"

USBDevice usbdevice;
uint32_t buffer[8];
int main(void)
{
	logInfo("Program Entry Point");
	usbdevice.out_buffer = &buffer;
    usb_init(&usbdevice);
    logInfo("Program finished. Entering loop");
    /* Loop forever */
	for(;;)
	{
		polling_FW();

	}
}
