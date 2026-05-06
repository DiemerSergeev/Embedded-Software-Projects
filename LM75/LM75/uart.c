#include "uart.h"
volatile uint8_t uart_num_var=0;

/*---------------USE-----------------------------------------------
UART_Init(x, 9600);							// Инициализация номера UART и скорость, где x-1 или 0,(UART0=0, UART1=1)
UART_Deinit();								// Деинициализация UART
UART_Transmit_Char('!');					// Char
UART_Transmit_Char(0x0d);					// Переход в начало строки
UART_Transmit_Char(0x0a);					// Переход на новую строку
UART_Transmit_Str("Hello",end_string);      // Передаём строку с 0x0d, 0x0a
UART_Transmit_Str("Hello",no_end_string);   // Передаём строку без 0x0d, 0x0a
UART_Transmit_Hex(0x00,2);				    // Передаём HEX, 2 символа, придёт 0x00
UART_Transmit_Hex(0x0BCD,2);				// Передаём HEX, 2 символа, придёт 0xCD
UART_Transmit_Hex(0x0BCD,4);				// Передаём HEX, 4 символа, придёт 0x0BCD
UART_Transmit_Hex(0x0BCD,6);				// Передаём HEX, 6 символов, придёт 0x000BCD
UART_Transmit_Float(0.1234,2);				// Передаём FLOAT, 2 символа, придёт 0.12
UART_Transmit_Float(0.1234,4);				// Передаём FLOAT, 4 символа, придёт 0.1234
UART_Transmit_Float(0.1234,6);				// Передаём FLOAT, 4 символов, придёт 0.123400
UART_Transmit_Dec(1234,2);					// Передаём DEC, 2 символа, придёт 34 
UART_Transmit_Dec(1234,4);					// Передаём DEC, 4 символа, придёт 1234 
UART_Transmit_Dec(1234,6);					// Передаём DEC, 6 символов, придёт 001234
UART_Transmit_Dec(0,6);					    // Передаём DEC, 6 символов, придёт 000000  

Включить RXCIE если требуется приём байт
-----------------------------------------------------------------*/

void UART_Init(uint8_t uart_number, unsigned long speed)  //Инициализация модуля UART
{
	uart_num_var=uart_number;
	if (uart_number==0)
	{
		DDRE|=0b00000010;   // PE0-RX0, PE1-TX0
		DDRE&=0b11111110;  
		
		PORTE|= (1<<PE1)|(1<<PE0); //подтяжка к vcc 

		unsigned int BAUD = (F_CPU / (16UL * speed)) - 1;
		UBRR0H = (unsigned char)(BAUD >> 8);
		UBRR0L = (unsigned char)BAUD;
	
		UDR0=0;
	
		UCSR0B=(1<<TXEN0)|(1<<RXEN0)|(0<<RXCIE0)|(0<<UCSZ02);    // Включаем передачу по UART
	
		UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);                         // 1 стоп-бит (USBS=0), 8-бит посылка (UCSZ1=1 и UCSZ0=1)
	}
	else
	{
		DDRD|=0b00001000;   // PD2-RX1, PD3-TX1
		DDRD&=0b11111011;
		
		PORTD|= (1<<PD3)|(1<<PD2); //подтяжка к vcc

		unsigned int BAUD = (F_CPU / (16UL * speed)) - 1;
		UBRR1H = (unsigned char)(BAUD >> 8);
		UBRR1L = (unsigned char)BAUD;
		
		UDR1=0;
		
		UCSR1B=(1<<TXEN1)|(1<<RXEN1)|(0<<RXCIE1)|(0<<UCSZ12);    // Включаем передачу по UART
		
		UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);                         // 1 стоп-бит (USBS=0), 8-бит посылка (UCSZ1=1 и UCSZ0=1)		
	}
	
	_delay_ms(10);
}



void UART_Deinit(void)  // Деинициализация модуля UART
{
	if (uart_num_var==0)
	{
		DDRE&=0b11111100;   // PE0-RX0, PE1-TX0
		PORTE&=0b11111100; 
		UDR0=0;
		UCSR0B=(0<<TXEN0)|(0<<RXEN0)|(0<<RXCIE0)|(0<<UCSZ02);   
		UCSR0C = (0<<UCSZ01)|(0<<UCSZ00);                         
	}
	else
	{
		DDRD&=0b11110011;   // PD2-RX1, PD3-TX1
		PORTD&=0b11110011; 
		UDR1=0;
		UCSR1B=(0<<TXEN1)|(0<<RXEN1)|(0<<RXCIE1)|(0<<UCSZ12);    
		UCSR1C = (0<<UCSZ11)|(0<<UCSZ10);                         	
	}
	_delay_ms(1);
}



