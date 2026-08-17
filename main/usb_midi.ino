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
 * 
 */

/******************************************************************************
 *                              INCLUDE STATEMENS                             *
 * The headers for each grouping should be sorted alphabetically:             *
 * 1 - The paired header file                                                 *
 * 2 - Other headers from your project                                        *
 * 3 - 3rd party library headers                                              *
 * 4 - Standard library headers                                               *
 ******************************************************************************/

#include "usb_midi.h"
#include <MIDI.h>


/******************************************************************************
 *                        DATA TYPES, CONSTANTS, MACROS                       *
 ******************************************************************************/

/******************************************************************************
 *                              STATIC VARIABLES                              *
 ******************************************************************************/

static usb_midi_data_t g_midi_msg{MIDI_NONE, 0, 0, 0};

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
#ifdef MIDI_SHORT_SYSEX
static void system_exclusive_cb(byte * p_data, unsigned int length);
#else
static void system_exclusive_chunk_cb(const byte * p_data, uint16_t length,
                                      bool last);
#endif /* MIDI_SHORT_SYSEX */
static void time_code_quarter_frame_cb(byte data);
static void song_position_cb(uint16_t beats);
static void song_select_cb(byte song_number);
#if 0
static void tune_request_cb();
#endif
#ifdef MIDI_GENERIC_REALTIME
static void real_time_system_cb(uint8_t real_time_byte);
#else
static void clock_cb();
static void start_cb();
static void continue_cb();
static void stop_cb();
static void active_sensing_cb();
static void system_reset_cb();
#endif /* MIDI_GENERIC_REALTIME */


/******************************************************************************
 *                           PUBLIC FUNCTION BODIES                           *
 ******************************************************************************/

/**
 * @brief   Setup function.
 * @par     Description
 * Setting of the Arduino's pins and the serial port.
 * @return  Nothing.
 */
uint8_t
usb_rx_init (uint32_t baud)
{
    // Setup the MIDI protocol speed.
    //
    Serial.begin(baud);

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
#ifdef MIDI_SHORT_SYSEX
    usbMIDI.setHandleSystemExclusive(system_exclusive_cb);
#else
    usbMIDI.setHandleSystemExclusive(system_exclusive_chunk_cb);
#endif /* MIDI_SHORT_SYSEX */
    usbMIDI.setHandleTimeCodeQuarterFrame(time_code_quarter_frame_cb);
    usbMIDI.setHandleSongPosition(song_position_cb);
    usbMIDI.setHandleSongSelect(song_select_cb);
#if 0
    usbMIDI.setHandleTuneRequest(tune_request_cb);
#endif
#ifdef MIDI_GENERIC_REALTIME
    usbMIDI.setHandleRealTimeSystem(real_time_system_cb);
#else
    usbMIDI.setHandleClock(clock_cb);
    usbMIDI.setHandleStart(start_cb);
    usbMIDI.setHandleContinue(continue_cb);
    usbMIDI.setHandleStop(stop_cb);
    usbMIDI.setHandleActiveSensing(active_sensing_cb);
    usbMIDI.setHandleSystemReset(system_reset_cb);
#endif /* MIDI_GENERIC_REALTIME */

	return (0);
}   /* usb_rx_init() */

/**
 * @brief   Loop function.
 * @par     Description
 * Reading and printing the temperature and the humidity.
 * @return  Nothing.
 */
uint8_t
usb_rx_loop (usb_midi_data_t * p_msg)
{
    p_msg->type = g_midi_msg.type;
    p_msg->data1 = g_midi_msg.data1;
    p_msg->data2 = g_midi_msg.data2;
    p_msg->data3 = g_midi_msg.data3;
    
    return (usbMIDI.read());
}   /* usb_rx_loop() */


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
    g_midi_msg.type = MIDI_NOTE_ON;
    g_midi_msg.data1 = channel;
    g_midi_msg.data2 = note;
    g_midi_msg.data3 = velocity;
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
 * Test: amidi -p hw:1,0,0 -S 'F0 43 10 4C 00 00 7E 00 F7'
 * @return  Nothing.
 */
static void
#ifdef MIDI_SHORT_SYSEX
system_exclusive_cb (byte * p_data, unsigned int length)
#else
system_exclusive_chunk_cb (const byte * p_data, uint16_t length, bool last)
#endif /* MIDI_SHORT_SYSEX */
{
    Serial.print("SysEx Message: ");

    uint8_t idx(0);

    for (; idx < length; ++idx)
    {
        Serial.print(*(p_data + idx), HEX);
        Serial.print(" ");
    }

#ifndef MIDI_SHORT_SYSEX
    if (true == last)
    {
        Serial.println("(end)");
    }
    else
    {
        Serial.println("(to be continued)");
    }
}   /* system_exclusive_chunk_cb() */
#else
    Serial.println("(end)");
}   /* system_exclusive_cb() */
#endif /* MIDI_SHORT_SYSEX */

