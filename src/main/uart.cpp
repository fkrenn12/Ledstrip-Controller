
#include "uart.h"

char buffer[UART_BUFFER_SIZE];
char* read_uart_line_blocking(void_func_void callback_idle , char eol='\n')
{
  char incomingByte = 0;
  char index = 0;
  buffer[0] = '\0';

  while (index < UART_BUFFER_SIZE)
  {
    if (Serial1.available() > 0) {
      incomingByte = Serial1.read();
      if (incomingByte == eol) {
        buffer[index] = '\0';
        break;
      }
      buffer[index++] = incomingByte;
    }
    else
    {
      callback_idle();
    }
  }
  return buffer;
}