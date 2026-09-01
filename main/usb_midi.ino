/**
 * @file    usb_midi.ino
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

// Serial1 is RX on 7 and TX on 8.
//
static MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, gh_midi);
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
static void midi_note_on_cb(byte channel, byte note, byte velocity);


/******************************************************************************
 *                           PUBLIC FUNCTION BODIES                           *
 ******************************************************************************/

/**
 * @brief   MIDI init function.
 * @par     Description
 * Setup for the serial MIDI interface with handlers.
 * @param[in] midi_channel  Selected MIDI channel (MIDI_CHANNEL_OMNI for
 *                          broadcast).
 * @return  Nothing.
 */
void
midi_init (uint8_t midi_channel)
{
    gh_midi.setHandleNoteOn(midi_note_on_cb);
#if 0
    gh_midi.setHandleNoteOff(midi_note_on_cb);
    gh_midi.setHandleAfterTouchPoly(midi_note_on_cb);
    gh_midi.setHandleControlChange(midi_note_on_cb);
    gh_midi.setHandleProgramChange(midi_note_on_cb);
    gh_midi.setHandleAfterTouchChannel(midi_note_on_cb);
    gh_midi.setHandlePitchBend(midi_note_on_cb);
    gh_midi.setHandleSystemExclusive(midi_note_on_cb);
    gh_midi.setHandleTimeCodeQuarterFrame(midi_note_on_cb);
    gh_midi.setHandleSongPosition(midi_note_on_cb);
    gh_midi.setHandleSongSelect(midi_note_on_cb);
    gh_midi.setHandleTuneRequest(midi_note_on_cb);
    gh_midi.setHandleClock(midi_note_on_cb);
    gh_midi.setHandleStart(midi_note_on_cb);
    gh_midi.setHandleContinue(midi_note_on_cb);
    gh_midi.setHandleStop(midi_note_on_cb);
    gh_midi.setHandleActiveSensing(midi_note_on_cb);
    gh_midi.setHandleSystemReset(midi_note_on_cb);
    gh_midi.setHandleTick(midi_note_on_cb);
    gh_midi.setHandleError(midi_note_on_cb);
#endif
    gh_midi.begin(midi_channel);
}   /* midi_init() */

/**
 * @brief   USB init function.
 * @par     Description
 * Handlers for every MIDI event received from USB.
 * Macro MIDI_SHORT_SYSEX is useful to select between the single SYSEX
 * management and the chunk handling.
 * @return  Always 0.
 */
