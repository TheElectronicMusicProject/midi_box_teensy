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
    // Setup the MIDI protocol speed.
    //
    Serial.begin(115200);

    if (false == Serial)
    {
        /* Firmware waiting for connection... */
    }

    Serial.println("STARTED!");

    midi_init(g_midi_channel);
    usb_rx_init();
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
    static state_machine_t state(IDLE);

    switch (state)
    {
        case IDLE:
            if ((millis() - current_count) >= g_led_timer)
            {
                digitalWriteFast(LED_BUILTIN, 0);
            }

            state = CHECK_FROM_USB;
        break;

        case ERROR:
            /* Fall through */
        break;

        case CHECK_FROM_USB:
            if (true == usb_rx_loop(&g_in_msg))
            {
                digitalWriteFast(LED_BUILTIN, 1);
                current_count = millis();
        
                state = FORWARD_FROM_USB;
            }
            else
            {
                state = IDLE;
            }
        break;

        case FORWARD_FROM_USB:
            if (true == usb_tx_loop(g_in_msg))
            {

            }
            else
            {

            }
            state = IDLE;
        break;

        case CHECK_FROM_MIDI:
#if 0
            if (true == gh_midi.read())
            {
                switch (gh_midi.getType())
                {
                    case midi::NoteOn:
                        Serial.println(String("Note On:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
                    break;

                    case midi::NoteOff:
                        Serial.println(String("Note Off:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
                    break;

                    case midi::Clock:
                        Serial.println(String("Note Clock:  ch= , note=" + String(gh_midi.getData1()) + ", velocity=" + String(gh_midi.getData2())));
                    break;

                    case midi::Start:
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
                        Serial.print("Unsopported message \n");
                    break;
                }

                state = FORWARD_FROM_MIDI;
            }
            else
            {
                state = IDLE;
            
#endif
        break;

        case FORWARD_FROM_MIDI:
            state = IDLE;
        break;

        default:
            state = ERROR;
        break;
    }
}   /* loop() */


/******************************************************************************
 *                           PRIVATE FUNCTION BODIES                          *
 ******************************************************************************/


/*** End of file ***/
