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

// Mensajes por defecto
char LedOnTx[] = "\rLED ON\r";
char LedOffTx[] = "\rLED OFF\r";
char msm[] = "\rBienvenido\r\r";
char myBuffer = ' ';

int main(void)
{
    // Apagar todas las interrupciones con <avr/interrupt.h>
    cli();

    // Iniciar comunicacion UART
    myUart();

    // Mensaje de inicio
    imprimir(msm);

    /* Replace with your application code */
    while (1)
    {
    }
}

ISR(USART_RX_vect)
{
    serialLed();
}

// Mis funciones

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

    // Activamos interrupciones
    sei();
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
