/*
 * CyberNerf_ATmega8A.cpp
 *
 * Created: 5/30/2022 9:15:43 PM
 * Author : tabr
 */ 

#define F_CPU 8000000
#define CRC_CALC_BYPASS
//#define ASCII_DECODE
//#define AUTO_SHOT
#define USE_UART_DEBUG_COMMANDS

//system includes
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h> // работа с прерываниями
#include <avr/sleep.h> // здесь описаны режимы сна
#include <avr/power.h>
//#include <util/crc16.h>
#include <string.h>
#include <avr/eeprom.h>

#define cbi(port,bit)               port&=~(1<<(bit)) //clear bit
#define sbi(port,bit)               port|=(1<<(bit))  //set bit
#define ibi(port,bit)               port^=(1<<(bit))  //invert bit
#define isBitSet(reg,bit)           ((reg)&(1<<(bit)))


//project includes
#include "CybergunClass.hpp"
#include "CybergunProtoClass.hpp"



#ifdef USE_UART_DEBUG_COMMANDS
#warning "ANY DATA CANNOT BE MORE THAN 250"
#endif //USE_UART_DEBUG_COMMANDS
#ifdef CRC_CALC_BYPASS
#warning "CRC bypass is ON!"
#endif //CRC_CALC_BYPASS
#ifdef ASCII_DECODE
#warning "WARNING! ASCII_DECODE is ON! Data may be damaged!"
#endif //ASCII_DECODE
#ifdef AUTO_SHOT
#warning "AUTO_SHOT is ON!"
#endif //AUTO_SHOT




CybergunClass Cybergun;
CybergunProtoClass CybergunProto;
volatile uint32_t uptime = 0;


//uint8_t a1=0;
//uint8_t a2=32;
//volatile uint8_t tmp=0;

class TRX_environmentInterface{
	public:
	static void rcv_isr(uint8_t byte);
	static void setup(void);
	static void send_data(uint8_t* data, uint8_t size);
};
class TRX_environmentUART  : public TRX_environmentInterface{
	public:
	const static uint8_t DATA_REGISTER_EMPTY	= (1<<UDRE);
	const static uint8_t RX_COMPLETE			= (1<<RXC);
	const static uint8_t FRAMING_ERROR			= (1<<FE);
	const static uint8_t PARITY_ERROR			= (1<<UPE);
	const static uint8_t DATA_OVERRUN			= (1<<DOR);
	static void send_data(uint8_t* data, uint8_t size){
		uint8_t tx_buffer_ptr = 0;
		while (tx_buffer_ptr < size){
			UDR=data[tx_buffer_ptr];
			while ((UCSRA & DATA_REGISTER_EMPTY)==0);//wait for TX complete
			tx_buffer_ptr++;
		}
	}
	static void init(void){
		// USART initialization
		// Communication Parameters: 8 Data, 1 Stop, No Parity
		// USART Receiver: On
		// USART Transmitter: On
		// USART Mode: Asynchronous
		
		// USART Baud Rate: 9600
		UCSRA=(0<<RXC) | (0<<TXC) | (0<<UDRE) | (0<<FE) | (0<<DOR) | (0<<UPE) | (0<<U2X) | (0<<MPCM);
		UCSRB=(1<<RXCIE) | (1<<TXCIE) | (0<<UDRIE) | (1<<RXEN) | (1<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);
		UCSRC=(1<<URSEL) | (0<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<UCPOL);
		UBRRH=0x00;
		UBRRL=0x33;
		
		/*
		// USART Baud Rate: 38400
		UCSRA=(0<<RXC) | (0<<TXC) | (0<<UDRE) | (0<<FE) | (0<<DOR) | (0<<UPE) | (0<<U2X) | (0<<MPCM);
		UCSRB=(1<<RXCIE) | (1<<TXCIE) | (0<<UDRIE) | (1<<RXEN) | (1<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);
		UCSRC=(1<<URSEL) | (0<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<UCPOL);
		UBRRH=0x00;
		UBRRL=0x0C;
		*/
	}
};
class TRX_environment : public TRX_environmentUART{
	public:
	static void rcv_isr(uint8_t byte){
		CybergunProto.rx_ISR(byte);
		}
};
//void TRX_environment::rcv_isr(uint8_t byte){
//	CybergunProto.rx_ISR(byte);
//}
ISR(USART_TXC_vect){
}
// Прерывание по окончанию приема данных по USART
ISR(USART_RXC_vect)
{
	#ifdef ASCII_DECODE
	//CybergunProto.rx_ISR(UDR-48);
	TRX_environment::rcv_isr(UDR-48);
	#else 
	//CybergunProto.rx_ISR(UDR);
	TRX_environment::rcv_isr(UDR);
	#endif //ASCII_DECODE
	/*
	if (++a1 >= 100){
		a1 = 0;
		UDR = a2++;
		if (a2 >= 127){
			a2 = 32;
		}
		
	}*/
	//tmp = UDR;
	//UDR = UDR;
	/*
	static uint8_t chr = 32;
	UDR = chr++;
	if (chr> 120){
		chr = 32;
	}
	*/
/*
	NUM = UDR; // Принимаем символ по USART
	byte_receive = 1;
	uart_send(NUM); // Посылаем символ по USART
	
	if(NUM == 'a') // Если принят символ "a", включаем светодиод
	PORTB |= (1 << PB0);
	if(NUM == 'b') // Если принят символ "b", выключаем светодиод
	PORTB &= ~(1 << PB0);
*/
}

