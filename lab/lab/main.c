/*
 * ADC10BITSLABUART.c
 *
 * Created: 9/11/2021 2:21:44 a. m.
 * Author : Miguel Palomino, Ivan Holguin & Miguel Rondón
 */

#define F_CPU 16000000L

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Mis funciones inicializadas
void config_usart(void);
void ANC_CONFG(void);

// Declaracion de variables
uint16_t signal_1 = 0; // unsigned int 16 bit variable to store the 10 bit ADC  value
uint16_t signal_2 = 0; // unsigned int 16 bit variable to store the 10 bit ADC  value
uint8_t numero = 0;
uint8_t dnumero = 0;
uint8_t cnumero = 0;

char myBuffer = ' '; // Variable inicial de UART
int RESTA = 0;

// Configurar el ADC
void ADC_init()
{
	// ADC habilitado, con prescaler 128
	ADCSRA = 0x87;

	// Voltaje de referencia VCC, 1 <<REFS0 (Alimentación AVCC)
	ADMUX = 0x40; // 0b100 0000 para 10B
}

// Funcion de lectura ADC con selector de canales
int ADC_read(char channel)
{
	if (channel == 1)
	{
		// Activado A1
		ADMUX = 0x41;
	}
	else if (channel == 0)
	{
		// Activado A0
		ADMUX = 0x40;
	}
	else if (channel == 2)
	{
		// Activado A2
		ADMUX = 0x42;
	}

	// Inicia conversión
	ADCSRA |= (1 << ADSC);

	//Esperar hasta que termine la conversión
	while (!(ADCSRA & (1 << ADIF)))
		;

	//limpiar bandera interrupción
	ADCSRA |= (1 << ADIF);
	_delay_ms(1);

	//retornar converción
	return ADCW;
}

