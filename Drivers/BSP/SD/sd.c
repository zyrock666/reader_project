#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/SD/sd.h"

uint8_t sd_type = SD_TYPE_NOT_SD;
SD_CardInfo sd_card_info;

static uint8_t spi1_read_write_byte(uint8_t tx_data)
{
    uint8_t rx_data = 0;
    HAL_SPI_TransmitReceive(&g_spi1_handler, &tx_data, &rx_data, 1, 1000);
    return rx_data;
}

uint8_t sd_write_byte(uint8_t data)
{
    uint8_t retval = 0;
    retval = spi1_read_write_byte(data);
    return retval;
}

uint8_t sd_read_byte(void)
{
    uint8_t data = 0;
    data = spi1_read_write_byte(SD_DUMMY_BYTE);
    return data;
}

void sd_send_cmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint32_t i = 0x00;
    uint8_t frame[6] = { 0 };
    frame[0] = (cmd | 0x40);            /* 第一个字节 */
    frame[1] = (uint8_t)(arg >> 24);    /* 第二个字节 */
    frame[2] = (uint8_t)(arg >> 16);    /* 第三个字节 */
    frame[3] = (uint8_t)(arg >> 8);     /* 第四个字节 */
    frame[4] = (uint8_t)(arg);          /* 第五个字节 */
    frame[5] = crc;                     /* 第六个字节 */

    for (i = 0; i < 6; i++) {
        sd_write_byte(frame[i]);        /* 发送命令 */
    }
}

SD_Res sd_get_response(uint8_t response)
{
    uint32_t count = 0xFF;

    /* 检查是否接收到response对应的响应 */
    while ((sd_read_byte() != response) && count) {
        count--;
    }

    if (count == 0) {
        /* 检测超时 */
        return SD_RESPONSE_FAILURE;
    } else {
        /* 得到response表示的响应 */
        return SD_RESPONSE_NO_ERROR;
    }
}

uint8_t sd_get_data_response(void)
{
    uint32_t i = 0;
    uint8_t response = 0, rvalue = 0;

    while (i <= 64) {
        /* 读取响应 */
        response = sd_read_byte();
        /* 屏蔽无关的数据位(前三位xxx) */
        response &= 0x1F;
        switch (response) {
        case SD_DATA_OK: {
            rvalue = SD_DATA_OK;
            break;
        }
        case SD_DATA_CRC_ERROR: {
            return SD_DATA_CRC_ERROR;
        }
        case SD_DATA_WRITE_ERROR: {
            return SD_DATA_WRITE_ERROR;
        }
        default: {
            rvalue = SD_DATA_OTHER_ERROR;
            break;
        }
        }
        /*!< 数据正确, 退出循环 */
        if (rvalue == SD_DATA_OK)
            break;
        /*!< 循环变量自加 */
        i++;
    }

    /*!< 等待空数据 */
    while (sd_read_byte() == 0);

    /*!< 返回响应 */
    return response;
}

SD_Res sd_init(void)
{
    int8_t i = 0;
    spi1_init();

    SPI1_CS_SET();

    /*!< 发送80个周期的空数据 */
    for (i = 0; i < 10; i++) {
        sd_write_byte(SD_DUMMY_BYTE);
    }

    if (sd_go_idle_state() == SD_RESPONSE_FAILURE) {
        printf("sd card non idle\r\n");
        return SD_RESPONSE_FAILURE;
    }

    /*!< 获取卡的类型 */
    i = 100;
    do {
        sd_get_card_type();
        --i;
    } while ((sd_type == SD_TYPE_NOT_SD) && i);

    /*!< 不支持的卡 */
    if (sd_type == SD_TYPE_NOT_SD) {
        printf("SD_TYPE_NOT_SD\r\n");
        return SD_RESPONSE_FAILURE;
    }
    printf("sd card init done\r\n");
    return sd_get_card_info(&sd_card_info);
}

