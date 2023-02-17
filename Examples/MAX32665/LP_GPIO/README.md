# LP_GPIO

Enter deep sleep with a GPIO wakeup. 

## Required Connections

-   Connect a USB cable between the PC and the CN2 (USB/PWR) connector.
-   Open an terminal application on the PC and connect to the EV kit's console UART at 115200, 8-N-1.

## Expected Output

The Console UART of the device will output these messages:

MCR->ctrl should be 0xb after a POR, 0xf in subsequent events. Bit 2 is automatically
set by the hardware.

```
****Low Power Mode GPIO Example****                                             
                                                                                
MCR->ctrl = 0xb
```
