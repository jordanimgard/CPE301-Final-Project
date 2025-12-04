// Code Contributors: Victor Dang, Jordan Imgard
// CPE 301 Group 13

#define RDA 0x80
#define TBE 0x20

// UART Pointers
volatile unsigned char *myUCSR0A  = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B  = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C  = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0   = (unsigned int *)0x00C4;
volatile unsigned char *myUDR0    = (unsigned char *)0x00C6;

// GPIO Pointers
volatile unsigned char *portB     = (unsigned char *)0x25;
volatile unsigned char *portDDRB  = (unsigned char *)0x24;
unsigned char* ddr_h = (unsigned char*) 0x101;
unsigned char* port_h = (unsigned char*) 0x102;

// Timer Pointers
volatile unsigned char *myTCCR1A  = (unsigned char *)0x80;
volatile unsigned char *myTCCR1B  = (unsigned char *)0x81;
volatile unsigned char *myTCCR1C  = (unsigned char *)0x82;
volatile unsigned char *myTIMSK1  = (unsigned char *)0x6F;
volatile unsigned char *myTIFR1   = (unsigned char *)0x36;
volatile unsigned int  *myTCNT1   = (unsigned int *)0x84;

//ADC Pointers for water sensor
volatile unsigned char *my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char *my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char *my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int *my_ADC_DATA = (unsigned int*) 0x78;


// setup all required functions, timers, lcd, motors, etc.
void setup() 
{

    //setup the LEDs required, red, green, blue, yellow
    //RED LED OUTPUT PH6 -> PIN 9
    *ddr_h |= 0b01000000; //RED
    /*
        to turn led ON in other functions: *port_h |= 0b01000000;
        OFF: *port_h &= 0b10111111;
    */

    //YELLOW LED OUTPUT PH5 -> PIN 8
    *ddr_h |= 0b00100000; //YELLOW
    /*
        to turn led ON in other functions: *port_h |= 0b00100000;
        OFF: *port_h &= 0b11011111;
    */

    //GREEN LED OUTPUT PH4 -> PIN 7
    *ddr_h |= 0b00010000; //GREEN
    /*
        to turn led ON in other functions: *port_h |= 0b00010000;
        OFF: *port_h &= 0b11101111;
    */

    //BLUE LED PB4 -> PIN 10
    *ddr_b |= 0b00010000; //BLUE
    /*
        to turn led ON in other functions: *port_b |= 0b00010000;
        OFF: *port_b &= 0b11101111;
    */

    // Start the UART
    U0Init(9600);

	adc_init();
}

void loop()
{
	
    //monitor and constantly update as the temperature is monitored and the water level
	//WATER SENSOR READING:
	unsigned int waterValue = adc_read(0);
	unsigned int threshold = 200;
	//if temperature drops below threshold
	if (waterValue < threshold) {
	//Turn on RED LED for ERROR:
		*port_h |= 0b01000000; // RED ON
		*port_h &= ~(0b00010000); //GREEN OFF
	} else {
		*port_h |= 0b00010000; // GREEN ON
		*port_h &= ~(0b01000000); //RED OFF
	}
	
	
    
    //transition to state IDLE w timestamp

    //if water level becomes too low
    //transition to state ERROR w timestamp

}

/*
 * UART FUNCTIONS
 */
void adc_init() {
    *my_ADCSRA |= 0b10000000;       // enable ADC
    *my_ADCSRA &= 0b11011111;       // disable trigger
    *my_ADCSRA &= 0b11110111;       // disable interrupt
    *my_ADCSRA &= 0b11111000;       // prescaler

    *my_ADCSRB &= 0b11110111;       // reset channel/gain
    *my_ADCSRB &= 0b11111000;       // free running

    *my_ADMUX &= 0b01111111;        // clear REFS1
    *my_ADMUX |= 0b01000000;        // AVCC reference
    *my_ADMUX &= 0b11011111;        // right adjust
    *my_ADMUX &= 0b11100000;        // reset channel
}
unsigned int adc_read(unsigned char adc_channel_num) 
{
    *my_ADMUX &= 0b11100000;        // clear channel bits
    *my_ADCSRB &= 0b11110111;       // clear MUX5

    if (adc_channel_num > 7) {
    	adc_channel_num -= 8;
        *my_ADCSRB |= 0b00001000;
    }
    *my_ADMUX += adc_channel_num;

    *my_ADCSRA |= 0b01000000;       // start conversion

    while((*my_ADCSRA & 0x40) != 0); // wait for conversion

    return *my_ADC_DATA;             // return value
}


void U0Init(unsigned long U0baud)
{
	unsigned long FCPU = 16000000;
	unsigned int tbaud;
	tbaud = (FCPU / 16 / U0baud - 1);
	*myUCSR0A = 0x20;
	*myUCSR0B = 0x18;
	*myUCSR0C = 0x06;
	*myUBRR0 = tbaud;
}

unsigned char kbhit() 
{
	return *myUCSR0A & RDA;
}

unsigned char getChar() 
{
	return *myUDR0;
}

void putChar(unsigned char U0pdata)
{
	while((*myUCSR0A & TBE) == 0);
  *myUDR0 = U0pdata;
}
