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

/*
 * @file    main.c
 * @brief   Demonstrates the various low power modes.
 *
 * @details Iterates through the various low power modes, using either the RTC
 *          alarm or a GPIO to wake from each.  #defines determine which wakeup
 *          source to use.  Once the code is running, you can measure the
 *          current used on the VCORE rail.
 *
 *          The power states shown are:
 *            1. Active mode power with all clocks on
 *            2. Active mode power with peripheral clocks disabled
 *            3. Active mode power with unused RAMs in light sleep mode
 *            4. Active mode power with unused RAMS shut down
 *            5. SLEEP mode
 *            6. BACKGROUND mode
 *            7. DEEPSLEEP mode
 *            8. BACKUP mode
 */

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
#include "mxc_sys.h"

/* Shadow register definitions */
#define MXC_R_SIR_SHR17 *((uint32_t *)(0x40005444))

#define DELAY_IN_MSEC 20
#define USE_CONSOLE 1

#define USE_BUTTON 0
#define USE_ALARM 0
#define USE_GPIO 1

#define DO_SLEEP 0
#define DO_DEEPSLEEP 1
#define DO_BACKUP 0

#if (!(USE_BUTTON || USE_ALARM || USE_GPIO))
#error "You must set either USE_BUTTON or USE_ALARM or USE_GPIO to 1."
#endif
#if (USE_BUTTON + USE_ALARM + USE_GPIO > 1 )
#error "You must select one of USE_BUTTON or USE_ALARM or USE_GPIO, not two or three."
#endif

#if USE_CONSOLE
#define PRINT(...) fprintf(stdout, __VA_ARGS__)
#else
#define PRINT(...)
#endif

// *****************************************************************************

void ECC_IRQHandler(void)
{
    PRINT("ECC Error\n");
    while(1) {}
}

// __attribute__((interrupt))
// void GPIOWAKE_IRQHandler(void)
// {
//     NVIC_DisableIRQ(GPIOWAKE_IRQn);
//     MXC_LP_ClearWakeStatus();
// }



#if USE_ALARM

volatile int alarmed;
void alarmHandler(void)
{
    int flags = MXC_RTC->ctrl;
    alarmed = 1;

    if ((flags & MXC_F_RTC_CTRL_ALSF) >> MXC_F_RTC_CTRL_ALSF_POS) {
        MXC_RTC->ctrl &= ~(MXC_F_RTC_CTRL_ALSF);
}

    if ((flags & MXC_F_RTC_CTRL_ALDF) >> MXC_F_RTC_CTRL_ALDF_POS) {
        MXC_RTC->ctrl &= ~(MXC_F_RTC_CTRL_ALDF);
    }
}

void setTrigger(int waitForTrigger)
{
    alarmed = 0;
    while (MXC_RTC_Init(0, 0) == E_BUSY) {}

    while (MXC_RTC_DisableInt(MXC_F_RTC_CTRL_ASE) == E_BUSY) {}

    while (MXC_RTC_SetSubsecondAlarm(0xFFFFFFFFUL - (DELAY_IN_MSEC*4096/1000)) == E_BUSY) {}

    while (MXC_RTC_EnableInt(MXC_F_RTC_CTRL_ASE) == E_BUSY) {}

    while (MXC_RTC_Start() == E_BUSY) {}

    if (waitForTrigger) {
        while (!alarmed) {}
    }

// Wait for serial transactions to complete.
#if USE_CONSOLE
    while (MXC_UART_ReadyForSleep(MXC_UART_GET_UART(CONSOLE_UART)) != E_NO_ERROR) {}
#endif // USE_CONSOLE
}

void waitSeconds(int sec)
{
    alarmed = 0;
    while (MXC_RTC_Init(0, 0) == E_BUSY) {}
    while (MXC_RTC_DisableInt(MXC_F_RTC_CTRL_ADE) == E_BUSY) {}
    while (MXC_RTC_SetTimeofdayAlarm(sec) == E_BUSY) {}
    while (MXC_RTC_EnableInt(MXC_F_RTC_CTRL_ADE) == E_BUSY) {}
    while (MXC_RTC_Start() == E_BUSY) {}
    while (!alarmed) {}
}

#endif // USE_ALARM

#if USE_BUTTON
volatile int buttonPressed = 0;
void buttonHandler(void *pb)
{
    buttonPressed = 1;
}

void setTrigger(int waitForTrigger)
{
    int tmp;

    buttonPressed = 0;
    if (waitForTrigger) {
        while (!buttonPressed) {}
    }

    // Debounce the button press.
    for (tmp = 0; tmp < 0x800000; tmp++) {
        __NOP();
    }

// Wait for serial transactions to complete.
#if USE_CONSOLE
    while (MXC_UART_ReadyForSleep(MXC_UART_GET_UART(CONSOLE_UART)) != E_NO_ERROR) {}
#endif // USE_CONSOLE
}
#endif // USE_BUTTON

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

