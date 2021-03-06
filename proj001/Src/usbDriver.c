#include "usbDriver.h"
/*
 * @brief : initializing the gpio pins for usb hs communication ie., pb14 (D+) and pb15 (D-)
 */
static void usb_gpio_init(void)
{
	//enable clock to the gpio pin
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);

	//configure the pb14 and pb15 as alternate mode
	MODIFY_REG(GPIOB->MODER,
			GPIO_MODER_MODER14 | GPIO_MODER_MODER15 ,
			GPIO_MODER_MODER14_1 | GPIO_MODER_MODER15_1);

	//usb pins D+ and D- are in alternate function  mode
	MODIFY_REG(GPIOB->AFR[1],
			GPIO_AFRH_AFSEL14 | GPIO_AFRH_AFSEL15,
			_VAL2FLD(GPIO_AFRH_AFSEL14,0xCUL) | _VAL2FLD(GPIO_AFRH_AFSEL15,0xCUL));

}

/*
 *  @brief : core initialization for usb according to the user manual
 */

static void usb_core_init(void)
{
	//Enable clock to usb otg hs module
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_OTGHSEN);

	//Unmask the Global Interrupt Line assertion to the core.
	MODIFY_REG(USB_OTG_HS_GLOBAL->GAHBCFG,
			USB_OTG_GAHBCFG_GINT,
			_VAL2FLD(USB_OTG_GAHBCFG_GINT,0x1UL));

	//Selecting USB2.0 High Speed and forcing the device to peripheral mode. Also setting the trdt value as specified in the manual
	MODIFY_REG(USB_OTG_HS_GLOBAL->GUSBCFG,
			USB_OTG_GUSBCFG_PHYSEL | USB_OTG_GUSBCFG_TRDT | USB_OTG_GUSBCFG_FDMOD,
			USB_OTG_GUSBCFG_PHYSEL | _VAL2FLD(USB_OTG_GUSBCFG_TRDT,0x9UL) | USB_OTG_GUSBCFG_FDMOD);

	//Unmasking mode mismatch, otg interrupt and rx buffer full mask
	MODIFY_REG(USB_OTG_HS_GLOBAL->GINTMSK,
			USB_OTG_GINTMSK_MMISM | USB_OTG_GINTMSK_OTGINT | USB_OTG_GINTMSK_RXFLVLM,
			USB_OTG_GINTMSK_MMISM | USB_OTG_GINTMSK_OTGINT | USB_OTG_GINTMSK_RXFLVLM);
}

/*
 * @brief : initializing the device core according to the manual
 */

static void device_core_init(void)
{
	//Setting the device to use the internal embedded PHY layer and in Full Speed mode
	MODIFY_REG(USB_OTG_HS_DEVICE->DCFG,
			USB_OTG_DCFG_DSPD,
			_VAL2FLD(USB_OTG_DCFG_DSPD,0x3UL));

	//Clearing all the registers
		WRITE_REG(USB_OTG_HS_GLOBAL->GINTMSK,0xFFFFFFFFUL);

	//Unmasking the following interrupts : USB reset, Enumeration done, Early suspend, USB suspend, SOF
	MODIFY_REG(USB_OTG_HS_GLOBAL->GINTMSK,
			USB_OTG_GINTMSK_SOFM | USB_OTG_GINTMSK_ESUSPM | USB_OTG_GINTMSK_USBSUSPM | USB_OTG_GINTMSK_USBRST | USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_IEPINT | USB_OTG_GINTMSK_OEPINT,
			USB_OTG_GINTMSK_SOFM | USB_OTG_GINTMSK_ESUSPM | USB_OTG_GINTMSK_USBSUSPM | USB_OTG_GINTMSK_USBRST | USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_IEPINT | USB_OTG_GINTMSK_OEPINT);

	//Enabling the VBUS Sensing
	SET_BIT(USB_OTG_HS_GLOBAL->GCCFG,USB_OTG_GCCFG_VBUSASEN);

	//Unmasking transfer and receive completed interrupt mask
	SET_BIT(USB_OTG_HS_DEVICE->DOEPMSK,USB_OTG_DOEPMSK_XFRCM);
	SET_BIT(USB_OTG_HS_DEVICE->DIEPMSK,USB_OTG_DIEPMSK_XFRCM);
}

