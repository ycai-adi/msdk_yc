/**
 * @file    lp.h
 * @brief   Low Power(LP) function prototypes and data types.
 */

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

/* Define to prevent redundant inclusion */
#ifndef LIBRARIES_PERIPHDRIVERS_INCLUDE_MAX32655_LP_H_
#define LIBRARIES_PERIPHDRIVERS_INCLUDE_MAX32655_LP_H_

/* **** Includes **** */
#include <stdint.h>
#include "pwrseq_regs.h"
#include "mcr_regs.h"
#include "gcr_regs.h"
#include "lpcmp.h"
#include "gpio.h"
#include "tmr.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int wfi;

/**
 * @defgroup pwrseq Low Power (LP)
 * @ingroup periphlibs
 * @{
 */

/**
 * @brief   Enumeration type for voltage selection
 *
 */
typedef enum { MXC_LP_V0_9 = 0, MXC_LP_V1_0, MXC_LP_V1_1 } mxc_lp_ovr_t;

/**
 * @brief   Enumeration type for PM Mode
 *
 */
typedef enum {
    MXC_LP_IPO = MXC_F_GCR_PM_IPO_PD,
    MXC_LP_IBRO = MXC_F_GCR_PM_IBRO_PD,
} mxc_lp_cfg_ds_pd_t;

/**
 * @brief   Enumeration type to select which low power mode to enter if low power
 *          request approved by the other core.
 */
typedef enum {
    MXC_LP_LPM, ///< Select low power mode
    MXC_LP_UPM, ///< Select micro-power mode
    MXC_LP_STANDBY, ///< Select standby mode
    MXC_LP_BACKUP, ///< Select backup mode
    MXC_LP_POWERDOWN /// < Select power down mode
} mxc_lp_modes_t;

/**
 * @brief   The callback used by the MXC_LP_DualCore_Handler() function.
 *
 * This callback routine is used by the MXC_LP_DualCore_Handler() to determine whether
 * the core is in a state in which it can enter a low power operating mode. The user should
 * define a function which approves or denies any requests to enter a low power mode from
 * the other core. Pass this function pointer to MXC_LP_ConfigDualCore() before any LP
 * requests are issued by the other core. 
 *
 * @return  E_NO_ERROR if the core is currently able to enter a low power mode, otherwise E_BAD_STATE. 
 */
typedef int (*mxc_lp_dualcore_rdy_t)(void);

/**
 * @brief      Places the device into SLEEP mode.  This function returns once an RTC or external interrupt occur.
 */
void MXC_LP_EnterSleepMode(void);

/**
 * @brief      Places the device into Low Power mode.  This function returns once an RTC or external interrupt occur.
 */
void MXC_LP_EnterLowPowerMode(void);

/**
 * @brief      Places the device into Micro Power mode.  This function returns once an RTC or external interrupt occur.
 */
void MXC_LP_EnterMicroPowerMode(void);

/**
 * @brief      Places the device into Standby mode.  This function returns once an RTC or external interrupt occur.
 */
void MXC_LP_EnterStandbyMode(void);

/**
 * @brief      Places the device into BACKUP mode.  CPU state is not maintained in this mode, so this function never returns.
 *             Instead, the device will restart once an RTC or external interrupt occur.
 */
void MXC_LP_EnterBackupMode(void);

/**
 * @brief      Places the device into Shutdown mode.  CPU state is not maintained in this mode, so this function never returns.
 *             Instead, the device will restart once an RTC, USB wakeup, or external interrupt occur.
 */
void MXC_LP_EnterPowerDownMode(void);

/*
 * @brief      Function to configure core to be able to send and receive low power requests in applications which use both the
 *             ARM and RISC-V cores. Call this function in both applications (Arm and RISC-V) before sending Low power requests
 *             from either core.
 *
 * @param      lprdy    Function pointer to the user-defined Low-Power status check function
 * @param      sema     The semaphore assigned to all LP requests. (Must be the same selection on for both the Arm and RISC-V initializations.)
 *
 * @return     E_NO_ERROR if sucessful, otherwise an error code.
 */
int MXC_LP_ConfigDualCore(mxc_lp_dualcore_rdy_t lprdy, int sema);

/*
 * @brief      Send request to other core to enter low power mode.
 *
 * @warning    If request approved by the other core, the device will enter the selected low power mode.
 *
 * @param      mode  Selects which low power mode to enter if 
 *
 * @return     E_NO_ERROR if request approved, an error code otherwise.
 */
int MXC_LP_DualCore_Request(mxc_lp_modes_t mode);

/*
 * @brief      Function to handle low power requests. Call from CM4/RISC-V ISR. This function determines if the interrupt
 *             was triggered by a low power request, if it determines it was not triggered by a low power request it returns
 *             an error. If this occurs, call MXC_SEMA_Handler to handle the interrupt or clear it manually. 
 *
 * @return     E_NO_ERROR if the interrupt was handled in this function, otherwise an error code.
 */
int MXC_LP_DualCore_Handler(void);

