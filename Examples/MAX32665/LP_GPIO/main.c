/******************************************************************************
 * Copyright (C) 2023 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include "mxc_device.h"
#include "mxc_errors.h"
#include "nvic_table.h"
#include "pb.h"
#include "led.h"
#include "lp.h"
#include "icc.h"
#include "rtc.h"
#include "uart.h"
#include "simo.h"
#include "rtc.h"

/* Shadow register definitions */
#define MXC_R_SIR_SHR17 *((uint32_t *)(0x40005444))

#define RUN_VOLTAGE     1000

#define DS_VOLTAGE      850

void ECC_IRQHandler(void)
{
    printf("ECC Error\n");
    while(1) {}
}

/*
 *  Switch the system clock to the HIRC / 4
 *
 *  Enable the HIRC, set the divide ration to /4, and disable the 96 MHz oscillator.
 */
void switchToHIRCD4(void)
{
    MXC_SETFIELD(MXC_GCR->clkcn, MXC_F_GCR_CLKCN_PSC, MXC_S_GCR_CLKCN_PSC_DIV4);
    MXC_GCR->clkcn |= MXC_F_GCR_CLKCN_HIRC_EN;
    MXC_SETFIELD(MXC_GCR->clkcn, MXC_F_GCR_CLKCN_CLKSEL, MXC_S_GCR_CLKCN_CLKSEL_HIRC);
    /* Disable unused clocks */
    while (!(MXC_GCR->clkcn & MXC_F_GCR_CLKCN_CKRDY)) {}
    /* Wait for the switch to occur */
    MXC_GCR->clkcn &= ~(MXC_F_GCR_CLKCN_HIRC96M_EN);
    SystemCoreClockUpdate();
}

/*
 *  Switch the system clock to the HIRC96
 *
 *  Enable the HIRC, set the divide ration to /1, and disable the 60 MHz oscillator.
 */
void switchToHIRC96(void)
{
    MXC_SETFIELD(MXC_GCR->clkcn, MXC_F_GCR_CLKCN_PSC, MXC_S_GCR_CLKCN_PSC_DIV1);
    MXC_GCR->clkcn |= MXC_F_GCR_CLKCN_HIRC96M_EN;
    MXC_SETFIELD(MXC_GCR->clkcn, MXC_F_GCR_CLKCN_CLKSEL, MXC_S_GCR_CLKCN_CLKSEL_HIRC96);
    /* Disable unused clocks */
    while (!(MXC_GCR->clkcn & MXC_F_GCR_CLKCN_CKRDY)) {}
    /* Wait for the switch to occur */
    MXC_GCR->clkcn &= ~(MXC_F_GCR_CLKCN_HIRC_EN);
    SystemCoreClockUpdate();
}

void prepForDeepSleep(void)
{
    MXC_ICC_Disable();
    MXC_LP_ICache0Shutdown();

    /* Shutdown unused power domains */
    MXC_PWRSEQ->lpcn |= MXC_F_PWRSEQ_LPCN_BGOFF;

    /* Prevent SIMO soft start on wakeup */
    MXC_LP_FastWakeupDisable();

    /* Enable VDDCSWEN=1 prior to enter backup/deepsleep mode */
    MXC_MCR->ctrl |= MXC_F_MCR_CTRL_VDDCSWEN;

    // Retain all SRAM
    MXC_PWRSEQ->lpcn |= MXC_S_PWRSEQ_LPCN_RAMRET_EN3;

    static int mcr_ctrl = 0;
    if(!mcr_ctrl) {
        printf("MCR->ctrl = 0x%x\n", MXC_MCR->ctrl); // 0xB after POR, 0xF after reset
        MXC_Delay(MXC_DELAY_MSEC(10));
        mcr_ctrl = 1;
    }

    switchToHIRCD4();

    MXC_SIMO_SetVregO_B(DS_VOLTAGE); /* Reduce VCOREB to 0.81v */
}

