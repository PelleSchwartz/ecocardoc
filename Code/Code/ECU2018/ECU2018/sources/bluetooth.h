/*
 * bluetooth.h
 *
 * Author: Håkon Westh-Hansen
 * Created: 1/4/2017
 *
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#define BLUE_SEND_BUFFER_SIZE 400

 /*=============*/
 /* Definitions */
 /*=============*/

#define BAUDBLUE 9600
#define BLUESERIAL Serial1

 /*=============*/
 /* Prototypes  */
 /*=============*/

void blue_init();
void blue_set_new_device(String name);

#endif /* BLUETOOTH_H_ */
