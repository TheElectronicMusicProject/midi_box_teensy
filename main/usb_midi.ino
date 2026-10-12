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
#include <string.h>


/******************************************************************************
 *                        DATA TYPES, CONSTANTS, MACROS                       *
 ******************************************************************************/

/******************************************************************************
 *                              STATIC VARIABLES                              *
 ******************************************************************************/

// Teensy 2.0: Serial1 is RX on 7 (D2) and TX on 8 (D3).
// Teensy 3.2: Serial1 is RX on 7 (0) and TX on 8 (1).
//
static MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, gh_midi);
static usb_midi_data_t g_midi_msg{MIDI_NONE, 0, 0, 0};

/******************************************************************************
 *                        PRIVATE FUNCTIONS PROTOTYPES                        *
 ******************************************************************************/

#ifdef MIDI_SHORT_SYSEX
static void system_exclusive_cb(byte * p_data, unsigned int length);
#else
static void system_exclusive_chunk_cb(const byte * p_data, uint16_t length,
                                      bool last);
#endif /* MIDI_SHORT_SYSEX */

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
    gh_midi.begin(midi_channel);
}   /* midi_init() */

/**
 * @brief   USB init function.
 * @par     Description
 * Designed for handlers, only for SysEx messages there is the setup.
 * @return  Always 0.
 */