void switchToHIRCD8(void)
{
    MXC_SETFIELD(MXC_GCR->clkcn, MXC_F_GCR_CLKCN_PSC, MXC_S_GCR_CLKCN_PSC_DIV8);
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

	*(volatile int *)0x40005434 = 1;
	*(volatile int *)0x40005440 = (*(volatile int *)0x40005440 & (~(0x3 << 24))) | (0x2 << 24);
	*(volatile int *)0x40005444 = (*(volatile int *)0x40005444 & (~(0x3 << 6)))  | (0x2 << 6);

    switchToHIRCD4();
    // switchToHIRCD8();

    ///// Hiep suggested waiting for VCB ready here
    /* Wait for VCOREB to be ready */
    while (!(MXC_SIMO->buck_out_ready & MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYB)) {}

    // MXC_SIMO_SetVregO_B(810); /* Reduce VCOREB to 0.81v -- this will stuck in brownout */
    MXC_SIMO_SetVregO_B(900); /* this fixes brown out issue */ 

    MXC_MCR->ctrl = (MXC_MCR->ctrl & ~(MXC_F_MCR_CTRL_VDDCSW)) | 
                    (0x2 << MXC_F_MCR_CTRL_VDDCSW_POS);

    /* Wait for VCOREA ready.  Should be ready already */
    while (!(MXC_SIMO->buck_out_ready & MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYC)) {}
}

void recoverFromDeepSleep(void)
{
#if !USE_GPIO
	uint32_t tmp = MXC_MCR->ctrl;
#endif
    /* Check to see if VCOREA is ready on  */
    if (!(MXC_SIMO->buck_out_ready & MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYC)) {
        /* Wait for VCOREB to be ready */
        while (!(MXC_SIMO->buck_out_ready & MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYB)) {}

        /* Move VCORE switch back to VCOREB */
        MXC_MCR->ctrl = (MXC_MCR->ctrl & ~(MXC_F_MCR_CTRL_VDDCSW)) |
                        (0x1 << MXC_F_MCR_CTRL_VDDCSW_POS);

        /* Raise the VCORE_B voltage */
        while (!(MXC_SIMO->buck_out_ready & MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYB)) {}
        MXC_SIMO_SetVregO_B(1000);
        while (!(MXC_SIMO->buck_out_ready & MXC_F_SIMO_BUCK_OUT_READY_BUCKOUTRDYB)) {}
    } else {

    MXC_MCR->ctrl = (MXC_MCR->ctrl & ~(MXC_F_MCR_CTRL_VDDCSW)) | 
                    (0x3 << MXC_F_MCR_CTRL_VDDCSW_POS);

    }


    MXC_LP_ICache0PowerUp();
    MXC_ICC_Enable();

    switchToHIRC96();

#if !USE_GPIO
	PRINT("VDDCSW on exit: %08X\n", tmp);
#endif
}

int main(void)
{

#if USE_GPIO & 0
    // Clear all ECC Errors -- write-1-to-clear
    MXC_GCR->ecc_er = (volatile uint32_t)MXC_GCR->ecc_er;
    MXC_GCR->ecc_ced = (volatile uint32_t)MXC_GCR->ecc_ced;

    // Enable interrupts for ECC errors
    MXC_GCR->ecc_irqen |= MXC_F_GCR_ECC_IRQEN_SYSRAM0ECCEN | MXC_F_GCR_ECC_IRQEN_SYSRAM1ECCEN |
                          MXC_F_GCR_ECC_IRQEN_SYSRAM2ECCEN | MXC_F_GCR_ECC_IRQEN_SYSRAM3ECCEN |
                          MXC_F_GCR_ECC_IRQEN_SYSRAM4ECCEN | MXC_F_GCR_ECC_IRQEN_SYSRAM5ECCEN;
    NVIC_EnableIRQ(ECC_IRQn);

    /* Enable ECC */
    MXC_MCR->eccen |= 0x3f;
#endif

    PRINT("\n\n****Low Power Mode Example****\n\n");

#if USE_ALARM
    PRINT("This code cycles through the MAX32665 power modes, using the RTC alarm to exit from "
          "each mode.  The modes will change every %d milliseconds.\n\n",
          DELAY_IN_MSEC);
    MXC_NVIC_SetVector(RTC_IRQn, alarmHandler);
#endif // USE_ALARM

#if USE_BUTTON
    PRINT("This code cycles through the MAX32665 power modes, using a push button 0 to exit from "
          "each mode and enter the next.\n\n");
    PB_RegisterCallback(0, (pb_callback)buttonHandler);
#endif // USE_BUTTON

    PRINT("Running in ACTIVE mode.\n");
#if !USE_CONSOLE
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_UART0);
#endif // !USE_CONSOLE


    MXC_Delay(MXC_DELAY_SEC(5));    /* Delay before starting to prevent bricks */


    /* Prevent SIMO leakage in DS by reducing the SIMO buck clock */
    // MXC_R_SIR_SHR17 &= ~(0xC0);

