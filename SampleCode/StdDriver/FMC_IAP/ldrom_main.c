/******************************************************************************
 * @file     LDROM_main.c
 * @version  V1.00
 * $Revision: 3 $
 * $Date: 15/03/19 1:12p $
 * @brief    FMC LDROM IAP sample program for M0519 series MCU
 *
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "M0519.h"

#define PLLCON_SETTING      CLK_PLLCON_72MHz_HXT
#define PLL_CLOCK           72000000

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external XTAL 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));


    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GP3 multi-function pins for UART0 RXD and TXD */
    SYS->P3_MFP = SYS_MFP_P30_UART0_RXD | SYS_MFP_P31_UART0_TXD;

}

void UART_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    UART_Open(UART0, 115200);
}

int main()
{
    void (*func)(void);

    /* Unlock protected register */
    SYS_UnlockReg();

    SYS_Init();
    UART_Init();

    printf("\n\n");
    printf("FMC IAP Sample Code [LDROM code]\n");

    /* Enable FMC ISP function */
    FMC_Open();

    printf("\n\nPress any key to branch to APROM...\n");
    getchar();

    printf("\n\nChange VECMAP and branch to LDROM...\n");
    UART_WAIT_TX_EMPTY(UART0);


    /* Mask all interrupt before changing VECMAP to avoid wrong interrupt handler fetched */
    __set_PRIMASK(1);

    /* Change VECMAP for booting to APROM */
    FMC_SetVectorPageAddr(FMC_APROM_BASE);

    /* Lock protected Register */
    SYS_LockReg();

    /* Jump to APROM by functional pointer */
    func = (void (*)(void)) M32(FMC_APROM_BASE + 4);
    func();

    while(1);
}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/