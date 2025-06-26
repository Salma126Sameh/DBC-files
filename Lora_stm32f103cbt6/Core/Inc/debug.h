/*
 * debug.h
 *
 *  Created on: May 7, 2025
 *      Author: omara
 */

#ifndef INC_DEBUG_H_
#define INC_DEBUG_H_


#ifdef DEBUG
    #include <stdio.h>
    #define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...) do {} while (0)
#endif


#endif /* INC_DEBUG_H_ */