SD_Res sd_go_idle_state(void)
{
    uint8_t times_ = 10;
    SPI1_CS_RESET();

    do {
        sd_send_cmd(SD_CMD_GO_IDLE_STATE, 0, 0x95);
        times_--;
    } while (sd_get_response(SD_IN_IDLE_STATE) && times_);

    SPI1_CS_SET();
    sd_write_byte(SD_DUMMY_BYTE);
    return SD_RESPONSE_NO_ERROR;
}

SD_Res sd_get_card_type(void)
{
    uint32_t i = 0xFF;
    uint32_t count = 0xFF;

    uint8_t R7R3_Resp[4];
    uint8_t R1_Resp;

    SPI1_CS_SET();

    /*!< 发送空字节延时 0xFF */
    sd_write_byte(SD_DUMMY_BYTE);

    /*!< 片选信号CS低电平 */
    SPI1_CS_RESET();


    do {
        /*!< 发送 CMD8 命令，带0x1AA检查参数*/
        sd_send_cmd(SD_CMD_SEND_IF_COND, 0x1AA, 0x87);

        /*!< 等待R1响应 */
        while (( (R1_Resp = sd_read_byte()) == 0xFF) && count) {
            count--;
        }
        if (count == 0) {
            /*!< 等待超时 */
            return SD_RESPONSE_FAILURE;
        }
//        R1_Resp = sd_read_byte();
    } while ((--i) && !(R1_Resp == 0x01) && !(R1_Resp == 0x05));

    //响应 = 0x05   非V2.0的卡
    if (R1_Resp & 0x04) {
        /* 激活SD卡 */
        do {
            /*!< 片选信号CS高电平 */
            SPI1_CS_SET();

            /*!< 发送空字节延时 0xFF */
            sd_write_byte(SD_DUMMY_BYTE);

            /*!< 片选信号CS低电平 */
            SPI1_CS_RESET();

            /*!< 发送CMD1完成V1 版本卡的初始化 */
            sd_send_cmd(SD_CMD_SEND_OP_COND, 0, 0xFF);
            /*!< Wait for no error Response (R1 Format) equal to 0x00 */
        } while (sd_get_response(SD_RESPONSE_NO_ERROR));
        //V1版本的卡完成初始化

        sd_type = SD_TYPE_V1;
    }
    //响应 0x01   V2.0的卡
    else {
        /*!< 读取CMD8 的R7响应 */
        for (i = 0; i < 4; i++) {
            R7R3_Resp[i] = sd_read_byte();
        }

        /*!< 片选信号CS高电平 */
        SPI1_CS_SET();

        /*!< 发送空字节延时 0xFF */
        sd_write_byte(SD_DUMMY_BYTE);

        /*!< 片选信号CS低电平 */
        SPI1_CS_RESET();

        //判断该卡是否支持2.7-3.6V电压
        if (R7R3_Resp[2]==0x01 && R7R3_Resp[3]==0xAA) {
            //支持电压范围，可以操作
            count = 20;
            //发卡初始化指令CMD55+ACMD41
            do {
                //CMD55，以强调下面的是ACMD命令
                sd_send_cmd(SD_CMD_APP_CMD, 0, 0xFF);

                sd_write_byte(SD_DUMMY_BYTE);
                sd_write_byte(SD_DUMMY_BYTE);
                sd_write_byte(SD_DUMMY_BYTE);
                sd_write_byte(SD_DUMMY_BYTE);
                sd_write_byte(SD_DUMMY_BYTE);
                sd_write_byte(SD_DUMMY_BYTE);
                sd_write_byte(SD_DUMMY_BYTE);
                sd_write_byte(SD_DUMMY_BYTE);
                sd_write_byte(SD_DUMMY_BYTE);
                //ACMD41命令带HCS检查位
                sd_send_cmd(SD_ACMD_SD_SEND_OP_COND, 0x40000000, 0xFF);

                if (count-- == 0)
                    return SD_RESPONSE_FAILURE; //重试次数超时
            } while (sd_get_response(SD_RESPONSE_NO_ERROR));

            //初始化指令完成，读取OCR信息，CMD58

            //-----------鉴别SDSC SDHC卡类型开始-----------

            count = 200;
            do {
                /*!< 片选信号CS高电平 */
                SPI1_CS_SET();

                /*!< 发送空字节延时 0xFF */
                sd_write_byte(SD_DUMMY_BYTE);

                /*!< 片选信号CS低电平 */
                SPI1_CS_RESET();

                /*!< 发送CMD58 读取OCR寄存器 */
                sd_send_cmd(SD_CMD_READ_OCR, 0, 0xFF);
            } while ( sd_get_response(SD_RESPONSE_NO_ERROR) || count-- == 0);

            if (count == 0)
                return SD_RESPONSE_FAILURE; //重试次数超时

            //响应正常，读取R3响应

            /*!< 读取CMD58的R3响应 */
            for (i = 0; i < 4; i++) {
                R7R3_Resp[i] = sd_read_byte();
            }

            //检查接收到OCR中的bit30(CCS)
            //CCS = 0:SDSC       CCS = 1:SDHC
            if (R7R3_Resp[0]&0x40) { //检查CCS标志
                sd_type = SD_TYPE_V2HC;
            } else {
                sd_type = SD_TYPE_V2;
            }
            //-----------鉴别SDSC SDHC版本卡的流程结束-----------
        }
    }

    /*!< 片选信号CS高电平 */
    SPI1_CS_SET();

    /*!< 发送空字节延时 0xFF */
    sd_write_byte(SD_DUMMY_BYTE);

    //初始化正常返回
    return SD_RESPONSE_NO_ERROR;
}

