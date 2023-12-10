/**
 * @file locks.h
 * 
 * Locks system calls header files
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>


typedef struct Lock {
    uint32_t holderID;
    bool acquired;

}Lock;

Lock* lockCreate(void);

/** @brief Acquires a lock (Spins if lock held by another thread)
 * 
 * @param lock lock to be acquired
 * 
 * @returns true if lock acquired, false if failed
*/
bool lockAcquire(Lock* lock);

/** @brief Releases a held lock
 * 
 * @param lock lock to be released
 * 
 * @returns true if lock released, false if failed
*/
bool lockRelease(Lock* lock);

