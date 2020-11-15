#ifndef CIRCBUFT_INT32_T_H_
#define CIRCBUFT_INT32_T_H_

// *******************************************************
// 
// circBufT.h
//
// Support for a circular buffer of int32_t values on the
//  Tiva processor.
// P.J. Bones UCECE
// Last modified:  7.3.2017
// 
// Used by fitnesswedgroup2: Matthew Johnson, Reto Schori, Max Young
// 
// *******************************************************
#include <stdint.h>

// *******************************************************
// Buffer structure
typedef struct {
	uint32_t size;		// Number of entries in buffer
	uint32_t windex;	// index for writing, mod(size)
	uint32_t rindex;	// index for reading, mod(size)
	int32_t *data;		// pointer to the data
} circBuf_t;

// *******************************************************
// initCircBuf: Initialize the circBuf instance. Reset both indices to
// the start of the buffer.  Dynamically allocate and clear the the 
// memory and return a pointer for the data.  Return NULL if 
// allocation fails.
int32_t *
initCircBuf (circBuf_t *buffer, uint32_t size);

// *******************************************************
// writeCircBuf: insert entry at the current windex location,
// advance windex, modulo (buffer size).
void
writeCircBuf (circBuf_t *buffer, int32_t entry);

// *******************************************************
// readCircBuf: return entry at the current rindex location,
// advance rindex, modulo (buffer size). The function deos not check
// if reading has advanced ahead of writing.
int32_t
readCircBuf (circBuf_t *buffer);

// *******************************************************
// freeCircBuf: Releases the memory allocated to the buffer data,
// sets pointer to NULL and other fields to 0. The buffer can
// re initialised by another call to initCircBuf().
void
freeCircBuf (circBuf_t *buffer);

#endif /*CIRCBUFT_INT32_T_H_*/