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
    // Serial1 is RX on 7 and TX on 8.
    //
    MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, gh_midi);

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
        Serial.println("MIDI rx = " + String(g_in_msg.type));
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
