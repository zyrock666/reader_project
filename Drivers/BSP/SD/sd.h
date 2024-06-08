#ifndef __SD_H
#define __SD_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    /**
    * @brief SD卡R1响应
    */
    SD_RESPONSE_NO_ERROR         = (0x00),
    SD_IN_IDLE_STATE             = (0x01),
    SD_ERASE_RESET               = (0x02),
    SD_ILLEGAL_COMMAND           = (0x04),
    SD_COM_CRC_ERROR             = (0x08),
    SD_ERASE_SEQUENCE_ERROR      = (0x10),
    SD_ADDRESS_ERROR             = (0x20),
    SD_PARAMETER_ERROR           = (0x40),
    SD_RESPONSE_FAILURE          = (0xFF),
    
    /**
    * @brief 数据响应
    */
    SD_DATA_OK                   = (0x05),
    SD_DATA_CRC_ERROR            = (0x0B),
    SD_DATA_WRITE_ERROR          = (0x0D),
    SD_DATA_OTHER_ERROR          = (0xFF)
} SD_Res;

/** 
  * @brief  Card Specific Data: CSD Register   
  */ 
typedef struct
{
  __IO uint8_t  CSDStruct;            /*!< CSD structure */
  __IO uint8_t  SysSpecVersion;       /*!< System specification version */
  __IO uint8_t  Reserved1;            /*!< Reserved */
  __IO uint8_t  TAAC;                 /*!< Data read access-time 1 */
  __IO uint8_t  NSAC;                 /*!< Data read access-time 2 in CLK cycles */
  __IO uint8_t  MaxBusClkFrec;        /*!< Max. bus clock frequency */
  __IO uint16_t CardComdClasses;      /*!< Card command classes */
  __IO uint8_t  RdBlockLen;           /*!< Max. read data block length */
  __IO uint8_t  PartBlockRead;        /*!< Partial blocks for read allowed */
  __IO uint8_t  WrBlockMisalign;      /*!< Write block misalignment */
  __IO uint8_t  RdBlockMisalign;      /*!< Read block misalignment */
  __IO uint8_t  DSRImpl;              /*!< DSR implemented */
  __IO uint8_t  Reserved2;            /*!< Reserved */
  __IO uint32_t DeviceSize;           /*!< Device Size */
  __IO uint8_t  MaxRdCurrentVDDMin;   /*!< Max. read current @ VDD min */
  __IO uint8_t  MaxRdCurrentVDDMax;   /*!< Max. read current @ VDD max */
  __IO uint8_t  MaxWrCurrentVDDMin;   /*!< Max. write current @ VDD min */
  __IO uint8_t  MaxWrCurrentVDDMax;   /*!< Max. write current @ VDD max */
  __IO uint8_t  DeviceSizeMul;        /*!< Device size multiplier */
  __IO uint8_t  EraseGrSize;          /*!< Erase group size */
  __IO uint8_t  EraseGrMul;           /*!< Erase group size multiplier */
  __IO uint8_t  WrProtectGrSize;      /*!< Write protect group size */
  __IO uint8_t  WrProtectGrEnable;    /*!< Write protect group enable */
  __IO uint8_t  ManDeflECC;           /*!< Manufacturer default ECC */
  __IO uint8_t  WrSpeedFact;          /*!< Write speed factor */
  __IO uint8_t  MaxWrBlockLen;        /*!< Max. write data block length */
  __IO uint8_t  WriteBlockPaPartial;  /*!< Partial blocks for write allowed */
  __IO uint8_t  Reserved3;            /*!< Reserded */
  __IO uint8_t  ContentProtectAppli;  /*!< Content protection application */
  __IO uint8_t  FileFormatGrouop;     /*!< File format group */
  __IO uint8_t  CopyFlag;             /*!< Copy flag (OTP) */
  __IO uint8_t  PermWrProtect;        /*!< Permanent write protection */
  __IO uint8_t  TempWrProtect;        /*!< Temporary write protection */
  __IO uint8_t  FileFormat;           /*!< File Format */
  __IO uint8_t  ECC;                  /*!< ECC code */
  __IO uint8_t  CSD_CRC;              /*!< CSD CRC */
  __IO uint8_t  Reserved4;            /*!< always 1*/
} SD_CSD;
/**
* @brief  Card Identification Data: CID 寄存器
*/
typedef struct {
    __IO uint8_t  ManufacturerID;       /*!< ManufacturerID */
    __IO uint16_t OEM_AppliID;          /*!< OEM/Application ID */
    __IO uint32_t ProdName1;            /*!< Product Name part1 */
    __IO uint8_t  ProdName2;            /*!< Product Name part2*/
    __IO uint8_t  ProdRev;              /*!< Product Revision */
    __IO uint32_t ProdSN;               /*!< Product Serial Number */
    __IO uint8_t  Reserved1;            /*!< Reserved1 */
    __IO uint16_t ManufactDate;         /*!< Manufacturing Date */
    __IO uint8_t  CID_CRC;              /*!< CID CRC */
    __IO uint8_t  Reserved2;            /*!< always 1 */
} SD_CID;

