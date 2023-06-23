#include "mxc_device.h"
#include "gpio.h"

int MXC_GPIO_InitAll()
{
	mxc_gpio_cfg_t uart = { MXC_GPIO0, (MXC_GPIO_PIN_10 | MXC_GPIO_PIN_11),
                            MXC_GPIO_FUNC_ALT2, MXC_GPIO_PAD_NONE };
	mxc_gpio_cfg_t pb = { MXC_GPIO0, MXC_GPIO_PIN_12, MXC_GPIO_FUNC_IN,
                          MXC_GPIO_PAD_PULL_UP, MXC_GPIO_VSSEL_VDDIOH };
	mxc_gpio_cfg_t led= { MXC_GPIO0, MXC_GPIO_PIN_13, MXC_GPIO_FUNC_OUT,
                          MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO };

	MXC_GPIO_UnlockConfig();
	MXC_GPIO_Config(&uart); // Console UART
	MXC_GPIO_Config(&pb);	// Push Button
	MXC_GPIO_Config(&led);	// LED
	MXC_GPIO_LockConfig();

	return E_NO_ERROR;
}
