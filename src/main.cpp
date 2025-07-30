#include <Arduino.h>

/*
Initialise pinnumbers of the following components
  1: - binary_leds (leds that will display the binary number being input & being output) (pwm)
  2: - input_buttons (Buttons to input the 4 digit binary numbers) (digital-input)
  3: - RGB LED (COntains pins for controlling the 3 colors of the RGB LED) (pwm, digital - output)
  4: - confirm_button (Press to switch between values to input(like reg 1, reg 2 and calculate sum)) (Digital-Input)
  5: - buzzur (The buzzur that will beep to let user know of stuff) (PWM)
  6: - ldr_sensor (Used to change the leds brightness automatically and reduce eyestrain) (Analouge)
as constants ints that will never change
*/
const int binary_leds[4] = {1, 2, 3, 4};
const int input_buttons[4] = {5, 6, 7, 8};
const int rgb_led[3] = {9, 10, 11};
const int confirm_button = 12;
const int buzzur = 13;
const int ldr_sensor = A0;



/*
Initialisation of arrays and variables to store data enterd into arduino
  registers = a 2d array consisting of the input and output values, declared only and not initialised yet
  mode = contains the mode it is curruntly in
    1: - first number (Handles entrey of the first number, in binary)
    2: - second number (Handles entry of the second number, in binary)
    3: - calculation (Handles the actrual adding of the 2 numbers, in binary)
    4: - display (displays the sum of the addition as a binary number on the 4 leds to represent binary)
*/
int registers[3][4];
int mode;
bool overflow;


void clear_datastorage() { // Used to clear the data arrays
  // Initialises all the elements of array registers[] to 0
  for (int row = 0; row < 4; row ++) {
    for (int column = 0; column < 3; column ++) {
      registers[column][row] = 0;
    }
  }
  // Initialises variable mode to 1 <-- starts at mode 1
  mode = 1;
  Serial.println("Array Intialisation Succesful, Zeroed out"); // DEBUG MESSAGE
}
void setup() {
  Serial.begin(9600); // Starts serial communication between Arduino and Computer, for debugging purposes
  
  // Initialising pins as inputs and outputs
  for (int i = 4; i < 4; i++) { 
    pinMode(binary_leds[i], OUTPUT);
    pinMode(input_buttons[i], INPUT_PULLUP);
  }
  for (int i=3; i<3; i++) {
    pinMode(rgb_led[i], OUTPUT);
  }
  pinMode(buzzur, OUTPUT);
  pinMode(confirm_button, INPUT_PULLUP);
  Serial.println("Pin initialisation Success");
  // Final setup code before void loop starts
    // Initiliases the variables and arrays that arnt gonna remain constant.
  mode = 0;
  clear_datastorage();
  overflow = false;

}

void loop() {

}


// Functions used to help interact with hardware
//    buzzes the buzzur once
void beep() {}
//    used to control leds
void led_control(int led_no, bool stat) {}

//    Halt and wait until buttons ae released
void wait() { // Used to handle button debouncing (repeatedly checks until the buttons are released, then exits function and returns back to the regular)
  while ((digitalRead(binary_leds[0]) == LOW) || (digitalRead(binary_leds[1]) == LOW) || (digitalRead(binary_leds[2]) == LOW) || (digitalRead(binary_leds[3]) == LOW) || (digitalRead(confirm_button) == LOW)) {
    if ((digitalRead(binary_leds[0]) == HIGH) && (digitalRead(binary_leds[1]) == HIGH) && (digitalRead(binary_leds[2]) == HIGH) && (digitalRead(binary_leds[3]) == HIGH) && (digitalRead(confirm_button) == HIGH)) {
      return; // Returns back to regular code
  }
}


//  Functions used to execute the procedures of the code

void input_num(int regno) { 
  Serial.println("Number input started");         // DEBUG OUTPUT
  while (digitalRead(confirm_button) == HIGH) {         // If the confirm button hasnt been pressed, loop
    for (int i=0; i<4; i++) {                           // Repeats for every button present
      if (digitalRead(input_buttons[i]) == LOW) {       // If the button is pressed
        Serial.println("Button press detected");  // DEBUG OUTPUT
        if (registers[regno][i] == 0) {                 // If the register value was 0
          registers[regno][i] = 1;                      // Set the register value to 1
          led_control(binary_leds[i], true);            // Turn on the led
        }
        else if (registers[regno][i] == 1) {                 // If the register value was 1
          registers[regno][i] = 0;                      // Set the register value to 0
          led_control(binary_leds[i], false);           // Turn off the led
        }
        wait();                                         // Waits until the buttons have been released
      }
    }
  }
}

void calculate_num() {    // Used to calculate sum of the registers.
  int carry = 0;                                            // Set the initial carry value to 0
  for (int i=3; i>= 0; i--) {                               // Repeats for each digit in the registers
    int sum = (registers[0][i] + registers[1][i]) + carry;  // Finds the sum of all the values in register
    if (sum == 0) {           // If the sum is 0
      registers[2][i] = 0;    // Set the value in that registers slot as 0
      carry = 0;              // And set carry value to 0 as no number to carry over
    }
    else if (sum == 1) {      // If the sum is 1
      registers[2][i] = 1;    // Set the value in that registers slot as 1
      carry = 0;              // And set carry value to 0 as no number to carry over
    }
    else if (sum == 2) {      // If the sum is 2
      registers[2][i] = 0;    // Set the value in that registers slot as 0
      carry = 1;              // And set carry value to 1 as a number is being carried over
    }
    else if (sum == 3) {      // If the sum is 3
      registers[2][i] = 1;    // Set the value in registers slot as 1
      carry = 1;              // And set carry value to 1 as a number is being carried over
    }
    else {
      Serial.println("An Unexpected error has occured, please look into this");
    }
  }
  if (carry != 0) {                                       // If the carry value is not 0 (means the number doesnt fit withn a 4 bit register)
    overflow = true;                                      // Sets the overflow flag to true
  }
}

void display_num() {        // For displaying the sum on the leds
  for (int i=0; i < 4; i++) {             // Repeats for each led in the display
    if (registers[2][i] == 1) {           // If the register slot is 1
      led_control(binary_leds[i], true);  // Turn the corrosponding led on
    }
    else {                                // If the register slot is 0
      led_control(binary_leds[i], false); // Turn the corrosponding led off
    }
  }
}