/**
 * @brief   System Common Time Code Quarter Frame (0xF1) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'F1 74'
 * for 29.97 fps (no timecode) and the values remains in memory as static
 * variable until the fps are changed.
 * amidi -p hw:1,0,0 -S 'F1 20'
 * for timecode 00:00:048.00.
 * @return  Nothing.
 */
static void
time_code_quarter_frame_cb (byte data)
{
    static int8_t smpte_message[8]{0};
    static uint8_t fps(0);
    uint8_t msg_type(data >> 0x4);
    uint8_t values(data & 0xF);

    if (7 == msg_type)
    {
        fps = (values >> 1) & 3;
        values &= 1;
    }

    if ((msg_type < 8) || (values < 10))
    {
        // Adds char 0 (decimal 48) for compensation?
        //
        smpte_message[msg_type] = values + '0';
        Serial.print("TimeCode: " + String(smpte_message[7]) +
                                    String(smpte_message[6]) + ":" +
                                    String(smpte_message[5]) +
                                    String(smpte_message[4]) + ":" +
                                    String(smpte_message[3]) +
                                    String(smpte_message[2]) + "." +
                                    String(smpte_message[1]) + 
                                    String(smpte_message[0]));
        
        switch (fps)
        {
            default:
                /* Fall through */
            case 0:
                Serial.println(" 24 fps");
            break;
            case 1:
                Serial.println(" 25 fps");
            break;
            case 2:
                Serial.println(" 29.97 fps");
            break;
            case 3:
                Serial.println(" 30 fps");
            break;
        }
    }
    else
    {
        Serial.print("TimeCode: invalid data = ");
        Serial.println(data, HEX);
    }
} /* time_code_quarter_frame_cb */

/**
 * @brief   System Common Song Position (0xF2) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'F2 3 0'
 * @return  Nothing.
 */
static void
song_position_cb (uint16_t beats)
{
    Serial.print("Song Position, beat=");
    Serial.println(beats);
} /* song_position_cb */

/**
 * @brief   System Common Song Select (0xF3) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'F3 12'
 * @return  Nothing.
 */
static void
song_select_cb (byte song_number)
{
    Serial.print("Song Select, song=");
    Serial.println(song_number, DEC);
} /* song_select_cb */

#if 0
/**
 * @brief   System Common Tune Request (0xF6) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'F6'
 * Currently received as a SysEx message, hence this function is not needed.
 * @return  Nothing.
 */
static void
tune_request_cb ()
{
    Serial.println("Tune Request");
} /* tune_request_cb */
#endif

#ifndef MIDI_GENERIC_REALTIME
/**
 * @brief   System Real Time Timing Clock (0xF8) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'F8'
 * @return  Nothing.
 */
static void
clock_cb ()
{
    Serial.println("Clock");
} /* clock_cb */

/**
 * @brief   System Real Time Start (0xFA) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'FA'
 * @return  Nothing.
 */
static void
start_cb ()
{
    Serial.println("Start");
} /* start_cb */

/**
 * @brief   System Real Time Continue (0xFB) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'FB'
 * @return  Nothing.
 */
static void
continue_cb ()
{
    Serial.println("Continue");
} /* continue_cb */

/**
 * @brief   System Real Time Stop (0xFC) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'FC'
 * @return  Nothing.
 */
static void
stop_cb ()
{
    Serial.println("Stop");
} /* stop_cb */

/**
 * @brief   System Real Time Active Sense (0xFE) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'FE'
 * @return  Nothing.
 */
static void
active_sensing_cb ()
{
    Serial.println("Active Sensing");
} /* active_sensing_cb */

/**
 * @brief   System Real Time System Reset (0xFF) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'FF'
 * @return  Nothing.
 */
static void
system_reset_cb ()
{
    Serial.println("System Reset");
} /* system_reset_cb */

#else

/**
 * @brief   System Real Time generic handler (0xFx) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'FA'
 * @return  Nothing.
 */
static void
real_time_system_cb (uint8_t real_time_byte)
{
    Serial.print("Real Time Message, code=");
    Serial.println(real_time_byte, HEX);
} /* real_time_system_cb */
#endif /* MIDI_GENERIC_REALTIME */


/*** End of file ***/
