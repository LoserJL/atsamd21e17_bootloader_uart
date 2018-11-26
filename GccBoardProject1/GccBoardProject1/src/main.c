/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "misc.h"

#define APP_START_ADDRESS     (0x5000)

static void check_is_jump_to_application(void)
{
	uint32_t app_start_address;

	/* Load the Reset Handler address of the application */
	app_start_address = *(uint32_t *)(APP_START_ADDRESS + 4);

	/**
	 * Test reset vector of application @APP_START_ADDRESS+4
	 * Stay in SAM-BA if *(APP_START+0x4) == 0xFFFFFFFF
	 * Application erased condition
	 */
	if (app_start_address == 0xFFFFFFFF) {
		/* Stay in bootloader */
		return;
	}

	/* Rebase the Stack Pointer */
	__set_MSP(*(uint32_t *) APP_START_ADDRESS);

	/* Rebase the vector table base address */
	SCB->VTOR = ((uint32_t) APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk);

	/* Jump to application Reset Handler in the application */
	asm("bx %0"::"r"(app_start_address));
}

void power_on()
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	/* Configure LEDs as outputs, turn them off */
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PA07, &pin_conf);
	port_pin_set_output_level(PIN_PA07, true);
}

void configure_nvm(void)
{
	struct nvm_config config_nvm;
	nvm_get_config_defaults(&config_nvm);
	config_nvm.manual_page_write = false;
	nvm_set_config(&config_nvm);
}
/*
void write_to_flash(void)
{
	uint8_t write_page_buffer[NVMCTRL_PAGE_SIZE];
	uint8_t read_page_buffer[NVMCTRL_PAGE_SIZE];
	for (uint32_t i = 0; i < NVMCTRL_PAGE_SIZE; i++) {
		write_page_buffer[i] = i;
		read_page_buffer[i] = 1;
	}
	enum status_code error_code;
	do
	{
		error_code = nvm_erase_row(
		100 * NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE);
	} while (error_code == STATUS_BUSY);
	
	do
	{
		error_code = nvm_write_buffer(
		100 * NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE,
		write_page_buffer, NVMCTRL_PAGE_SIZE);
	} while (error_code == STATUS_BUSY);
	
	do
	{
		error_code = nvm_read_buffer(
		100 * NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE,
		read_page_buffer, NVMCTRL_PAGE_SIZE);
	} while (error_code == STATUS_BUSY);

	for (uint32_t i = 0; i < NVMCTRL_PAGE_SIZE; i++) {
		printf("%x ",read_page_buffer[i]);
	}
	printf("\r\n");
}
*/


void write_to_flash(int page_base, uint8_t *write_page_buffer)
{
	int i;
	enum status_code error_code;
	do
	{
		printf("start erase.....\r\n");
		error_code = nvm_erase_row(
		page_base * NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE);
	} while (error_code == STATUS_BUSY);
	printf("erase end.....\r\n");
	
	printf("start write.....\r\n");
	for(i=0; i<4; i++)
	{
		do
		{
			printf("write %d.....\r\n", i+1);
			error_code = nvm_write_buffer(
			page_base * NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE + i*64,
			write_page_buffer+i*64, NVMCTRL_PAGE_SIZE);
		} while (error_code == STATUS_BUSY);
		printf("write %d end.....\r\n", i+1);
	}
	printf("write end.....\r\n");
	
}

#if CONSOLE_OUTPUT_ENABLED
extern struct usart_module cdc_uart_module;
#endif

int main (void)
{
	uint8_t temp[1024];
	int page_base = 80;
	int j;

	check_is_jump_to_application();

	system_init();

	/* Insert application code here, after the board has been initialized. */
	//power_on();

	console_init();
	printf("start-bootloader----------\r\n");
	configure_nvm();
	printf("start-send-data----------\r\n");
	while(1)
	{
		if(usart_read_buffer_wait(&cdc_uart_module, temp, 1024) == STATUS_OK)
		{
			if(temp[0] == 0xff && temp[1] == 0xff && temp[2] == 0xff && temp[3] == 0xff && temp[4] == 0xff && temp[5] == 0xff)
			{
				printf("---------updata end-----------\r\n");
				break;
			}

			printf("start-write-data----------\r\n");
			for(j=0;j<4;j++)
			{
				write_to_flash(page_base++, temp+j*256);
			}
			
			printf("--------------------------\r\n");
			printf("continue-send-data----------\r\n");
		}
	}

	while(1);
}
