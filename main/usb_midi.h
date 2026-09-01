/**
 * @file    usb_midi.h
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

/**
 * Type of MIDI message.
 */
typedef enum
{
	MIDI_NONE,
	MIDI_NOTE_ON,
	MIDI_NOTE_OFF,
	MIDI_AFTER_TOUCH_POLY,
	MIDI_CONTROL_CHANGE,
	MIDI_PROGRAM_CHANGE,
	MIDI_AFTER_TOUCH_CHANNEL,
	MIDI_PITCH_CHANGE,
	MIDI_SYSEX,
	MIDI_SYS_QUARTER,
	MIDI_SYS_SONG_POSITION,
	MIDI_SYS_SONG_SELECT,
	MIDI_SYS_TUNE_REQUEST,
	MIDI_SYS_CLOCK,
	MIDI_SYS_START,
	MIDI_SYS_CONTINUE,
	MIDI_SYS_STOP,
	MIDI_SYS_ACTIVE_SENSE,
	MIDI_SYS_RESET,
	MIDI_SYS_GENERIC
} midi_type_t;

/**
 * Generic MIDI message data handler.
 * The type id for MIDI message type.
 * The data1, data2 and data3 variables contains the received message.
 * The p_data4 is a pointer to the received message in case of MIDI_SYSEX.
 */
typedef struct
{
	midi_type_t type;
	uint32_t data1;
	uint32_t data2;
	uint32_t data3;
	uint8_t * p_data4;
} usb_midi_data_t;


/******************************************************************************
 *                         PUBLIC FUNCTIONS PROTOTYPES                        *
 ******************************************************************************/
void midi_init(uint8_t midi_channel);
uint8_t usb_rx_init();
bool usb_rx_loop(usb_midi_data_t * p_msg);
bool usb_tx_loop(usb_midi_data_t msg);
bool midi_rx_loop(usb_midi_data_t * p_msg);

#endif /* USB_MIDI_H */


/*** End of file ***/
