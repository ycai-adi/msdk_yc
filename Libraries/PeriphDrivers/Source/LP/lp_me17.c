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
#include <stddef.h>
#include "mxc_device.h"
#include "mxc_assert.h"
#include "mxc_sys.h"
#include "gcr_regs.h"
#include "mcr_regs.h"
#include "lp.h"
#include "lpcmp.h"
#include "sema.h"

#ifndef __riscv
/* ARM */
#define SET_SLEEPDEEP(X) (SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk)
#define CLR_SLEEPDEEP(X) (SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk)
#else
/* RISCV */
/* These bits do not exist for RISCV core */
#define SET_SLEEPDEEP(X)
#define CLR_SLEEPDEEP(X)
#endif

#define LP_REQ_CODE 0x00051EE4
#define WAITING_FOR_RESPONSE 0xA5A5A5A5

typedef struct {
    mxc_lp_dualcore_rdy_t check_lprdy;
    int sema;
    int initiated;
    int resp;
    int dualcore_config;
} mxc_lp_req_state_t;

int wfi = 0;
static mxc_lp_req_state_t g_lpreq = { NULL, 0, 0, 0, 0 };

void MXC_LP_EnterSleepMode(void)
{
    MXC_LP_ClearWakeStatus();

    /* Clear SLEEPDEEP bit */
    CLR_SLEEPDEEP();

    /* Go into Sleep mode and wait for an interrupt to wake the processor */
    __WFI();
}

void MXC_LP_EnterLowPowerMode(void)
{
    MXC_LP_ClearWakeStatus();
    MXC_MCR->ctrl |= MXC_F_MCR_CTRL_ERTCO_EN; // Enabled for deep sleep mode

    /* Set SLEEPDEEP bit */
    SET_SLEEPDEEP();

    /* Go into low power mode and wait for an interrupt to wake the processor */
    MXC_GCR->pm |= MXC_S_GCR_PM_MODE_LPM;
    __WFI();
}

void MXC_LP_EnterMicroPowerMode(void)
{
    MXC_LP_ClearWakeStatus();
    MXC_MCR->ctrl |= MXC_F_MCR_CTRL_ERTCO_EN; // Enabled for deep sleep mode

    /* Set SLEEPDEEP bit */
    SET_SLEEPDEEP();

    /* Go into Deepsleep mode and wait for an interrupt to wake the processor */
    MXC_GCR->pm |= MXC_S_GCR_PM_MODE_UPM; // UPM mode
    __WFI();
}

void MXC_LP_EnterStandbyMode(void)
{
    MXC_LP_ClearWakeStatus();
    MXC_MCR->ctrl |= MXC_F_MCR_CTRL_ERTCO_EN; // Enabled for deep sleep mode

    /* Set SLEEPDEEP bit */
    SET_SLEEPDEEP();

    /* Go into standby mode and wait for an interrupt to wake the processor */
    MXC_GCR->pm |= MXC_S_GCR_PM_MODE_STANDBY; // standby mode
    __WFI();
}

void MXC_LP_EnterBackupMode(void)
{
    MXC_LP_ClearWakeStatus();

    MXC_GCR->pm &= ~MXC_F_GCR_PM_MODE;
    MXC_GCR->pm |= MXC_S_GCR_PM_MODE_BACKUP;

    while (1) {}
    // Should never reach this line - device will jump to backup vector on exit from background mode.
}

void MXC_LP_EnterPowerDownMode(void)
{
    MXC_GCR->pm &= ~MXC_F_GCR_PM_MODE;
    MXC_GCR->pm |= MXC_S_GCR_PM_MODE_POWERDOWN;

    while (1) {}
    // Should never reach this line - device will reset on exit from shutdown mode.
}

int MXC_LP_ConfigDualCore(mxc_lp_dualcore_rdy_t lprdy, int sema)
{
    // Check if this has already been configured
    if(g_lpreq.dualcore_config) {
        return E_BAD_STATE;
    } else if(sema < 0 || sema > MXC_CFG_SEMA_INSTANCES) {
        return E_INVALID;
    }

    MXC_SEMA_Init();

    wfi = 0;

    g_lpreq.check_lprdy = lprdy;
    g_lpreq.initiated = 0;
    g_lpreq.resp = 0;
    g_lpreq.sema = sema;
    g_lpreq.dualcore_config = 1;

    return E_NO_ERROR;
}