/*
    MXC_LP_USBSWLPDisable();

    MXC_LP_ROM0LightSleepEnable();
    MXC_LP_USBFIFOLightSleepEnable();
    MXC_LP_CryptoLightSleepEnable();
    MXC_LP_SRCCLightSleepEnable();
    MXC_LP_ICacheXIPLightSleepEnable();
    MXC_LP_ICache1LightSleepEnable();
    MXC_LP_SysRam5LightSleepEnable();
    MXC_LP_SysRam4LightSleepEnable();
    MXC_LP_SysRam3LightSleepEnable();
    MXC_LP_SysRam2LightSleepEnable();
    MXC_LP_SysRam1LightSleepDisable();
    MXC_LP_SysRam0LightSleepDisable(); // Global variables are in RAM0 and RAM1

    PRINT("All unused RAMs placed in LIGHT SLEEP mode.\n");
    setTrigger(1);

    MXC_LP_ROM0Shutdown();
    MXC_LP_USBFIFOShutdown();
    MXC_LP_CryptoShutdown();
    MXC_LP_SRCCShutdown();
    MXC_LP_ICacheXIPShutdown();
    MXC_LP_ICache1Shutdown();
    MXC_LP_SysRam5Shutdown();
    MXC_LP_SysRam4Shutdown();
    MXC_LP_SysRam3Shutdown();
    MXC_LP_SysRam2Shutdown();
    MXC_LP_SysRam1PowerUp();
    MXC_LP_SysRam0PowerUp(); // Global variables are in RAM0 and RAM1

    PRINT("All unused RAMs shutdown.\n");

    setTrigger(1);
*/


#if USE_BUTTON
    MXC_LP_EnableGPIOWakeup((mxc_gpio_cfg_t *)&pb_pin[0]);
    MXC_GPIO_SetWakeEn(pb_pin[0].port, pb_pin[0].mask);
#endif // USE_BUTTON
#if USE_ALARM
	waitSeconds(5);
    MXC_LP_EnableRTCAlarmWakeup();
#endif // USE_ALARM
#if USE_GPIO

    /* Enable GPIO wakeup  (GPIOn_IRQHandler)*/
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

    // /* Enable GPIO wakeup  (GPIOWAKE_IRQHandler)*/
    // NVIC_ClearPendingIRQ(GPIOWAKE_IRQn);

    // MXC_LP_ClearWakeStatus();
    // // MXC_GCR->pm |= MXC_F_GCR_PM_GPIO_WE;
    // MXC_LP_EnableGPIOWakeup((mxc_gpio_cfg_t *)&pb_pin[0]);
    // MXC_LP_EnableGPIOWakeup((mxc_gpio_cfg_t *)&pb_pin[1]);
    // NVIC_EnableIRQ(GPIOWAKE_IRQn);

#endif

    // *(volatile int *)0x40005434 = 1;
    // *(volatile int *)0x40005440 = (*(volatile int *)0x40005440 & (~(0x3 << 24))) | (0x2 << 24);
    // *(volatile int *)0x40005444 = (*(volatile int *)0x40005444 & (~(0x3 << 6)))  | (0x2 << 6);


    while (1) {
#if DO_SLEEP
        PRINT("Entering SLEEP mode.\n");
        setTrigger(0);
        MXC_LP_EnterSleepMode();
        PRINT("Waking up from SLEEP mode.\n");

#endif // DO_SLEEP
#if DO_DEEPSLEEP
  #if USE_GPIO
        MXC_Delay(MXC_DELAY_USEC(100));
        prepForDeepSleep();
        LED_Off(0); // GPIO signal is high when asleep
        MXC_LP_EnterDeepSleepMode();
        LED_On(0); // GPIO pin is low when awake
        recoverFromDeepSleep();
  #else
        PRINT("Entering DEEPSLEEP mode.\n");
        LED_Off(0);
        setTrigger(0);
        prepForDeepSleep();
        MXC_LP_EnterDeepSleepMode();
        recoverFromDeepSleep();
        PRINT("Waking up from DEEPSLEEP mode.\n");
        LED_On(0);
        setTrigger(1);
  #endif
#endif // DO_DEEPSLEEP

#if DO_BACKUP
  #if USE_GPIO
        PRINT("Entering BACKUP mode.\n");
        prepForDeepSleep();
        MXC_LP_EnterBackupMode(NULL);
        recoverFromDeepSleep();
  #else
        PRINT("Entering BACKUP mode.\n");
        setTrigger(0);
        prepForDeepSleep();
        MXC_LP_EnterBackupMode(NULL);
        recoverFromDeepSleep();
  #endif
#endif // DO_BACKUP
    }
}

void HardFault_Handler(void)
{
    while (1) {
        LED_On(0);
        MXC_Delay(MXC_DELAY_USEC(100));
        LED_Off(0);
        MXC_Delay(MXC_DELAY_USEC(200));
    }

}
