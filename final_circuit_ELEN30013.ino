#include "TM1637.h"
#define CLK 2//pins definitions for TM1637 and can be changed to other ports       
#define DIO 3
#define HEATING_PIN 9
#define COOLING_PIN 10
TM1637 tm1637(CLK,DIO);

/* FOR SIGNAL CONDITIONING */
double period;
unsigned long result;
float Ra;
unsigned long constant = 23322;
double temperature;
int j;
double one_over_t;
double first_term, second_term, third_term;
int NUM_SAMPLES = 23;
double sum_period = 0;
double error;

/* FOR CONTROLLER */
const int  buttonPin1 = 11, buttonPin2 = 12, buttonPin3 = 13 ; 
int buttonPushCounter[] = {0,0};   // counter for the number of button presses
int buttonState1 = 0, buttonState2 = 0, buttonState3 = 0;         // current state of the button
int lastButtonState1 = 0, lastButtonState2 = 0, lastButtonState3 = 0;     // previous state of the button
int set_temp = 25;

void setup() {
  
  Serial.begin(9600);
  pinMode(A0,INPUT);
  pinMode(HEATING_PIN, OUTPUT);  // HEATING
  pinMode(COOLING_PIN, OUTPUT); // COOLING 

  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);

  // For 4-module, 7-segment display
  tm1637.init();
  tm1637.set(BRIGHTEST);
}

void loop() {

  checkInputButtons();
  
  getAveragePeriod();

  calculateThermistorResistance();

  calculateTemperature();
  
  displayTemperature();

  error = set_temp - temperature; //Serial.print(" ERROR :"); Serial.println(error);

  driving_circuit(&error);

  // Reset Values
  j = 0;
  sum_period = 0;
  
}

/*--------------------------------------------------------------------------------------------------*/
void calculateTemperature(){
  //temperature = 0.1879*pow(Ra,2) - 7.6421*Ra + 84.014;
  first_term = 0.96*pow(10,4);
  second_term = (1.98*pow(10,3))*log(Ra);
  third_term = 6.99*pow(log(Ra),3);

  /*first_term = 2.567*pow(10,4);
  second_term = (-0.7641*pow(10,3))*log(Ra);
  third_term = 18.904*pow(log(Ra),3);*/

  one_over_t = first_term + second_term + third_term; //Serial.print("one_over_t: "); Serial.println(one_over_t);  
  temperature = (pow(10,7)/one_over_t)-273.0; //Serial.print("  TEMPERATURE: "); Serial.print(temperature);
}

/*--------------------------------------------------------------------------------------------------*/
void calculateThermistorResistance(){
    // Get Thermistor Resistance
  Ra = (1.625*period)-constant; //Serial.print("Ra: "); Serial.print(Ra);
  //Serial.print("    FREQUENCY (Hz):"); Serial.println( 1000000/period );
}

/*--------------------------------------------------------------------------------------------------*/