SD_Res sd_get_csd_register(SD_CSD* sd_csd)
{
    uint32_t i = 0;
    SD_Res rvalue = SD_RESPONSE_FAILURE;
    uint8_t csd_table[16];

    /*!< SD chip select low */
    SPI1_CS_RESET();
    /*!< Send CMD9 (CSD register) or CMD10(CSD register) */
    sd_send_cmd(SD_CMD_SEND_CSD, 0, 0xFF);
    /*!< Wait for response in the R1 format (0x00 is no errors) */
    if (!sd_get_response(SD_RESPONSE_NO_ERROR))
    {
        if (!sd_get_response(SD_START_DATA_SINGLE_BLOCK_READ))
        {
            for (i = 0; i < 16; i++)
            {
                /*!< Store CSD register value on CSD_Tab */
                csd_table[i] = sd_read_byte();
            }
        }
        /*!< Get CRC bytes (not really needed by us, but required by SD) */
        sd_write_byte(SD_DUMMY_BYTE);
        sd_write_byte(SD_DUMMY_BYTE);
        /*!< Set response value to success */
        rvalue = SD_RESPONSE_NO_ERROR;
    }
    /*!< SD chip select high */
    SPI1_CS_SET();
    /*!< Send dummy byte: 8 Clock pulses of delay */
    sd_write_byte(SD_DUMMY_BYTE);

    /*!< Byte 0 */
    sd_csd->CSDStruct = (csd_table[0] & 0xC0) >> 6;
    sd_csd->SysSpecVersion = (csd_table[0] & 0x3C) >> 2;
    sd_csd->Reserved1 = csd_table[0] & 0x03;

    /*!< Byte 1 */
    sd_csd->TAAC = csd_table[1];

    /*!< Byte 2 */
    sd_csd->NSAC = csd_table[2];

    /*!< Byte 3 */
    sd_csd->MaxBusClkFrec = csd_table[3];

    /*!< Byte 4 */
    sd_csd->CardComdClasses = csd_table[4] << 4;

    /*!< Byte 5 */
    sd_csd->CardComdClasses |= (csd_table[5] & 0xF0) >> 4;
    sd_csd->RdBlockLen = csd_table[5] & 0x0F;

    /*!< Byte 6 */
    sd_csd->PartBlockRead = (csd_table[6] & 0x80) >> 7;
    sd_csd->WrBlockMisalign = (csd_table[6] & 0x40) >> 6;
    sd_csd->RdBlockMisalign = (csd_table[6] & 0x20) >> 5;
    sd_csd->DSRImpl = (csd_table[6] & 0x10) >> 4;
    sd_csd->Reserved2 = 0; /*!< Reserved */

    sd_csd->DeviceSize = (csd_table[6] & 0x03) << 10;

    //V1卡与SDSC卡的信息
    if ((sd_type == SD_TYPE_V1) || (sd_type == SD_TYPE_V2))
    {
        /*!< Byte 7 */
        sd_csd->DeviceSize |= (csd_table[7]) << 2;

        /*!< Byte 8 */
        sd_csd->DeviceSize |= (csd_table[8] & 0xC0) >> 6;

        sd_csd->MaxRdCurrentVDDMin = (csd_table[8] & 0x38) >> 3;
        sd_csd->MaxRdCurrentVDDMax = (csd_table[8] & 0x07);

        /*!< Byte 9 */
        sd_csd->MaxWrCurrentVDDMin = (csd_table[9] & 0xE0) >> 5;
        sd_csd->MaxWrCurrentVDDMax = (csd_table[9] & 0x1C) >> 2;
        sd_csd->DeviceSizeMul = (csd_table[9] & 0x03) << 1;
        /*!< Byte 10 */
        sd_csd->DeviceSizeMul |= (csd_table[10] & 0x80) >> 7;
    }
    //SDHC卡的信息
    else if (sd_type == SD_TYPE_V2HC)
    {
        sd_csd->DeviceSize = (csd_table[7] & 0x3F) << 16;

        sd_csd->DeviceSize |= (csd_table[8] << 8);

        sd_csd->DeviceSize |= (csd_table[9]);
    }


    sd_csd->EraseGrSize = (csd_table[10] & 0x40) >> 6;
    sd_csd->EraseGrMul = (csd_table[10] & 0x3F) << 1;

    /*!< Byte 11 */
    sd_csd->EraseGrMul |= (csd_table[11] & 0x80) >> 7;
    sd_csd->WrProtectGrSize = (csd_table[11] & 0x7F);

    /*!< Byte 12 */
    sd_csd->WrProtectGrEnable = (csd_table[12] & 0x80) >> 7;
    sd_csd->ManDeflECC = (csd_table[12] & 0x60) >> 5;
    sd_csd->WrSpeedFact = (csd_table[12] & 0x1C) >> 2;
    sd_csd->MaxWrBlockLen = (csd_table[12] & 0x03) << 2;

    /*!< Byte 13 */
    sd_csd->MaxWrBlockLen |= (csd_table[13] & 0xC0) >> 6;
    sd_csd->WriteBlockPaPartial = (csd_table[13] & 0x20) >> 5;
    sd_csd->Reserved3 = 0;
    sd_csd->ContentProtectAppli = (csd_table[13] & 0x01);

    /*!< Byte 14 */
    sd_csd->FileFormatGrouop = (csd_table[14] & 0x80) >> 7;
    sd_csd->CopyFlag = (csd_table[14] & 0x40) >> 6;
    sd_csd->PermWrProtect = (csd_table[14] & 0x20) >> 5;
    sd_csd->TempWrProtect = (csd_table[14] & 0x10) >> 4;
    sd_csd->FileFormat = (csd_table[14] & 0x0C) >> 2;
    sd_csd->ECC = (csd_table[14] & 0x03);

    /*!< Byte 15 */
    sd_csd->CSD_CRC = (csd_table[15] & 0xFE) >> 1;
    sd_csd->Reserved4 = 1;

    /*!< Return the reponse */
    return rvalue;
}

