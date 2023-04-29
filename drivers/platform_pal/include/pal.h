#ifndef ZH_PAL_H
#define ZH_PAL_H

#include <linux/types.h>
#include <asm/ioctl.h>

/* struct definitions */
struct pal_fpga_ver {
	__u8  fpga_ver_num; 		//当前版本号为01
	__u32 fpag_compile_date; 	//FPGA编译日期，表示为年-月-日，格式为YYYY - MM - DD
	__u32 fpga_compile_time; 	//FPGA编译时间，表示为时-分-秒，24小时制，格式为HH : MM : SS

};

struct pal_info {
	__u32 pal_ping_addr; 		//PAL接收视频数据的乒内存地址
	__u32 pal_pong_addr; 		//PAL接收视频数据的乓内存地址
	__u32 pal_recv_data_len; 	//PAL接收视频数据的长度
	__u32 pal_dma_status; 
	//PAL接收视频数据DMA状态。SOC读取完数据将相应状态清0。
	//BIT[1:0]-乒显示存储区域；BIT[3:2]-乓显示存储区域
	//BIT[1:0]或BIT[3:2]=01：DMA未开始
	//BIT[1:0]或BIT[3:2]=01：DMA开始                                        
	//BIT[1:0]或BIT[3:2]=10：DMA进行中                                      
	//BIT[1:0]或BIT[3:2]=11：DMA完成"
	__u32 pal_rx_max_len;
};

struct pal_phycisal_info {
	__u32 pal_data_addr;        //当前可取数据的物理地址
	__u32 pal_data_len;         //当前可取数据的长度
};

/* ioctl cmd definitions */
#define PAL_GET_PING_ADDR		_IOR('p', 0x01, struct pal_info *)
#define PAL_SET_PING_ADDR		_IOW('p', 0x02, struct pal_info *)
#define PAL_GET_PONG_ADDR		_IOR('p', 0x03, struct pal_info *)
#define PAL_SET_PONG_ADDR		_IOW('p', 0x04, struct pal_info *)
#define PAL_GET_RX_LEN			_IOR('p', 0x05, struct pal_info *)
#define PAL_GET_FPGA_VER		_IOR('p', 0x06, struct pal_fpag_ver *)


#endif
