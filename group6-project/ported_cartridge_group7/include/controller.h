/**
 * @file controller.h
 * 
 * Button enum type and controller syscall
*/

#pragma once
#include <stdint.h>


//Button enum type
typedef enum Button {
    w = 0,
    a,
    x,
    d,
    u,
    i,
    j,
    k,
    NONE
} Button;

uint32_t get_controller_sys();


/** @brief get controller - Gets the current value of the controller
 * 
 * @return A Button type representing the button being pressed (or none)
*/
Button get_controller() {
    return (Button) get_controller_sys();
}

