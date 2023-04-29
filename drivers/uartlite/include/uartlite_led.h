#ifndef UART_LED_H
#define UART_LED_H

#define PL_LED_BASE_ADDR		0x43C00000

#define RSVD_UART0				"ttyUL0"        //预留串口
#define SERVO_UART				"ttyUL1"		//伺服串口(双向)
#define C_G_UART				"ttyUL2"		//C地面串口(双向)
#define U_G_UART				"ttyUL3"		//U地面串口(双向)
#define C_UP_UART				"ttyUL4"		//C上行串口(单向 发送)
#define U_UP_UART				"ttyUL5"		//U上行串口(单向 发送)
#define C_TELE_UART				"ttyUL6"		//C单遥测串口(单向 接收)
#define U_TELE_UART				"ttyUL7"		//U单遥测串口(单向 接收)
#define RSVD_USART8				"ttyUL8"		//预留同步串口
#define RSVD_USART9				"ttyUL9"		//预留同步串口

#define SERVO_TX_LED_ON			((unsigned int)(1<<0)) //伺服串口数据发送指示灯
#define SERVO_RX_LED_ON			((unsigned int)(1<<1)) //伺服串口数据接收指示灯
#define C_G_TX_LED_ON			((unsigned int)(1<<2)) //C地面串口数据发送指示灯
#define C_G_RX_LED_ON			((unsigned int)(1<<3)) //C地面串口数据接收指示灯
#define U_G_TX_LED_ON			((unsigned int)(1<<4)) //U地面串口数据发送指示灯
#define	LED_ERR_ON				((unsigned int)(1<<5))

#define C_UP_TX_LED_ON			((unsigned int)(1<<12)) //C上行串口数据发送指示灯
#define U_UP_TX_LED_ON			((unsigned int)(1<<13)) //U上行串口数据发送指示灯
#define C_TELE_RX_LED_ON		((unsigned int)(1<<14)) //C单遥测串口数据接收指示灯
#define U_TELE_RX_LED_ON		((unsigned int)(1<<15)) //U单遥测串口数据接收指示灯
#define U_G_RX_LED_ON			((unsigned int)(1<<16)) //U地面串口数据接收指示灯
#define	LED_RUN_ON				((unsigned int)(1<<17))

#define SERVO_TX_LED_OFF		(~((unsigned int)(1<<0))) //伺服串口数据发送指示灯
#define SERVO_RX_LED_OFF		(~((unsigned int)(1<<1))) //伺服串口数据接收指示灯
#define C_G_TX_LED_OFF			(~((unsigned int)(1<<2))) //C地面串口数据发送指示灯
#define C_G_RX_LED_OFF			(~((unsigned int)(1<<3))) //C地面串口数据接收指示灯
#define U_G_TX_LED_OFF			(~((unsigned int)(1<<4))) //U地面串口数据发送指示灯
#define	LED_ERR_OFF				(~((unsigned int)(1<<5)))

#define C_UP_TX_LED_OFF			(~((unsigned int)(1<<12))) //C上行串口数据发送指示灯
#define U_UP_TX_LED_OFF			(~((unsigned int)(1<<13))) //U上行串口数据发送指示灯
#define C_TELE_RX_LED_OFF		(~((unsigned int)(1<<14))) //C单遥测串口数据接收指示灯
#define U_TELE_RX_LED_OFF		(~((unsigned int)(1<<15))) //U单遥测串口数据接收指示灯
#define U_G_RX_LED_OFF			(~((unsigned int)(1<<16))) //U地面串口数据接收指示灯
#define	LED_RUN_OFF				(~((unsigned int)(1<<17)))

#endif
