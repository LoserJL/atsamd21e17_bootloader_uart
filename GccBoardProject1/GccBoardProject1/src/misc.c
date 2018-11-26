#include <asf.h>
#include "misc.h"

#if CONSOLE_OUTPUT_ENABLED
#include "conf_uart_serial.h"
#endif

#if CONSOLE_OUTPUT_ENABLED

struct usart_module cdc_uart_module;

/**
 * \brief Initializes the console output
 */
void console_init(void)
{
	struct usart_config uart_conf;

	usart_get_config_defaults(&uart_conf);
	uart_conf.mux_setting = CONF_STDIO_MUX_SETTING;
	uart_conf.pinmux_pad0 = CONF_STDIO_PINMUX_PAD0;
	uart_conf.pinmux_pad1 = CONF_STDIO_PINMUX_PAD1;
	uart_conf.pinmux_pad2 = CONF_STDIO_PINMUX_PAD2;
	uart_conf.pinmux_pad3 = CONF_STDIO_PINMUX_PAD3;
	uart_conf.baudrate    = CONF_STDIO_BAUDRATE;

	stdio_serial_init(&cdc_uart_module, CONF_STDIO_USART_MODULE, &uart_conf);
	usart_enable(&cdc_uart_module);
}
#endif