uint8_t
usb_rx_init ()
{
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
 * @brief   USB read function (input messages from USB).
 * @par     Description
 * Reading and saving the USB MIDI data.
 * @param[out] p_msg    Pointer to the struct which will contain the type of
 *                      message and three 32 bits of data. It is populated
 *                      if a message is detected only.
 * @return  true if a message has been received, false otherwise.
 */
bool
usb_rx_loop (usb_midi_data_t * p_msg)
{
    bool ret(usbMIDI.read());

    if (true == ret)
    {
        p_msg->type = g_midi_msg.type;
        p_msg->data1 = g_midi_msg.data1;
        p_msg->data2 = g_midi_msg.data2;
        p_msg->data3 = g_midi_msg.data3;
    }

    return ret;
}   /* usb_rx_loop() */

/**
 * @brief   USB write function (forward messages from USB).
 * @par     Description
 * Transmitting the USB MIDI data.
 * @note    See each handler description for the data usage.
 * @param[in] msg   Struct message which contains the type of message and three
 *                  32 bits of data (populated if the message needs them,
 *                  ignored otherwise).
 * @return  true if the type of sent message is managed, false otherwise.
 */
bool
usb_tx_loop (usb_midi_data_t msg)
{
    bool ret(true);

    switch (msg.type)
    {
        case MIDI_NOTE_ON:
            gh_midi.sendNoteOn(msg.data2,
                               msg.data3,
                               msg.data1);
            Serial.println("MIDI rx = " + String(msg.type));
        break;

        case MIDI_NOTE_OFF:
            gh_midi.sendNoteOff(msg.data2,
                                msg.data3,
                                msg.data1);
        break;

        case MIDI_AFTER_TOUCH_POLY:
            gh_midi.sendAfterTouch(msg.data2,
                                   msg.data3,
                                   msg.data1);
        break;

        case MIDI_CONTROL_CHANGE:
            gh_midi.sendControlChange(msg.data2,
                                      msg.data3,
                                      msg.data1);
        break;

        case MIDI_PROGRAM_CHANGE:
            gh_midi.sendProgramChange(msg.data2,
                                      msg.data1);
        break;

        case MIDI_AFTER_TOUCH_CHANNEL:
            gh_midi.sendAfterTouch(msg.data2,
                                msg.data1);
        break;

        case MIDI_PITCH_CHANGE:
            gh_midi.sendPitchBend((int) msg.data2,
                                  msg.data1);
        break;

        case MIDI_SYSEX:
            gh_midi.sendSysEx(msg.data1,
                              (const byte *) msg.p_data4);
        break;

        case MIDI_SYS_QUARTER:
            gh_midi.sendTimeCodeQuarterFrame(msg.data1);
        break;

        case MIDI_SYS_SONG_POSITION:
            gh_midi.sendSongPosition(msg.data1);
        break;

        case MIDI_SYS_SONG_SELECT:
            gh_midi.sendSongSelect(msg.data1);
        break;

        case MIDI_SYS_TUNE_REQUEST:
            gh_midi.sendTuneRequest();
        break;

        case MIDI_SYS_CLOCK:
            gh_midi.sendClock();
        break;

        case MIDI_SYS_START:
            gh_midi.sendStart();
        break;

        case MIDI_SYS_CONTINUE:
            gh_midi.sendContinue();
        break;

        case MIDI_SYS_STOP:
            gh_midi.sendStop();
        break;

        case MIDI_SYS_ACTIVE_SENSE:
            gh_midi.sendActiveSensing();
        break;

        case MIDI_SYS_RESET:
            gh_midi.sendSystemReset();
        break;

        case MIDI_SYS_GENERIC:
            gh_midi.sendRealTime((midi::MidiType) msg.data1);
        break;
        
        case MIDI_NONE:
            /* Fall through */
        default:
            Serial.print("Unsupported message" + String(msg.type) + "\n");
            ret = false;
        break;
    }

    return ret;
}   /* usb_tx_loop() */

/**
 * @brief   MIDI read function (input messages from MIDI).
 * @par     Description
 * Reading and saving the MIDI data.
 * @param[out] p_msg    Pointer to the struct which will contain the type of
 *                      message and three 32 bits of data. It is populated
 *                      if a message is detected only.
 * @return  true if a message has been received, false otherwise.
 */
bool
midi_rx_loop (usb_midi_data_t * p_msg)
{
    bool ret(gh_midi.read());

    p_msg->data1 = gh_midi.getData1();
    p_msg->data2 = gh_midi.getData2();

    uint32_t ch = gh_midi.getInputChannel();
    Serial.println("Channel= " + String(ch));

    switch (gh_midi.getType())
    {
        case midi::NoteOn:
            p_msg->type = MIDI_NOTE_ON;
            Serial.println(String("Note On:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::NoteOff:
            p_msg->type = MIDI_NOTE_OFF;
            Serial.println(String("Note Off:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::Clock:
            p_msg->type = MIDI_SYS_CLOCK;
            Serial.println(String("Note Clock:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::Start:
            p_msg->type = MIDI_SYS_START;
            Serial.println(String("Note Start:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::Tick:
            Serial.println(String("Note Tick:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::Continue:
            Serial.println(String("Note Continue:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::Stop:
            Serial.println(String("Note Stop:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::ActiveSensing:
            Serial.println(String("Note ActiveSensing:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::ControlChange:
            Serial.println(String("Note ControlChange:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::PitchBend:
            Serial.println(String("Note PitchBend:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::AfterTouchPoly:
            Serial.println(String("Note AfterTouchPoly:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::AfterTouchChannel:
            Serial.println(String("Note AfterTouchChannel:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::ProgramChange:
            Serial.println(String("Note ProgramChange:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::SystemExclusive:
            Serial.println(String("Note SystemExclusive:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::TimeCodeQuarterFrame:
            Serial.println(String("Note TimeCodeQuarterFrame:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::SongPosition:
            Serial.println(String("Note SongPosition:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::SongSelect:
            Serial.println(String("Note SongSelect:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::TuneRequest:
            Serial.println(String("Note TuneRequest:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::SystemReset:
            Serial.println(String("Note SystemReset:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
        break;

        case midi::InvalidType:
            /* Fall through */
        default:
            Serial.println("Unsopported message");
        break;
    }

    return ret;
}   /* midi_rx_loop() */


/******************************************************************************
 *                           PRIVATE FUNCTION BODIES                          *
 ******************************************************************************/

/**
 * @brief   Note On (0xA) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S '91 1 1'
 * @param[in] channel   Selected channel (1-16).
 * @param[in] note      Selected note (0-127).
 * @param[in] velocity  Selected velocity (0-127).
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
 * @param[in] channel   Selected channel (1-16).
 * @param[in] note      Selected note (0-127).
 * @param[in] velocity  Selected velocity (0-127).
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
    g_midi_msg.type = MIDI_NOTE_OFF;
    g_midi_msg.data1 = channel;
    g_midi_msg.data2 = note;
    g_midi_msg.data3 = velocity;
}   /* note_off_cb() */

/**
 * @brief   Polyphonic pressure (0xA) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'A0 1 1'
 * @param[in] channel   Selected channel (1-16).
 * @param[in] note      Selected note (0-127).
 * @param[in] velocity  Selected velocity (0-127).
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
    g_midi_msg.type = MIDI_AFTER_TOUCH_POLY;
    g_midi_msg.data1 = channel;
    g_midi_msg.data2 = note;
    g_midi_msg.data3 = velocity;
}   /* after_touch_poly_cb() */

/**
 * @brief   Control change (0xB) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'B0 1 1'
 * @param[in] channel   Selected channel (1-16).
 * @param[in] control   Selected controller (0-127).
 * @param[in] value     Selected value (0-127).
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
    g_midi_msg.type = MIDI_CONTROL_CHANGE;
    g_midi_msg.data1 = channel;
    g_midi_msg.data2 = control;
    g_midi_msg.data3 = value;
}   /* control_change_cb() */

/**
 * @brief   Program change (0xC) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'C0 1'
 * @param[in] channel   Selected channel (1-16).
 * @param[in] program   Selected patch number (0-127).
 * @return  Nothing.
 */
static void
program_change_cb (uint8_t channel, uint8_t program)
{
    Serial.print("Program Change, ch=");
    Serial.print(channel, DEC);
    Serial.print(", program=");
    Serial.println(program, DEC);
    g_midi_msg.type = MIDI_PROGRAM_CHANGE;
    g_midi_msg.data1 = channel;
    g_midi_msg.data2 = program;
    g_midi_msg.data3 = 0;
}   /* program_change_cb() */

/**
 * @brief   Channel Pressure (0xD) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'D0 1'
 * @param[in] channel   Selected channel (1-16).
 * @param[in] pressure  Selected pressure value (0-127).
 * @return  Nothing.
 */
static void
after_touch_channel_cb (uint8_t channel, uint8_t pressure)
{
    Serial.print("Channel Pressure, ch=");
    Serial.print(channel, DEC);
    Serial.print(", pressure=");
    Serial.println(pressure, DEC);
    g_midi_msg.type = MIDI_AFTER_TOUCH_CHANNEL;
    g_midi_msg.data1 = channel;
    g_midi_msg.data2 = pressure;
    g_midi_msg.data3 = 0;
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
 * @param[in] channel   Selected channel (1-16).
 * @param[in] pitch     Selected pitch (0-16383). It is a 14 bits variable
 *                      usually received as two 7 bits data bytes (LSB and MSB).
 * @return  Nothing.
 */
static void
pitch_change_cb (uint8_t channel, int pitch)
{
    Serial.print("Pitch Bend, ch=");
    Serial.print(channel, DEC);
    Serial.print(", pitch=");
    Serial.println(pitch, DEC);
    g_midi_msg.type = MIDI_PITCH_CHANGE;
    g_midi_msg.data1 = channel;
    g_midi_msg.data2 = pitch;
    g_midi_msg.data3 = 0;
}   /* pitch_change_cb() */

/**
 * @brief   System (0xF) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'F0 43 10 4C 00 00 7E 00 F7'
 * @param[out] p_data   Received data pointer (start of data).
 * @param[in] length    Lenght of received data.
 * @param[in] last      Optional: true if it is the last chunk received.
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

    g_midi_msg.type = MIDI_SYSEX;
    g_midi_msg.data1 = length;
    g_midi_msg.data2 = last;
    g_midi_msg.data3 = 0;
    g_midi_msg.p_data4 = (uint8_t *) p_data;

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
 * @param[in] data  Data byte message which is processed which indicates timing
 *                  with absolute time code (a single location needs 8 messages
 *                  in the format MIDI Time Code hours:minutes:seconds:frames).
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

    g_midi_msg.type = MIDI_SYS_QUARTER;
    g_midi_msg.data1 = data;
    g_midi_msg.data2 = 0;
    g_midi_msg.data3 = 0;
} /* time_code_quarter_frame_cb */

/**
 * @brief   System Common Song Position (0xF2) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'F2 3 0'
 * @param[in] beats 14 bits value instruct the sequencer to jump to a new
 *                  position in the song, expressed as number of sixteenth notes
 *                  from the start of the song.
 * @return  Nothing.
 */
static void
song_position_cb (uint16_t beats)
{
    Serial.print("Song Position, beat=");
    Serial.println(beats);
    g_midi_msg.type = MIDI_SYS_SONG_POSITION;
    g_midi_msg.data1 = beats;
    g_midi_msg.data2 = 0;
    g_midi_msg.data3 = 0;
} /* song_position_cb */

/**
 * @brief   System Common Song Select (0xF3) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'F3 12'
 * @param[in] song_number   Data byte to instruct the sequencer to select a new
 *                          song.
 * @return  Nothing.
 */
static void
song_select_cb (byte song_number)
{
    Serial.print("Song Select, song=");
    Serial.println(song_number, DEC);
    g_midi_msg.type = MIDI_SYS_SONG_SELECT;
    g_midi_msg.data1 = song_number;
    g_midi_msg.data2 = 0;
    g_midi_msg.data3 = 0;
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
    g_midi_msg.type = MIDI_SYS_TUNE_REQUEST;
    g_midi_msg.data1 = 0;
    g_midi_msg.data2 = 0;
    g_midi_msg.data3 = 0;
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
    g_midi_msg.type = MIDI_SYS_CLOCK;
    g_midi_msg.data1 = 0;
    g_midi_msg.data2 = 0;
    g_midi_msg.data3 = 0;
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
    g_midi_msg.type = MIDI_SYS_START;
    g_midi_msg.data1 = 0;
    g_midi_msg.data2 = 0;
    g_midi_msg.data3 = 0;
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
    g_midi_msg.type = MIDI_SYS_CONTINUE;
    g_midi_msg.data1 = 0;
    g_midi_msg.data2 = 0;
    g_midi_msg.data3 = 0;
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
    g_midi_msg.type = MIDI_SYS_STOP;
    g_midi_msg.data1 = 0;
    g_midi_msg.data2 = 0;
    g_midi_msg.data3 = 0;
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
    g_midi_msg.type = MIDI_SYS_ACTIVE_SENSE;
    g_midi_msg.data1 = 0;
    g_midi_msg.data2 = 0;
    g_midi_msg.data3 = 0;
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
    g_midi_msg.type = MIDI_SYS_RESET;
    g_midi_msg.data1 = 0;
    g_midi_msg.data2 = 0;
    g_midi_msg.data3 = 0;
} /* system_reset_cb */

#else

/**
 * @brief   System Real Time generic handler (0xFx) message callback.
 * @par     Description
 * Test: amidi -p hw:1,0,0 -S 'FA'
 * @param[in] real_time_byte    A byte, if used.
 * @return  Nothing.
 */
static void
real_time_system_cb (uint8_t real_time_byte)
{
    Serial.print("Real Time Message, code=");
    Serial.println(real_time_byte, HEX);
    g_midi_msg.type = MIDI_SYS_GENERIC;
    g_midi_msg.data1 = real_time_byte;
    g_midi_msg.data2 = 0;
    g_midi_msg.data3 = 0;
} /* real_time_system_cb */
#endif /* MIDI_GENERIC_REALTIME */

static void
midi_note_on_cb (byte channel, byte note, byte velocity)
{

}	/* midi_note_on_cb() */

/*** End of file ***/
