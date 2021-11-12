/*
 * ADC10BITSLABUART.c
 *
 * Created: 9/11/2021 11:21:44 a. m.
 * Author : Miguel Palomino
 */
#define F_CPU 16000000L
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void config_usart(void);
void ANC_CONFG(void);
uint8_t ADC_8bit_High_Byte = 0; // unsigned int 8 bit variable to get the ADC High Byte
uint8_t ADC_8bit_Low_Byte = 0;	// unsigned int 8 bit variable to get the ADC Low Byte
uint16_t ADC_10bit_Result = 0;	// unsigned int 16 bit variable to store the 10 bit ADC  value
uint16_t ADC_10bit_Result2 = 0; // unsigned int 16 bit variable to store the 10 bit ADC  value
uint8_t numero = 0;
uint8_t dnumero = 0;
uint8_t cnumero = 0;
char myBuffer = ' ';
int RESTA = 0;

void ADC_init()
{
	ADCSRA = 0x87; //ADC enable, prescaler 128
	ADMUX = 0x40;  // Voltaje de referncia VCC, 1 <<REFS0
}

int ADC_read(char channel)

{
	if (channel == 1)
	{
		ADMUX = 0x41; //activado A1
	}
	else if (channel == 0)
	{
		ADMUX = 0x40; //activado A0
	}
	else if (channel == 2)
	{
		ADMUX = 0x42; //activado A2
	}

	ADCSRA |= (1 << ADSC); //inicia conversión

	while (!(ADCSRA & (1 << ADIF)))
		;				   //Esperar hasta que termine la conversión
	ADCSRA |= (1 << ADIF); //limpiar bandera interrupción
	_delay_ms(1);
	return ADCW; //return ADC word
}

int main(void)

