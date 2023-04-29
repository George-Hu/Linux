#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
 
#define USAR1  "/dev/ttyPS2"
#define USAR2  "/dev/ttyPS3"
#define USAR3  "/dev/ttyPS4"
#define USAR4  "/dev/ttyPS5"
char buf[255] = {0};
 
int set_port_attr (int fd,int  baudrate, int  databit, const char *stopbit, char parity, int vtime,int vmin );
static void set_baudrate (struct termios *opt, unsigned int baudrate);
static void set_data_bit (struct termios *opt, unsigned int databit);
static void set_stopbit (struct termios *opt, const char *stopbit);
static void set_parity (struct termios *opt, char parity);

int  set_port_attr(int fd,int  baudrate, int  databit, const char *stopbit, char parity, int vtime,int vmin ) {
    struct termios opt;
    tcgetattr(fd, &opt);       //获取初始设置
        
    set_baudrate(&opt, baudrate);
    set_data_bit(&opt, databit);
    set_parity(&opt, parity);
    set_stopbit(&opt, stopbit);
        
    opt.c_cflag &= ~CRTSCTS;   // 不使用硬件流控制
    opt.c_cflag |= CLOCAL | CREAD; //CLOCAL--忽略 modem 控制线,本地连线, 不具数据机控制功能, CREAD--使能接收标志 
 
    /*
    IXON--启用输出的 XON/XOFF 流控制
    IXOFF--启用输入的 XON/XOFF 流控制
    IXANY--允许任何字符来重新开始输出
    IGNCR--忽略输入中的回车
    */
    opt.c_iflag &= ~(IXON | IXOFF | IXANY);
    opt.c_oflag &= ~OPOST; //启用输出处理
    /*
    ICANON--启用标准模式 (canonical mode)。允许使用特殊字符 EOF, EOL,
        EOL2, ERASE, KILL, LNEXT, REPRINT, STATUS, 和 WERASE，以及按行的缓冲。
    ECHO--回显输入字符
    ECHOE--如果同时设置了 ICANON，字符 ERASE 擦除前一个输入字符，WERASE 擦除前一个词
    ISIG--当接受到字符 INTR, QUIT, SUSP, 或 DSUSP 时，产生相应的信号
    */
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); 
    opt.c_cc[VMIN] = vmin;   //设置非规范模式下的超时时长和最小字符数：阻塞模式起作用
    opt.c_cc[VTIME] = vtime; //VTIME与VMIN配合使用，是指限定的传输或等待的最长时间 单位：0.1S
        
    tcflush (fd, TCIFLUSH);                 /* TCIFLUSH-- update the options and do it NOW */
    return (tcsetattr (fd, TCSANOW, &opt)); /* TCSANOW--改变立即发生 */
}
 
 
static void set_baudrate(struct termios *opt, unsigned int baudrate) {
    cfsetispeed(opt, baudrate);
    cfsetospeed(opt, baudrate);
}
 
static void set_stopbit(struct termios *opt, const char *stopbit) {
    if (0 == strcmp (stopbit, "1")) {
        opt->c_cflag &= ~CSTOPB;                                                           /* 1位停止位t          */
    }else if (0 == strcmp (stopbit, "1.5")) {
        opt->c_cflag &= ~CSTOPB;                                                           /* 1.5位停止位         */
    }else if (0 == strcmp (stopbit, "2")) {
        opt->c_cflag |= CSTOPB;                                                            /* 2 位停止位          */
    }else {
        opt->c_cflag &= ~CSTOPB;                                                           /* 1 位停止位          */
    }
}
 
static void set_data_bit(struct termios *opt, unsigned int databit) {
    opt->c_cflag &= ~CSIZE;
    switch (databit) {
    case 8:
        opt->c_cflag |= CS8;
    break;
    case 7:
        opt->c_cflag |= CS7;
    break;
    case 6:
        opt->c_cflag |= CS6;
    break;
    case 5:
        opt->c_cflag |= CS5;
    break;
    default:
        opt->c_cflag |= CS8;
    break;
    }
}
 
