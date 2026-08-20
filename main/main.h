/**
 * @file    main.h
 * 
 * @brief   Useful material for the source file.
 * 
 * @author  Filippo Graziani (circuitry.passion@gmail.com)
 * 
 * @date    21/07/2026
 * 
 * @par     Description
 * Put here a description.
 */

#ifndef MAIN_H
#   define MAIN_H

/******************************************************************************
 *                              INCLUDE STATEMENS                             *
 * The headers for each grouping should be sorted alphabetically:             *
 * 1 - The paired header file                                                 *
 * 2 - Other headers from your project                                        *
 * 3 - 3rd party library headers                                              *
 * 4 - Standard library headers                                               *
 ******************************************************************************/



/******************************************************************************
 *                            PROTOTYPES OR MACROS                            *
 ******************************************************************************/
 
 

/******************************************************************************
 *                                   #DEFINE                                  *
 ******************************************************************************/



/******************************************************************************
 *                      TYPEDEFS (STRUCTS, ENUMS, UNIONS)                     *
 ******************************************************************************/

typedef enum state_machine
{
	IDLE,
	ERROR,
	CHECK_FROM_USB,
	FORWARD_FROM_USB,
	CHECK_FROM_MIDI,
	FORWARD_FROM_MIDI
} state_machine_t;

/******************************************************************************
 *                         PUBLIC FUNCTIONS PROTOTYPES                        *
 ******************************************************************************/


#endif /* MAIN_H */


/*** End of file ***/
