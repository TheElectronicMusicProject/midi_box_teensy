/**
 * @file    main.ino
 *
 * @brief   My MIDI box
 *
 * @author  Filippo Graziani (circuitry.passion@gmail.com)
 *
 * @date    21/07/2026
 *
 * @par     Description
 * Select Teensy 2.0
 * Select Tools-USB type-MIDI
 * test with amidi commands
 */

/******************************************************************************
 *                              INCLUDE STATEMENS                             *
 * The headers for each grouping should be sorted alphabetically:             *
 * 1 - The paired header file                                                 *
 * 2 - Other headers from your project                                        *
 * 3 - 3rd party library headers                                              *
 * 4 - Standard library headers                                               *
 ******************************************************************************/

#include "main.h"
#include <MIDI.h>
#include <stdint.h>


/******************************************************************************
 *                        DATA TYPES, CONSTANTS, MACROS                       *
 ******************************************************************************/

/******************************************************************************
 *                              STATIC VARIABLES                              *
 ******************************************************************************/

/******************************************************************************
 *                        PRIVATE FUNCTIONS PROTOTYPES                        *
 ******************************************************************************/

static void note_on(uint8_t channel, uint8_t note, uint8_t velocity);
static void note_off(uint8_t channel, uint8_t note, uint8_t velocity);
static void after_touch_poly(uint8_t channel, uint8_t note, uint8_t velocity);
static void control_change(uint8_t channel, uint8_t control, uint8_t value);
static void program_change(uint8_t channel, uint8_t program);
static void after_touch_channel(uint8_t channel, uint8_t pressure);
static void pitch_change(uint8_t channel, int pitch);


/******************************************************************************
 *                           PUBLIC FUNCTION BODIES                           *
 ******************************************************************************/

/**
 * @brief   Setup function.
 * @par     Description
 * Setting of the Arduino's pins and the serial port.
 * @return  Nothing.
 */
void
setup ()
{
    // Setup the MIDI protocol speed.
    //
    Serial.begin(31250);

    if (false == Serial)
    {
        /* Firmware waiting for connection... */
    }

    Serial.println("STARTED!");

    // Set the handlers for MIDI.
    //
    usbMIDI.setHandleNoteOn(note_on);
    usbMIDI.setHandleNoteOff(note_off);
    usbMIDI.setHandleAfterTouchPoly(after_touch_poly);
    usbMIDI.setHandleControlChange(control_change);
    usbMIDI.setHandleProgramChange(program_change);
    usbMIDI.setHandleAfterTouchChannel(after_touch_channel);
    usbMIDI.setHandlePitchChange(pitch_change);
}   /* setup() */

/**
 * @brief   Loop function.
 * @par     Description
 * Reading and printing the temperature and the humidity.
 * @return  Nothing.
 */
void
loop ()
{
    usbMIDI.read();
}   /* loop() */


/******************************************************************************
 *                           PRIVATE FUNCTION BODIES                          *
 ******************************************************************************/

static void
note_on (uint8_t channel, uint8_t note, uint8_t velocity)
{
    Serial.print("Note On, ch=");
    Serial.print(channel, DEC);
    Serial.print(", note=");
    Serial.print(note, DEC);
    Serial.print(", velocity=");
    Serial.println(velocity, DEC);
}   /* note_on() */

static void
note_off(uint8_t channel, uint8_t note, uint8_t velocity)
{

}   /* note_off() */

static void
after_touch_poly (uint8_t channel, uint8_t note, uint8_t velocity)
{

}   /* after_touch_poly() */

static void
control_change (uint8_t channel, uint8_t control, uint8_t value)
{

}   /* control_change() */

static void
program_change (uint8_t channel, uint8_t program)
{

}   /* program_change() */

static void
after_touch_channel (uint8_t channel, uint8_t pressure)
{

}   /* after_touch_channel() */

static void
pitch_change (uint8_t channel, int pitch)
{

}   /* pitch_change() */

/*** End of file ***/