/**
* @brief SD Card information
*/
typedef struct {
    SD_CSD SD_csd;
    SD_CID SD_cid;
    uint64_t CardCapacity;  /*!< Card Capacity */
    uint64_t CardBlockSize; /*!< Card Block Size */
} SD_CardInfo;

/**
* @brief 数据块开始和停止的Token
*/
#define SD_START_DATA_SINGLE_BLOCK_READ     0xFE

#define SD_START_DATA_MULTIPLE_BLOCK_READ   0xFE

#define SD_START_DATA_SINGLE_BLOCK_WRITE    0xFE

#define SD_START_DATA_MULTIPLE_BLOCK_WRITE  0xFC

#define SD_STOP_DATA_MULTIPLE_BLOCK_WRITE   0xFD

/**
* @brief CMD命令定义
*/
#define SD_CMD_GO_IDLE_STATE          0   /*!< CMD0 = 0x40 */
#define SD_CMD_SEND_OP_COND           1   /*!< CMD1 = 0x41 */
#define SD_CMD_SEND_IF_COND           8   /*!< CMD8 = 0x48 */
#define SD_CMD_SEND_CSD               9   /*!< CMD9 = 0x49 */
#define SD_CMD_SEND_CID               10  /*!< CMD10 = 0x4A */
#define SD_CMD_STOP_TRANSMISSION      12  /*!< CMD12 = 0x4C */
#define SD_CMD_SEND_STATUS            13  /*!< CMD13 = 0x4D */
#define SD_CMD_SET_BLOCKLEN           16  /*!< CMD16 = 0x50 */
#define SD_CMD_READ_SINGLE_BLOCK      17  /*!< CMD17 = 0x51 */
#define SD_CMD_READ_MULT_BLOCK        18  /*!< CMD18 = 0x52 */
#define SD_CMD_SET_BLOCK_COUNT        23  /*!< CMD23 = 0x57 */
#define SD_CMD_WRITE_SINGLE_BLOCK     24  /*!< CMD24 = 0x58 */
#define SD_CMD_WRITE_MULT_BLOCK       25  /*!< CMD25 = 0x59 */
#define SD_CMD_PROG_CSD               27  /*!< CMD27 = 0x5B */
#define SD_CMD_SET_WRITE_PROT         28  /*!< CMD28 = 0x5C */
#define SD_CMD_CLR_WRITE_PROT         29  /*!< CMD29 = 0x5D */
#define SD_CMD_SEND_WRITE_PROT        30  /*!< CMD30 = 0x5E */
#define SD_CMD_SD_ERASE_GRP_START     32  /*!< CMD32 = 0x60 */
#define SD_CMD_SD_ERASE_GRP_END       33  /*!< CMD33 = 0x61 */
#define SD_CMD_UNTAG_SECTOR           34  /*!< CMD34 = 0x62 */
#define SD_CMD_ERASE_GRP_START        35  /*!< CMD35 = 0x63 */
#define SD_CMD_ERASE_GRP_END          36  /*!< CMD36 = 0x64 */
#define SD_CMD_UNTAG_ERASE_GROUP      37  /*!< CMD37 = 0x65 */
#define SD_CMD_ERASE                  38  /*!< CMD38 = 0x66 */
#define SD_CMD_READ_OCR               58 /*!< CMD58 */
#define SD_CMD_APP_CMD                55 /*!< CMD55 返回0x01*/
#define SD_ACMD_SD_SEND_OP_COND       41 /*!< ACMD41  返回0x00*/

#define SD_DUMMY_BYTE                 0xFF /*！< 发送一个空字节 */

//SD卡的类型
#define SD_TYPE_NOT_SD      0   //非SD卡
#define SD_TYPE_V1          1   //V1.0的卡
#define SD_TYPE_V2          2   //SDSC
#define SD_TYPE_V2HC        4   //SDHC

/**
* @brief  向SD卡发送一个字节
* @param  data: 要发送的数据
* @retval 接收到的数据
*/
uint8_t sd_write_byte(uint8_t data);

/**
* @brief  从SD卡读取一个字节
* @param  None
* @retval 接收到的数据
*/
uint8_t sd_read_byte(void);