{
	sei();
	DDRD = 0x02;
	DDRB = 0xFF;
	ADC_init(); //llama método
	config_usart();
	ANC_CONFG();

	int cabeza[20] = {68, 65, 84, 79, 9, 83, 69, 78, 65, 76, 49, 9, 83, 69, 78, 65, 76, 50, 10, 13};

	for (int x = 0; x < 20; x++)
	{
		while (!(UCSR0A & (1 << UDRE0)))
			; // Wait for empty transmit buffer       */
		UDR0 = cabeza[x];
	}

	/* Replace with your application code */
	while (1)

	{

		ADC_10bit_Result = ADC_read(0);

		uint16_t temp = 0; //Temporary Storage (16bit)

		uint8_t Ones_Position = 0; // 8 bit variables to be send using usart
		uint8_t Tens_Position = 0;
		uint8_t Hundreths_Position = 0;
		uint8_t Thousands_Position = 0;

		temp = ADC_10bit_Result;

		Ones_Position = temp % 10;		// Eg 1023 % 10  -> Remainder -> 3
		temp = temp / 10;				// Eg temp = 1023/10 ->102
		Tens_Position = temp % 10;		// Eg 102  % 10  -> Remainder -> 2
		temp = temp / 10;				// Eg temp = 102/10 ->10
		Hundreths_Position = temp % 10; // Eg 10   % 10  -> Remainder -> 0
		Thousands_Position = temp / 10; // Eg 10   / 10  -> Quotient  -> 1

		//ASCII Conversion
		Ones_Position += 0x30;
		Tens_Position += 0x30;
		Hundreths_Position += 0x30;
		Thousands_Position += 0x30;

		//REPETICION PARA EL SEGUNDO POTENCIOMETRO

		ADC_10bit_Result2 = ADC_read(1);

		//PUNTO 3
		RESTA = ADC_10bit_Result - ADC_10bit_Result2;
		if (RESTA > 0)
		{
			PORTB = (1 << 5);
		}
		else
		{
			PORTB = (0 << 5);
		}

		uint16_t temp2 = 0; //Temporary Storage (16bit)

		uint8_t Ones_Position2 = 0; // 8 bit variables to be send using usart
		uint8_t Tens_Position2 = 0;
		uint8_t Hundreths_Position2 = 0;
		uint8_t Thousands_Position2 = 0;

		temp2 = ADC_10bit_Result2;

		Ones_Position2 = temp2 % 10;	  // Eg 1023 % 10  -> Remainder -> 3
		temp2 = temp2 / 10;				  // Eg temp = 1023/10 ->102
		Tens_Position2 = temp2 % 10;	  // Eg 102  % 10  -> Remainder -> 2
		temp2 = temp2 / 10;				  // Eg temp = 102/10 ->10
		Hundreths_Position2 = temp2 % 10; // Eg 10   % 10  -> Remainder -> 0
		Thousands_Position2 = temp2 / 10; // Eg 10   / 10  -> Quotient  -> 1

		//ASCII Conversion
		Ones_Position2 += 0x30;
		Tens_Position2 += 0x30;
		Hundreths_Position2 += 0x30;
		Thousands_Position2 += 0x30;

		int a[9] = {48, 49, 50, 51, 52, 53, 54, 56, 57};
		int b[9] = {48, 49, 50, 51, 52, 53, 54, 56, 57};
		int c[9] = {48, 49, 50, 51, 52, 53, 54, 56, 57};

		while (!(UCSR0A & (1 << UDRE0)))
			;			   // Wait for empty transmit buffer       */
		UDR0 = c[cnumero]; // Put data into buffer, sends the data */

		while (!(UCSR0A & (1 << UDRE0)))
			;			   // Wait for empty transmit buffer       */
		UDR0 = a[dnumero]; // Put data into buffer, sends the data */

		while (!(UCSR0A & (1 << UDRE0)))
			;			  // Wait for empty transmit buffer       */
		UDR0 = b[numero]; // Put data into buffer, sends the data */

		while (!(UCSR0A & (1 << UDRE0)))
			; // Wait for empty transmit buffer       */
		UDR0 = '	';

		while (!(UCSR0A & (1 << UDRE0)))
			;					   // Wait for empty transmit buffer       */
		UDR0 = Thousands_Position; // Put data into buffer, sends the data */

		while (!(UCSR0A & (1 << UDRE0)))
			;					   // Wait for empty transmit buffer       */
		UDR0 = Hundreths_Position; // Put data into buffer, sends the data */

		while (!(UCSR0A & (1 << UDRE0)))
			;				  // Wait for empty transmit buffer       */
		UDR0 = Tens_Position; // Put data into buffer, sends the data */

		while (!(UCSR0A & (1 << UDRE0)))
			;				  // Wait for empty transmit buffer       */
		UDR0 = Ones_Position; // Put data into buffer, sends the data */

		// Send '-' Character //
		while (!(UCSR0A & (1 << UDRE0)))
			; // Wait for empty transmit buffer       */
		UDR0 = '	';

		while (!(UCSR0A & (1 << UDRE0)))
			;						// Wait for empty transmit buffer       */
		UDR0 = Thousands_Position2; // Put data into buffer, sends the data */

		while (!(UCSR0A & (1 << UDRE0)))
			;						// Wait for empty transmit buffer       */
		UDR0 = Hundreths_Position2; // Put data into buffer, sends the data */

		while (!(UCSR0A & (1 << UDRE0)))
			;				   // Wait for empty transmit buffer       */
		UDR0 = Tens_Position2; // Put data into buffer, sends the data */

		while (!(UCSR0A & (1 << UDRE0)))
			;				   // Wait for empty transmit buffer       */
		UDR0 = Ones_Position2; // Put data into buffer, sends the data */

		while (!(UCSR0A & (1 << UDRE0)))
			;	   // Wait for empty transmit buffer       */
		UDR0 = 10; // Put data into buffer, sends the data */

		while (!(UCSR0A & (1 << UDRE0)))
			;	   // Wait for empty transmit buffer       */
		UDR0 = 13; // Put data into buffer, sends the data */

		numero++;
		if (numero == 9)
		{
			dnumero++;
			numero = 0;
			if (dnumero == 9)
			{
				cnumero++;
				dnumero = 0;
			}
		}
		_delay_ms(500);
	}
}

void config_usart(void)
{
	UCSR0A = 0x00;
	UCSR0B = 0b10011000;
	UCSR0C = 0b00000110;

	UBRR0 = 103;
}

void ANC_CONFG(void)
{

	ADCSRB |= (0 << ACME);							   //Se puede definir la entrada negativa del AC con MUX
	ACSR |= (1 << ACIE) | (0 << ACIS1) | (0 << ACIS0); //interrupcion AC activada, interrupción en flanco de subida de la salida
	DIDR0 |= (1 << ADC3D);							   //buffer de entrada digital apagado en A3
	DIDR1 |= (1 << AIN0D);							   //buffer de entrada digital apagado en AIN0
}

ISR(USART_RX_vect)
{
	myBuffer = UDR0;

	// Verificar si la transmicion esta libre
	while (!(UCSR0A & (1 << UDRE0)))
		;

	// Encender led
	if (myBuffer == 'D')
	{
		PORTB ^= (1 << 4);

		//_delay_ms(1000);
	}
	else
	{
		UDR0 = myBuffer;
	}
}

ISR(ANALOG_COMP_vect)

{
	if ((ACSR & 0x20) == 0) // Verificar ACO (bit de salida)
	{
		PORTB = (1 << 0); // Enciende LED1 si Ref es mayor
	}
	else
	{
		PORTB = (0 << 0); // Apaga led
	}
}