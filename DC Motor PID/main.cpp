/*
 * DC Motor PID.cpp
 *
 * Created: 2017-07-29 3:04:13 PM
 * Author : Tony
 */ 

// Include statements
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

/* The control frequency must divide evenly (result in an integer) into the count frequency
*/
#define COUNT_FREQ 15625
#define CTRL_FREQ 125

/* Calibration settings for the rotary encoder
*/
#define COUNTS_PER_ROTATION 22
#define ROTATIONS_PER_OUTPUT_ROTATION 44
#define DEGREES_PER_ROTATION 360

/* PID settings
*/
#define KCONST 1024 //Multiplication constant to normalize the constants below
#define KP 1024
#define KI 50
#define KD 50

/*  Encoder information
 *   encLastState holds the values of the previous state of the encoder sensor values
 *   encState holds the current state of the encoder sensor values
 *   There are 11 pairs of poles - 11N and 11S on the encoder wheel
 *   The ratio of motor turns to output shaft turns is 44
 *   We will trigger count on the rising edge of one channel and if both encoder signals are the same it is going +'ve direction
 *   and if they are different is is -'ve
*/
volatile uint16_t encCount;
volatile uint16_t encLastCount;
volatile int16_t exLast;
volatile int16_t integrationSum;
volatile uint16_t spx;

const float degPerCnt = DEGREES_PER_ROTATION/(COUNTS_PER_ROTATION*ROTATIONS_PER_OUTPUT_ROTATION);

int main(void)
{

	// Setup the encoder pins and the motor control PWM pins
	DDRB &= (uint8_t)~((1<<PORTB0)|(1<<PORTB1));
	DDRB |= (1<<PORTB2)|(1<<PORTB3);
	// Turn on the pin change interupts
	PCICR |= (1<<PCIE0);
	PCMSK0 |= (1<<PCINT0);

	// Start the PWM signal
	TCCR1A |= (1<<COM1B1)|(1<<WGM10);
	TCCR1B |= (1<<CS10);
	TCCR2A |= (1<<COM2A1)|(1<<WGM20);
	TCCR2B |= (1<<CS20);
	  
	// Set PWM to 0 to start
	OCR1BL = 0;
	OCR2A = 0;

	// Setup timer0 for control loop
	TCCR0A |= (1<<WGM01);
	TCCR0B |= (1<<CS02)|(1<<CS00);
	OCR0A = COUNT_FREQ/CTRL_FREQ-1;
	TIMSK0 |= (1<<OCIE0A);
	// Initialize the variables
	encCount=0;
	spx=0;
	exLast=0;
    /* Replace with your application code */
    while (1) 
    {
    }
}

/* Control loop
    Description:  This is the control loop PID portion of the controller. Allow the interrupts to continue while
    the control loop is running so that no steps are lost during the loop.
*/
ISR(TIMER0_COMPA_vect, ISR_NOBLOCK){
  // Control loop
  uint16_t x = encCount;
  int16_t ex = (spx-x);
  integrationSum += ex;

  int16_t output = ((KP*ex)+(KD*(ex-exLast)*CTRL_FREQ)+(KI*integrationSum/CTRL_FREQ))/KCONST;
  if (output>=0){
    if (output>255){
      output = 255;
    }
    OCR1BL = output;
    OCR2A = 0;
  }
  else{
    if(output<-255){
      output=255;
    }
    else{
      output*=-1;
    }
    OCR1BL = 0;
    OCR2A = output;
  }
  
  /* Saving this for implementation later
	 uint16_t dx = x-encLastCount;
	 int16_t velocity = dx*CTRL_FREQ;
  */
  exLast = ex;
  encLastCount = x;
}

/* Handle the change from
   Pin 8 and 9 are the pin change interrupt pins to be used - PB0 and PB1 respectively
*/
ISR(PCINT0_vect){
  // Anytime the pinstate changes on the two pins used for the encoder, we will read their state and decide the direction
  uint8_t encState = PINB;

  // We check the direction and either add or subtract from the encoder counter
  if((encState&(1<<PORTB0))>>PORTB0 == (encState&(1<<PORTB1))>>PORTB1){
    encCount++;
  }
  else{
    encCount--;
  }
}
