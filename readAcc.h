/*
 * readAcc.h
 *
 * fitnesswedgroup2
 *
 *  Created on: 14/03/2020
 *      Author: Reto
 */

#ifndef READACC_H_
#define READACC_H_

/*
 *
 * vector used to store xyz
 * data from accelerometer
 *
 */
typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
} vector3_t;

/*
 * Zero the accelerometer in its current position
 * measurements from here are relative to the zeroed
 * position
 */
void updateRefPos();

void
initAccl (void);

vector3_t
getAcclData (void);



#endif /* READACC_H_ */
