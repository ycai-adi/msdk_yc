/*******************************************************************************
 * Copyright (C) Maxim Integrated Products, Inc., All Rights Reserved.
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
#include "mxc_device.h"
#include "mxc_sys.h"
#include "mxc_assert.h"
#include "board.h"
#include "uart.h"
#include "gpio.h"
#include "mxc_pins.h"
#include "led.h"
#include "mxc_sys.h"
#include "lpgcr_regs.h"
#include "simo_regs.h"
#include "mxc_delay.h"


/***** Global Variables *****/
mxc_uart_regs_t* ConsoleUart = MXC_UART_GET_UART(CONSOLE_UART);

/* GPIO Information
 *
 * 	camera_1v2_ctrl_pin	:	Enables or disables camera 1.2V power
 * 		0 : Camera 1.2V power is turned off
 * 		1 : Camera 1.2V power is turned on
 *
 *	camera_2v8_ctrl_pin	:	Enables or disables camera 2.8V power
 * 		0 : Camera 2.8V power is turned off
 * 		1 : Camera 2.8V power is turned on
 *
 *	cnn_boost_en_pin	:	Enables or disables external regulator for CNN power
 *		0 : External CNN regulator is disabled
 *		1 : External CNN regulator is enabled
 */

const mxc_gpio_cfg_t camera_1v2_ctrl_pin  = {MXC_GPIO0, MXC_GPIO_PIN_4, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH};
const mxc_gpio_cfg_t camera_2v8_ctrl_pin  = {MXC_GPIO0, MXC_GPIO_PIN_2, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH};
const mxc_gpio_cfg_t cnn_boost_en_pin = {MXC_GPIO2, MXC_GPIO_PIN_5, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH};

const mxc_gpio_cfg_t led_pin[] = {
    {MXC_GPIO2, MXC_GPIO_PIN_0, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH},
    {MXC_GPIO2, MXC_GPIO_PIN_1, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH},
};
const unsigned int num_leds = (sizeof(led_pin) / sizeof(mxc_gpio_cfg_t));


/******************************************************************************/
static void Camera_Static_Config(void)
{
    const mxc_gpio_cfg_t camera_xsleep_pin    = {MXC_GPIO0, MXC_GPIO_PIN_3,  MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO};
    const mxc_gpio_cfg_t camera_xshutdown_pin = {MXC_GPIO0, MXC_GPIO_PIN_6,  MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO};
    const mxc_gpio_cfg_t camera_fae_pin       = {MXC_GPIO0, MXC_GPIO_PIN_10, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO};
    const mxc_gpio_cfg_t camera_clk_sel_pin   = {MXC_GPIO0, MXC_GPIO_PIN_11, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO};
    const mxc_gpio_cfg_t camera_trigger_pin   = {MXC_GPIO0, MXC_GPIO_PIN_8,  MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO};
    const mxc_gpio_cfg_t camera_ctx_sel_pin   = {MXC_GPIO0, MXC_GPIO_PIN_12, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO};

    MXC_GPIO_Config(&camera_xsleep_pin);
    MXC_GPIO_OutSet(camera_xsleep_pin.port, camera_xsleep_pin.mask);

    MXC_GPIO_Config(&camera_xshutdown_pin);
    MXC_GPIO_OutSet(camera_xshutdown_pin.port, camera_xshutdown_pin.mask);

    MXC_GPIO_Config(&camera_fae_pin);
    MXC_GPIO_OutClr(camera_fae_pin.port, camera_fae_pin.mask);

    MXC_GPIO_Config(&camera_clk_sel_pin);
    MXC_GPIO_OutSet(camera_clk_sel_pin.port, camera_clk_sel_pin.mask);

    MXC_GPIO_Config(&camera_trigger_pin);
    MXC_GPIO_OutClr(camera_trigger_pin.port, camera_trigger_pin.mask);

    MXC_GPIO_Config(&camera_ctx_sel_pin);
    MXC_GPIO_OutClr(camera_ctx_sel_pin.port, camera_ctx_sel_pin.mask);
}

/******************************************************************************/
void mxc_assert(const char* expr, const char* file, int line)
{
    printf("MXC_ASSERT %s #%d: (%s)\n", file, line, expr);

    while (1);
}

/******************************************************************************/
int Board_Init(void)
{
#ifndef __riscv
    int err;

    // Enable GPIO
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_GPIO0);
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_GPIO1);
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_GPIO2);

    if ((err = Console_Init()) < E_NO_ERROR) {
        return err;
    }

    if ((err = LED_Init()) != E_NO_ERROR) {
        MXC_ASSERT_FAIL();
        return err;
    }

    // Initialize CNN boost control GPIO
    if ((err = MXC_GPIO_Config(&cnn_boost_en_pin)) != E_NO_ERROR) {
        MXC_ASSERT_FAIL();
        return err;
    }

    // Initialize camera power control GPIO
    if (((err = MXC_GPIO_Config(&camera_1v2_ctrl_pin)) != E_NO_ERROR) ||
        ((err = MXC_GPIO_Config(&camera_2v8_ctrl_pin)) != E_NO_ERROR)) {
        MXC_ASSERT_FAIL();
        return err;
    }

    // Initialize static camera module i/o pins
    Camera_Static_Config();

    MXC_SIMO->vrego_c = 0x43; // Set CNN voltage

#endif // __riscv

    return E_NO_ERROR;
}

/******************************************************************************/
int Console_Init(void)
{
    int err;

    if ((err = MXC_UART_Init(ConsoleUart, CONSOLE_BAUD, MXC_UART_IBRO_CLK)) != E_NO_ERROR) {
        return err;
    }

    return E_NO_ERROR;
}

/******************************************************************************/
int Console_Shutdown(void)
{
    int err;

    if ((err = MXC_UART_Shutdown(ConsoleUart)) != E_NO_ERROR) {
        return err;
    }

    return E_NO_ERROR;
}

/******************************************************************************/
void NMI_Handler(void)
{
    __NOP();
}

#ifdef __riscv
/******************************************************************************/
int Debug_Init(void)
{
    // Set up RISCV JTAG pins (P1[0..3] AF2)
    MXC_GPIO1->en0_clr = 0x0f;
    MXC_GPIO1->en1_set = 0x0f;
    MXC_GPIO1->en2_clr = 0x0f;

    return E_NO_ERROR;
}
#endif // __riscv

/******************************************************************************/
int Camera_Power(int on)
{
	if (on) {
    	MXC_GPIO_OutSet(camera_1v2_ctrl_pin.port, camera_1v2_ctrl_pin.mask);
    	MXC_GPIO_OutSet(camera_2v8_ctrl_pin.port, camera_2v8_ctrl_pin.mask);
    } else {
    	MXC_GPIO_OutClr(camera_2v8_ctrl_pin.port, camera_2v8_ctrl_pin.mask);
    	MXC_GPIO_OutClr(camera_1v2_ctrl_pin.port, camera_1v2_ctrl_pin.mask);
    }

    return E_NO_ERROR;
}

/******************************************************************************/
int CNN_Boost_Enable(int enable)
{
	if (enable)
    	MXC_GPIO_OutSet(cnn_boost_en_pin.port, cnn_boost_en_pin.mask);
    else
    	MXC_GPIO_OutClr(cnn_boost_en_pin.port, cnn_boost_en_pin.mask);

    return E_NO_ERROR;
}
