/*  
 * IO_test.ino
 * 
 * Description: This sketch blinks all the extern LED's on
 * the motorboard. This is a nice way to check that everything
 * is running smootly, and that any of the LED's has not been
 * soldered incorrectly on.
 * 
 * Author: Håkon Westh-Hansen
 * Data: 31/3/2017
 * 
 */

#include "light_global.h"
#include "light_io.h"

void setup() {                
  // initialize the digitals pin as outputs
  light_io_init();
}

void loop() {
  // toggles off and on all the outputs
  io_test();
}
