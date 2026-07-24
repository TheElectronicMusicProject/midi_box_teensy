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
#define SHORT_SYSEX

/******************************************************************************
 *                              STATIC VARIABLES                              *
 ******************************************************************************/

/******************************************************************************
 *                        PRIVATE FUNCTIONS PROTOTYPES                        *
 ******************************************************************************/

static void note_on_cb(uint8_t channel, uint8_t note, uint8_t velocity);
static void note_off_cb(uint8_t channel, uint8_t note, uint8_t velocity);
static void after_touch_poly_cb(uint8_t channel, uint8_t note,
                                uint8_t velocity);
static void control_change_cb(uint8_t channel, uint8_t control, uint8_t value);
static void program_change_cb(uint8_t channel, uint8_t program);
static void after_touch_channel_cb(uint8_t channel, uint8_t pressure);
static void pitch_change_cb(uint8_t channel, int pitch);
#ifdef SHORT_SYSEX
static void system_exclusive_cb(byte * p_data, unsigned int length);
#else
static void system_exclusive_chunk_cb(const byte * p_data, uint16_t length,
                                      bool last);
#endif /* SHORT_SYSEX */


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
    usbMIDI.setHandleNoteOn(note_on_cb);
    usbMIDI.setHandleNoteOff(note_off_cb);
    usbMIDI.setHandleAfterTouchPoly(after_touch_poly_cb);
    usbMIDI.setHandleControlChange(control_change_cb);
    usbMIDI.setHandleProgramChange(program_change_cb);
    usbMIDI.setHandleAfterTouchChannel(after_touch_channel_cb);
    usbMIDI.setHandlePitchChange(pitch_change_cb);
#ifdef SHORT_SYSEX
    usbMIDI.setHandleSystemExclusive(system_exclusive_cb);
#else
    usbMIDI.setHandleSystemExclusive(system_exclusive_chunk_cb);
#endif /* SHORT_SYSEX */
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

/**
 * @brief   Note On (0xA) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S '91 1 1'
 * @return  Nothing.
 */
static void
note_on_cb (uint8_t channel, uint8_t note, uint8_t velocity)
{
    Serial.print("Note On, ch=");
    Serial.print(channel, DEC);
    Serial.print(", note=");
    Serial.print(note, DEC);
    Serial.print(", velocity=");
    Serial.println(velocity, DEC);
}   /* note_on_cb() */

/**
 * @brief   Note Off (0x8) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S '80 1 1'
 * @return  Nothing.
 */
static void
note_off_cb (uint8_t channel, uint8_t note, uint8_t velocity)
{
    Serial.print("Note Off, ch=");
    Serial.print(channel, DEC);
    Serial.print(", note=");
    Serial.print(note, DEC);
    Serial.print(", velocity=");
    Serial.println(velocity, DEC);
}   /* note_off_cb() */

/**
 * @brief   Polyphonic pressure (0xA) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'A0 1 1'
 * @return  Nothing.
 */
static void
after_touch_poly_cb (uint8_t channel, uint8_t note, uint8_t velocity)
{
    Serial.print("Polyphonic Pressure, ch=");
    Serial.print(channel, DEC);
    Serial.print(", note=");
    Serial.print(note, DEC);
    Serial.print(", velocity=");
    Serial.println(velocity, DEC);
}   /* after_touch_poly_cb() */

/**
 * @brief   Control change (0xB) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'B0 1 1'
 * @return  Nothing.
 */
static void
control_change_cb (uint8_t channel, uint8_t control, uint8_t value)
{
    Serial.print("Control Change, ch=");
    Serial.print(channel, DEC);
    Serial.print(", control=");
    Serial.print(control, DEC);
    Serial.print(", value=");
    Serial.println(value, DEC);
}   /* control_change_cb() */

/**
 * @brief   Program change (0xC) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'C0 1'
 * @return  Nothing.
 */
static void
program_change_cb (uint8_t channel, uint8_t program)
{
    Serial.print("Program Change, ch=");
    Serial.print(channel, DEC);
    Serial.print(", program=");
    Serial.println(program, DEC);
}   /* program_change_cb() */

/**
 * @brief   Channel Pressure (0xD) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'D0 1'
 * @return  Nothing.
 */
static void
after_touch_channel_cb (uint8_t channel, uint8_t pressure)
{
    Serial.print("Channel Pressure, ch=");
    Serial.print(channel, DEC);
    Serial.print(", pressure=");
    Serial.println(pressure, DEC);
}   /* after_touch_channel_cb() */

/**
 * @brief   Pitch Bend (0xE) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'E0 0 0'
 * for minimum pitch (-8192).
 * amidi -p hw:1,0,0 -S 'E0 0 40'
 * for middle pitch (0).
 * amidi -p hw:1,0,0 -S 'E0 7F 7F'
 * for maximum pitch (8191).
 * @return  Nothing.
 */
static void
pitch_change_cb (uint8_t channel, int pitch)
{
    Serial.print("Pitch Bend, ch=");
    Serial.print(channel, DEC);
    Serial.print(", pitch=");
    Serial.println(pitch, DEC);
}   /* pitch_change_cb() */

/**
 * @brief   System (0xF) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'F0 F7'
 * @return  Nothing.
 */
static void
#ifdef SHORT_SYSEX
system_exclusive_cb (byte * p_data, unsigned int length)
#else
system_exclusive_chunk_cb (const byte * p_data, uint16_t length, bool last)
#endif /* SHORT_SYSEX */
{
    Serial.print("SysEx Message: ");
    Serial.print(*p_data, HEX);

#ifndef SHORT_SYSEX
    if (true == last)
    {
        Serial.println(" (end)");
    }
    else
    {
        Serial.println(" (to be continued)");
    }
}   /* system_exclusive_chunk_cb() */
#else
    Serial.println(" (end)");
}   /* system_exclusive_cb() */
#endif /* SHORT_SYSEX */

static void
time_code_quarter_frame_cb (byte data)
{
    // TBD
} /* time_code_quarter_frame_cb */


static void
song_position_cb (uint16_t beats)
{
    Serial.print("Song Position, beat=");
    Serial.println(beats);
} /* song_position_cb */

static void
song_select_cb (byte songNumber)
{
    Serial.print("Song Select, song=");
    Serial.println(songNumber, DEC);
} /* song_select_cb */

static void
tune_request_cb ()
{
    Serial.println("Tune Request");
} /* tune_request_cb */

static void
clock_cb ()
{
    Serial.println("Clock");
} /* clock_cb */

static void
start_cb ()
{
    Serial.println("Start");
} /* start_cb */

static void
continue_cb ()
{
    Serial.println("Continue");
} /* continue_cb */

static void
stop_cb ()
{
  Serial.println("Stop");
} /* stop_cb */

static void
active_sensing_cb ()
{
    Serial.println("Actvice Sensing");
} /* active_sensing_cb */

static void
system_reset_cb ()
{
    Serial.println("System Reset");
} /* system_reset_cb */

static void
real_time_system_cb (uint8_t realtimebyte)
{
    Serial.print("Real Time Message, code=");
    Serial.println(realtimebyte, HEX);
} /* real_time_system_cb */

/*** End of file ***/