/**
 * @brief      Set ovr bits to set the voltage the micro will run at.
 *
 * @param[in]  ovr   The ovr options are only 0.9V, 1.0V, and 1.1V use enum mxc_lp_ovr_t
 */
void MXC_LP_SetOVR(mxc_lp_ovr_t ovr);

/**
 * @brief      Turn bandgap on
 */
void MXC_LP_BandgapOn(void);

/**
 * @brief      Turn bandgap off
 */
void MXC_LP_BandgapOff(void);

/**
 * @brief      Is the bandgap on or off
 *
 * @return     1 = bandgap on , 0 = bandgap off
 */
int MXC_LP_BandgapIsOn(void);

/**
 * @brief      clear all wake up status
 */
void MXC_LP_ClearWakeStatus(void);

/**
 * @brief      Enables the selected GPIO port and its selected pins to wake up the device from any low power mode.
 *             Call this function multiple times to enable pins on multiple ports.  This function does not configure
 *             the GPIO pins nor does it setup their interrupt functionality.
 * @param      wu_pins      The port and pins to configure as wakeup sources.  Only the gpio and mask fields of the
 *                          structure are used.  The func and pad fields are ignored.
 */

void MXC_LP_EnableGPIOWakeup(mxc_gpio_cfg_t *wu_pins);

/**
 * @brief      Disables the selected GPIO port and its selected pins as a wake up source.
 *             Call this function multiple times to disable pins on multiple ports.
 * @param      wu_pins      The port and pins to disable as wakeup sources.  Only the gpio and mask fields of the
 *                          structure are used.  The func and pad fields are ignored.
 */
void MXC_LP_DisableGPIOWakeup(mxc_gpio_cfg_t *wu_pins);

/**
 * @brief      Enables the RTC alarm to wake up the device from any low power mode.
 */
void MXC_LP_EnableRTCAlarmWakeup(void);

/**
 * @brief      Disables the RTC alarm from waking up the device.
 */
void MXC_LP_DisableRTCAlarmWakeup(void);

/**
 * @brief      Enables Timer to wakeup from any low power mode. 
 * 
 * @param      tmr  Pointer to timer module.
 */
void MXC_LP_EnableTimerWakeup(mxc_tmr_regs_t *tmr);

/**
 * @brief      Disables Timer from waking up device.  
 * 
 * @param      tmr  Pointer to timer module.
 */
void MXC_LP_DisableTimerWakeup(mxc_tmr_regs_t *tmr);

/**
 * @brief      Enables the USB to wake up the device from any low power mode.
 */
void MXC_LP_EnableUSBWakeup(void);

/**
 * @brief      Disables the USB from waking up the device.
 */
void MXC_LP_DisableUSBWakeup(void);

/**
 * @brief      Enables the WUT alarm to wake up the device from any low power mode.
 */
void MXC_LP_EnableWUTAlarmWakeup(void);

/**
 * @brief      Disables the WUT alarm from waking up the device.
 */
void MXC_LP_DisableWUTAlarmWakeup(void);

/**
 * @brief      Enables the LPCMP to wake up the device from any low power mode.
 */
void MXC_LP_EnableLPCMPWakeup(mxc_lpcmp_cmpsel_t cmp);

/**
 * @brief      Disables the LPCMP from waking up the device.
 */
void MXC_LP_DisableLPCMPWakeup(mxc_lpcmp_cmpsel_t cmp);

/**
 * @brief      Enables the HA0 to wake up the device from any low power mode.
 */
void MXC_LP_EnableHA0Wakeup(void);

/**
 * @brief      Disables the HA)0 from waking up the device.
 */
void MXC_LP_DisableHA0Wakeup(void);
/**
 * @brief      Enables the HA1 to wake up the device from any low power mode.
 */
void MXC_LP_EnableHA1Wakeup(void);

/**
 * @brief      Disables the HA1 from waking up the device.
 */
void MXC_LP_DisableHA1Wakeup(void);

/**
 * @brief      Configure which clocks are powered down at deep sleep and which are not affected.
 *
 * @note       Need to configure all clocks at once any clock not passed in the mask will be unaffected by Deepsleep.  This will
 *             always overwrite the previous settings of ALL clocks.
 *
 * @param[in]  mask  The mask of the clocks to power down when part goes into deepsleep
 *
 * @return     #E_NO_ERROR or error based on /ref MXC_Error_Codes
 */
int MXC_LP_ConfigDeepSleepClocks(uint32_t mask);

/**
 * @brief      Enable NFC Oscilator Bypass
 */
void MXC_LP_NFCOscBypassEnable(void);

/**
 * @brief      Disable NFC Oscilator Bypass
 */
void MXC_LP_NFCOscBypassDisable(void);

/**
 * @brief      Is NFC Oscilator Bypass Enabled
 *
 * @return     1 = enabled, 0 = disabled
 */
int MXC_LP_NFCOscBypassIsEnabled(void);

/**@} end of group pwrseq */

#ifdef __cplusplus
}
#endif

#endif // LIBRARIES_PERIPHDRIVERS_INCLUDE_MAX32655_LP_H_
