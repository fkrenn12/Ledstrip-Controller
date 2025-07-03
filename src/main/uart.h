#ifndef UART_H
#define UART_H

#include <Arduino.h>

#define UART_BUFFER_SIZE 1024
#define UART_BAUDRATE 921600
#define UART_RX_PIN 1

typedef void (*void_func_void)(void); 
char* read_uart_line_blocking(void_func_void, char);
#endif
