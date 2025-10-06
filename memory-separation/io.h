#ifndef IO_H
#define IO_H

#include <stddef.h>

void uart_init();
void uart_writeText(const char *buffer);
void uart_loadOutputFifo();
unsigned char uart_readByte();
unsigned int uart_isReadByteReady();
void uart_writeByteBlocking(unsigned char ch);
void uart_update();
/** Read until newline or (bufsize - 1). Returns number of read characters.
 * Puts '\0' after last read character
 **/
int uart_readLine(char *buf, size_t bufsize);

#endif