int main(void)
{
	// Activar las interrupciones globales
	sei();

	// Configurar puerto RX/TX
	DDRD = 0x02;

	// Configurar puertos de salidas
	DDRB = 0xFF;

	// Llamar métodos
	ADC_init();
	ANC_CONFG();

	config_usart();

	// Encabezado de datos [Dato	Signal1		Signal2]
	int cabeza[20] = {68, 65, 84, 79, 9, 83, 69, 78, 65, 76, 49, 9, 83, 69, 78, 65, 76, 50, 10, 13};

	// Enviar encabezados
	for (int x = 0; x < 20; x++)
	{
		// Verificar si la transmicion esta libre
		while (!(UCSR0A & (1 << UDRE0)))
			;

		// Escribir registro almacenado
		UDR0 = cabeza[x];
	}

	while (1)
	{
		// Obtener datos de A0
		signal_1 = ADC_read(0);

		//Almacenamiento temporal (16bit) para la conversion
		uint16_t temp = 0;

		// Variable (8bit) para la posicion de cada digito
		uint8_t Ones_Position = 0;
		uint8_t Tens_Position = 0;
		uint8_t Hundreths_Position = 0;
		uint8_t Thousands_Position = 0;

		// Almacenamos la señal 1
		temp = signal_1;

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

		signal_2 = ADC_read(1);

		uint16_t temp2 = 0; //Temporary Storage (16bit)

		uint8_t Ones_Position2 = 0; // 8 bit variables to be send using usart
		uint8_t Tens_Position2 = 0;
		uint8_t Hundreths_Position2 = 0;
		uint8_t Thousands_Position2 = 0;

		temp2 = signal_2;

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

		//PUNTO 3
		RESTA = signal_1 - signal_2;
		if (RESTA > 0)
		{
			PORTB = (1 << 5);
		}
		else
		{
			PORTB = (0 << 5);
		}

		// Declaramos los numeros de 0 a 9
		int a[9] = {48, 49, 50, 51, 52, 53, 54, 56, 57};
		int b[9] = {48, 49, 50, 51, 52, 53, 54, 56, 57};
		int c[9] = {48, 49, 50, 51, 52, 53, 54, 56, 57};

		// Verificar si la transmicion esta libre
		while (!(UCSR0A & (1 << UDRE0)))
			;
		UDR0 = c[cnumero]; // Escribir registro almacenado

		while (!(UCSR0A & (1 << UDRE0)))
			;			   // Verificar si la transmicion esta libre
		UDR0 = a[dnumero]; // Escribir registro almacenado

		while (!(UCSR0A & (1 << UDRE0)))
			;			  // Verificar si la transmicion esta libre
		UDR0 = b[numero]; // Escribir registro almacenado

		while (!(UCSR0A & (1 << UDRE0)))
			; // Verificar si la transmicion esta libre
		UDR0 = '	';

		while (!(UCSR0A & (1 << UDRE0)))
			;					   // Verificar si la transmicion esta libre
		UDR0 = Thousands_Position; // Escribir registro almacenado

		while (!(UCSR0A & (1 << UDRE0)))
			;					   // Verificar si la transmicion esta libre
		UDR0 = Hundreths_Position; // Escribir registro almacenado

		while (!(UCSR0A & (1 << UDRE0)))
			;				  // Verificar si la transmicion esta libre
		UDR0 = Tens_Position; // Escribir registro almacenado

		while (!(UCSR0A & (1 << UDRE0)))
			;				  // Verificar si la transmicion esta libre
		UDR0 = Ones_Position; // Escribir registro almacenado

		// Enviamos el tab (un espacio)
		while (!(UCSR0A & (1 << UDRE0)))
			; // Verificar si la transmicion esta libre
		UDR0 = '	';

		while (!(UCSR0A & (1 << UDRE0)))
			;						// Verificar si la transmicion esta libre
		UDR0 = Thousands_Position2; // Escribir registro almacenado

		while (!(UCSR0A & (1 << UDRE0)))
			;						// Verificar si la transmicion esta libre
		UDR0 = Hundreths_Position2; // Escribir registro almacenado

		while (!(UCSR0A & (1 << UDRE0)))
			;				   // Verificar si la transmicion esta libre
		UDR0 = Tens_Position2; // Escribir registro almacenado

		while (!(UCSR0A & (1 << UDRE0)))
			;				   // Verificar si la transmicion esta libre
		UDR0 = Ones_Position2; // Escribir registro almacenado

		while (!(UCSR0A & (1 << UDRE0)))
			;	   // Verificar si la transmicion esta libre
		UDR0 = 10; // Escribir registro almacenado

		while (!(UCSR0A & (1 << UDRE0)))
			;	   // Verificar si la transmicion esta libre
		UDR0 = 13; // Escribir registro almacenado

		// Bucle de control para las unidades, decenas y centenas
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

// Configuracion de puertos para 9600b tipo UART Asincrona Normal con RX & TX
void config_usart(void)
{
	UCSR0A = 0x00;
	UCSR0B = 0b10011000;
	UCSR0C = 0b00000110;

	// Configurar el baud
	UBRR0 = 103;
}

// Configuracion de comparador analogico
void ANC_CONFG(void)
{
	// Se puede definir la entrada negativa del AC con MUX
	ADCSRB |= (0 << ACME);

	// Interrupcion AC activada, interrupción en flanco de subida de la salida
	ACSR |= (1 << ACIE) | (0 << ACIS1) | (0 << ACIS0);

	// Buffer de entrada digital apagado en AIN0 & AIN1
	DIDR1 |= (1 << AIN0D) | (1 << AIN1D);
}

ISR(USART_RX_vect)
{
	// Almacenamos dato actual
	myBuffer = UDR0;

	// Verificar si la transmicion esta libre
	while (!(UCSR0A & (1 << UDRE0)))
		;

	// Encender led
	if (myBuffer == 'D')
	{
		PORTD ^= (1 << 4);
	}
	else
	{
		// Escribir registro almacenado
		UDR0 = myBuffer;
	}
}

ISR(ANALOG_COMP_vect)
{
	// Verificar ACO (bit de salida)
	if ((ACSR & 0x20) == 0)
	{
		// Enciende LED1 si Ref es mayor
		PORTD = (1 << 3);
	}
	else
	{
		// Apaga led
		PORTD = (0 << 3);
	}
}