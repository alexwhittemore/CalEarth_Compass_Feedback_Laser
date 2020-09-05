/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

const byte trigger_pin = 2;
const byte laser_pin = 12;
volatile byte triggered = LOW;
volatile unsigned long last_trigger_time = micros();
volatile unsigned long this_trigger_time = micros();
volatile unsigned long last_rotation_duration = 0;
volatile unsigned long calc_rotation_duration = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(laser_pin, OUTPUT);
  pinMode(trigger_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(trigger_pin), trigger, FALLING);
}

int count = 100;
float current_rotation_rate = 0; // Hz
float calc_rotation_rate = 0; // Hz

const uint16_t interval_count = 4;
bool quadrants_pass_fail[4] = {true, false, true, false}; // Instead of degrees, we'll test with quadrants. In this scenario, an "interval" is rotation_duration/4

unsigned long sweep_start_time = 0; // The time in micros() when we start our sweep.
const float sweep_start_delay = 198/360.0; // Degrees between the white dot pulse and true 0* elevation (or, 360*, since we're counting down from the top).
unsigned long sweep_start_delay_micros = 0; // The start delay translated to micros based on our current rotation rate.
unsigned long interval_time = 0;

void trigger() {
  triggered = HIGH;
  this_trigger_time = micros();
  calc_rotation_duration = this_trigger_time - last_trigger_time;
  if (calc_rotation_duration > 50000) {
    // There's some kind of glitch where last_rotation_duration is 1160 or something. Accept only rational values for rotation rate.
    last_rotation_duration = calc_rotation_duration;
  }
  last_trigger_time = this_trigger_time;
  sweep_start_delay_micros = sweep_start_delay * last_rotation_duration;
  sweep_start_time = this_trigger_time + sweep_start_delay_micros;
  interval_time = last_rotation_duration/interval_count;
}

unsigned long current_time = 0;
unsigned long interval_start_time = 0;
uint16_t interval = 0; // Start at the highest "degree" and count down (in this case, quadrant).

void loop() {
  current_time = micros();
  if (triggered && (current_time>sweep_start_time)) { // This is a bad comparison of timestamps rather than duration, but I can't seem to fix it easily.
    // We have started the sweep.
    triggered = LOW;
    // We're supposed to start our sweep
    interval = interval_count-1;
    interval_start_time = current_time;
    if (quadrants_pass_fail[interval]){
      digitalWrite(laser_pin, HIGH);
    } else {
      digitalWrite(laser_pin, LOW);
    }
  }
  if (current_time-interval_start_time >= interval_time) {
    // we're in the next interval
    interval_start_time = current_time;
    interval --;
    if (quadrants_pass_fail[interval]){
      digitalWrite(laser_pin, HIGH);
    } else {
      digitalWrite(laser_pin, LOW);
    }
  }
}