// Отправка
void UART_Transmit_Char( unsigned char data )                 // Функция отправки данных
{
	if(uart_num_var==0)
	{
		while ( !(UCSR0A & (1<<UDRE0)) );                     // Ожидание опустошения буфера приема
		UDR0 = data;	
	}
	else
	{
		while ( !(UCSR1A & (1<<UDRE1)) );                     // Ожидание опустошения буфера приема
		UDR1 = data;
	}
                                                              // Начало передачи данных
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




void UART_Transmit_Hex(uint16_t data, int width) 
{
	char str[12]; // "0x" + максимальное количество символов HEX + '\0'
	int i;

	str[0] = '0';
	str[1] = 'x';

	for (i = width - 1; i >= 0; i--) {
		int nibble = (data >> (i * 4)) & 0xF;
		if (nibble < 10) 
		{
			str[2 + (width - 1 - i)] = '0' + nibble;
		} 
		else 
		{
			str[2 + (width - 1 - i)] = 'A' + (nibble - 10);
		}
	}

	str[2 + width] = '\0'; // Завершаем строку нулевым символом

	UART_Transmit_Str(str, end_string);
}

	
 void UART_Transmit_Float(float data, int precision) {
	 char str[32];
	 int int_part = (int)data;
	 long frac_part;
	 int i = 0, j;

	 // Обработка отрицательных чисел
	 if (data < 0) {
		 str[i++] = '-';
		 int_part = -int_part;
		 data = -data;
	 }

	 // Преобразование целой части
	 if (int_part == 0 && data == 0) { // Обработка нуля
		 str[i++] = '0';
		 } else {
		 int temp_int_part = int_part;
		 if (temp_int_part == 0)
		 {
			 str[i++] = '0';
		 }
		 while (temp_int_part > 0) {
			 str[i++] = '0' + (temp_int_part % 10);
			 temp_int_part /= 10;
		 }
		 if (int_part != 0) // Добавлено условие
		 {
			 for (j = 0; j < i / 2; j++) {
				 char temp = str[j];
				 str[j] = str[i - j - 1];
				 str[i - j - 1] = temp;
			 }
		 }
	 }

	 // Добавляем точку
	 str[i++] = '.';

	 // Преобразование дробной части (без pow и round)
	 long multiplier = 1;
	 for (j = 0; j < precision; j++) {
		 multiplier *= 10;
	 }
	 frac_part = (long)((data - int_part) * multiplier + 0.5); // Ручное округление

	 // Преобразование дробной части в строку
	 char frac_str[precision + 1];
	 for (j = precision - 1; j >= 0; j--) {
		 frac_str[j] = '0' + (frac_part % 10);
		 frac_part /= 10;
	 }
	 frac_str[precision] = '\0';

	 // Копируем дробную часть в основную строку
	 for (j = 0; j < precision; j++) {
		 str[i++] = frac_str[j];
	 }

	 str[i] = '\0';

	 UART_Transmit_Str(str, end_string);
 }
	


void UART_Transmit_Dec(uint16_t data, int width) 
{
	char str[10]; // Максимальное количество символов + нулевой символ
	int i = width - 1; // Индекс для записи символов в строку
	int j;

	str[width] = '\0'; // Завершаем строку нулевым символом

	if (data == 0) {
		for (j = 0; j < width; j++) {
			str[j] = '0';
		}
		} else {
		while (data > 0 && i >= 0) {
			str[i] = '0' + (data % 10);
			data /= 10;
			i--;
		}

		// Заполняем оставшиеся позиции нулями
		while (i >= 0) {
			str[i] = '0';
			i--;
		}
	}

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
	TCCR1B |= (1<<CS12);     // Установим делитель 
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




uint8_t rx_done (void)     // Вернуть готов ли кадр к тому чтобы его забрали из буфера
{
 return (done);	
}


void rx_clear (void)       // Очистить буфер приёма
{
 for (uint8_t i=0; i<LEN; i++) buffer[i]='0';   // Очистка буфера кадра	
 IDX=0;
 done=0;
}


ISR(USART0_RX_vect)         // Прерывание UART0 по приёму
{
	char bf= UDR0;
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