/**
 * @file threads.h
 * 
 * Threading syscalls for cartridge
*/
#pragma once

#include <stdbool.h>

typedef void (*TThreadEntry)(void *);

/** @brief Creates a new thread and adds it to the ready list
 * 
 * @param entry entry function for new thread
 * 
 * @param param parameter for new thread's function
 * 
 * @return True if thread added, false if no more room for threads
*/
bool threadCreate(TThreadEntry entry, void* param);

/** @brief Yields thread and does a context switch with next ready thread
 * 
 * @returns true if yield successful, false if there are no other threads on ready
 * list
*/
bool threadYield(void);

/** @brief Gracefully destroys thread an does a context switch with new thread
 * 
 * @returns void
*/
void threadExit(void);