void recoverFromDeepSleep(void)
{
#if 1
    /* Check to see if VCOREA is ready on  */
    if (!(MXC_SIMO->buck_out_ready & MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYC)) {
        LED_On(1);
        /* Wait for VCOREB to be ready */
        while (!(MXC_SIMO->buck_out_ready & MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYB)) {}

        /* Move VCORE switch back to VCOREB */
        MXC_MCR->ctrl = (MXC_MCR->ctrl & ~(MXC_F_MCR_CTRL_VDDCSW)) |
                        (0x1 << MXC_F_MCR_CTRL_VDDCSW_POS);

        /* Raise the VCORE_B voltage */
        while (!(MXC_SIMO->buck_out_ready & MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYB)) {}
        MXC_SIMO_SetVregO_B(RUN_VOLTAGE);
        while (!(MXC_SIMO->buck_out_ready & MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYB)) {}
        LED_Off(1);
    }
#else
    /* Raise the VCORE_B voltage */
    MXC_SIMO_SetVregO_B(RUN_VOLTAGE);

    // Wait for VREGO_B ready to prepare to switch VCORE back to VCOREB
    while (0u == (MXC_SIMO->buck_out_ready & MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYB)) {}

    // Move VCORE switch back to VCOREB.
    MXC_MCR->ctrl |= 0x00000002u;
    MXC_MCR->ctrl &= 0xFFFFFFFBu;

    MXC_MCR->ctrl |= (0x1u << MXC_F_MCR_CTRL_VDDCSWEN_POS) | (0x1u << MXC_F_MCR_CTRL_USBSWEN_N_POS); // 50 uA average current

    // Crash
    // MXC_MCR->ctrl |= (0x1u << MXC_F_MCR_CTRL_VDDCSWEN_POS);
    // MXC_MCR->ctrl &= ~MXC_F_MCR_CTRL_USBSWEN_N;

    // Now that we've switched back to VCOREB, make sure the voltage regulators are ready before we continue.
    while ((MXC_SIMO->buck_out_ready & (MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYA | MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYB | MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYC)) !=
                                       (MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYA | MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYB | MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYC))
    {
    }
#endif

    MXC_LP_ICache0PowerUp();
    MXC_ICC_Enable();

    switchToHIRC96();
}

int main(void)
{
    printf("\n\n****Low Power Mode GPIO Example****\n\n");

    /* Delay before starting to prevent bricks */
    MXC_Delay(MXC_DELAY_SEC(3));

    /* Set VREGO_C (VCOREA) at the run voltage */
    MXC_SIMO_SetVregO_C(RUN_VOLTAGE);

    /* Prevent SIMO leakage in DS by reducing the SIMO buck clock */
    MXC_R_SIR_SHR17 &= ~(0xC0);

    /* Disable USB switch */
    MXC_LP_USBSWLPDisable(); // ~7 uA in DS when switch is disabled
    // MXC_LP_USBSWLPEnable(); // ~420 uA in DS when switch is enabled

    /* Enable GPIO wakeup */
    MXC_GPIO_IntConfig((mxc_gpio_cfg_t *)&pb_pin[0], MXC_GPIO_INT_BOTH);
    MXC_GPIO_EnableInt(pb_pin[0].port, pb_pin[0].mask);

    NVIC_ClearPendingIRQ(MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(pb_pin[0].port)));
    NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(pb_pin[0].port)));
    MXC_LP_EnableGPIOWakeup((mxc_gpio_cfg_t *)&pb_pin[0]);

    /* Enable GPIO wakeup */
    MXC_GPIO_IntConfig((mxc_gpio_cfg_t *)&pb_pin[1], MXC_GPIO_INT_BOTH);
    MXC_GPIO_EnableInt(pb_pin[1].port, pb_pin[1].mask);

    NVIC_ClearPendingIRQ(MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(pb_pin[1].port)));
    NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(pb_pin[1].port)));
    MXC_LP_EnableGPIOWakeup((mxc_gpio_cfg_t *)&pb_pin[1]);

    /* Setup RTC */
    if (MXC_RTC_Init(0, 0) != E_NO_ERROR) {
        printf("Failed RTC Initialization\n");
        printf("Example Failed\n");
        while (1) {}
    }

    if (MXC_RTC_Start() != E_NO_ERROR) {
        printf("Failed RTC_Start\n");
        printf("Example Failed\n");
        while (1) {}
    }

    // Clear all ECC Errors -- write-1-to-clear
    MXC_GCR->ecc_er = (volatile uint32_t)MXC_GCR->ecc_er;
    MXC_GCR->ecc_ced = (volatile uint32_t)MXC_GCR->ecc_ced;

    // Enable interrupts for ECC errors
    MXC_GCR->ecc_irqen |= MXC_F_GCR_ECC_IRQEN_SYSRAM0ECCEN | MXC_F_GCR_ECC_IRQEN_SYSRAM1ECCEN |
                          MXC_F_GCR_ECC_IRQEN_SYSRAM2ECCEN | MXC_F_GCR_ECC_IRQEN_SYSRAM3ECCEN |
                          MXC_F_GCR_ECC_IRQEN_SYSRAM4ECCEN | MXC_F_GCR_ECC_IRQEN_SYSRAM5ECCEN;
    NVIC_EnableIRQ(ECC_IRQn);

    LED_Off(1);

    while (1) {

        // printf("Entering DEEPSLEEP mode.\n");
        LED_On(0); // GPIO pin is low when awake
        MXC_Delay(MXC_DELAY_USEC(100));
    
        prepForDeepSleep();

        LED_Off(0); // GPIO signal is high when asleep
        MXC_LP_EnterDeepSleepMode();

        recoverFromDeepSleep();
    }
}
