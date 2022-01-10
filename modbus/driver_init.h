/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef DRIVER_INIT_H_INCLUDED
#define DRIVER_INIT_H_INCLUDED

#include "atmel_start_pins.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <hal_atomic.h>
#include <hal_delay.h>
#include <hal_gpio.h>
#include <hal_init.h>
#include <hal_io.h>
#include <hal_sleep.h>

#include <hal_timer.h>

#include <hal_usart_sync.h>
#include <hpl_uart_base.h>
#include <hal_usart_async.h>

extern struct timer_descriptor TIMER_0;

extern struct usart_sync_descriptor  debug_UART0;
extern struct usart_async_descriptor USART_0;

void debug_UART0_PORT_init(void);
void debug_UART0_CLOCK_init(void);
void debug_UART0_init(void);
void debug_UART0_example(void);

void USART_0_PORT_init(void);
void USART_0_CLOCK_init(void);
void USART_0_init(void);

/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_H_INCLUDED
