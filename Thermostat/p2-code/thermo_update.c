#include "thermo.h"

int set_temp_from_ports(temp_t *temp) {
// Uses the two global variables (ports) THERMO_SENSOR_PORT and
// THERMO_STATUS_PORT to set the fields of `temp`. If
// THERMO_SENSOR_PORT is negative or above its maximum trusted value
// (associated with +45.0 deg C), this function sets the
// tenths_degrees to 0 and the temp_mode to 3 for `temp` before
// returning 1.  Otherwise, converts the sensor value to deg C using
// shift operations.  Further converts to deg F if indicated from
// THERMO_STATUS_PORT. Sets the fields of `temp` to appropriate
// values. `temp_mode` is 1 for Celsius, and 2 for Fahrenheit. Returns
// 0 on success. This function DOES NOT modify any global variables
// but may access them.
//
// CONSTRAINTS: Uses only integer operations. No floating point
// operations are used as the target machine does not have a FPU. Does
// not use any math functions such as abs().
    if(THERMO_SENSOR_PORT > 28800 || THERMO_SENSOR_PORT < 0 || THERMO_STATUS_PORT & (1 << 2)) { // Checking if the sensors report an error
        temp -> tenths_degrees = 0;
        temp -> temp_mode = 3;
        return 1;
    }

    int mask = 0b11111;
    mask = mask & THERMO_SENSOR_PORT; // Gets the first 5 numbers of the bit string
    temp -> tenths_degrees = (THERMO_SENSOR_PORT >> 5) - 450; // Convert the number into Celsius
    temp -> temp_mode = 1; // Make the temprature mode into Celsius

    if(mask >= 16) { // If the sensor is above 16, round the number up
        temp -> tenths_degrees += 1;
    }
    
    if(THERMO_STATUS_PORT & (1 << 5)) { // Checks if the status port is farenheit
         temp -> tenths_degrees = (temp -> tenths_degrees * 9) / 5 + 320; // Converts temperature to farenheit
         temp -> temp_mode = 2; // Switch temp mode to farenheit
         return 0;
    }


    return 0;
}
int set_display_from_temp(temp_t temp, int *display) {
// Alters the bits of integer pointed to by display to reflect the
// temperature in struct arg temp.  If temp has a temperature value
// that is below minimum or above maximum temperature allowable or if
// the temp_mode is not Celsius or Fahrenheit, sets the display to
// read "ERR" and returns 1. Otherwise, calculates each digit of the
// temperature and changes bits at display to show the temperature
// according to the pattern for each digit.  This function DOES NOT
// modify any global variables except if `display` points at one.
// 
// CONSTRAINTS: Uses only integer operations. No floating point
// operations are used as the target machine does not have a FPU. Does
// not use any math functions such as abs().
    int zero = 0b0000000;

    if((temp.tenths_degrees > 450 || temp.tenths_degrees < -450) && temp.temp_mode == 1){ // Checks if the temperature is out of bounds in celsius
        zero = zero | (0x0 << 28);
        zero = zero | (0b0110111 << 21); // Shift the bits 21 to the left
        zero = zero | (0b1011111 << 14); // Shift the bits 14 to the left
        zero = zero | (0b1011111 << 7);  // Shift the bits 7 to the left
        zero = zero | (0x0 << 0);
        *display = zero; // Set the pointer to display equal to the counter variable
        return 1;
    }
    else if ((temp.tenths_degrees > 1130 || temp.tenths_degrees < -490) && temp.temp_mode == 2){
        zero = zero | (0x0 << 28);
        zero = zero | (0b0110111 << 21); // Shift the bits 21 to the left
        zero = zero | (0b1011111 << 14); // Shift the bits 14 to the left
        zero = zero | (0b1011111 << 7); // Shift the bits 7 to the left
        zero = zero | (0x0 << 0);
        *display = zero; // Set the pointer to display equal to the counter variable
        return 1;
    }
    else if (temp.temp_mode != 1 && temp.temp_mode != 2) {
        zero = zero | (0x0 << 28);
        zero = zero | (0b0110111 << 21); // Shift the bits 21 to the left
        zero = zero | (0b1011111 << 14); // Shift the bits 14 to the left
        zero = zero | (0b1011111 << 7);  // Shift the bits 7 to the left
        zero = zero | (0x0 << 0);
        *display = zero; // Set the pointer to display equal to the counter variable
        return 1;
    }
   
    int x = 0;

    int digits [10]; // Make an array of digit bit strings
    digits[0] = 0b1111011;
    digits[1] = 0b1001000;
    digits[2] = 0b0111101;
    digits[3] = 0b1101101;
    digits[4] = 0b1001110;
    digits[5] = 0b1100111;
    digits[6] = 0b1110111;
    digits[7] = 0b1001001;
    digits[8] = 0b1111111;
    digits[9] = 0b1101111;

    int divisor = temp.tenths_degrees;

    if(divisor < 0) { // Checks if the degree is less than 0
        divisor = divisor / -1;
        x = 1;
    }

    int temp_tenths = divisor % 10; 
    divisor = divisor / 10;         // Divide remainder by 10 to get 10ths remiander

    int temp_ones = divisor % 10;  
    divisor = divisor / 10;         // Divide remainder by 10 to get ones remiander

    int temp_tens = divisor % 10;   
    divisor = divisor / 10;         // Divide remainder by 10 to get tens remiander

    int temp_hundreds = divisor % 10; // Get hundreds remainder
    
    if(temp.temp_mode == 1) {       // Checks if the temp mode is in Celsius
        zero = zero |(0x1 << 28);   // Left shifts the bits by 28 positions
    }
    else {
        zero = zero |(0x1 << 29); // Left shifts the bits by 29
    }

    if(temp_hundreds > 0) {
        zero = zero | (digits[temp_hundreds] << 21); // Left shift the hundreds binary bits by 21 positions
    }

    // Checks if the tens or hundreds position is valid
    if((temp_tens != 0 && temp_hundreds != 0) || (temp_tens == 0 && temp_hundreds != 0) || (temp_tens != 0 && temp_hundreds == 0)) { 
        zero = zero | (digits[temp_tens] << 14); // Left shift the tens binary bits by 14 positions
    }

    if(x == 1 && temp_tens == 0 && temp_hundreds == 0){ // Checks if the degrees are less than 0 and the tens and hundreds positions are 0
        zero = zero | 0b0000100 << 14;
    }
    else if(x == 1 && temp_tens != 0 && temp_hundreds == 0){ // Checks if the degrees are less than 0 and the tens position is not equal to 0
        zero = zero | 0b0000100 << 21;
    }

    zero = zero | (digits[temp_ones] << 7); // Left shift the ones position by 7
    
    zero = zero | (digits[temp_tenths] << 0); // Left shift the tenths position by 1

    *display = zero; // Set pointer of display equal to the temp variable
    return 0;
}

int thermo_update() {
// Called to update the thermometer display.  Makes use of
// set_temp_from_ports() and set_display_from_temp() to access
// temperature sensor then set the display. Always sets the display
// even if the other functions returns an error. Returns 0 if both
// functions complete successfully and return 0. Returns 1 if either
// function or both return a non-zero values.
// 
// CONSTRAINT: Does not allocate any heap memory as malloc() is NOT
// available on the target microcontroller.  Uses stack and global
// memory only.

    temp_t tmp; // Make a temp_t counter variable
    int display = 0;
    int funcone = set_temp_from_ports(&tmp); // Call set_temp_from_ports on tmp variable
    int functwo = set_display_from_temp(tmp, &display); // Call set_display_from_temp on tmp and display variables
    THERMO_DISPLAY_PORT = display;
    if (funcone == 0 && functwo == 0){ // Checks if the functions ran properly
        return 0;
    }
    else {
        return 1;
    }
}