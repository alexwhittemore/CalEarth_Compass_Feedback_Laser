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
  pinMode(12, OUTPUT);
  pinMode(trigger_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(trigger_pin), trigger, FALLING);
}

void trigger() {
  triggered = HIGH;
  this_trigger_time = micros();
  calc_rotation_duration = this_trigger_time - last_trigger_time;
  if (calc_rotation_duration > 50000) {
    // There's some kind of glitch where last_rotation_duration is 1160 or something. Accept only rational values for rotation rate.
    last_rotation_duration = calc_rotation_duration;
  }
  last_trigger_time = this_trigger_time;
}

int count = 100;
float current_rotation_rate = 0; // Hz
float calc_rotation_rate = 0; // Hz

//bool degrees_pass_fail[365]

// the loop function runs over and over again forever
void loop() {
  // If 10 is low, light the laser for 10ms. We've just passed the white dot.
  if (triggered) {
    triggered = LOW;
    current_rotation_rate = 1000000.0 / last_rotation_duration;
    digitalWrite(12, HIGH);
    delayMicroseconds(count * 10);
    digitalWrite(12, LOW);

    //last_rotation_duration;
    Serial.println(current_rotation_rate);

    // Delay to prevent re-triggering on the same dot strike
    delay(15);
    --count;
    if (count == 0)
      count = 100;
  }
  /*
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(12, HIGH);
    delay(1000);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    digitalWrite(12, LOW);
    delay(1000);                       // wait for a second
  */
}
