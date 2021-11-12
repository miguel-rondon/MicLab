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
#include <string.h>

// Inicializar mis funciones
void myUart();
void imprimir(char texto[]);
void serialLed();

void ADC_init();
int ADC_read(char channel);
void adc_led();

// Mensajes por defecto
char LedOnTx[] = "\rLED ON\r";
char LedOffTx[] = "\rLED OFF\r";
char msm[] = "\rBienvenido\r\r";
char myBuffer = ' ';

// Variables
uint16_t ADC_A0 = 0;	// unsigned int 16 bit variable to store the 10 bit ADC

int main(void)
{
	// Apagar todas las interrupciones con <avr/interrupt.h>
	cli();

	// Iniciar comunicacion UART
	myUart();

	// Iniciar ADC
	ADC_init();

	// Mensaje de inicio
	imprimir(msm);

	// Activamos las salidas
	DDRB = 0xff;
	
	// Activamos RX/TX
	DDRD = 0x02;

	// Activamos interrupciones
	sei();

	/* Replace with your application code */
	while (1)
	{
		adc_led();
	}
}

ISR(USART_RX_vect)
{
	serialLed();
}

/*
*	----------------------------------------------------
*	Mis funciones
*	----------------------------------------------------
*	--> Orientadas al UART
*/

void myUart()
{
	// Modo de uso tipo "asynchronous fast" UMLSEL0 -> 00
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

	// Activamos la interrupcion RX
	UCSR0B |= (1 << RXCIE0);
}

void imprimir(char texto[])
{
	/*
	*	Detectamos el tamaño del texto con la libreria <string.h>
	*	Es posible usar la "sizeof()" como alternativa
	*/

	char ntext = (unsigned)strlen(texto);

	for (int i = 0; i < (int)(ntext); i++)
	{
		// Verificar si la transmicion esta libre
		while (!(UCSR0A & (1 << UDRE0)))
			;
		UDR0 = texto[i];
	}
}

void serialLed()
{
	// Almacenamos dato actual
	myBuffer = UDR0;

	// Verificar si la transmicion esta libre
	while (!(UCSR0A & (1 << UDRE0)))
		;

	// Si se escribe la letra D, generamos la accion ON/OFF LED
	if (myBuffer == 'D')
	{
		// Encender/Apagar led
		PORTB ^= (1 << 4);

		// Mensaje de confirmacion dependiente del puerto
		if (PORTB & (1 << 4))
		{
			imprimir(LedOnTx);
		}
		else
		{
			imprimir(LedOffTx);
		}
	}
	else
	{
		// Escribir registro almacenado
		UDR0 = myBuffer;
	}
}

/*
*	----------------------------------------------------
*	Mis funciones
*	----------------------------------------------------
*	--> Orientadas al ADC
*/

void ADC_init()
{
	// ADC enable, prescaler 128
	ADCSRA = 0x87;
	// Voltaje de referncia VCC, 1 <<REFS0
	ADMUX = 0x40;
}

int ADC_read(char channel)
{
	if (channel == 1)
	{
		// Activar A1
		ADMUX = 0x41;
	}
	else if (channel == 0)
	{
		// Activar A0
		ADMUX = 0x40;
	}
	else if (channel == 2)
	{
		// Activar A2
		ADMUX = 0x42;
	}

	// Inicia conversión
	ADCSRA |= (1 << ADSC);

	// Esperar hasta que termine la conversión
	while (!(ADCSRA & (1 << ADIF)))
		;

	// Limpiar bandera interrupción
	ADCSRA |= (1 << ADIF);

	// Esperar 1 milisegundo
	_delay_ms(1);

	return ADCW;
}

void adc_led()
{
	// Tomar Dato analogico de A0
		ADC_A0 = ADC_read(0);

		if (ADC_A0 > 500)
		{
			PORTB = (1 << 3);
		} else {
			PORTB = (0 << 3);
		}
}