uint8_t
usb_rx_init ()
{
#ifdef MIDI_SHORT_SYSEX
    usbMIDI.setHandleSystemExclusive(system_exclusive_cb);
#else
    usbMIDI.setHandleSystemExclusive(system_exclusive_chunk_cb);
#endif /* MIDI_SHORT_SYSEX */

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
        // MIDI message from 128 to 255.
        //
        uint8_t usb_msg_type(usbMIDI.getType());
        
        uint8_t usb_msg_ch(usbMIDI.getChannel());

        // Two data bytes, each from 0 to 127.
        //
        uint8_t usb_msg_data1(usbMIDI.getData1());
        uint8_t usb_msg_data2(usbMIDI.getData2());

        switch (usb_msg_type)
        {
            // Note On (0xA)
            // Test: amidi -p hw:1,0,0 -S '91 1 1'
            //
            case usbMIDI.NoteOn:
                Serial.print("Note On, ch=");
                Serial.print(usb_msg_ch, DEC);
                Serial.print(", note=");
                Serial.print(usb_msg_data1, DEC);
                Serial.print(", velocity=");
                Serial.println(usb_msg_data2, DEC);
                p_msg->type = MIDI_NOTE_ON;
                p_msg->data1 = usb_msg_ch;
                p_msg->data2 = usb_msg_data1;
                p_msg->data3 = usb_msg_data2;
            break;

            // Note Off (0x8)
            // Test: amidi -p hw:1,0,0 -S '80 1 1'
            //
            case usbMIDI.NoteOff:
                Serial.print("Note Off, ch=");
                Serial.print(usb_msg_ch, DEC);
                Serial.print(", note=");
                Serial.print(usb_msg_data1, DEC);
                Serial.print(", velocity=");
                Serial.println(usb_msg_data2, DEC);
                p_msg->type = MIDI_NOTE_OFF;
                p_msg->data1 = usb_msg_ch;
                p_msg->data2 = usb_msg_data1;
                p_msg->data3 = usb_msg_data2;
            break;

            // Polyphonic pressure (0xA)
            // Test: amidi -p hw:1,0,0 -S 'A0 1 1'
            //
            case usbMIDI.AfterTouchPoly:
                Serial.print("Polyphonic Pressure, ch=");
                Serial.print(usb_msg_ch, DEC);
                Serial.print(", note=");
                Serial.print(usb_msg_data1, DEC);
                Serial.print(", velocity=");
                Serial.println(usb_msg_data2, DEC);
                p_msg->type = MIDI_AFTER_TOUCH_POLY;
                p_msg->data1 = usb_msg_ch;
                p_msg->data2 = usb_msg_data1;
                p_msg->data3 = usb_msg_data2;
            break;

            // Control change (0xB)
            // Test: amidi -p hw:1,0,0 -S 'B0 1 1'
            //
            case usbMIDI.ControlChange:
                Serial.print("Control change, ch=");
                Serial.print(usb_msg_ch, DEC);
                Serial.print(", control=");
                Serial.print(usb_msg_data1, DEC);
                Serial.print(", value=");
                Serial.println(usb_msg_data2, DEC);
                p_msg->type = MIDI_CONTROL_CHANGE;
                p_msg->data1 = usb_msg_ch;
                p_msg->data2 = usb_msg_data1;
                p_msg->data3 = usb_msg_data2;
            break;

            // Program change (0xC)
            // Test: amidi -p hw:1,0,0 -S 'C0 1'
            //
            case usbMIDI.ProgramChange:
                Serial.print("Program Change, ch=");
                Serial.print(usb_msg_ch, DEC);
                Serial.print(", program=");
                Serial.println(usb_msg_data1, DEC);
                p_msg->type = MIDI_PROGRAM_CHANGE;
                p_msg->data1 = usb_msg_ch;
                p_msg->data2 = usb_msg_data1;
                p_msg->data3 = 0;
            break;

            // Channel Pressure (0xD)
            // Test: amidi -p hw:1,0,0 -S 'D0 1'
            //
            case usbMIDI.AfterTouchChannel:
                Serial.print("Channel Pressure, ch=");
                Serial.print(usb_msg_ch, DEC);
                Serial.print(", pressure=");
                Serial.println(usb_msg_data1, DEC);
                p_msg->type = MIDI_AFTER_TOUCH_CHANNEL;
                p_msg->data1 = usb_msg_ch;
                p_msg->data2 = usb_msg_data1;
                p_msg->data3 = 0;
            break;

            // Pitch Bend (0xE)
            // Test: amidi -p hw:1,0,0 -S 'E0 0 0'
            // for minimum pitch (-8192).
            // amidi -p hw:1,0,0 -S 'E0 0 40'
            // for middle pitch (0).
            // amidi -p hw:1,0,0 -S 'E0 7F 7F'
            // for maximum pitch (8191).
            //
            case usbMIDI.PitchBend:
                Serial.print("Pitch Change, ch=");
                Serial.print(usb_msg_ch, DEC);
                Serial.print(", pitch=");
                Serial.println(usb_msg_data1 + (usb_msg_data2 * 128), DEC);
                p_msg->type = MIDI_PITCH_CHANGE;
                p_msg->data1 = usb_msg_ch;
                p_msg->data2 = usb_msg_data1 + (usb_msg_data2 * 128);
                p_msg->data3 = 0;
            break;

            // System Common Time Code Quarter Frame (0xF1)
            // Test: amidi -p hw:1,0,0 -S 'F1 74'
            // for 29.97 fps (no timecode) and the values remains in memory as
            // static variable until the fps are changed.
            // amidi -p hw:1,0,0 -S 'F1 20'
            // for timecode 00:00:048.00.
            //
            case usbMIDI.TimeCodeQuarterFrame:
            {
                static int8_t smpte_message[8]{0};
                static uint8_t fps(0);
                uint8_t msg_type(usb_msg_data1 >> 0x4);
                uint8_t values(usb_msg_data1 & 0xF);

                Serial.print("Timecode, index=");
                Serial.print(msg_type, DEC);
                Serial.print(", digit=");
                Serial.println(values, DEC);

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
                    Serial.println("TimeCode: " + String(smpte_message[7]) +
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
                    Serial.println(usb_msg_data1, HEX);
                }

                p_msg->type = MIDI_SYS_QUARTER;
                p_msg->data1 = usb_msg_data1;
                p_msg->data2 = 0;
                p_msg->data3 = 0;
            }
            break;

            // System Common Song Position (0xF2)
            // Test: amidi -p hw:1,0,0 -S 'F2 3 0'
            //
            case usbMIDI.SongPosition:
                Serial.print("Song Position, beat=");
                Serial.println(usb_msg_data1 + usb_msg_data2 * 128);
                p_msg->type = MIDI_SYS_SONG_POSITION;
                p_msg->data1 = usb_msg_data1 + usb_msg_data2 * 128;
                p_msg->data2 = 0;
                p_msg->data3 = 0;
            break;

            // System Common Song Select (0xF3)
            // Test: amidi -p hw:1,0,0 -S 'F3 12'
            //
            case usbMIDI.SongSelect:
                Serial.print("Song Select, song=");
                Serial.println(usb_msg_data1, DEC);
                p_msg->type = MIDI_SYS_SONG_SELECT;
                p_msg->data1 = usb_msg_data1;
                p_msg->data2 = 0;
                p_msg->data3 = 0;
            break;

            // System Common Tune Request (0xF6)
            // Test: amidi -p hw:1,0,0 -S 'F6'
            //
            case usbMIDI.TuneRequest:
                Serial.println("Tune Request");
                p_msg->type = MIDI_SYS_TUNE_REQUEST;
                p_msg->data1 = 0;
                p_msg->data2 = 0;
                p_msg->data3 = 0;
            break;

            // System Real Time Timing Clock (0xF8)
            // Test: amidi -p hw:1,0,0 -S 'F8'
            //
            case usbMIDI.Clock:
                Serial.println("Clock");
                p_msg->type = MIDI_SYS_CLOCK;
                p_msg->data1 = 0;
                p_msg->data2 = 0;
                p_msg->data3 = 0;
            break;

            // System Real Time Start (0xFA)
            // Test: amidi -p hw:1,0,0 -S 'FA'
            //
            case usbMIDI.Start:
                Serial.println("Start");
                p_msg->type = MIDI_SYS_START;
                p_msg->data1 = 0;
                p_msg->data2 = 0;
                p_msg->data3 = 0;
            break;

            // System Real Time Continue (0xFB)
            // Test: amidi -p hw:1,0,0 -S 'FB'
            //
            case usbMIDI.Continue:
                Serial.println("Continue");
                p_msg->type = MIDI_SYS_CONTINUE;
                p_msg->data1 = 0;
                p_msg->data2 = 0;
                p_msg->data3 = 0;
            break;

            // System Real Time Stop (0xFC)
            // Test: amidi -p hw:1,0,0 -S 'FC'
            //
            case usbMIDI.Stop:
                Serial.println("Stop");
                p_msg->type = MIDI_SYS_STOP;
                p_msg->data1 = 0;
                p_msg->data2 = 0;
                p_msg->data3 = 0;
            break;

            // System Real Time Stop (0xFC)
            // Test: amidi -p hw:1,0,0 -S 'FC'
            //
            case usbMIDI.ActiveSensing:
                Serial.println("Active Sensing");
                p_msg->type = MIDI_SYS_ACTIVE_SENSE;
                p_msg->data1 = 0;
                p_msg->data2 = 0;
                p_msg->data3 = 0;
            break;

            // System Real Time System Reset (0xFF)
            // Test: amidi -p hw:1,0,0 -S 'FF'
            //
            case usbMIDI.SystemReset:
                Serial.println("System Reset");
                p_msg->type = MIDI_SYS_RESET;
                p_msg->data1 = 0;
                p_msg->data2 = 0;
                p_msg->data3 = 0;
            break;

            case usbMIDI.SystemExclusive:
                p_msg->type = g_midi_msg.type;
                p_msg->data1 = g_midi_msg.data1;
                p_msg->data2 = g_midi_msg.data2;
                p_msg->data3 = g_midi_msg.data3;
                
                if (true == g_midi_msg.data2)
                {
                    memcpy(p_msg->data4, g_midi_msg.data4, p_msg->data1);
                }
            break;

            case usbMIDI.InvalidType:
                /* Fall through */
            default:
                Serial.print("Unsupported message " + String(usb_msg_type) +
                             "\n");
                ret = false;
            break;
        }
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
usb_tx_loop (usb_midi_data_t * p_msg)
{
    bool ret(true);

    switch (p_msg->type)
    {
        case MIDI_NOTE_ON:
            gh_midi.sendNoteOn(p_msg->data2,
                               p_msg->data3,
                               p_msg->data1);
            Serial.println("MIDI rx = " + String(p_msg->type));
        break;

        case MIDI_NOTE_OFF:
            gh_midi.sendNoteOff(p_msg->data2,
                                p_msg->data3,
                                p_msg->data1);
        break;

        case MIDI_AFTER_TOUCH_POLY:
            gh_midi.sendAfterTouch(p_msg->data2,
                                   p_msg->data3,
                                   p_msg->data1);
        break;

        case MIDI_CONTROL_CHANGE:
            gh_midi.sendControlChange(p_msg->data2,
                                      p_msg->data3,
                                      p_msg->data1);
        break;

        case MIDI_PROGRAM_CHANGE:
            gh_midi.sendProgramChange(p_msg->data2,
                                      p_msg->data1);
        break;

        case MIDI_AFTER_TOUCH_CHANNEL:
            gh_midi.sendAfterTouch(p_msg->data2,
                                p_msg->data1);
        break;

        case MIDI_PITCH_CHANGE:
            gh_midi.sendPitchBend((int) p_msg->data2,
                                  p_msg->data1);
        break;

        case MIDI_SYSEX:
            gh_midi.sendSysEx(p_msg->data1,
                              (const byte *) p_msg->data4);
        break;

        case MIDI_SYS_QUARTER:
            gh_midi.sendTimeCodeQuarterFrame(p_msg->data1);
        break;

        case MIDI_SYS_SONG_POSITION:
            gh_midi.sendSongPosition(p_msg->data1);
        break;

        case MIDI_SYS_SONG_SELECT:
            gh_midi.sendSongSelect(p_msg->data1);
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

        case MIDI_SYS_TICK:
            gh_midi.sendTick();
        break;

        case MIDI_SYS_GENERIC:
            gh_midi.sendRealTime((midi::MidiType) p_msg->data1);
        break;
        
        case MIDI_NONE:
            /* Fall through */
        default:
            Serial.print("Unsupported message" + String(p_msg->type) + "\n");
            ret = false;
        break;
    }

    if (true == ret)
    {
        p_msg->type = MIDI_NONE;
        p_msg->data1 = 0;
        p_msg->data2 = 0;
        p_msg->data3 = 0;
        memset(p_msg->data4, 0, MIDI_BUFFER_LEN);
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

    if (true == ret)
    {
        uint32_t midi_ch(gh_midi.getInputChannel());

        if (midi::SystemExclusive != gh_midi.getType())
        {
            p_msg->data1 = gh_midi.getData1();
            p_msg->data2 = gh_midi.getData2();
        }
        else
        {
            const uint8_t * p_sysex_data(gh_midi.getSysExArray());
            uint32_t idx(0);

            p_msg->data1 = gh_midi.getSysExArrayLength();

            if (p_msg->data1 > MIDI_BUFFER_LEN)
            {
                Serial.println("ATTENTION! Losing " +
                               String(p_msg->data1 - MIDI_BUFFER_LEN) +
                               " bytes!");
                p_msg->data1 = MIDI_BUFFER_LEN;
                
            }

            for (uint32_t sysex_idx = 0; sysex_idx < p_msg->data1; ++sysex_idx)
            {
                p_msg->data4[idx] = *(p_sysex_data + sysex_idx);
                ++idx;
            }
        }

        p_msg->data3 = midi_ch;

        //Serial.print("Channel= ");
        //Serial.println(midi_ch);

        switch (gh_midi.getType())
        {
            case midi::NoteOn:
                p_msg->type = MIDI_NOTE_ON;
            break;

            case midi::NoteOff:
                p_msg->type = MIDI_NOTE_OFF;
            break;

            case midi::Clock:
                p_msg->type = MIDI_SYS_CLOCK;
            break;

            case midi::Start:
                p_msg->type = MIDI_SYS_START;
            break;

            case midi::Continue:
                p_msg->type = MIDI_SYS_CONTINUE;
            break;

            case midi::Stop:
                p_msg->type = MIDI_SYS_STOP;
            break;

            case midi::ActiveSensing:
                p_msg->type = MIDI_SYS_ACTIVE_SENSE;
            break;

            case midi::ControlChange:
                p_msg->type = MIDI_CONTROL_CHANGE;
            break;

            case midi::PitchBend:
                p_msg->type = MIDI_PITCH_CHANGE;
            break;

            case midi::AfterTouchPoly:
                p_msg->type = MIDI_AFTER_TOUCH_POLY;
            break;

            case midi::AfterTouchChannel:
                p_msg->type = MIDI_AFTER_TOUCH_CHANNEL;
            break;

            case midi::ProgramChange:
                p_msg->type = MIDI_PROGRAM_CHANGE;
            break;

            case midi::SystemExclusive:
                p_msg->type = MIDI_SYSEX;
            break;

            case midi::TimeCodeQuarterFrame:
                p_msg->type = MIDI_SYS_QUARTER;
            break;

            case midi::SongPosition:
                p_msg->type = MIDI_SYS_SONG_POSITION;
            break;

            case midi::SongSelect:
                p_msg->type = MIDI_SYS_SONG_SELECT;
            break;

            case midi::TuneRequest:
                p_msg->type = MIDI_SYS_TUNE_REQUEST;
            break;

            case midi::SystemReset:
                p_msg->type = MIDI_SYS_RESET;
            break;

            case midi::Tick:
                p_msg->type = MIDI_SYS_TICK;
            break;

            case midi::InvalidType:
                /* Fall through */
            default:
                p_msg->type = MIDI_NONE;
                Serial.println("Unsopported message");
            break;
        }
    }

    return ret;
}   /* midi_rx_loop() */

bool
midi_tx_loop (usb_midi_data_t * p_msg)
{
    bool ret(true);

    Serial.println(String("Type=" + String(p_msg->type)));

    switch (p_msg->type)
    {
        case MIDI_NOTE_ON:
            usbMIDI.sendNoteOn(p_msg->data1, p_msg->data2, p_msg->data3);
        break;

        case MIDI_NOTE_OFF:
            usbMIDI.sendNoteOff(p_msg->data1, p_msg->data2, p_msg->data3);
        break;

        case MIDI_AFTER_TOUCH_POLY:
            usbMIDI.sendAfterTouchPoly(p_msg->data1, p_msg->data2, p_msg->data3);
        break;

        case MIDI_CONTROL_CHANGE:
            usbMIDI.sendControlChange(p_msg->data1, p_msg->data2, p_msg->data3);
        break;

        case MIDI_PROGRAM_CHANGE:
            usbMIDI.sendProgramChange(p_msg->data1, p_msg->data3);
        break;

        case MIDI_AFTER_TOUCH_CHANNEL:
            usbMIDI.sendAfterTouch(p_msg->data1, p_msg->data3);
        break;

        case MIDI_PITCH_CHANGE:
            // data2 is MSB, data 1 il LSB.
            // Total is a 14 bit variable.
            //
            usbMIDI.sendPitchBend((p_msg->data2 << 7) | p_msg->data1,
                                  p_msg->data3);
        break;

        case MIDI_SYSEX:
            usbMIDI.sendSysEx(p_msg->data1, p_msg->data4);
        break;

        case MIDI_SYS_QUARTER:
            usbMIDI.sendTimeCodeQuarterFrame(p_msg->data1 >> 4,
                                             p_msg->data1 & 0x0F);
        break;

        case MIDI_SYS_SONG_POSITION:
            // data2 is MSB, data 1 il LSB.
            // Total is a 14 bit variable.
            //
            usbMIDI.sendSongPosition((p_msg->data2 << 7) | p_msg->data1);
        break;

        case MIDI_SYS_SONG_SELECT:
            usbMIDI.sendSongSelect(p_msg->data1);
        break;

        case MIDI_SYS_TUNE_REQUEST:
            usbMIDI.sendTuneRequest();
        break;

        case MIDI_SYS_CLOCK:
            usbMIDI.sendClock();
        break;

        case MIDI_SYS_START:
            usbMIDI.sendStart();
        break;

        case MIDI_SYS_CONTINUE:
            usbMIDI.sendContinue();
        break;

        case MIDI_SYS_STOP:
            usbMIDI.sendStop();
        break;

        case MIDI_SYS_ACTIVE_SENSE:
            usbMIDI.sendActiveSensing();
        break;

        case MIDI_SYS_RESET:
            usbMIDI.sendSystemReset();
        break;

        case MIDI_SYS_TICK:
            usbMIDI.sendTick();
        break;
        
        case MIDI_SYS_GENERIC:
            /* Fall through */
        case MIDI_NONE:
            /* Fall through */
        default:
            Serial.print("Unsupported message" + String(p_msg->type) + "\n");
            ret = false;
        break;
    }

    if (true == ret)
    {
        usbMIDI.send_now();
        p_msg->type = MIDI_NONE;
        p_msg->data1 = 0;
        p_msg->data2 = 0;
        p_msg->data3 = 0;
        memset(p_msg->data4, 0, MIDI_BUFFER_LEN);
    }

    return ret;
}   /* midi_tx_loop() */

/******************************************************************************
 *                           PRIVATE FUNCTION BODIES                          *
 ******************************************************************************/

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
    static uint8_t idx(0);

    Serial.print("SysEx Message: ");

    g_midi_msg.type = MIDI_SYSEX;
    g_midi_msg.data1 = length;
    g_midi_msg.data2 = last;
    g_midi_msg.data3 = 0;

    for (uint32_t sysex_idx = 0; sysex_idx < length; ++sysex_idx)
    {
        g_midi_msg.data4[idx] = *(p_data + sysex_idx);
        Serial.print(*(p_data + idx), HEX);
        Serial.print(" ");
        ++idx;
    }

#ifndef MIDI_SHORT_SYSEX
    if (true == last)
    {
        idx = 0;
        Serial.println("(end)");
    }
    else
    {
        Serial.println("(to be continued)");
    }
}   /* system_exclusive_chunk_cb() */
#else
    idx = 0;
    Serial.println("(end)");
}   /* system_exclusive_cb() */
#endif /* MIDI_SHORT_SYSEX */

/*** End of file ***/
