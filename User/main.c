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
    HAL_Init();                         /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* ����ʱ��, 72Mhz */
    delay_init(72);                     /* ��ʱ��ʼ�� */
    usart_init(115200);                 /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                         /* ��ʼ��LED */
    lcd_init();                         /* ��ʼ��LCD */
    key_init();                         /* ��ʼ������ */
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
        printf("SD����û���ļ�ϵͳ���������и�ʽ��...\r\n");
        res_sd = f_mkfs("", 0, NULL, 1);
        
        if (res_sd == FR_OK) {
            printf("SD���ѳɹ���ʽ���ļ�ϵͳ\r\n");
            res_sd = f_mount(NULL, "", 1);
            res_sd = f_mount(&fs, "", 1);
        } else {
            printf("��ʽ��ʧ��!!!\r\n");
        }
    } else if (res_sd != FR_OK) {
        printf("SD�������ļ�ϵͳʧ�� (%d)\r\n", res_sd);
        printf("������ΪSD����ʼ�����ɹ�\r\n");
    } else {
        printf("�ļ�ϵͳ���سɹ�, ���Խ��ж�д����!!!\r\n");
    }
    
    printf("��������дSD������!!!\r\n");
    res_sd = f_open(&fnew, "test.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (res_sd == FR_OK) {
        printf("����test.txt�ļ��ɹ�, ���ļ�д������!!!\r\n");
        res_sd = f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &fnum);
        if (res_sd == FR_OK) {
            printf("д���ļ��ɹ�, д���ֽ�����: %d\r\n", fnum);
            printf("д�������Ϊ: \r\n%s\r\n", WriteBuffer);
        } else {
            printf("д���ļ�ʧ��!!! (%d)\r\n", res_sd);
        }
        f_close(&fnew);
    } else {
        printf("����test.txt�ļ�ʧ��!!!\r\n");
    }
    
    printf("���������ļ���ȡ����...\r\n");
    res_sd = f_open(&fnew, "test.txt", FA_OPEN_EXISTING | FA_READ);
    if (res_sd == FR_OK) {
        printf("�ɹ����ļ�!!!\r\n");
        res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
        if (res_sd == FR_OK) {
            printf("�ɹ���ȡ�ļ�, �����ֽ�����: %d\r\n", fnum);
            printf("�������ļ�����Ϊ: \r\n%s\r\n", ReadBuffer);
        } else {
            printf("�ļ���ȡʧ��!!! (%d)\r\n", res_sd);
        }
    } else {
        printf("���ļ�ʧ��!!!\r\n");
    }
    f_close(&fnew);
    f_mount(NULL, "", 0);
}

