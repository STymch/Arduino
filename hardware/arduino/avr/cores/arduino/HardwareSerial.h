/*
  HardwareSerial.h - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
  Modified 3 December 2013 by Matthijs Kooijman
*/

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>

#include "Stream.h"

// Define constants and variables for buffering incoming serial data.  We're
// using a ring buffer (I think), in which head is the index of the location
// to which to write the next incoming character and tail is the index of the
// location from which to read.
// NOTE: a "power of 2" buffer size is reccomended to dramatically
//       optimize all the modulo operations for ring buffers.
// WARNING: When buffer sizes are increased to > 256, the buffer index
// variables are automatically increased in size, but the extra
// atomicity guards needed for that are not implemented. This will
// often work, but occasionally a race condition can occur that makes
// Serial behave erratically. See https://github.com/arduino/Arduino/issues/2405
#if !defined(SERIAL_TX_BUFFER_SIZE)
#if ((RAMEND - RAMSTART) < 1023)
#define SERIAL_TX_BUFFER_SIZE 16
#else
#define SERIAL_TX_BUFFER_SIZE 64
#endif
#endif
#if !defined(SERIAL_RX_BUFFER_SIZE)
#if ((RAMEND - RAMSTART) < 1023)
#define SERIAL_RX_BUFFER_SIZE 16
#else
#define SERIAL_RX_BUFFER_SIZE 64
#endif
#endif
#if (SERIAL_TX_BUFFER_SIZE>256)
typedef uint16_t tx_buffer_index_t;
#else
typedef uint8_t tx_buffer_index_t;
#endif
#if  (SERIAL_RX_BUFFER_SIZE>256)
typedef uint16_t rx_buffer_index_t;
#else
typedef uint8_t rx_buffer_index_t;
#endif

// Define config for Serial.begin(baud, config);
#define SERIAL_5N1 0x000
#define SERIAL_6N1 0x002
#define SERIAL_7N1 0x004
#define SERIAL_8N1 0x006
#define SERIAL_9N1 0x106

#define SERIAL_5N2 0x008
#define SERIAL_6N2 0x00A
#define SERIAL_7N2 0x00C
#define SERIAL_8N2 0x00E
#define SERIAL_9N2 0x10E

#define SERIAL_5E1 0x020
#define SERIAL_6E1 0x022
#define SERIAL_7E1 0x024
#define SERIAL_8E1 0x026
#define SERIAL_9E1 0x126

#define SERIAL_5E2 0x028
#define SERIAL_6E2 0x02A
#define SERIAL_7E2 0x02C
#define SERIAL_8E2 0x02E
#define SERIAL_9E2 0x12E

#define SERIAL_5O1 0x030
#define SERIAL_6O1 0x032
#define SERIAL_7O1 0x034
#define SERIAL_8O1 0x036
#define SERIAL_9O1 0x136

#define SERIAL_5O2 0x038
#define SERIAL_6O2 0x03A
#define SERIAL_7O2 0x03C
#define SERIAL_8O2 0x03E
#define SERIAL_9O2 0x13E

class HardwareSerial : public Stream
{
  protected:
    volatile uint8_t * const _ubrrh;
    volatile uint8_t * const _ubrrl;
    volatile uint8_t * const _ucsra;
    volatile uint8_t * const _ucsrb;
    volatile uint8_t * const _ucsrc;
    volatile uint8_t * const _udr;
    // Has any byte been written to the UART since begin()
    bool _written;

    volatile rx_buffer_index_t _rx_buffer_head;
    volatile rx_buffer_index_t _rx_buffer_tail;
    volatile tx_buffer_index_t _tx_buffer_head;
    volatile tx_buffer_index_t _tx_buffer_tail;

    // Don't put any members after these buffers, since only the first
    // 32 bytes of this struct can be accessed quickly using the ldd
    // instruction.
    unsigned char _rx_buffer[SERIAL_RX_BUFFER_SIZE];
    unsigned char _tx_buffer[SERIAL_TX_BUFFER_SIZE];

  public:
    inline HardwareSerial(
      volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
      volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
      volatile uint8_t *ucsrc, volatile uint8_t *udr);
    void begin(unsigned long baud) { begin(baud, SERIAL_8N1); }
    void begin(unsigned long, uint16_t);
    void end();
    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    int availableForWrite(void);
    virtual void flush(void);
    virtual size_t write(uint16_t);
    inline size_t write(unsigned long n) { return write((uint16_t)n); }
    inline size_t write(long n) { return write((uint16_t)n); }
    inline size_t write(int n) { return write((uint16_t)n); }
    inline size_t write(int8_t n) { return write((uint16_t)n); }
    inline size_t write(uint8_t n) { return write((uint16_t)n); }
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool() { return true; }

    // Interrupt handlers - Not intended to be called externally
    inline void _rx_complete_irq(void);
    void _tx_udr_empty_irq(void);
};

#if defined(UBRRH) || defined(UBRR0H)
  extern HardwareSerial Serial;
  #define HAVE_HWSERIAL0
#endif
#if defined(UBRR1H)
  extern HardwareSerial Serial1;
  #define HAVE_HWSERIAL1
#endif
#if defined(UBRR2H)
  extern HardwareSerial Serial2;
  #define HAVE_HWSERIAL2
#endif
#if defined(UBRR3H)
  extern HardwareSerial Serial3;
  #define HAVE_HWSERIAL3
#endif

extern void serialEventRun(void) __attribute__((weak));

#endif
