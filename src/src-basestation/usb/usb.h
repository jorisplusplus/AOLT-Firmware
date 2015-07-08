#ifndef _USB_H_
#define _USB_H_
//DOM-IGNORE-END

#if __XC16_VERSION__ == 1020
#error XC16 v1.20 is not compatible with this firmware, please use a later version of the XC16 compiler.
#endif

#if __XC8_VERSION == 1300
#error XC8 v1.30 is not compatible with this firmware, please use either XC8 v1.21 or a later version of the XC8 compiler.
#endif


// *****************************************************************************
// *****************************************************************************
// Section: All necessary USB Library headers
// *****************************************************************************
// *****************************************************************************

#include <system_config/system.h>
#include <system_config.h>          // Must be defined by the application

#include <usb/usb_common.h>         // Common USB library definitions
#include <usb/usb_ch9.h>            // USB device framework definitions

#if defined( USB_SUPPORT_DEVICE )
    #include <usb/usb_device.h>     // USB Device abstraction layer interface
#endif

#if defined( USB_SUPPORT_HOST )
    #include <usb/usb_host.h>       // USB Host abstraction layer interface
#endif

#if defined ( USB_SUPPORT_OTG )
    #include <usb/usb_otg.h>
#endif

#include <usb/usb_hal.h>            // Hardware Abstraction Layer interface

// *****************************************************************************
// *****************************************************************************
// Section: MCHPFSUSB Firmware Version
// *****************************************************************************
// *****************************************************************************

#define USB_MAJOR_VER   2       // Firmware version, major release number.
#define USB_MINOR_VER   10      // Firmware version, minor release number.
#define USB_DOT_VER     0       // Firmware version, dot release number.

#endif // _USB_H_
/*************************************************************************
 * EOF
 */

