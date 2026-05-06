#include "uart.h"

/*---------------USE-----------------------------------------------
UART_init(9600); // Инициализация и скорость
UART_Transmit_Char('!');//о правильно работе программы
UART_Transmit_Char(0x0d);//переход в начало строки
UART_Transmit_Char(0x0a);//переход на новую строку
UART_Transmit_Str("Hello"); //Передаём строку
-----------------------------------------------------------------*/

void UART_init(uint16_t speed)  //Инициализация модуля USART
{
	DDRD|=0b00001000;   // PD2-RX1, PD3-TX1
	DDRD&=0b11111011;  
		
	PORTD|= (1<<2)|(1<<3); //подтяжка к vcc 
		
				
	unsigned long int BAUD = F_CPU/16;
	BAUD=(BAUD/speed)-1;
	
	UBRR1H = (uint8_t) ((BAUD & 0xFF00)>>8);
	UBRR1L = (uint8_t) (BAUD & 0x00FF);
	
	UDR1=0;
	
	UCSR1B=(1<<TXEN1)|(1<<RXEN1)|(1<<RXCIE1)|(0<<UCSZ12);    // Включаем передачу по UART
	
	UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);                         // 1 стоп-бит (USBS=0), 8-бит посылка (UCSZ1=1 и UCSZ0=1)
	_delay_ms(10);
}

// Отправка
void UART_Transmit_Char( unsigned char data )                 // Функция отправки данных
{
	while ( !(UCSR1A & (1<<UDRE1)) );                         // Ожидание опустошения буфера приема
	UDR1 = data;                                              // Начало передачи данных
}


void UART_Transmit_Str(char *s, uint8_t end_str)              //  Отправка строки
{
	while (*s != 0) UART_Transmit_Char(*s++);
	if (end_str==1)
	{
	UART_Transmit_Char(0x0d);                                 // Переход в начало строки
	UART_Transmit_Char(0x0a);                                 // Переход на новую строку		
	}

}


void UART_Transmit_Hex (uint16_t data)  // Отправить HEX
{
	char str[12];
	sprintf(str,"0x%08X",data);
	UART_Transmit_Str(str, end_string);
}


void UART_Transmit_Float (float data)  // Отправить Float
{
	char str[12];
	sprintf(str,"%.8f",data);
	UART_Transmit_Str(str, end_string);
}


void UART_Transmit_Dec (uint16_t data) // Отправить Dec
{
	char str[12];
	sprintf(str,"%.8d",data);
	UART_Transmit_Str(str, end_string);
}




// Приём
volatile unsigned char done=0;      // Переменная окончания кадра
volatile unsigned char IDX=0;       // Сётчик буфера байт


void Timer_Start(void)       // Таймер на 0.1 секунды для сброса если вдруг кадр по UART пришёл оборваный
{
	TCCR1B &= 0b11100000;    // Очистка таймера
	TCNT1H=0x00;
	TCNT1L=0x00;
	OCR1AH=0x00;
	OCR1AL=0x00;
	TIMSK &= 0b11011000;
	TIFR|=(1<<OCF1A);
	
	TCCR1B |= (1<<WGM12);    // Устанавливаем режим СТС (сброс по совпадению)
	TIMSK |= (1<<OCIE1A);	 // Устанавливаем бит разрешения прерывания 1ого счетчика по совпадению с OCR1A(H и L)
	OCR1AH = 0b00011110;     // Записываем в регистр число для сравнения
	OCR1AL = 0b10000100;
	TCCR1B |= (1<<CS12);     // Установим делитель.
}


void Timer_Stop(void)        // Остановка таймера
{
	TCCR1B &= 0b11100000;    // Очистка таймера
	TCNT1H=0x00;
	TCNT1L=0x00;
	TIMSK &= 0b11011000;
	TIFR|=(1<<OCF1A);
	
	OCR1AH = 0x00;
	OCR1AL = 0x00;
}


ISR (TIMER1_COMPA_vect)    // Таймер, прерывание раз в 0.1 секунды
{
	Timer_Stop();          // Сброс таймера
    rx_clear();            // Очистка буфера
}




uint8_t rx_done (void)       // Вернуть готов ли кадр к тому чтобы его забрали из буфера
{
 return (done);	
}


void rx_clear (void)        // Очистить буфер приёма
{
 for (uint8_t i=0; i<LEN; i++) buffer[i]='0';   // Очистка буфера кадра	
 IDX=0;
 done=0;
}


ISR(USART1_RX_vect)         // Прерывание UART1 по приёму
{
	char bf= UDR1;
	buffer[IDX]=bf;
	IDX++;
	Timer_Start();

	if (bf == 0x0A || IDX >= LEN)
	{
		IDX=0;
		done=1;
		Timer_Stop();
	}

}


















