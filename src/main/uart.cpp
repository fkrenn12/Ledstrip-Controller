
#include "uart.h"

#define EOL1 '\n'
#define EOL2 '\r'

char buffer[UART_BUFFER_SIZE];

char* read_uart_line_blocking(void_func_void callback_idle)
{
  while (1){
    char incomingByte = 0;
    u16_t index = 0;
    buffer[0] = '\0';

    while (index < UART_BUFFER_SIZE)
    {
      if (Serial1.available() > 0) {
        incomingByte = Serial1.read();
        if (incomingByte == EOL1 || incomingByte == EOL2) {
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
    if (strlen(buffer) > 0 && buffer[0] != '\0') return buffer;
  }
}