/*
 * @brief : enabling the trans-receivers and disabling the power down to enable usb communication
 */

static void connect_usb(void)
{
	//Disabling powerdown
	SET_BIT(USB_OTG_HS_GLOBAL->GCCFG,USB_OTG_GCCFG_PWRDWN);

	//Disabling soft disconnect
	CLEAR_BIT(USB_OTG_HS_DEVICE->DCTL,USB_OTG_DCTL_SDIS);
}

/*
 * @brief : disabling the trans-receivers and enabling the power down mode
 */

static void disconnect_usb(void)
{
	//Enabling soft disconnect
	SET_BIT(USB_OTG_HS_DEVICE->DCTL,USB_OTG_DCTL_SDIS);

	//Enabling powerdown
	CLEAR_BIT(USB_OTG_HS_GLOBAL->GCCFG,USB_OTG_GCCFG_PWRDWN);
}

/*
 * @brief : setting the address of the device for reset purpose
 * @param add : the address that you want for the device
 */

static void setAddress(uint8_t add)
{
	MODIFY_REG(USB_OTG_HS_DEVICE->DCFG , USB_OTG_DCFG_DAD , _VAL2FLD(USB_OTG_DCFG_DAD,add));
}

/*
 * @brief : refresh the addresses of all end points after usb reset
 */

static void refresh_fifo_address(void)
{
	//Getting the starting address of tx FIFO by calculating where the RX FIFO ends
	uint32_t startAddress = _FLD2VAL(USB_OTG_GRXFSIZ_RXFD,USB_OTG_HS_GLOBAL->GRXFSIZ) * 4;

	//Setting the start address of TX0 FIFO register
	MODIFY_REG(USB_OTG_HS_GLOBAL->DIEPTXF0_HNPTXFSIZ,USB_OTG_TX0FSA,_VAL2FLD(USB_OTG_TX0FSA,startAddress));

	startAddress +=  _FLD2VAL(USB_OTG_TX0FD,USB_OTG_HS_GLOBAL->DIEPTXF0_HNPTXFSIZ) * 4;

	for (uint8_t i = 0 ; i <= MAXENDPOINT-1 ; i++ )
	{
		MODIFY_REG(USB_OTG_HS_GLOBAL->DIEPTXF[i], USB_OTG_NPTXFSA , _VAL2FLD(USB_OTG_NPTXFSA,startAddress));
		startAddress +=  _FLD2VAL(USB_OTG_NPTXFD,USB_OTG_HS_GLOBAL->DIEPTXF[i]) * 4;
	}
}

/*
 * @brief : allocating space and calculating the ram space required for each rx end points
 * @param : size -> size in bytes of the data packet
 */
static void configure_rxfifo(uint8_t size)
{
	//fifo memory space allocation for rx including ability to receive back to back packets
	size = 10 + ( 2 * ( (size/4) + 1 ) );

	//the ram size required by rx fifo configuration
	MODIFY_REG(USB_OTG_HS_GLOBAL->GRXFSIZ,
			USB_OTG_GRXFSIZ_RXFD ,
			_VAL2FLD(USB_OTG_GRXFSIZ_RXFD,size));
	refresh_fifo_address();
}

/*
 * @brief : allocating space and calculating the ram space required for each tx end points
 * @param : size -> size in bytes of the data packet
 * 			endpoint_no -> the index of the end point to be configured
 */

