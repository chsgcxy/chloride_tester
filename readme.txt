# notes:

## about directory source-code

    this is where opensource code and lib in the project from.

	- the emwin and free-rtos are from en.stm32cubef2 
	- the usb lib and fatfs is from stm32_f105-07_f2_f4_usb-host-device_lib
	- the stm32f207 lib is from stm32f2xx_stdperiph_lib

    First I choose stdperiph lib for I am more familiar with it.
    
    The usb lib in en.stm32cubef2 is based on hal lib, but I choosed The
    stdperiph lib, so I must used the usb lib in stm32_f105-07_f2_f4_usb-host-device_lib, 
    and it is easier to porting.

    The fatfs in from stm32_f105-07_f2_f4_usb-host-device_lib also.

    It seems that the freertos does not depend on the lib type, user can use
    std lib or cube hal lib, so I use the em.stm32cubef2 version.