int MXC_LP_DualCore_Request(mxc_lp_modes_t mode)
{
    if(mode < MXC_LP_LPM || mode > MXC_LP_POWERDOWN) {
        return E_BAD_PARAM;
    } else if(!g_lpreq.dualcore_config) {
        return E_BAD_STATE;
    } else if(MXC_SEMA_GetSema(g_lpreq.sema)) {
        return E_BUSY;
    }

    g_lpreq.initiated = 1;
    g_lpreq.resp = WAITING_FOR_RESPONSE;

    // Send Request
    #ifndef __riscv
    MXC_SEMA->mail1 = LP_REQ_CODE;
    MXC_SEMA->irq1 |= MXC_F_SEMA_IRQ1_RV32_IRQ;
    #else
    MXC_SEMA->mail0 = LP_REQ_CODE;
    MXC_SEMA->irq0 |= MXC_F_SEMA_IRQ0_CM4_IRQ;
    #endif

    // Wait for response
    while(g_lpreq.resp == WAITING_FOR_RESPONSE) {}

    if(g_lpreq.resp == E_NO_ERROR) {
        switch(mode) {
        case MXC_LP_LPM:
            MXC_LP_EnterLowPowerMode();
            break;
        case MXC_LP_UPM:
            MXC_LP_EnterMicroPowerMode();
            break;
        case MXC_LP_STANDBY:
            MXC_LP_EnterStandbyMode();
            break;
        case MXC_LP_BACKUP:
            MXC_LP_EnterBackupMode();
            break;
        case MXC_LP_POWERDOWN:
            MXC_LP_EnterPowerDownMode();
            break;
        default:
            return E_BAD_PARAM;
        }
    } else {
        return E_BAD_STATE;
    }

    g_lpreq.initiated = 0;
    g_lpreq.resp = 0;

    MXC_SEMA_FreeSema(g_lpreq.sema);

    // Wakeup the other core
    #ifndef __riscv
    MXC_SEMA->irq1 |= MXC_F_SEMA_IRQ1_RV32_IRQ;
    #else
    MXC_SEMA->irq0 |= MXC_F_SEMA_IRQ0_CM4_IRQ;
    #endif

    return E_NO_ERROR;
}

#include <stdio.h>
int MXC_LP_DualCore_Handler(void)
{
    uint32_t mail;

    // check semaphore
    if(MXC_SEMA_CheckSema(g_lpreq.sema) == E_NO_ERROR) {
        return E_SHUTDOWN;
    }

    // Clear interrupt and read mail
    #ifndef __riscv
    mail = MXC_SEMA->mail0;
    MXC_SEMA->irq0 &= ~MXC_F_SEMA_IRQ0_CM4_IRQ;
    #else
    mail = MXC_SEMA->mail1;
    MXC_SEMA->irq1 &= ~MXC_F_SEMA_IRQ1_RV32_IRQ;
    #endif

    if(g_lpreq.initiated) {
        g_lpreq.resp = mail;
    } else {
        // Handle sleep req
        if(mail == LP_REQ_CODE) {
            // Call LP status checker if it has been initialized
            mail = E_BAD_STATE;

            if(g_lpreq.check_lprdy != NULL) {
                if(g_lpreq.check_lprdy() == E_NO_ERROR) {
                    mail = E_NO_ERROR;
                }
            }

            // Send response to other core
            #ifndef __riscv
            MXC_SEMA->mail1 = mail;
            MXC_SEMA->irq1 |= MXC_F_SEMA_IRQ1_RV32_IRQ;
            #else
            MXC_SEMA->mail0 = mail;
            MXC_SEMA->irq0 |= MXC_F_SEMA_IRQ0_CM4_IRQ;
            #endif

            // If this core approved the LP request, enter __WFI
            if(mail == E_NO_ERROR) {
                wfi = 1;
            }
        }
    }

    return E_NO_ERROR;
}

void MXC_LP_SetOVR(mxc_lp_ovr_t ovr)
{
    //not supported yet
}

void MXC_LP_BandgapOn(void)
{
    MXC_PWRSEQ->lpcn &= ~MXC_F_PWRSEQ_LPCN_BG_DIS;
}

void MXC_LP_BandgapOff(void)
{
    MXC_PWRSEQ->lpcn |= MXC_F_PWRSEQ_LPCN_BG_DIS;
}

int MXC_LP_BandgapIsOn(void)
{
    return (MXC_PWRSEQ->lpcn & MXC_F_PWRSEQ_LPCN_BG_DIS);
}

void MXC_LP_ClearWakeStatus(void)
{
    /* Write 1 to clear */
    MXC_PWRSEQ->lpwkst0 = 0xFFFFFFFF;
    MXC_PWRSEQ->lpwkst1 = 0xFFFFFFFF;
    MXC_PWRSEQ->lpwkst2 = 0xFFFFFFFF;
    MXC_PWRSEQ->lpwkst3 = 0xFFFFFFFF;
    MXC_PWRSEQ->lppwst = 0xFFFFFFFF;
}

