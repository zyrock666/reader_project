#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/SD/sd.h"
#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

void fatfs_demo(void);

int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
    delay_init(72);                     /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    key_init();                         /* 初始化按键 */
    sd_init();
    printf("sd card block size: %llu\r\n", sd_card_info.CardBlockSize);
    printf("sd card capacity: %llu\r\n", sd_card_info.CardCapacity);
    
//    fatfs_demo();
    lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30,  70, 200, 16, 16, "SPI TEST", RED);
    lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    while (1)
    {
        
    }
}

void fatfs_demo(void)
{
    FATFS fs;
    FIL fnew;
    FRESULT res_sd;
    UINT fnum;
    BYTE ReadBuffer[1024] = { 0 };
    BYTE WriteBuffer[] = "hello world!!!!\r\n";
    BYTE work[FF_MAX_SS];
    
    res_sd = f_mount(&fs, "", 0);
    if (res_sd == FR_NO_FILESYSTEM) {
        printf("SD卡还没有文件系统，即将进行格式化...\r\n");
        res_sd = f_mkfs("", 0, NULL, 1);
        
        if (res_sd == FR_OK) {
            printf("SD卡已成功格式化文件系统\r\n");
            res_sd = f_mount(NULL, "", 1);
            res_sd = f_mount(&fs, "", 1);
        } else {
            printf("格式化失败!!!\r\n");
        }
    } else if (res_sd != FR_OK) {
        printf("SD卡挂载文件系统失败 (%d)\r\n", res_sd);
        printf("可能因为SD卡初始化不成功\r\n");
    } else {
        printf("文件系统挂载成功, 可以进行读写测试!!!\r\n");
    }
    
    printf("即将进行写SD卡测试!!!\r\n");
    res_sd = f_open(&fnew, "test.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (res_sd == FR_OK) {
        printf("创建test.txt文件成功, 向文件写入数据!!!\r\n");
        res_sd = f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &fnum);
        if (res_sd == FR_OK) {
            printf("写入文件成功, 写入字节数据: %d\r\n", fnum);
            printf("写入的数据为: \r\n%s\r\n", WriteBuffer);
        } else {
            printf("写入文件失败!!! (%d)\r\n", res_sd);
        }
        f_close(&fnew);
    } else {
        printf("创建test.txt文件失败!!!\r\n");
    }
    
    printf("即将进行文件读取测试...\r\n");
    res_sd = f_open(&fnew, "test.txt", FA_OPEN_EXISTING | FA_READ);
    if (res_sd == FR_OK) {
        printf("成功打开文件!!!\r\n");
        res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
        if (res_sd == FR_OK) {
            printf("成功读取文件, 读到字节数据: %d\r\n", fnum);
            printf("读到的文件数据为: \r\n%s\r\n", ReadBuffer);
        } else {
            printf("文件读取失败!!! (%d)\r\n", res_sd);
        }
    } else {
        printf("打开文件失败!!!\r\n");
    }
    f_close(&fnew);
    f_mount(NULL, "", 0);
}