void driving_circuit(double *error){
   if(*error >= 3){
    analogWrite(COOLING_PIN, LOW); 
    digitalWrite(HEATING_PIN, 200);
    }
    else if(*error < 3 && *error >= 2){
    analogWrite(COOLING_PIN, LOW); 
    digitalWrite(HEATING_PIN, 50);
    }
    else if(*error < 2 && *error >= 1){
    analogWrite(COOLING_PIN, LOW); 
    digitalWrite(HEATING_PIN, 5);
    }
   else if(*error < 1 && *error >= 0.7){
    analogWrite(COOLING_PIN, LOW); 
    digitalWrite(HEATING_PIN, 2);
    }
    else if(*error < 0.7 && *error >= 0.4){
    analogWrite(COOLING_PIN, LOW); 
    digitalWrite(HEATING_PIN, LOW);
    }
   
    else if(*error < 0.4  && *error >= 0.1){
    analogWrite(COOLING_PIN, LOW); 
    digitalWrite(HEATING_PIN, LOW);
    }
   else if(*error < 0.1 && *error >= -0.1){
    analogWrite(COOLING_PIN, LOW); 
    digitalWrite(HEATING_PIN, LOW);
    }

   else if(*error < -0.1 && *error >= -0.4){
      analogWrite(COOLING_PIN, LOW); 
      digitalWrite(HEATING_PIN, LOW);
    } 
 
   else if(*error < -0.4 && *error >= -0.7){
      analogWrite(COOLING_PIN, LOW); 
      digitalWrite(HEATING_PIN, LOW);
    }

   else if(*error < -0.7 && *error >= -1){
      analogWrite(COOLING_PIN, 2); 
      digitalWrite(HEATING_PIN, LOW);
    } 
   else if(*error < -1 && *error >= -2){
      analogWrite(COOLING_PIN, 5); 
      digitalWrite(HEATING_PIN, LOW);
    } 
   else if(*error < -2 && *error >= -3){
      analogWrite(COOLING_PIN, 50); 
      digitalWrite(HEATING_PIN, LOW);
    } 
   else if(*error < -3){
      analogWrite(COOLING_PIN, 200); 
      digitalWrite(HEATING_PIN, LOW);
    }
  
  }

/*--------------------------------------------------------------------------------------------------*/

void getAveragePeriod(){
  
  for(j = 0; j < NUM_SAMPLES; j = j + 1){
    sum_period = pulseIn(A0,HIGH) + pulseIn(A0,LOW) + sum_period;
    //Serial.print("sum_period: "); Serial.println(sum_period);    
  }
  
  period = sum_period/NUM_SAMPLES; //Serial.print("PERIOD: "); Serial.println(period);
}

/*--------------------------------------------------------------------------------------------------*/

void displayTemperature(){
  // Convert temperature to String
  String disp = String(temperature,2); //Serial.println(disp);
  if(temperature < 10){
      tm1637.display(0,'0');
      tm1637.display(1,disp[0]-'0');
      tm1637.display(2,disp[2]-'0');
      tm1637.display(3,disp[3]-'0');
    }
  else{
      tm1637.display(0,disp[0]-'0');
      tm1637.display(1,disp[1]-'0');
      tm1637.display(2,disp[3]-'0');
      tm1637.display(3,disp[4]-'0');
      }
}

/*--------------------------------------------------------------------------------------------------*/

void checkInputButtons(){

  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);
  buttonState3 = digitalRead(buttonPin3);
  
  // compare the buttonState to its previous state
  if (buttonState1 != lastButtonState1) {
    if (buttonState1 == HIGH) {
      if (buttonPushCounter[0] == 9)
        buttonPushCounter[0] = 0;
      else 
        buttonPushCounter[0] = buttonPushCounter[0]+1;
        
      //Serial.print("button1: ");
      Serial.print(buttonPushCounter[0]);
      Serial.println(buttonPushCounter[1]);
    } 
    // Delay a little bit to avoid bouncing
    delay(50);
    lastButtonState1 = buttonState1;
  }
  else if(buttonState2 != lastButtonState2){
    if (buttonState2 == HIGH) {
      if (buttonPushCounter[1] == 9)
        buttonPushCounter[1] = 0;
      else 
        buttonPushCounter[1] = buttonPushCounter[1]+1;
        
      //Serial.print("button2: ");
      Serial.print(buttonPushCounter[0]);
      Serial.println(buttonPushCounter[1]);
      
    } 
    // Delay a little bit to avoid bouncing
    delay(50);
    lastButtonState2 = buttonState2;
  }
  else if(buttonState3 != lastButtonState3){
    if (buttonState3 == HIGH) {
      set_temp = 10*buttonPushCounter[0]+buttonPushCounter[1];
      Serial.print("SET_TEMP: "); Serial.print(set_temp);
      buttonPushCounter[0] = 0;
      buttonPushCounter[1] = 0;
    } 
    // Delay a little bit to avoid bouncing
    delay(50);
    lastButtonState3 = buttonState3;
  }
}


