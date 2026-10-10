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
 * Designed for handlers, it does nothing.
 * @return  Always 0.
 */
uint8_t
usb_rx_init ()
{
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
                Serial.print("Polyphonic Pressure, ch=");
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
                Serial.print(usb_msg_data1, DEC);
                p_msg->type = MIDI_CONTROL_CHANGE;
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
                Serial.print(usb_msg_data1, DEC);
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
                Serial.print(usb_msg_data1 + (usb_msg_data2 << 0x80), DEC);
                p_msg->type = MIDI_PITCH_CHANGE;
                p_msg->data1 = usb_msg_ch;
                p_msg->data2 = usb_msg_data1 + (usb_msg_data2 << 0x80);
                p_msg->data3 = 0;
            break;

////////////////////////////////////////////////////////////////////////TO DO /////////////////7
            // System (0xF)
            // Test: amidi -p hw:1,0,0 -S 'F0 43 10 4C 00 00 7E 00 F7'
            //
            case usbMIDI.SystemExclusive: // 0xF0
                // Messages larger than usbMIDI's internal buffer are truncated.
                // To receive large messages, you *must* use the 3-input function
                // handler.  See InputFunctionsComplete for details.
                Serial.print("SysEx Message: ");
                printBytes(usbMIDI.getSysExArray(), data1 + data2 * 256);
                Serial.println();
            break;

            case usbMIDI.TimeCodeQuarterFrame: // 0xF1
                Serial.print("TimeCode, index=");
                Serial.print(data1 >> 4, DEC);
                Serial.print(", digit=");
                Serial.println(data1 & 15, DEC);
            break;

            case usbMIDI.SongPosition: // 0xF2
                Serial.print("Song Position, beat=");
                Serial.println(data1 + data2 * 128);
            break;

            case usbMIDI.SongSelect: // 0xF3
                Serial.print("Sond Select, song=");
                Serial.println(data1, DEC);
            break;

            case usbMIDI.TuneRequest: // 0xF6
                Serial.println("Tune Request");
            break;

            case usbMIDI.Clock: // 0xF8
                Serial.println("Clock");
            break;

            case usbMIDI.Start: // 0xFA
                Serial.println("Start");
            break;

            case usbMIDI.Continue: // 0xFB
                Serial.println("Continue");
            break;

            case usbMIDI.Stop: // 0xFC
                Serial.println("Stop");
            break;

            case usbMIDI.ActiveSensing: // 0xFE
                Serial.println("Actvice Sensing");
            break;

            case usbMIDI.SystemReset: // 0xFF
                Serial.println("System Reset");
            break;

            case usbMIDI.InvalidType:
                /* Fall through */
            default:
                Serial.print("Unsupported message" + String(usb_msg_type) +
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

            for (; idx < p_msg->data1; ++idx)
            {
                p_msg->data4[idx] = *(p_sysex_data + idx);
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
            usbMIDI.sendPitchBend((p_msg->data2 << 7) | p_msg->data1, p_msg->data3);
        break;

        case MIDI_SYSEX:
            usbMIDI.sendSysEx(p_msg->data1, p_msg->data4);
        break;

        case MIDI_SYS_QUARTER:
            usbMIDI.sendTimeCodeQuarterFrame(p_msg->data1 >> 4, p_msg->data1 & 0x0F);
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
    }

    return ret;
}   /* midi_tx_loop() */

/******************************************************************************
 *                           PRIVATE FUNCTION BODIES                          *
 ******************************************************************************/


/*** End of file ***/