/**
* @brief 发送SD命令
* @param cmd: 要发送的命令
* @param arg: 命令参数
* @param crc: crc校验码
*/
void sd_send_cmd(uint8_t cmd, uint32_t arg, uint8_t crc);

/**
* @brief  获取SD的R1响应
* @param  要检查的R1响应类型
* @retval SD响应：
            - SD_RESPONSE_FAILURE:失败
            - SD_RESPONSE_NO_ERROR:成功
*/
SD_Res sd_get_response(uint8_t response);

/**
* @brief  获取SD卡的数据响应
* @param  None
* @retval 返回响应状态 status: 读取到的数据响应格式 xxx0<status>1
            -status 010: 接受数据
            -status 101: crc校验错误
            -status 110: 写入错误
            -status 111: 其他错误
*/
uint8_t sd_get_data_response(void);

/**
* @brief  初始化SD卡
* @param  None
* @retval SD响应: 
            - SD_RESPONSE_FAILURE: 初始化失败
            - SD_RESPONSE_NO_ERROR: 初始化成功
*/
SD_Res sd_init(void);

/**
* @brief  让SD卡进入SPI空闲模式
* @param  None
* @retval SD卡响应:
            - SD_RESPONSE_FAILURE: 失败
            - SD_RESPONSE_NO_ERROR: 成功
*/
SD_Res sd_go_idle_state(void);

/**
* @brief  获取SD卡的版本类型，并区分SDSC和SDHC
* @param  无
* @retval SD响应:
*         - SD_RESPONSE_FAILURE: 失败
*         - SD_RESPONSE_NO_ERROR: 成功
*/
SD_Res sd_get_card_type(void);

/**
* @brief  读取卡的CSD寄存器
*         在SPI模式下，读取CSD的方式与读取数据块类似
* @param  sd_csd: 存储CSD寄存器的SD_CSD结构体指针
* @retval SD响应:
*         - SD_RESPONSE_FAILURE: 失败
*         - SD_RESPONSE_NO_ERROR: 成功
*/
SD_Res sd_get_csd_register(SD_CSD* sd_csd);

/**
* @brief  获取SD卡的信息.
* @param  SD_CardInfo 结构体指针
* @retval SD响应:
*         - SD_RESPONSE_FAILURE: 失败
*         - SD_RESPONSE_NO_ERROR: 成功
*/
SD_Res sd_get_card_info(SD_CardInfo *cardinfo);

/**
  * @brief  Read the CID card register.
  *         Reading the contents of the CID register in SPI mode is a simple 
  *         read-block transaction.
  * @param  SD_cid: pointer on an CID register structure
  * @retval The SD Response: 
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Res sd_get_cid_register(SD_CID* sd_cid);

/**
* @brief  从SD卡读取一个数据块
* @param  pBuffer: 指针，用于存储读取到的数据
* @param  ReadAddr: 要读取的SD卡内部地址
* @param  BlockSize: 块大小
* @retval SD响应:
*         - SD_RESPONSE_FAILURE: 失败
*         - SD_RESPONSE_NO_ERROR: 成功
*/
SD_Res sd_read_block(uint8_t *p_buffer, uint64_t read_addr, uint16_t block_size);

/**
* @brief  往SD卡写入一个数据块
* @param  pBuffer: 指针，指向将要写入的数据
* @param  WriteAddr: 要写入的SD卡内部地址
* @param  BlockSize: 块大小
* @retval SD响应:
*         - SD_RESPONSE_FAILURE: 失败
*         - SD_RESPONSE_NO_ERROR: 成功
*/
SD_Res sd_write_block(uint8_t* p_buffer, uint64_t write_addr, uint16_t block_size);

/**
  * @brief  Reads multiple block of data from the SD.
  * @param  pBuffer: pointer to the buffer that receives the data read from the 
  *                  SD.
  * @param  ReadAddr: SD's internal address to read from.
  * @param  BlockSize: the SD card Data block size.
  * @param  NumberOfBlocks: number of blocks to be read.
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Res sd_read_multi_blocks(uint8_t* p_buffer, uint64_t read_addr, uint16_t block_size, uint32_t num_of_blocks);

/**
  * @brief  Writes many blocks on the SD
  * @param  pBuffer: pointer to the buffer containing the data to be written on 
  *                  the SD.
  * @param  WriteAddr: address to write on.
  * @param  BlockSize: the SD card Data block size.
  * @param  NumberOfBlocks: number of blocks to be written.
  * @retval The SD Response: 
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Res sd_write_multi_blocks(uint8_t* p_buffer, uint64_t write_addr, uint16_t block_size, uint32_t num_of_blocks);

extern uint8_t sd_type;
extern SD_CardInfo sd_card_info;


#ifdef __cplusplus
}
#endif


#endif




