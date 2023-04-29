#ifndef LED_CTRL_H
#define LED_CTRL_H


#define SERVO_TX_LED			((unsigned int)(1<<0)) //伺服串口数据发送指示灯
#define SERVO_RX_LED			((unsigned int)(1<<1)) //伺服串口数据接收指示灯
#define C_G_TX_LED				((unsigned int)(1<<2)) //C地面串口数据发送指示灯
#define C_G_RX_LED				((unsigned int)(1<<3)) //C地面串口数据接收指示灯
#define U_G_TX_LED				((unsigned int)(1<<4)) //U地面串口数据发送指示灯
#define	LED_ERR					((unsigned int)(1<<5))

#define C_UP_TX_LED				((unsigned int)(1<<12)) //C上行串口数据发送指示灯
#define U_UP_TX_LED				((unsigned int)(1<<13)) //U上行串口数据发送指示灯
#define C_TELE_RX_LED			((unsigned int)(1<<14)) //C单遥测串口数据接收指示灯
#define U_TELE_RX_LED			((unsigned int)(1<<15)) //U单遥测串口数据接收指示灯
#define U_G_RX_LED				((unsigned int)(1<<16)) //U地面串口数据接收指示灯
#define	LED_RUN					((unsigned int)(1<<17))





#endif