static void configure_txfifo(uint8_t size, uint8_t endpoint_no)
{
	size = ( size + 3 ) / 4;

	if(endpoint_no == 0)
	{
		MODIFY_REG(USB_OTG_HS_GLOBAL->DIEPTXF0_HNPTXFSIZ, USB_OTG_TX0FD , _VAL2FLD(USB_OTG_TX0FD,size));
	}
	else
	{
		MODIFY_REG(USB_OTG_HS_GLOBAL->DIEPTXF[endpoint_no - 1], USB_OTG_NPTXFD , _VAL2FLD(USB_OTG_NPTXFD,size));
	}
	refresh_fifo_address();
}


/* @brief : Configures the end point 0
 * @parameters : endpointsize - The maximum size (in bytes) of the end point packets which is 64 bytes
 */

static void configure_endpoint0(uint8_t endpoint_size)
{
	//unmask all the interrupts regarding the end-point 0 to interrupt application when an event occurs
	SET_BIT(USB_OTG_HS_DEVICE->DAINTMSK, 1 << 0 | 1 << 16);

	//set nak, configure the maximum packet size and activate the end point
	MODIFY_REG(IEPNT(0)->DIEPCTL,
			USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_SNAK | USB_OTG_DIEPCTL_USBAEP,
			_VAL2FLD(USB_OTG_DIEPCTL_MPSIZ,endpoint_size) | USB_OTG_DIEPCTL_SNAK | USB_OTG_DIEPCTL_USBAEP);

	//clears NAK and enables data end point transmission
	SET_BIT(OEPNT(0)->DOEPCTL,
			USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);

	//configuring the rx and tx0 fifo
	configure_rxfifo(64);    //maximum possible size
	configure_txfifo(endpoint_size, 0);
}

static void configure_in_endpoint(uint8_t endpoint_no , USBEndPointType eptype , uint8_t endpoint_size)
{
	//unmask all the interrupts regarding the end-points to interrupt application when an event occurs
	SET_BIT(USB_OTG_HS_DEVICE->DAINTMSK, 1 << endpoint_no);

	//set nak, configure the maximum packet size and activate the end point and specify the type of the end point
	MODIFY_REG(IEPNT(endpoint_no)->DIEPCTL,
				USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_EPTYP | USB_OTG_DIEPCTL_SNAK | USB_OTG_DIEPCTL_TXFNUM | USB_OTG_DIEPCTL_USBAEP | USB_OTG_DIEPCTL_SD0PID_SEVNFRM,
				_VAL2FLD(USB_OTG_DIEPCTL_MPSIZ,endpoint_size) | _VAL2FLD(USB_OTG_DIEPCTL_MPSIZ,eptype) | USB_OTG_DIEPCTL_SNAK | _VAL2FLD(USB_OTG_DIEPCTL_TXFNUM,endpoint_no) | USB_OTG_DIEPCTL_USBAEP | USB_OTG_DIEPCTL_SD0PID_SEVNFRM);

	//configuring the tx fifo
	configure_txfifo(endpoint_size, endpoint_no);
}

/*
 * @brief : this function reads the packet
 * @param size : the size of the packet that has to be read
 * @param buffer : the buffer where we store the popped data
 */

static void read_Packet(uint16_t size,void* buffer)
{
	//Getting the address of the only rx fifo buffer
	uint32_t* rxfifo = FIFO(0);

	//Loop to extract data from the fifo one word (32 bit or 4 bytes) at a time
	for(; size>=4 ; size-=4 , buffer +=4)
	{
		//popping data from the fifo and storing it in a variable
		uint32_t data = *rxfifo;
		//storing the popped data from the fifo to the buffer to get the data
		*((uint32_t*) buffer) = data;
	}

	//This are executes only if the size of the data is not a multiple of 32 bits or 4 bytes
	if (size > 0)
		{
			//Pops the data one byte at a time
			uint32_t data = *rxfifo;

			//Manipulating the data so we can get the correct order
			for( ; size>0 ; size-- , buffer++ , data >>= 8)
			{
				//Storing the data in the buffer with the correct alignment
				*((uint8_t*)buffer) = 0xFF & data;
			}
		}
}