void MXC_LP_EnableGPIOWakeup(mxc_gpio_cfg_t *wu_pins)
{
    MXC_GCR->pm |= MXC_F_GCR_PM_GPIO_WE;

    switch (1 << MXC_GPIO_GET_IDX(wu_pins->port)) {
    case MXC_GPIO_PORT_0:
        MXC_PWRSEQ->lpwken0 |= wu_pins->mask;
        break;

    case MXC_GPIO_PORT_1:
        MXC_PWRSEQ->lpwken1 |= wu_pins->mask;
        break;
    case MXC_GPIO_PORT_2:
        MXC_PWRSEQ->lpwken2 |= wu_pins->mask;
        break;
    case MXC_GPIO_PORT_3:
        MXC_PWRSEQ->lpwken3 |= wu_pins->mask;
        break;
    }
}

void MXC_LP_DisableGPIOWakeup(mxc_gpio_cfg_t *wu_pins)
{
    switch (1 << MXC_GPIO_GET_IDX(wu_pins->port)) {
    case MXC_GPIO_PORT_0:
        MXC_PWRSEQ->lpwken0 &= ~wu_pins->mask;
        break;

    case MXC_GPIO_PORT_1:
        MXC_PWRSEQ->lpwken1 &= ~wu_pins->mask;
        break;
    case MXC_GPIO_PORT_2:
        MXC_PWRSEQ->lpwken2 &= ~wu_pins->mask;
        break;
    case MXC_GPIO_PORT_3:
        MXC_PWRSEQ->lpwken3 &= ~wu_pins->mask;
        break;
    }

    if (MXC_PWRSEQ->lpwken3 == 0 && MXC_PWRSEQ->lpwken2 == 0 && MXC_PWRSEQ->lpwken1 == 0 &&
        MXC_PWRSEQ->lpwken0 == 0) {
        MXC_GCR->pm &= ~MXC_F_GCR_PM_GPIO_WE;
    }
}

void MXC_LP_EnableRTCAlarmWakeup(void)
{
    MXC_GCR->pm |= MXC_F_GCR_PM_RTC_WE;
}

void MXC_LP_DisableRTCAlarmWakeup(void)
{
    MXC_GCR->pm &= ~MXC_F_GCR_PM_RTC_WE;
}

void MXC_LP_EnableTimerWakeup(mxc_tmr_regs_t *tmr)
{
    MXC_ASSERT(MXC_TMR_GET_IDX(tmr) > 3);

    if (tmr == MXC_TMR4) {
        MXC_PWRSEQ->lppwen |= MXC_F_PWRSEQ_LPPWEN_TMR4;
    } else {
        MXC_PWRSEQ->lppwen |= MXC_F_PWRSEQ_LPPWEN_TMR5;
    }
}

void MXC_LP_DisableTimerWakeup(mxc_tmr_regs_t *tmr)
{
    MXC_ASSERT(MXC_TMR_GET_IDX(tmr) > 3);

    if (tmr == MXC_TMR4) {
        MXC_PWRSEQ->lppwen &= ~MXC_F_PWRSEQ_LPPWEN_TMR4;
    } else {
        MXC_PWRSEQ->lppwen &= ~MXC_F_PWRSEQ_LPPWEN_TMR5;
    }
}

void MXC_LP_EnableWUTAlarmWakeup(void)
{
    MXC_GCR->pm |= MXC_F_GCR_PM_WUT_WE;
}

void MXC_LP_DisableWUTAlarmWakeup(void)
{
    MXC_GCR->pm &= ~MXC_F_GCR_PM_WUT_WE;
}

void MXC_LP_EnableLPCMPWakeup(mxc_lpcmp_cmpsel_t cmp)
{
    MXC_ASSERT((cmp >= MXC_LPCMP_CMP0) && (cmp <= MXC_LPCMP_CMP3));

    if (cmp == MXC_LPCMP_CMP0) {
        MXC_PWRSEQ->lppwen |= MXC_F_PWRSEQ_LPPWEN_AINCOMP0;
    } else {
        MXC_PWRSEQ->lppwen |= MXC_F_PWRSEQ_LPPWEN_LPCMP;
    }
}

void MXC_LP_DisableLPCMPWakeup(mxc_lpcmp_cmpsel_t cmp)
{
    MXC_ASSERT((cmp >= MXC_LPCMP_CMP0) && (cmp <= MXC_LPCMP_CMP3));

    if (cmp == MXC_LPCMP_CMP0) {
        MXC_PWRSEQ->lppwen &= ~MXC_F_PWRSEQ_LPPWEN_AINCOMP0;
    } else {
        MXC_PWRSEQ->lppwen &= ~MXC_F_PWRSEQ_LPPWEN_LPCMP;
    }
}

int MXC_LP_ConfigDeepSleepClocks(uint32_t mask)
{
    if (!(mask & (MXC_F_GCR_PM_IBRO_PD | MXC_F_GCR_PM_IPO_PD))) {
        return E_BAD_PARAM;
    }

    MXC_GCR->pm |= mask;
    return E_NO_ERROR;
}
