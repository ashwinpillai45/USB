/*
 * systemconfig.c
 *
 *  Created on: Nov 16, 2021
 *      Author: ashwin pillai
 */

#include "main.h"
#include "stm32f4xx.h"
#include "stm32f429xx.h"
#include "Helper/logger.h"
/*
 * @param : void
 * @return : void
 * @description : In this func we will be using hse as clock source for pll to generate
 *                the below mentioned values. The idea is to generate 48MHz of clock
 *                for comms.
 *                PLLM = 4 PLLN = 72 PLLP = 2 PLLQ = 3 AHBP = 1 APB1 = 2 APB2 = 1
 *
 */
static void clock_config(void) {
	//Configuring the flash latency for HCLK = 72MHz
	MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY,
			_VAL2FLD(FLASH_ACR_LATENCY,FLASH_ACR_LATENCY_2WS));
	//Enabling HSE
	SET_BIT(RCC->CR, RCC_CR_HSEON);
	//Wait till HSE is stable
	while(!(READ_BIT(RCC->CR,RCC_CR_HSERDY)));
	//Configuring PLL
	MODIFY_REG(RCC->PLLCFGR,
			RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLP | RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLQ,
			_VAL2FLD(RCC_PLLCFGR_PLLM,4) | _VAL2FLD(RCC_PLLCFGR_PLLN,72) | _VAL2FLD(RCC_PLLCFGR_PLLP,0) | RCC_PLLCFGR_PLLSRC_HSE | RCC_PLLCFGR_PLLQ_3);

	//Activate PLL and wait till it is generated.
	SET_BIT(RCC->CR,RCC_CR_PLLON);
	while(!(READ_BIT(RCC->CR,RCC_CR_PLLRDY)));
	//Configuring BUS peripheral clocks
	MODIFY_REG(RCC->CFGR,
				RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2,
				RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PPRE2_DIV1);

	//Changing Sysclock to pll
	MODIFY_REG(RCC->CFGR,
					(RCC_CFGR_SW),
					RCC_CFGR_SW_PLL);

	//Wait till PLL is selected as clock source for sysclock
		while(!(READ_BIT(RCC->CFGR,RCC_CFGR_SWS_PLL)));
	//HSI OFF
	CLEAR_BIT(RCC->CR, RCC_CR_HSION);
}

static void clock_check(void)
{
	//Enable Clock to GPIOC
	SET_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOCEN);

	//Configuring the parameters for GPIO C pin 9
	//Parameters : Alternate function : AF0, Speed : Low
	SET_BIT(GPIOC->MODER,GPIO_MODER_MODER9_1);     // GPIOC.9 is configured as alternate function
	MODIFY_REG(GPIOC->OSPEEDR , GPIO_OSPEEDR_OSPEED9 , _VAL2FLD(GPIO_OSPEEDR_OSPEED9,0)); // Speed is set to low
	MODIFY_REG(GPIOC->AFR[1],GPIO_AFRH_AFSEL9,_VAL2FLD(GPIO_AFRH_AFSEL9,0)); // AF0 is selected

	//Enable MCO registers
	MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2,_VAL2FLD(RCC_CFGR_MCO2,0x3));
	//Prescaler value of 2
	MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2PRE,RCC_CFGR_MCO2PRE_2);
}



void SystemInit(void) {
	//logInfo("Entered System Configuration");
	clock_check();
	clock_config();

}
