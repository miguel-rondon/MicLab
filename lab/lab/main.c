/*
 * lab.c
 *
 * Created: 09/11/2021 06:29:54 p. m.
 * Author : stevi
 */

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int main(void)
{
	// Apagar todas las interrupciones con <avr/interrupt.h>
	cli();

	// Modo de uso tipo asyncrono UMLSEL0 -> 00
	UCSR0C &= ~(1 << UMSEL00);
	UCSR0C &= ~(1 << UMSEL01);

	// Desactivar paridad UPM0 -> 01
	UCSR0C &= ~(1 << UPM00);
	UCSR0C &= ~(1 << UPM01);

	// Parada con un bit USB -> 0 y si tiene 1 entonces define 2bit de parada
	UCSR0C &= ~(1 << USBS0);

	// Definir tama�o de datos a 8bits
	UCSR0C |= (1 << UCSZ00);  // 1
	UCSR0C |= (1 << UCSZ01);  // 1
	UCSR0B &= ~(1 << UCSZ02); // 0

	// Calculo del Baudrate
	UCSR0A |= (1 << U2X0);
	// --> Valor del registro UBRRn
	UBRR0 = (F_CPU / 8 / 9600) - 1;

	// Configuracion de pines RX & TX
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);

	// Activamos interrupciones
	sei();

	// declaramos la variable
	char myBuffer = ' ';

	/* Replace with your application code */
	while (1)
	{
		// Verificar si la transmicion esta libre
		while (!(UCSR0A & (1 << RXC0)))
			;
		myBuffer = UDR0;

		// Escribir el dato
		while (!(UCSR0A & (1 << UDRE0)))
			;
		UDR0 = myBuffer;
	}
}
