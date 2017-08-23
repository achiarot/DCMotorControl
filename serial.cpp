#include <stdint.h>
#include <avr/interrupt.h>
static char [64] rxBuffer;
static uint8_t rxStartIndex;
static uint8_t rxEndIndex;
static char [64] txBuffer;
static uint8_t txStartIndex;
static uint8_t txEndIndex;

class Serial{
    public:
        Serial(int baudRate=9600);
        bool print(string message);
        bool println(string message)
        bool sendByte(uint8_t messageByte);
        bool available();
        char read_char();


};

Serial::Serial(int baudRate=9600){
    //setup the serial port registers
    UCSR0A |= 0x40;  //0b01000000
    UCSR0B |= 0x98;  //0b100110--
    UCSR0C |= 0x26;  //0b00100110
    UBRR0H |= 0x00;
    UBRR0L |= 0x67;  //0b01100111
}


bool Serial::print(string message){

    return 1;
}

bool Serial::println(string message){

    return print(message+"\n");
}

/*Will return 1 if it was able to load the data into the txBuffer */
bool Serial::sendByte(uint8_t messageByte){

    return 1;
}

/* Will return 1 if there is data in the rxBuffer to read*/
bool Serial::available(){
    
    return 1;
}

/* Receive complete will always be enabled if serial has been enabled.
*/
ISR(USART_RX_vect){

}

/* Data Register empty interupt will be enabled when transmitting and dissabled when there is nothing else to transmit
*/
ISR(USART_UDRE_vect){

}