SD_Res sd_get_card_info(SD_CardInfo *cardinfo)
{
    SD_Res status = SD_RESPONSE_FAILURE;

    //读取CSD寄存器
    status = sd_get_csd_register(&(cardinfo->SD_csd));
    //读取CID寄存器
    status = sd_get_cid_register(&(cardinfo->SD_cid));

    if ((sd_type == SD_TYPE_V1) || (sd_type == SD_TYPE_V2)) {
        //块数目基数： CSize + 1
        cardinfo->CardCapacity = (cardinfo->SD_csd.DeviceSize + 1) ;
        //块数目 = 块数目基数*块数目乘数。块数目乘数： 2的 (C_SIZE_MULT + 2)次方
        cardinfo->CardCapacity *= (1 << (cardinfo->SD_csd.DeviceSizeMul + 2));
        // 块大小：2的READ_BL_LEN 次方
        cardinfo->CardBlockSize = 1 << (cardinfo->SD_csd.RdBlockLen);
        //卡容量 = 块数目*块大小
        cardinfo->CardCapacity *= cardinfo->CardBlockSize;
    } else if (sd_type == SD_TYPE_V2HC) { //SDHC卡
        cardinfo->CardCapacity = (uint64_t)(cardinfo->SD_csd.DeviceSize + 1) * 512 * 1024;
        cardinfo->CardBlockSize = 512;
    }

    /*!< 返回SD响应 */
    return status;
}

