
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/console.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/clk.h>


#include "../include/uartlite_led.h"


#define LED_ULITE_STATUS			0x08

#define LED_ULITE_STATUS_RXVALID	0x01
#define LED_ULITE_STATUS_RXFULL		0x02
#define LED_ULITE_STATUS_TXEMPTY	0x04
#define LED_ULITE_STATUS_TXFULL		0x08


void *pl_led_base_addr;
int uartlite_led_mem_init_flag=0;


int uartlite_led_mem_init(void) 
{
	if (0 == uartlite_led_mem_init_flag) {
		if (!request_mem_region(PL_LED_BASE_ADDR, 4, "uartlite_led")) {
			pr_err("%s uartlite_led Memory region busy\n", __func__);	
			return -EBUSY;
		}

		pl_led_base_addr = ioremap(PL_LED_BASE_ADDR, 4);
		if (!pl_led_base_addr) {		
			pr_err("%s Unable to map registers\n", __func__);
			release_mem_region(PL_LED_BASE_ADDR, 4);
			return -EBUSY;
		}
		pr_info("%s  ok!\n",__func__);
		uartlite_led_mem_init_flag = 1;
	}
	return 0;
}
EXPORT_SYMBOL_GPL(uartlite_led_mem_init);

int uartlite_led_mem_exit(void) 
{
	if (1 == uartlite_led_mem_init_flag) {
		release_mem_region(PL_LED_BASE_ADDR, 4);
		iounmap(pl_led_base_addr);	

		pr_info("%s  ok!\n",__func__);
		uartlite_led_mem_init_flag = 0;
	}
	return 0;	
}
EXPORT_SYMBOL_GPL(uartlite_led_mem_exit);

//void uartlite_led_on(u32 led_value, void __iomem *addr)
void uartlite_led_on(u32 led_value) 
{
	u32 val;

	if (0 == uartlite_led_mem_init()) {
		val = ioread32(pl_led_base_addr);
		val = val | led_value;
		iowrite32(val, pl_led_base_addr);
	}
}

//void uartlite_led_off(u32 led_value, void __iomem *addr) 
void uartlite_led_off(u32 led_value) 
{
	u32 val;

	if (0 == uartlite_led_mem_init()) {
		val = ioread32(pl_led_base_addr);
		val = val & led_value;
		iowrite32(val, pl_led_base_addr);
	}
}

void uartlite_ctrl_led_on_off(struct uart_port *port) 
{
	int stat;

	pr_info("%s port->name is %s.\n",__func__,port->name);
	//struct tty_port *tport = &port->state->port;

	stat = ioread32(port->membase + LED_ULITE_STATUS);

	if (!strcmp(port->name,SERVO_UART)) { //伺服串口(双向)
		if (stat & LED_ULITE_STATUS_RXVALID) { /* 接收中 */
			uartlite_led_on(SERVO_RX_LED_ON);
		}
		else {
			uartlite_led_off(SERVO_RX_LED_OFF);
		}
		if (~(stat & LED_ULITE_STATUS_TXEMPTY)) { /* 发送中 */
			uartlite_led_on(SERVO_TX_LED_ON);
		}
		else {
			uartlite_led_off(SERVO_TX_LED_OFF);
		}
	} else if (!strcmp(port->name,C_G_UART)) { //C地面串口(双向)
		if (stat & LED_ULITE_STATUS_RXVALID) { /* 接收中 */
			uartlite_led_on(C_G_RX_LED_ON);
		}
		else {
			uartlite_led_off(C_G_RX_LED_OFF);
		}
		if (~(stat & LED_ULITE_STATUS_TXEMPTY)) { /* 发送中 */
			uartlite_led_on(C_G_TX_LED_ON);
		}
		else {
			uartlite_led_off(C_G_TX_LED_OFF);
		}
	} else if (!strcmp(port->name,U_G_UART)) { //U地面串口(双向)
		if (stat & LED_ULITE_STATUS_RXVALID) { /* 接收中 */
			uartlite_led_on(U_G_RX_LED_ON);
		}
		else {
			uartlite_led_off(U_G_RX_LED_OFF);
		}
		if (~(stat & LED_ULITE_STATUS_TXEMPTY)) { /* 发送中 */
			uartlite_led_on(U_G_TX_LED_ON);
		}
		else {
			uartlite_led_off(U_G_TX_LED_OFF);
		}
	} else if (!strcmp(port->name,C_UP_UART)) { //C上行串口(单向)
		if (stat & LED_ULITE_STATUS_RXVALID) { /* 接收中 */
			uartlite_led_on(C_UP_TX_LED_ON);
		}
		else {
			uartlite_led_off(C_UP_TX_LED_OFF);
		}
	} else if (!strcmp(port->name,U_UP_UART)) { //U上行串口(单向)		
		if (stat & LED_ULITE_STATUS_RXVALID) { /* 接收中 */
			uartlite_led_on(U_UP_TX_LED_ON);
		}
		else {
			uartlite_led_off(U_UP_TX_LED_OFF);
		}
	} else if (!strcmp(port->name,C_TELE_UART)) { //C单遥测串口(单向)
		if (stat & LED_ULITE_STATUS_RXVALID) { /* 接收中 */
			uartlite_led_on(C_TELE_RX_LED_ON);
		}
		else {
			uartlite_led_off(C_TELE_RX_LED_OFF);
		}
	} else if (!strcmp(port->name,U_TELE_UART)) { //U单遥测串口(单向)
		if (stat & LED_ULITE_STATUS_RXVALID) { /* 接收中 */
			uartlite_led_on(U_TELE_RX_LED_ON);
		}
		else {
			uartlite_led_off(U_TELE_RX_LED_OFF);
		}
	}
	else {
		pr_info("%s %s has no led!",__func__,port->name);
	}
}
EXPORT_SYMBOL_GPL(uartlite_ctrl_led_on_off);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("huqiqiao <huqq@zoonghe.com>");
MODULE_DESCRIPTION("uartlite serial with led on/off driver");
MODULE_INFO(intree, "Y");
