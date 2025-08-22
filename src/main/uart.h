#ifndef UART_H
#define UART_H

#include <Arduino.h>
#include "main.h"

typedef void (*void_func_void)(void); 
char* read_uart_line_blocking(void_func_void);
#endif