/*
 * @brief : This function is used to send the data
 * @param size: The size of the packet that has to sent in bytes
 * @param endpoint_no : the end point number from which you have to send the data
 * @param buffer : The data you want to send from your device
 */

static void write_packet(uint16_t size, uint8_t endpoint_no , void const *buffer)
{
	uint32_t* txfifo = FIFO(endpoint_no);
	USB_OTG_INEndpointTypeDef* iepnt = IEPNT(endpoint_no);

	//Configures the transmission
	MODIFY_REG(iepnt->DIEPTSIZ,
			USB_OTG_DIEPTSIZ_PKTCNT | USB_OTG_DIEPTSIZ_XFRSIZ,
			_VAL2FLD(USB_OTG_DIEPTSIZ_PKTCNT,1) | _VAL2FLD(USB_OTG_DIEPTSIZ_XFRSIZ,size));

	//Enabling the transmission after clearing the STALL and NAK of the endpoint
	MODIFY_REG(iepnt->DIEPCTL,
			USB_OTG_DIEPCTL_STALL,
			USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
	//Convert size into word
	size = ( size + 3 ) / 4;

	for(;size > 0 ; size-- ,buffer +=4 )
	{
		//Pushes the data into the buffer
		*txfifo = *((uint32_t*)buffer);
	}
}


/*
 * @brief : flush the rx fifo after use
 */

static void flushRXFIFO(void)
{
	//Flush the rx fifo
	SET_BIT(USB_OTG_HS_GLOBAL->GRSTCTL,USB_OTG_GRSTCTL_RXFFLSH);

}

/*
 * @brief : flush the tx fifo after use
 */

static void flushTXFIFO(uint8_t endpoint_no)
{
	//Flush the corresponding tx fifo wrt to end point number
	MODIFY_REG(USB_OTG_HS_GLOBAL->GRSTCTL,USB_OTG_GRSTCTL_TXFNUM , _VAL2FLD(USB_OTG_GRSTCTL_TXFNUM,endpoint_no) | USB_OTG_GRSTCTL_TXFFLSH);
}


/*
 * @brief : deconfiguring all of the end points after a usb reset
 * @param : endpoint_no : the end point you want to de-configure
 */

static void deconfigure_in_endpoint(uint8_t endpoint_no )
{
	USB_OTG_INEndpointTypeDef* iepnt = IEPNT(endpoint_no);
	USB_OTG_OUTEndpointTypeDef* oepnt = OEPNT(endpoint_no);

	//Mask the interrupts of the targeted end point
	CLEAR_BIT(USB_OTG_HS_DEVICE->DAINTMSK, (1 << endpoint_no) | (1 << 16 << endpoint_no));

	//Clear all the pending interrupts
	SET_BIT(iepnt->DIEPINT,0x29FF);
	SET_BIT(oepnt->DOEPINT,0x715F);

	//Deactivating and disabling in end point
	if(iepnt->DIEPCTL & USB_OTG_DIEPCTL_EPENA)
	{
		SET_BIT(iepnt->DIEPCTL,USB_OTG_DIEPCTL_EPDIS);
	}
	CLEAR_BIT(iepnt->DIEPCTL,USB_OTG_DIEPCTL_USBAEP);

	//Deactivating and disabling out end point barring end point 0
	if(endpoint_no != 0)
	{
	if(oepnt->DOEPCTL & USB_OTG_DOEPCTL_EPENA)
	{
		SET_BIT(oepnt->DOEPCTL,USB_OTG_DOEPCTL_EPDIS);
	}
	CLEAR_BIT(oepnt->DOEPCTL,USB_OTG_DOEPCTL_USBAEP);
	}

	//Flushing the FIFO registers
	flushRXFIFO();
	flushTXFIFO(endpoint_no);
}

/*
 * @brief : the reset handler for usb_rst
 */

static void usb_rst_handler(void)
{
	logInfo("USB reset occurred");
	for(uint8_t i =0; i<=MAXENDPOINT;i++)
	{
	deconfigure_in_endpoint(i);
	}
	usbe.on_usb_reset();
}

/*
 * @brief : the interrupt handler for usb enumeration done
 */

static void enum_done_handler(void)
{
	logInfo("USB eneumeration done");
	configure_endpoint0(8);					//for now we will hardcode the maximum packet size to be 8 bytes
}

/*
 * @brief : the interrupt handler for rx fifo full
 */

static void rxflvl_handler(void)
{
	logInfo("RX FIFO full");

	//reading the value of rx status register and storing it in a variable
	uint32_t receiveStat = USB_OTG_HS_GLOBAL->GRXSTSP;

	//storing from which end point we got the interrupt signal
	uint8_t endpoint_no = _FLD2VAL(USB_OTG_GRXSTSP_EPNUM,receiveStat);

	//storing number of bytes received from the host
	uint16_t bytecnt = _FLD2VAL(USB_OTG_GRXSTSP_BCNT,receiveStat);

	//storing the packet status to monitor the status
	uint8_t pktsts = _FLD2VAL(USB_OTG_GRXSTSP_PKTSTS, receiveStat);

	switch(pktsts)
	{
	case 0x02:								//OUT data packet received
		usbe.on_setup_packet_received(endpoint_no,bytecnt);
		break;
	case 0x06 : 							//SETUP packet received

		break;
	case 0x03 : 							//OUT transfer completed
		SET_BIT(OEPNT(endpoint_no)->DOEPCTL,
					USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);    //re-enabling the end point after the core disables it
		break;
	case 0x04 : 							//SETUP transfer completed
		SET_BIT(OEPNT(endpoint_no)->DOEPCTL,
					USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);	  //re-enabling the end point after the core disables it
		break;
	}
}

/*
 * @brief : the handlers for any usb related interrupts
 */

static void gint_handlers(void)
{
 volatile uint32_t gint_stat = USB_OTG_HS_GLOBAL->GINTSTS;

 if(gint_stat & USB_OTG_GINTSTS_USBRST)				//usb reset handler
 {
	 usb_rst_handler();
	 SET_BIT(USB_OTG_HS_GLOBAL->GINTSTS,USB_OTG_GINTSTS_USBRST);     //clearing the interrupt flag
 }
 else if(gint_stat & USB_OTG_GINTSTS_ENUMDNE)		//enumeration done handler
 {
	 enum_done_handler();
	 SET_BIT(USB_OTG_HS_GLOBAL->GINTSTS,USB_OTG_GINTSTS_ENUMDNE);    //clearing the interrupt flag
 }
 else if(gint_stat & USB_OTG_GINTSTS_RXFLVL)		//receive fifo empty handler
 {
	 rxflvl_handler();
	 SET_BIT(USB_OTG_HS_GLOBAL->GINTSTS,USB_OTG_GINTSTS_RXFLVL);	//clearing the interrupt flag
 }
 else if(gint_stat & USB_OTG_GINTSTS_IEPINT)		//in end point handler
 {


 }
 else if(gint_stat & USB_OTG_GINTSTS_OEPINT)		//out end point handler
 {


 }
}

const UsbDriver usbdriver = {
		.usb_gpio_init = &usb_gpio_init,
			.usb_core_init = &usb_core_init,
			.device_core_init=  &device_core_init,
			.connect_usb=&connect_usb,
			.disconnect_usb=&disconnect_usb,
			.setAddress=&setAddress,
			.configure_in_endpoint=&configure_in_endpoint,
			.read_Packet=&read_Packet,
			.write_packet=&write_packet,
			.flushRXFIFO=&flushRXFIFO,
			.flushTXFIFO=&flushTXFIFO,
			.polling=&gint_handlers
};

/******************************************END OF PROGRAM*********************************************************/
