
dgbus.h,3186
#define __DGBUS_H__85,2865
#define DGBUSSTART	87,2886
#define DGBUSSHIFT	88,2921
#define DGBUSBYTES	89,2944
#define DIAG_CTRLR_LOAD	90,2969
#define ASSUMED_CXBUS_NUMBER	91,2996
#define SP_HARDWARE	97,3145
#define SSP_HARDWARE	98,3171
#define WRITE_WCS_SPC_COMMAND_REGISTER	103,3240
#define WRITE_WCS_SPC_DATA_REGISTER	104,3283
#define SET_CONTROL_REGISTER_WCS_LOADED	106,3324
#define RESET_ROM_ADDRESS_REGISTER	107,3368
#define INCREMENT_ROM_ADDRESS_REGISTER	109,3408
#define READ_ROM	110,3451
#define READ_WCS_SPC_COMMAND_REGISTER	112,3475
#define READ_WCS_SPC_DATA_REGISTER	113,3517
#define SPC_READ_REGISTER	117,3620
#define SPC_WRITE_REGISTER	118,3652
#define SPC_READ_REGISTER_AND_INCREMENT_INITIALIZE_COUNTER	119,3685
#define SPC_WRITE_REGISTER_AND_INCREMENT_INITIALIZE_COUNTER	120,3746
#define SPC_REG_INITIALIZE_COUNTER	124,3873
#define SPC_REG_RAM_OUTPUT	125,3913
#define SPC_REG_SETUP_AND_STATUS	126,3946
#define WCS_CMD_DOWNLOAD_IMAGE 128,3985
#define WCS_DIRECTION_RP_TO_IP 129,4021
#define WCS_DIRECTION_IP_TO_RP 130,4057
#define WCS_IMAGE_COMPRESSION_NONE 131,4094
#define WCS_IMAGE_COMPRESSION_UNIX 132,4133
#define AIP_WCS_CMD_REG 134,4193
#define AIP_WCS_INIT_CNT 136,4224
#define AIP_WCS_WR_RAM 137,4280
#define AIP_WCS_WR_DAT_1 139,4346
#define AIP_WCS_WR_DAT_2 140,4376
#define AIP_WCS_WR_DAT_4 141,4406
#define AIP_WCS_LOADED 143,4437
#define WCS_CMD_REG	145,4468
#define FSIP_WCS_CMD_REG 146,4495
#define FSIP_WCS_INIT_CNT 147,4526
#define FSIP_WCS_WR_DAT_1 148,4557
#define FSIP_WCS_WR_DAT_2 149,4588
#define FSIP_WCS_WR_DAT_4 150,4619
#define FSIP_WCS_WR_RAM 151,4650
#define FSIP_WCS_WR_PAR 152,4681
#define FSIP_WCS_LOADED 153,4712
#define FSIP_NUM_MODULES 155,4744
#define FSIP_WCS_SIZE 156,4772
#define DBUS_REG_TIMEOUT 162,4916
#define DBUS_STATUS_TIMEOUT 163,4961
#define DBUS_READ	168,5074
#define CXBUS_ERROR	173,5156
#define DBUS_INTERNAL_ERROR	174,5183
#define WCS_PARITY_ERROR	175,5217
#define WCS_CNTRL_INSTR_ERROR	176,5248
#define DBUS_SW_READY 177,5284
#define UCODE_STATUS_MASK	178,5329
#define DBUS_INVALID_REQUEST 179,5361
#define FLOW_START 184,5427
#define DBUS_SW_MAXDELAY 193,5676
#define DBUS_SW_POLL_TIME	198,5806
#define DBUS_LOGGER_DELAY 203,5912
#define DBUS_ERROR_MASK 205,5955
#define WCS_ERROR_MASK 206,6052
#define XILINX_READY 209,6186
#define MASTER_ENABLE	213,6272
#define ENABLE_LED	214,6301
#define WCS_DBUS_CMD_ENABLE	215,6327
#define WCS_LOADED	216,6361
#define WCS_LOADING	217,6387
#define DBUS_ROM_LOAD_TIME	225,6590
#define DBUS_XILINX_TIME 228,6702
#define IDPROM_BASE 232,6817
#define IDPROM_MAGIC 233,6853
#define IDPROM_VERSION 234,6881
typedef struct dgbus_eeprom_ dgbus_eeprom_236,6909
} dgbus_eeprom_t;dgbus_eeprom_t258,7711
typedef struct rp1_eeprom_ rp1_eeprom_260,7730
} rp1_eeprom_t;rp1_eeprom_t266,7873
} protected_uchar;protected_uchar271,7943
struct dgtype_ dgtype_279,8100
#define DBUS_SOFT_REG_RESET 307,9068
#define DBUS_HARD_REG_RESET	308,9104
#define DBUS_RESET_VALUE	309,9137
#define FSIP_NO_EOIR_VERSION 316,9290
#define AIP_NO_EOIR_VERSION 317,9326
#define MIP_NO_EOIR_VERSION 318,9362
dbus_prot_rd 324,9478
