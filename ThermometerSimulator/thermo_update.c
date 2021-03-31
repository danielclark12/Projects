#include "thermo.h"

int set_temp_from_ports(temp_t *temp){
    if(THERMO_SENSOR_PORT > 64000){
        return 1;
    }

    if((THERMO_SENSOR_PORT % 64) >= 32){
        temp->tenths_degrees = ((THERMO_SENSOR_PORT / 64) + 1) - 500;
    }
    else{
        temp->tenths_degrees = (THERMO_SENSOR_PORT / 64) - 500;
    }

    if((THERMO_STATUS_PORT & 1) == 1){
        temp->is_fahrenheit = 1;
        temp->tenths_degrees = ((temp->tenths_degrees * 9) / 5) + 320;
    }
    else{
        temp->is_fahrenheit = 0;
    }
    return 0;


}
// max c tenths is 500 low is -500// max f -580 1220
int set_display_from_temp(temp_t temp, int *display){
    int bit_patterns[10] = {0b1111110,0b0001100,0b0110111,0b0011111, // array of all bit patterns
    0b1001101,0b1011011,0b1111011,0b0001110,0b1111111,0b1011111};

    int negative = 0b0000001; // pattern for negative sign

    if(temp.is_fahrenheit == 1){// 1 for f 0 for c
        if(temp.tenths_degrees > 1220 || temp.tenths_degrees < -580)
        return 1;
    }
    else if(temp.is_fahrenheit == 0){ // temp is C
        if(temp.tenths_degrees > 500 || temp.tenths_degrees < -500)
        return 1;
    }
    else{ // temp is something other than C or F
        return 1;
    }

    int bit = 0;
    int deg = temp.tenths_degrees;
    if(deg < 0){ // check for a negative number and add on bits
        bit |= negative;
        bit = bit << 7;
        deg = -deg;
    }
    // get each digit individually
    int temp_tenths = deg % 10;
    int temp_ones = (deg / 10) % 10;
    int temp_tens = (deg / 100) % 10;
    int temp_hundreds = (deg / 1000) % 10;

    //check if hundreds and tens are not 0 and shift accordingly
    if(temp_hundreds != 0){
        bit |= bit_patterns[temp_hundreds];
        bit = bit << 7;
    }
    if(temp_tens != 0){
        bit |= bit_patterns[temp_tens];
        bit = bit << 7;
    }

    bit |= bit_patterns[temp_ones];
    bit = bit << 7;
    bit |= bit_patterns[temp_tenths];

    if(temp.is_fahrenheit == 1){
        negative = negative << 29; // turn on f bit
        bit |= negative;
    }
    else{
        negative = negative << 28; // turn on c bit
        bit |= negative;
    }

    *display = bit;

    return 0;
}

int thermo_update(){
    temp_t temp;
    THERMO_DISPLAY_PORT = set_temp_from_ports(&temp);
    // checks for errors from both functions
    if(set_temp_from_ports(&temp) == 1)
        return 1;
    else if(set_display_from_temp(temp,&THERMO_DISPLAY_PORT) == 1)
        return 1;

    else{
        set_display_from_temp(temp,&THERMO_DISPLAY_PORT); // sets the display using correct temps
    }
    return 0;

}
