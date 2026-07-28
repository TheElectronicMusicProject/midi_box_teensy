/**
 * @file    usb_rx.h
 * 
 * @brief   Useful material for the source file.
 * 
 * @author  Filippo Graziani (circuitry.passion@gmail.com)
 * 
 * @date    21/07/2026
 * 
 * @par     Description
 * Put here a description.
 */

#ifndef USB_MIDI_H
#   define USB_MIDI_H

/******************************************************************************
 *                              INCLUDE STATEMENS                             *
 * The headers for each grouping should be sorted alphabetically:             *
 * 1 - The paired header file                                                 *
 * 2 - Other headers from your project                                        *
 * 3 - 3rd party library headers                                              *
 * 4 - Standard library headers                                               *
 ******************************************************************************/

#include <stdint.h>


/******************************************************************************
 *                            PROTOTYPES OR MACROS                            *
 ******************************************************************************/

#undef MIDI_SHORT_SYSEX
#define MIDI_GENERIC_REALTIME
 

/******************************************************************************
 *                                   #DEFINE                                  *
 ******************************************************************************/



/******************************************************************************
 *                      TYPEDEFS (STRUCTS, ENUMS, UNIONS)                     *
 ******************************************************************************/



/******************************************************************************
 *                         PUBLIC FUNCTIONS PROTOTYPES                        *
 ******************************************************************************/
uint8_t usb_rx_init(uint32_t baud);
uint8_t usb_rx_loop();

#endif /* USB_MIDI_H */


/*** End of file ***/