static void set_parity(struct termios *opt, char parity) {
    switch (parity) {
    case 'N':                                                                                   /* 无校验          */
    case 'n':
        opt->c_cflag &= ~PARENB;
        break;
    case 'E':                                                                                   /* 偶校验          */
    case 'e':
        opt->c_cflag |= PARENB;
        opt->c_cflag &= ~PARODD;
        break;
    case 'O':                                                                                   /* 奇校验           */
    case 'o':
        opt->c_cflag |= PARENB;
        opt->c_cflag |= ~PARODD;
        break;
    case 'S':
    case 's':
        opt->c_cflag &= ~PARENB;                                                                 /*清除校验位   disable pairty checking Space校验  */
        opt->c_cflag &= ~CSTOPB;        
        opt->c_iflag |= INPCK;   
        break;
    default:                                                                                    /* 其它选择为无校验 */
        opt->c_cflag &= ~PARENB;
        break;
    }
}
 
void main() {
    int fd1,fd2,fd3,fd4;
    int ret;
    int rxlen = 0;
    fd_set rfds;
    struct timeval tv;
    int retval;
 
    fd1 = open(USAR1, O_RDWR | O_NOCTTY );//阻塞式读写             非阻塞| O_NDELAY | O_NONBLOCK
    if (fd1 < 0) {
      perror("open uart1 error\n");
    }
    fd2 = open(USAR2, O_RDWR | O_NOCTTY );//阻塞式读写             非阻塞| O_NDELAY | O_NONBLOCK
    if (fd2 < 0) {
      perror("open uart2 error\n");
    }
    fd3 = open(USAR3, O_RDWR | O_NOCTTY );//阻塞式读写             非阻塞| O_NDELAY | O_NONBLOCK
    if (fd3 < 0) {
      perror("open uart3 error\n");
    }
    fd4 = open(USAR4, O_RDWR | O_NOCTTY );//阻塞式读写             非阻塞| O_NDELAY | O_NONBLOCK
    if (fd4 < 0) {
      perror("open uart4 error\n");
    }
    ret = set_port_attr(fd1, B115200, 8, "1", 'N',1,255 );    /* 115200 8n1      */
    if(ret < 0) {
      printf("set uart1 arrt faile \n");
      exit(-1);
    }
    ret = set_port_attr(fd2, B115200, 8, "1", 'N',1,255 );    /* 115200 8n1      */
    if(ret < 0) {
      printf("set uart2 arrt faile \n");
      exit(-1);
    }
    ret = set_port_attr(fd3, B115200, 8, "1", 'N',1,255 );    /* 115200 8n1      */
    if(ret < 0) {
      printf("set uart3 arrt faile \n");
      exit(-1);
    }
    ret = set_port_attr(fd4, B115200, 8, "1", 'N',1,255 );    /* 115200 8n1      */
    if(ret < 0) {
      printf("set uart4 arrt faile \n");
      exit(-1);
    }
    while(1) {
       FD_ZERO(&rfds);
       FD_SET(fd1, &rfds);
       FD_SET(fd2, &rfds);
       FD_SET(fd3, &rfds);
       FD_SET(fd4, &rfds);
    //   tv.tv_sec = 1;                           //in block mode is not used
    //   tv.tv_usec = 0;
       ret = select(fd4 + 1, &rfds, NULL, NULL, NULL);  //block mode
       if(ret > 0) {
           if(FD_ISSET(fd1,&rfds)) {
               rxlen = read(fd1, buf, 255);
               if(rxlen > 0) {
                 printf("len = %d\n\r",rxlen);
                 printf("rx:%s\n\r",buf);
                 //write(fd1, buf, rxlen);
               }
           }
           if(FD_ISSET(fd2,&rfds)) {
               rxlen = read(fd2, buf, 255);
               if(rxlen > 0) {
                 printf("len = %d\n\r",rxlen);
                 printf("rx:%s\n\r",buf);
                 //write(fd2, buf, rxlen);
               }
           }
           if(FD_ISSET(fd3,&rfds)) {
               rxlen = read(fd3, buf, 255);
               if(rxlen > 0)
               {
                 printf("len = %d\n\r",rxlen);
                 printf("rx:%s\n\r",buf);
                 //write(fd3, buf, rxlen);
               }
           }
           if(FD_ISSET(fd4,&rfds)) {
               rxlen = read(fd4, buf, 255);
               if(rxlen > 0) {
                 printf("len = %d\n\r",rxlen);
                 printf("rx:%s\n\r",buf);
                 //write(fd4, buf, rxlen);
               }
           }
       }     
    }
}