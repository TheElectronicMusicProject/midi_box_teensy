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
#include "usb_midi.h"
#include <MIDI.h>
#include <HardwareSerial.h>


/******************************************************************************
 *                        DATA TYPES, CONSTANTS, MACROS                       *
 ******************************************************************************/


/******************************************************************************
 *                              STATIC VARIABLES                              *
 ******************************************************************************/

static const uint8_t g_midi_channel(1);
static const uint32_t g_led_timer(1000);
static usb_midi_data_t g_in_msg;

// Serial1 is RX on 7 and TX on 8.
//
static MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, gh_midi);

/******************************************************************************
 *                        PRIVATE FUNCTIONS PROTOTYPES                        *
 ******************************************************************************/




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
    usb_rx_init(115200);
    gh_midi.begin(g_midi_channel);
    pinMode(LED_BUILTIN, OUTPUT);
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
    static uint32_t current_count(0);

    if (true == usb_rx_loop(&g_in_msg))
    {
        switch (g_in_msg.type)
        {
            case MIDI_NOTE_ON:
                gh_midi.sendNoteOn(g_in_msg.data2,
                                   g_in_msg.data3,
                                   g_in_msg.data1);
                Serial.println("MIDI rx = " + String(g_in_msg.type));
            break;

            case MIDI_NOTE_OFF:
                gh_midi.sendNoteOff(g_in_msg.data2,
                                    g_in_msg.data3,
                                    g_in_msg.data1);
            break;

            case MIDI_AFTER_TOUCH_POLY:
                gh_midi.sendAfterTouch(g_in_msg.data2,
                                       g_in_msg.data3,
                                       g_in_msg.data1);
            break;

            case MIDI_CONTROL_CHANGE:
                gh_midi.sendControlChange(g_in_msg.data2,
                                          g_in_msg.data3,
                                          g_in_msg.data1);
            break;

            case MIDI_PROGRAM_CHANGE:
                gh_midi.sendProgramChange(g_in_msg.data2,
                                          g_in_msg.data1);
            break;

            case MIDI_AFTER_TOUCH_CHANNEL:
                gh_midi.sendAfterTouch(g_in_msg.data2,
                                       g_in_msg.data1);
            break;

            case MIDI_PITCH_CHANGE:
                gh_midi.sendPitchBend((int) g_in_msg.data2,
                                      g_in_msg.data1);
            break;

            case MIDI_SYSEX:
                gh_midi.sendSysEx(g_in_msg.data1,
                                  (const byte *) g_in_msg.p_data4);
            break;

            case MIDI_SYS_QUARTER:
                gh_midi.sendTimeCodeQuarterFrame(g_in_msg.data1);
            break;

            case MIDI_SYS_SONG_POSITION:
                gh_midi.sendSongPosition(g_in_msg.data1);
            break;

            case MIDI_SYS_SONG_SELECT:
                gh_midi.sendSongSelect(g_in_msg.data1);
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
                gh_midi.sendRealTime((midi::MidiType) g_in_msg.data1);
            break;
            
            case MIDI_NONE:
                /* Fall through */
            default:
                Serial.print("Unsopported message" + String(g_in_msg.type) + "\n");
            break;
        }
        
        digitalWriteFast(LED_BUILTIN, 1);
        current_count = millis();
    }

    if ((millis() - current_count) >= g_led_timer)
    {
        digitalWriteFast(LED_BUILTIN, 0);
    }
}   /* loop() */


/******************************************************************************
 *                           PRIVATE FUNCTION BODIES                          *
 ******************************************************************************/


/*** End of file ***/
