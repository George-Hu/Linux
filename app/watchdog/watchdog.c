#include <stdio.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>

int errnon = -1;

#define HI_APPCOMM_MAX_PATH_LEN (64)
#define HAL_FD_INITIALIZATION_VAL (-1)
#define HAL_WATCHDOG_DEV "/dev/watchdog"
static int fd = HAL_FD_INITIALIZATION_VAL;

int watchdog_init(int seconds) 
{
    int ret = 1;

    if (seconds < 2 || seconds > 1000) {
        printf("Interval time should not be less then two and bigger then 100. %d\n", seconds);
        return -1;
    }

    fd = open(HAL_WATCHDOG_DEV, O_RDWR);

    if (fd < 0) {
        printf("open [%s] failed\n",HAL_WATCHDOG_DEV);
        return -1;
    }

    ret = ioctl(fd, WDIOC_SETTIMEOUT, &seconds);
    if(-1 == ret) {
        printf("WDIOC_SETTIMEOUT: failed, errnon(%d)\n", errnon);
        return -1;
    }
    printf("WDIOC_SETTIMEOUT: timeout=%ds\n", seconds);

    ret = ioctl(fd, WDIOC_KEEPALIVE);/**feed dog */
    if(-1 == ret) {
        printf("WDIOC_KEEPALIVE: failed, errnon(%d)\n", errnon);
        return -1;
    }

    return 1;
}

int hal_watchdog_feed(void) 
{
    int ret = 1;
    ret = ioctl(fd, WDIOC_KEEPALIVE);/**feed dog */
    if(-1 == ret) {
        printf("WDIOC_KEEPALIVE: failed, errnon(%d)\n", errnon);
        return -1;
    }
    return 1;
}

int hal_watchdog_deinit(void) 
{
    int ret;

    ret = close(fd);
    if (0 > ret) {
        printf("watchdog fd[%d] close,fail,errnon(%d)\n",fd,errnon);
        return -1;
    }

    return 1;
}

int main(void) 
{
    watchdog_init(3);

    while(1) {
        sleep(1);
        hal_watchdog_feed();
    }

    return 1;
}

