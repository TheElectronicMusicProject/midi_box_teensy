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
static usb_midi_data_t g_out_msg;


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
                state = CHECK_FROM_MIDI;
            }
            else
            {
                state = IDLE;
            }
        break;

        case CHECK_FROM_MIDI:
            if (true == midi_rx_loop(&g_out_msg))
            {
                state = FORWARD_FROM_MIDI;
            }
            else
            {
                state = IDLE;
            }
        break;

        case FORWARD_FROM_MIDI:
            state = IDLE;
        break;

        case ERROR:
            /* Fall through */
        default:
            state = IDLE;
        break;
    }
}   /* loop() */


/******************************************************************************
 *                           PRIVATE FUNCTION BODIES                          *
 ******************************************************************************/


/*** End of file ***/
