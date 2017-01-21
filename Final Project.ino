extern volatile unsigned long timer0_overflow_count; // Allows timer0_overflow_count from wiring.c to be editable
extern volatile unsigned long timer0_millis; // Same thing

const int low_pressure = 5;
const int good_pressure = 6;
const int high_pressure = 7;

const int low_frequency = 8;
const int good_frequency = 9;
const int high_frequency = 10;

const int compression_prompt_red = A3;
const int compression_prompt_green = 4;
const int AED_indicator = 11;
const int EMS_indicator = 12;

const int analog_slider = A2;
const int blue_button = A5;
const int yellow_button = A4;

const int speaker = A1; // A0

volatile float compression_depth_top = 0;
volatile float compression_depth_bottom = 0;
volatile float compression_depth = 0;

volatile int current_time = 0;
volatile int time_since_1 = 0;
volatile int time_since_2 = 0;
volatile int time_since_3 = 0;

volatile int good_depth = HIGH;
volatile int low_depth = LOW;
volatile int high_depth = LOW;

volatile int good_speed = HIGH;
volatile int low_speed = LOW;
volatile int high_speed = LOW;

volatile bool AED_state = false;
volatile bool EMS_state = false;

int time_since_start;

ISR(TIMER0_ovf_vect){ // Will give error but it works anyways
  timer0_overflow_count++;
}

unsigned long praiseOrion(){ //New, improved millis
  unsigned long m;
  uint8_t oldSREG = SREG;
  cli();
  m = timer0_millis;
  SREG = oldSREG;

  return m;
}

void setup() {
  
  Serial.begin(9600);

  time_since_start = 0;
  
  pinMode(low_pressure, OUTPUT);
  pinMode(good_pressure, OUTPUT);
  pinMode(high_pressure, OUTPUT);

  pinMode(low_frequency, OUTPUT);
  pinMode(good_frequency, OUTPUT);
  pinMode(high_frequency, OUTPUT);

  pinMode(AED_indicator, OUTPUT);
  pinMode(EMS_indicator, OUTPUT);
  pinMode(compression_prompt_red, OUTPUT);
  pinMode(compression_prompt_green, OUTPUT);

  pinMode(blue_button, INPUT);
  pinMode(yellow_button, INPUT);

  attachInterrupt(1, onInterrupt2, FALLING);
  attachInterrupt(0, onInterrupt, RISING);

}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 30; i++) {

    if (AED_state == true) {
      digitalWrite(AED_indicator, HIGH);
    } else {
      digitalWrite(AED_indicator, LOW);
    }

    if (EMS_state == true) {
      digitalWrite(EMS_indicator, HIGH);
    } else {
      digitalWrite(EMS_indicator, LOW);
    }

    digitalWrite(low_pressure, low_depth);
    digitalWrite(good_pressure, good_depth);
    digitalWrite(high_pressure, high_depth);
    
    digitalWrite(low_frequency, low_speed);
    digitalWrite(good_frequency, good_speed);
    digitalWrite(high_frequency, high_speed);
    delay(300);
    tone(speaker, 260);
    digitalWrite(compression_prompt_green, HIGH);
    digitalWrite(compression_prompt_red, LOW);
    delay(300);
    noTone(speaker);
    digitalWrite(compression_prompt_green, LOW);
    digitalWrite(compression_prompt_red, LOW);
  }
  if (AED_state == true) {
    digitalWrite(compression_prompt_red, HIGH);
    digitalWrite(compression_prompt_green, LOW);
    tone(speaker, 500);
    delay(5000);
    noTone(speaker);
  } else {
    digitalWrite(compression_prompt_red, LOW);
    digitalWrite(compression_prompt_green, HIGH);
    delay(6000);
    
    
  }
}

void onInterrupt() { 
  Serial.println("INTERRUPT");
  if (digitalRead(yellow_button) == HIGH) {
    if (EMS_state == false) {
      EMS_state = true;
      Serial.print("NEW EMS STATE: true");
    }

    delay(50);
  } else if (digitalRead(blue_button) == HIGH) {
    if (AED_state == false) {
      AED_state = true;
      Serial.print("NEW AED STATE: true");
    } else if (AED_state == true) {
      AED_state = false;
      Serial.print("NEW AED STATE: false");
    }
    delay(50);
  } else {
    Serial.println("reading slider");
    // SLIDER HANDLER:

      time_since_3 = time_since_2;
      time_since_2 = time_since_1;
      time_since_1 = millis() - current_time;
      current_time = millis();
  
      float avg_time = ((float)time_since_3 + (float)time_since_2 + (float)time_since_3)/3.0;

      Serial.println(avg_time);
    
      if (avg_time < 400) {
        low_speed = LOW;
        good_speed = LOW;
        high_speed = HIGH;
      } else if (avg_time > 800) {
        low_speed = HIGH;
        good_speed = LOW;
        high_speed = LOW; 
      } else {
        low_speed = LOW;
        good_speed = HIGH;
        high_speed = LOW;
      }

      compression_depth_top = analogRead(analog_slider);

      Serial.print("RISING SLIDER: ");
      Serial.println(compression_depth_top);

      compression_depth = compression_depth_bottom - compression_depth_top;

      Serial.print("DEPTH READING: ");
      Serial.println(compression_depth);

      if (compression_depth > 500) {
        low_depth = LOW;
        good_speed = LOW;
        high_depth = HIGH;
      } else if (compression_depth < 100) {
        low_depth = HIGH;
        good_depth = LOW;
        high_depth = LOW; 
      } else {
        low_depth = LOW;
        good_depth = HIGH;
        high_depth = LOW;
      }
    }
}

void onInterrupt2(){

  compression_depth_bottom = analogRead(analog_slider);
  Serial.print("FALLING SLIDER: ");
  Serial.println(compression_depth_bottom);
}