SD_Res sd_get_cid_register(SD_CID* sd_cid)
{
    uint32_t i = 0;
    SD_Res rvalue = SD_RESPONSE_FAILURE;
    uint8_t cid_table[16];

    /*!< SD chip select low */
    SPI1_CS_RESET();

    /*!< Send CMD10 (CID register) */
    sd_send_cmd(SD_CMD_SEND_CID, 0, 0xFF);

    /*!< Wait for response in the R1 format (0x00 is no errors) */
    if (!sd_get_response(SD_RESPONSE_NO_ERROR))
    {
        if (!sd_get_response(SD_START_DATA_SINGLE_BLOCK_READ))
        {
            /*!< Store CID register value on cid_table */
            for (i = 0; i < 16; i++)
            {
                cid_table[i] = sd_read_byte();
            }
        }
        /*!< Get CRC bytes (not really needed by us, but required by SD) */
        sd_write_byte(SD_DUMMY_BYTE);
        sd_write_byte(SD_DUMMY_BYTE);
        /*!< Set response value to success */
        rvalue = SD_RESPONSE_NO_ERROR;
    }
    /*!< SD chip select high */
    SPI1_CS_SET();
    /*!< Send dummy byte: 8 Clock pulses of delay */
    sd_write_byte(SD_DUMMY_BYTE);

    /*!< Byte 0 */
    sd_cid->ManufacturerID = cid_table[0];

    /*!< Byte 1 */
    sd_cid->OEM_AppliID = cid_table[1] << 8;

    /*!< Byte 2 */
    sd_cid->OEM_AppliID |= cid_table[2];

    /*!< Byte 3 */
    sd_cid->ProdName1 = cid_table[3] << 24;

    /*!< Byte 4 */
    sd_cid->ProdName1 |= cid_table[4] << 16;

    /*!< Byte 5 */
    sd_cid->ProdName1 |= cid_table[5] << 8;

    /*!< Byte 6 */
    sd_cid->ProdName1 |= cid_table[6];

    /*!< Byte 7 */
    sd_cid->ProdName2 = cid_table[7];

    /*!< Byte 8 */
    sd_cid->ProdRev = cid_table[8];

    /*!< Byte 9 */
    sd_cid->ProdSN = cid_table[9] << 24;

    /*!< Byte 10 */
    sd_cid->ProdSN |= cid_table[10] << 16;

    /*!< Byte 11 */
    sd_cid->ProdSN |= cid_table[11] << 8;

    /*!< Byte 12 */
    sd_cid->ProdSN |= cid_table[12];

    /*!< Byte 13 */
    sd_cid->Reserved1 |= (cid_table[13] & 0xF0) >> 4;
    sd_cid->ManufactDate = (cid_table[13] & 0x0F) << 8;

    /*!< Byte 14 */
    sd_cid->ManufactDate |= cid_table[14];

    /*!< Byte 15 */
    sd_cid->CID_CRC = (cid_table[15] & 0xFE) >> 1;
    sd_cid->Reserved2 = 1;

    /*!< Return the reponse */
    return rvalue;
}

