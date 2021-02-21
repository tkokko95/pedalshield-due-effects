/*
A simple bitdepth reducer, resulting in a cool Lo-Fi sound
*/

// Setting up vars for ADC/DAC and the potentiometers and the shift value
int ADC0_in, ADC1_in, DAC0_out, DAC1_out;
int POT0, POT2;
int shift;

// Unused constants for the LED, the stomp-switch and the programmable switch
// Left here for future reference

/*
const int LED = 3;
const int STOMP = 7; 
const int TGL = 2; 
*/

void setup() {
    // Setting up the CPU for ADC/DAC
    ADC->ADC_MR |= 0x80;   // DAC in free running mode.
    ADC->ADC_CR=2;         // Starts ADC conversion.
    ADC->ADC_CHER=0x1CC0;  // Enable ADC channels 0,1,8,9 and 10  
    analogWrite(DAC0,0);  // Enables DAC0
    analogWrite(DAC1,0);  // Enables DAC1

}

void loop() {

    while((ADC->ADC_ISR & 0x1CC0)!=0x1CC0);// wait for ADC 0, 1, 8, 9, 10 conversion complete.
    ADC0_in=ADC->ADC_CDR[7];               // read data from ADC0
    ADC1_in=ADC->ADC_CDR[6];               // read data from ADC1   
    POT0=ADC->ADC_CDR[10];               // read data from ADC8 
    //POT1=ADC->ADC_CDR[11];                 // read data from ADC9 (unused, remember to declare if you're going to use this)
    POT2=ADC->ADC_CDR[12];                 // read data from ADC10

    /* 
    POT0 controls the bitshift amount. Depending on the pot, the bits will be shifted
    by 0-3 bits.
    */
    shift = map(POT0, 0, 4095, 0, 3);

    /*
    This is where the actual signal processing happens. The bits will be shifted
    to the left, reducing the bit depth. This will introduce quantization noise
    to the signal.
    */
    ADC0_in = ADC0_in << shift;
    ADC1_in = ADC1_in << shift;

    // POT2 controls the output volume
    DAC0_out = map(ADC0_in, 0, 4095, 1, POT2);
    DAC1_out = map(ADC1_in, 0, 4095, 1, POT2);

    dacc_set_channel_selection(DACC_INTERFACE, 0);          //select DAC channel 0
    dacc_write_conversion_data(DACC_INTERFACE, DAC0_out);   //write on DAC
    dacc_set_channel_selection(DACC_INTERFACE, 1);          //select DAC channel 1
    dacc_write_conversion_data(DACC_INTERFACE, DAC1_out);   //write on DAC
}