#ifndef __SD_H
#define __SD_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    /**
    * @brief SD��R1��Ӧ
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
    * @brief ������Ӧ
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
* @brief  Card Identification Data: CID �Ĵ���
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
* @brief ���ݿ鿪ʼ��ֹͣ��Token
*/
#define SD_START_DATA_SINGLE_BLOCK_READ     0xFE

#define SD_START_DATA_MULTIPLE_BLOCK_READ   0xFE

#define SD_START_DATA_SINGLE_BLOCK_WRITE    0xFE

#define SD_START_DATA_MULTIPLE_BLOCK_WRITE  0xFC

#define SD_STOP_DATA_MULTIPLE_BLOCK_WRITE   0xFD

/**
* @brief CMD�����
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
#define SD_CMD_APP_CMD                55 /*!< CMD55 ����0x01*/
#define SD_ACMD_SD_SEND_OP_COND       41 /*!< ACMD41  ����0x00*/

#define SD_DUMMY_BYTE                 0xFF /*��< ����һ�����ֽ� */

//SD��������
#define SD_TYPE_NOT_SD      0   //��SD��
#define SD_TYPE_V1          1   //V1.0�Ŀ�
#define SD_TYPE_V2          2   //SDSC
#define SD_TYPE_V2HC        4   //SDHC

/**
* @brief  ��SD������һ���ֽ�
* @param  data: Ҫ���͵�����
* @retval ���յ�������
*/
uint8_t sd_write_byte(uint8_t data);

/**
* @brief  ��SD����ȡһ���ֽ�
* @param  None
* @retval ���յ�������
*/
uint8_t sd_read_byte(void);

/**
* @brief ����SD����
* @param cmd: Ҫ���͵�����
* @param arg: �������
* @param crc: crcУ����
*/
void sd_send_cmd(uint8_t cmd, uint32_t arg, uint8_t crc);

/**
* @brief  ��ȡSD��R1��Ӧ
* @param  Ҫ����R1��Ӧ����
* @retval SD��Ӧ��
            - SD_RESPONSE_FAILURE:ʧ��
            - SD_RESPONSE_NO_ERROR:�ɹ�
*/
SD_Res sd_get_response(uint8_t response);

/**
* @brief  ��ȡSD����������Ӧ
* @param  None
* @retval ������Ӧ״̬ status: ��ȡ����������Ӧ��ʽ xxx0<status>1
            -status 010: ��������
            -status 101: crcУ�����
            -status 110: д�����
            -status 111: ��������
*/
uint8_t sd_get_data_response(void);

/**
* @brief  ��ʼ��SD��
* @param  None
* @retval SD��Ӧ: 
            - SD_RESPONSE_FAILURE: ��ʼ��ʧ��
            - SD_RESPONSE_NO_ERROR: ��ʼ���ɹ�
*/
SD_Res sd_init(void);

/**
* @brief  ��SD������SPI����ģʽ
* @param  None
* @retval SD����Ӧ:
            - SD_RESPONSE_FAILURE: ʧ��
            - SD_RESPONSE_NO_ERROR: �ɹ�
*/
SD_Res sd_go_idle_state(void);

/**
* @brief  ��ȡSD���İ汾���ͣ�������SDSC��SDHC
* @param  ��
* @retval SD��Ӧ:
*         - SD_RESPONSE_FAILURE: ʧ��
*         - SD_RESPONSE_NO_ERROR: �ɹ�
*/
SD_Res sd_get_card_type(void);

/**
* @brief  ��ȡ����CSD�Ĵ���
*         ��SPIģʽ�£���ȡCSD�ķ�ʽ���ȡ���ݿ�����
* @param  sd_csd: �洢CSD�Ĵ�����SD_CSD�ṹ��ָ��
* @retval SD��Ӧ:
*         - SD_RESPONSE_FAILURE: ʧ��
*         - SD_RESPONSE_NO_ERROR: �ɹ�
*/
SD_Res sd_get_csd_register(SD_CSD* sd_csd);

/**
* @brief  ��ȡSD������Ϣ.
* @param  SD_CardInfo �ṹ��ָ��
* @retval SD��Ӧ:
*         - SD_RESPONSE_FAILURE: ʧ��
*         - SD_RESPONSE_NO_ERROR: �ɹ�
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
* @brief  ��SD����ȡһ�����ݿ�
* @param  pBuffer: ָ�룬���ڴ洢��ȡ��������
* @param  ReadAddr: Ҫ��ȡ��SD���ڲ���ַ
* @param  BlockSize: ���С
* @retval SD��Ӧ:
*         - SD_RESPONSE_FAILURE: ʧ��
*         - SD_RESPONSE_NO_ERROR: �ɹ�
*/
SD_Res sd_read_block(uint8_t *p_buffer, uint64_t read_addr, uint16_t block_size);

/**
* @brief  ��SD��д��һ�����ݿ�
* @param  pBuffer: ָ�룬ָ��Ҫд�������
* @param  WriteAddr: Ҫд���SD���ڲ���ַ
* @param  BlockSize: ���С
* @retval SD��Ӧ:
*         - SD_RESPONSE_FAILURE: ʧ��
*         - SD_RESPONSE_NO_ERROR: �ɹ�
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