SD_Res sd_read_block(uint8_t *p_buffer, uint64_t read_addr, uint16_t block_size)
{
    uint32_t i = 0;
    SD_Res rvalue = SD_RESPONSE_FAILURE;

    //SDHC卡块大小固定为512，且读命令中的地址的单位是sector
    if (sd_type == SD_TYPE_V2HC) {
        block_size = 512;
        read_addr /= 512;
    }

    /*!< 片选CS低电平*/
    SPI1_CS_RESET();

    /*!< 发送 CMD17 (SD_CMD_READ_SINGLE_BLOCK) 以读取一个数据块 */
    sd_send_cmd(SD_CMD_READ_SINGLE_BLOCK, read_addr, 0xFF);

    /*!< 检查R1响应 */
    if (!sd_get_response(SD_RESPONSE_NO_ERROR)) {
        /*!< 检查读取单个数据块的Token */
        if (!sd_get_response(SD_START_DATA_SINGLE_BLOCK_READ)) {
            /*!< 读取一个数据块的数据 :  NumByteToRead 个数据 */
            for (i = 0; i < block_size; i++) {
                /*!< 接收一个字节到pBuffer */
                *p_buffer = sd_read_byte();

                /*!< 指针加1 */
                p_buffer++;
            }
            /*!< 读取 CRC 校验字节 (此处不校验，但SD卡有该流程，需要接收) */
            sd_read_byte();
            sd_read_byte();
            /*!< 设置返回值，表示成功接收到寄存器数据*/
            rvalue = SD_RESPONSE_NO_ERROR;
        }
    }
    /*!< 片选信号CS高电平 */
    SPI1_CS_SET();

    /*!< 发送 dummy 空字节: 8 时钟周期的延时 */
    sd_write_byte(SD_DUMMY_BYTE);

    /*!< 返回接收状态 */
    return rvalue;
}

SD_Res sd_write_block(uint8_t* p_buffer, uint64_t write_addr, uint16_t block_size)
{
    uint32_t i = 0;
    SD_Res rvalue = SD_RESPONSE_FAILURE;

    //SDHC卡块大小固定为512，且写命令中的地址的单位是sector
    if (sd_type == SD_TYPE_V2HC) {
        block_size = 512;
        write_addr /= 512;
    }

    /*!< 片选CS低电平*/
    SPI1_CS_RESET();

    /*!< 发送 CMD24 (SD_CMD_WRITE_SINGLE_BLOCK) 以写入一个数据块 */
    sd_send_cmd(SD_CMD_WRITE_SINGLE_BLOCK, write_addr, 0xFF);

    /*!< 检查R1响应 */
    if (!sd_get_response(SD_RESPONSE_NO_ERROR)) {
        /*!< 发送 dummy 空字节 */
        sd_write_byte(SD_DUMMY_BYTE);

        /*!< 发送 一个数据块起始Token表示 开始传输数据 */
        sd_write_byte(0xFE);

        /*!< 写入一个数据块的数据 */
        for (i = 0; i < block_size; i++) {
            /*!< 发送指针指向的字节 */
            sd_write_byte(*p_buffer);
            /*!< 指针加1 */
            p_buffer++;
        }
        /*!< 两个字节的空CRC校验码，默认不验证 */
        sd_read_byte();
        sd_read_byte();

        /*!< 读取SD卡数据响应 */
        if (sd_get_data_response() == SD_DATA_OK) {
            rvalue = SD_RESPONSE_NO_ERROR;
        }
    }
    /*!< 片选信号CS高电平 */
    SPI1_CS_SET();
    /*!< 发送 dummy 空字节: 8 时钟周期的延时 */
    sd_write_byte(SD_DUMMY_BYTE);

    /*!< 返回接收状态 */
    return rvalue;
}

