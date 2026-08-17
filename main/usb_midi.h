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

typedef enum
{
	MIDI_NONE,
	MIDI_NOTE_ON,
	MIDI_NOTE_OFF,
} midi_type_t;

typedef struct
{
	midi_type_t type;
	uint8_t data1;
	uint8_t data2;
	uint8_t data3;
} usb_midi_data_t;


/******************************************************************************
 *                         PUBLIC FUNCTIONS PROTOTYPES                        *
 ******************************************************************************/
uint8_t usb_rx_init(uint32_t baud);
uint8_t usb_rx_loop(usb_midi_data_t * p_msg);

#endif /* USB_MIDI_H */


/*** End of file ***/