ISR(TIMER0_OVF_vect) //8ms
{
	#ifdef AUTO_SHOT
	static uint8_t shot_counter = 0;// 255x8ms = shot each 2.048 sec
	#endif //AUTO_SHOT
	static uint8_t one_sec_counter = 0;
	//[128]	16384
	//[0]	32768
	// Reinitialize Timer1 value
	//TCNT0=TCNT_MIN+TCNT_rand;
	//startTX	= true;//может и не надо, т.к. проц всё равно проснётся тольк оот этого прерывания
	// Reinitialize Timer 0 value
	//TCNT0=0x83;//1ms
	TCNT0=0x06;//8ms
	CybergunProto.ms_ISR();
	if (++one_sec_counter == 125){
		one_sec_counter = 0;
		//one second
		uptime++;
	}
	#ifdef AUTO_SHOT
	if (--shot_counter == 0){
		//ms100_counter = 250;
		if (Cybergun.Storage.get_shots() > 0){
			Cybergun.shot_detected();
		}
	}
	#endif //AUTO_SHOT
	
}

	



int main(void)
{
	TRX_environment::rcv_isr(1);
	//_delay_ms(11);
    // Input/Output Ports initialization
    // Port B initialization
    // Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In
    DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (1<<DDB1) | (0<<DDB0);
    // State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T
    PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

    // Port C initialization
    // Function: Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In
    DDRC=(0<<DDC6) | (0<<DDC5) | (1<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
    // State: Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T
    PORTC=(0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

    // Port D initialization
    // Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In
    DDRD=(1<<DDD7) | (1<<DDD6) | (1<<DDD5) | (1<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
    // State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T
    PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);
	
		for (int i=0;i<6;i++){
			i++;
			ibi(PORTD, 4);
			_delay_ms(500);
		}
		_delay_ms(1000);


    // Timer/Counter 0 initialization
    // Clock source: System Clock
    
	// Clock value: 125.000 kHz
    //TCCR0=(0<<CS02) | (1<<CS01) | (1<<CS00);
    //TCNT0=0x83;
	
	// Clock value: 31.250 kHz
	TCCR0=(1<<CS02) | (0<<CS01) | (0<<CS00);
	TCNT0=0x06;

    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: Timer1 Stopped
    // Mode: Normal top=0xFFFF
    // OC1A output: Disconnected
    // OC1B output: Disconnected
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer1 Overflow Interrupt: Off
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off
    /*
	TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
    TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
    TCNT1H=0x00;
    TCNT1L=0x00;
    ICR1H=0x00;
    ICR1L=0x00;
    OCR1AH=0x00;
    OCR1AL=0x00;
    OCR1BH=0x00;
    OCR1BL=0x00;
	*/
	// Timer/Counter 1 initialization
	// Clock source: System Clock
	// Clock value: 1000.000 kHz
	// Mode: Fast PWM top=ICR1
	// OC1A output: Non-Inverted PWM
	// OC1B output: Disconnected
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	// Timer Period: 20 ms
	// Output Pulse(s):
	// OC1A Period: 20 ms Width: 0.017001 ms
	// Timer1 Overflow Interrupt: Off
	// Input Capture Interrupt: Off
	// Compare A Match Interrupt: Off
	// Compare B Match Interrupt: Off
	TCCR1A=(1<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (1<<WGM11) | (0<<WGM10);
	TCCR1B=(0<<ICNC1) | (0<<ICES1) | (1<<WGM13) | (1<<WGM12) | (0<<CS12) | (1<<CS11) | (0<<CS10);
	TCNT1H=0x00;
	TCNT1L=0x00;
	ICR1H=0x4E;
	ICR1L=0x1F;
	//OCR1AH=0x09;//180
	//OCR1AL=0xc3;//180
	//OCR1AH=0x02;//0
	//OCR1AL=0x57;//0
	//OCR1AH=0x04;//"closed"
	//OCR1AL=0xc0;//"closed"
	OCR1AH=0x08;//"opened"
	OCR1AL=0x80;//"opened"
	//OCR1AH=0x00;//0-0
	//OCR1AL=0x00;//0-0
	OCR1BH=0x00;
	OCR1BL=0x00;

    // Timer/Counter 2 initialization
    // Clock source: System Clock
    // Clock value: Timer2 Stopped
    // Mode: Normal top=0xFF
    // OC2 output: Disconnected
    //ASSR=0<<AS2;
    //TCCR2=(0<<PWM2) | (0<<COM21) | (0<<COM20) | (0<<CTC2) | (0<<CS22) | (0<<CS21) | (0<<CS20);
    //TCNT2=0x00;
    //OCR2=0x00;

    // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (1<<TOIE0);

    // External Interrupt(s) initialization
    // INT0: Off
    // INT1: Off
    MCUCR=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);

    // USART initialization
	TRX_environment::init();

    // Analog Comparator initialization
    // Analog Comparator: Off
    // The Analog Comparator's positive input is
    // connected to the AIN0 pin
    // The Analog Comparator's negative input is
    // connected to the AIN1 pin
    ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);
    SFIOR=(0<<ACME);

    // ADC initialization
    // ADC disabled
    ADCSRA=(0<<ADEN) | (0<<ADSC) | (0<<ADFR) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);

    // SPI initialization
    // SPI disabled
    SPCR=(0<<SPIE) | (0<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);

    // TWI initialization
    // TWI disabled
    TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);
	
	Cybergun.update_status();
	#warning "Cybergun.update_status() should be called on any change of storage"
    // Global enable interrupts
    asm("sei");
    while (1) 
    {
		CybergunProto.main();
		OCR1AH=0x04;//"closed"
		OCR1AL=0xc0;//"closed"
		_delay_ms(1000);
		OCR1AH=0x08;//"opened"
		OCR1AL=0x80;//"opened"
		_delay_ms(1000);
    }
}

#include "CybergunClass.cpp"
#include "CybergunStorageClass.cpp"
#include "CybergunProtoClass.cpp"