SD_Res sd_read_multi_blocks(uint8_t* p_buffer, uint64_t read_addr, uint16_t block_size, uint32_t num_of_blocks)
{
    uint32_t i = 0, Offset = 0;
    SD_Res rvalue = SD_RESPONSE_FAILURE;

    //SDHC?????С????512??????????е??????λ??sector
    if (sd_type == SD_TYPE_V2HC)
    {
        block_size = 512;
        read_addr /= 512;
    }

    /*!< SD chip select low */
    SPI1_CS_RESET();
    /*!< Data transfer */
    while (num_of_blocks--)
    {
        /*!< Send CMD17 (SD_CMD_READ_SINGLE_BLOCK) to read one block */
        sd_send_cmd (SD_CMD_READ_SINGLE_BLOCK, read_addr + Offset, 0xFF);
        /*!< Check if the SD acknowledged the read block command: R1 response (0x00: no errors) */
        if (sd_get_response(SD_RESPONSE_NO_ERROR))
        {
            return  SD_RESPONSE_FAILURE;
        }
        /*!< Now look for the data token to signify the start of the data */
        if (!sd_get_response(SD_START_DATA_SINGLE_BLOCK_READ))
        {
            /*!< Read the SD block data : read NumByteToRead data */
            for (i = 0; i < block_size; i++)
            {
                /*!< Read the pointed data */
                *p_buffer = sd_read_byte();
                /*!< Point to the next location where the byte read will be saved */
                p_buffer++;
            }
            /*!< Set next read address*/
            Offset += 512;
            /*!< get CRC bytes (not really needed by us, but required by SD) */
            sd_read_byte();
            sd_read_byte();
            /*!< Set response value to success */
            rvalue = SD_RESPONSE_NO_ERROR;
        }
        else
        {
            /*!< Set response value to failure */
            rvalue = SD_RESPONSE_FAILURE;
        }

        /* ???? Send dummy byte ???????????? */
        sd_write_byte(SD_DUMMY_BYTE);
    }
    /*!< SD chip select high */
    SPI1_CS_SET();
    /*!< Send dummy byte: 8 Clock pulses of delay */
    sd_write_byte(SD_DUMMY_BYTE);
    /*!< Returns the reponse */
    return rvalue;
}

SD_Res sd_write_multi_blocks(uint8_t* p_buffer, uint64_t write_addr, uint16_t block_size, uint32_t num_of_blocks)
{
    uint32_t i = 0, Offset = 0;
    SD_Res rvalue = SD_RESPONSE_FAILURE;

    //SDHC?????С????512????д?????е??????λ??sector
    if (sd_type == SD_TYPE_V2HC)
    {
        block_size = 512;
        write_addr /= 512;
    }

    /*!< SD chip select low */
    SPI1_CS_RESET();
    /*!< Data transfer */
    while (num_of_blocks--)
    {
        /*!< Send CMD24 (SD_CMD_WRITE_SINGLE_BLOCK) to write blocks */
        sd_send_cmd(SD_CMD_WRITE_SINGLE_BLOCK, write_addr + Offset, 0xFF);
        /*!< Check if the SD acknowledged the write block command: R1 response (0x00: no errors) */
        if (sd_get_response(SD_RESPONSE_NO_ERROR))
        {
            return SD_RESPONSE_FAILURE;
        }
        /*!< Send dummy byte */
        sd_write_byte(SD_DUMMY_BYTE);
        /*!< Send the data token to signify the start of the data */
        sd_write_byte(SD_START_DATA_SINGLE_BLOCK_WRITE);
        /*!< Write the block data to SD : write count data by block */
        for (i = 0; i < block_size; i++)
        {
            /*!< Send the pointed byte */
            sd_write_byte(*p_buffer);
            /*!< Point to the next location where the byte read will be saved */
            p_buffer++;
        }
        /*!< Set next write address */
        Offset += 512;
        /*!< Put CRC bytes (not really needed by us, but required by SD) */
        sd_read_byte();
        sd_read_byte();
        /*!< Read data response */
        if (sd_get_data_response() == SD_DATA_OK)
        {
            /*!< Set response value to success */
            rvalue = SD_RESPONSE_NO_ERROR;
        }
        else
        {
            /*!< Set response value to failure */
            rvalue = SD_RESPONSE_FAILURE;
        }
    }
    /*!< SD chip select high */
    SPI1_CS_SET();
    /*!< Send dummy byte: 8 Clock pulses of delay */
    sd_write_byte(SD_DUMMY_BYTE);
    /*!< Returns the reponse */
    return rvalue;
}



