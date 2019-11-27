/*********************************************************************
CURRENT LOGGER BOARD
Designed fall semester 2018 by Luke Robinson


*********************************************************************/
//oled libraries and defines
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 20
Adafruit_SSD1306 display(OLED_RESET);

//sd defines
#include <SD.h>
#include <SPI.h>
File sdlog;
const int chipSelect = BUILTIN_SDCARD;
char filename[12];
int file_number = 0;


//additional i/o
#define BUILTIN_LED 13
#define LED_1 7
#define LED_2 8
#define BUTTON_1 5
#define BUTTON_2 6
#define LOW_CURRENT_PIN A8
#define HIGH_CURRENT_PIN A9

//analog reading parameters
#define SAMPLE_ITERATIONS 50 //keep this below 100 to avoid blocking
#define ADC_RESOLUTION 16 //16 bits is needed for the below calibration parameters
#define LOW_CURRENT_CAL .000420011 - 13.42192794
#define HIGH_CURRENT_CAL 0.001991111 - 15.59586005
#define UPDATE_INTERVAL 100 //in ms
int current_sensor_pin = LOW_CURRENT_PIN;
boolean recording = false;
unsigned long last_reading = 0;
unsigned long last_amp_hour_calc = 0;
int sample_number = 0;
unsigned long sample_start_time = 0;
float  amp_hours = 0;



void setup()   {
  //serial for debugging
  Serial.begin(115200);

  //io config
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  analogReadResolution(ADC_RESOLUTION);
  digitalWrite(BUILTIN_LED, HIGH);   // turn the LED on (HIGH is the voltage level)








  //initialize the OLED and display a message
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("CURRENT");
  display.setCursor(0, 16);
  display.println("LOGGER");
  display.display();
  delay(500);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("LOW CURR.");
  display.setCursor(0, 16);
  display.println("5A MAX");
  display.display();
  delay(1000);
  display.clearDisplay();


  //if button 1 is low on startup, enter calibration procedure
  if (digitalRead(BUTTON_1) == LOW) {
    display.setTextSize(2);

    while (true) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println(analogRead(LOW_CURRENT_PIN));
      display.setCursor(0, 16);
      display.println(analogRead(HIGH_CURRENT_PIN));
      display.display();
      delay(1000);
    }


  }






  //sd card config
  if (!SD.begin(chipSelect)) {
    display.setCursor(0, 0);
    display.println("SD FAILED");
    display.setCursor(0, 16);
    //display.setTextSize(2);
    display.println("reinsert the card or reformat");
    delay(400);
  }

  //find the next file to write to
  sprintf(filename, "%02d.CSV", file_number);
  do {
    file_number++;
    sprintf(filename, "%02d.CSV", file_number);
  } while (SD.exists(filename));


  //check if the sd card is inserted

}


void loop() {


  //button one switches current sensors
  if (digitalRead(BUTTON_1) == LOW) {
    digitalWrite(LED_1, HIGH);

    display.clearDisplay();
    if (current_sensor_pin == LOW_CURRENT_PIN) {
      current_sensor_pin = HIGH_CURRENT_PIN;
      display.setCursor(0, 0);
      display.setTextSize(2);
      display.println("HIGH CURR.");
      display.setCursor(0, 16);
      display.println("100A MAX");
      display.display();
    }
    else {
      current_sensor_pin = LOW_CURRENT_PIN;
      display.setCursor(0, 0);
      display.setTextSize(2);
      display.println("LOW CURR.");
      display.setCursor(0, 16);
      display.println("5A MAX");
      display.display();
    }

    delay(1000);
    digitalWrite(LED_1, LOW);

  }

  //button two toggles recording
  if (digitalRead(BUTTON_2) == LOW) {
    digitalWrite(LED_1, HIGH);
    display.clearDisplay();

    if (recording == true) {
      recording = false;
      display.setCursor(0, 0);
      display.setTextSize(2);

      display.println("REC SAVED.");
      display.setCursor(0, 16);
      display.println(filename);
      display.display();

      //close the file
      sdlog.close();
      //update filenumbers and names
      file_number++;
      sprintf(filename, "%02d.CSV", file_number);
      delay(2000);

    }
    else {
      recording = true;

      display.setCursor(0, 0);
      display.setTextSize(2);
      display.println("RECORDING");
      display.setCursor(0, 16);
      display.println(filename);
      display.display();
      sdlog = SD.open(filename, FILE_WRITE);
      sample_number = 0;
      amp_hours = 0;
      sample_start_time = millis();
    }
    delay(1000);
    digitalWrite(LED_1, LOW);
    amp_hours = 0;
  }



  //read the current sensor if we need to update
  if (last_reading < millis() - UPDATE_INTERVAL) {
    last_reading = millis();

    float amps = 0;
    float adc_sum = 0;
    for (int i = 0; i < SAMPLE_ITERATIONS; i++) {
      adc_sum = adc_sum + analogRead(current_sensor_pin);
      delay(1);
    }
    float adc_reading = adc_sum / SAMPLE_ITERATIONS;

    // Serial.println(adc);

    //try using the coefficents for a linear eq from excell
    if (current_sensor_pin == LOW_CURRENT_PIN) amps = adc_reading * LOW_CURRENT_CAL;
    if (current_sensor_pin == HIGH_CURRENT_PIN) amps = adc_reading * HIGH_CURRENT_CAL;


    if (amps < 0 && amps > -.02) amps = 0;


    //calculate amp hours
    float time_segment = (millis() - last_amp_hour_calc);
    last_amp_hour_calc = millis();
    time_segment = time_segment / 3600.0;

    Serial.print("time segment:");
    Serial.println(time_segment);
    amp_hours += time_segment * abs(amps);
    Serial.print("mAH:");
    Serial.println(amp_hours);


    /* ALTERNATE CODE FOR CONVERTING NOT USING
        LINEAR CALIBRATION SCALE

          float voltage = 3.322 * adc / 65535.0 ;
          voltage = voltage - 1.61;
          Serial.print("VOLTS:");
          Serial.println(voltage, 4);
          float amps = voltage / .185;
    */


    Serial.print("AMPS:");
    Serial.println(amps, 4);
    Serial.println();




    //save the data if recording
    if (recording) {
      //log to file
      //save timestamp in ms
      sdlog.print((millis() - sample_start_time));
      sdlog.print(", ");
      sdlog.println(amps);
      sample_number++;

      //display
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(2);
      display.print(amps);
      display.println(" A");
      display.setTextSize(1);

      display.setCursor(0, 16);
      display.print("mAH:");
      display.print(amp_hours);
      display.setCursor(104, 0);
      if (current_sensor_pin == LOW_CURRENT_PIN) display.println("LOW");
      if (current_sensor_pin == HIGH_CURRENT_PIN) display.println("HIGH");
      display.setCursor(0, 24);
      display.print("Time: ");
      display.print((millis() - sample_start_time) / 1000);
      display.print("s, rec ");
      display.print(UPDATE_INTERVAL);
      display.println("ms");


      //flash led2 indicating recording
      digitalWrite(LED_2, !digitalRead(LED_2));


    }
    else {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(3);
      display.print(amps);
      display.println(" A");
      display.setCursor(0, 24);
      display.setTextSize(1);
      display.print("mAH:");
      display.print(amp_hours);
      display.setCursor(104, 24);

      if (current_sensor_pin == LOW_CURRENT_PIN) display.print("LOW");
      if (current_sensor_pin == HIGH_CURRENT_PIN) display.print("HIGH");

    }
    display.display();

  }
}

