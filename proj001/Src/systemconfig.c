/*
 * systemconfig.c
 *
 *  Created on: Nov 16, 2021
 *      Author: ashwin pillai
 */

#include "main.h"

/*
 * @param : void
 * @return : void
 * @description : In this func we will be using hse as clock source for pll to generate
 *                the below mentioned values. The idea is to generate 48MHz of clock
 *                for comms.
 *                PLLM = 4 PLLN = 72 PLLP = 2 PLLQ = 3 AHBP = 1 APB1 = 2 APB2 = 1
 *
*/
static void clock_config(void)
{

}

void SystemInit(void)
{
clock_config();
}
