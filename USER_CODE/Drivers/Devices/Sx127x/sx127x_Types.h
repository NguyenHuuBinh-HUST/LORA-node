/*!
 * \file      sx127x_Types.h
 *
 * \brief     SX127X driver types
 *
 * \copyright @SANSLAB
 *
 * \code
 *              _____         _   _  _____ _               ____  
 *             / ____|  /\   | \ | |/ ____| |        /\   |  _ \ 
 *            | (___   /  \  |  \| | (___ | |       /  \  | |_) |
 *             \___ \ / /\ \ | . ` |\___ \| |      / /\ \ |  _ < 
 *             ____) / ____ \| |\  |____) | |____ / ____ \| |_) |
 *            |_____/_/    \_\_| \_|_____/|______/_/    \_\____/ 
 *            (C)Smart Applications and Network System Laboratory
 *
 * \endcode
 *
 * \author    Trung Ha Nguyen ( Sanslab )
 */
 
#ifndef SX127X_TYPES_H
#define SX127X_TYPES_H
#ifdef __cplusplus
extern "C" {
#endif

#include "spi.h"
#include "Std_Types.h"
	
#define Sx127x_SPITypeDef    SPI_HandleTypeDef
	
#define Sx127x_GPIOTypeDef   GPIO_TypeDef

#define CMD_BUFFER_LEN              2

	
/*!
 * Constant values need to compute the RSSI value
 */
#define RSSI_OFFSET_LF              -164
	
#define RSSI_OFFSET_HF              -157
	
#define RF_MID_BAND_THRESH          525000000
	
	
/*!
 * Constant values for frequency calculation
 */
#define SX127X_FXOSC                32000000.0

#define SX127X_FSTEP                (SX127X_FXOSC / 524288)

typedef enum
{
	CAD_BUSY = 0x00, //!< Channel is busy
	CAD_FREE,        //!< Channel is free
} Cad_State;

typedef HAL_StatusTypeDef (*SPI_Write_Read)(Sx127x_SPITypeDef *hspi, \
                                         uint8_t *pTxData, uint8_t *pRxData, \
										                     uint16_t Size, uint32_t Timeout);

typedef struct 
{
	SPI_Write_Read write_read;
}Sx127x_CfgType;

/*!
 * Sx127x modes
 */
typedef enum
{
	RF_STDBY = 0,  //!< The radio is standby 
	RF_SLEEP,      //!< The radio is sleep
	RF_RX_RUNNING, //!< The radio is in reception state
	RF_TX_RUNNING, //!< The radio is in transmission state
	RF_CAD,        //!< The radio is doing channel activity detection
}Sx127x_State;

/*!
 * Sx127x modems
 */
typedef enum 
{
	FSK = 0x00,
	LoRa,
}Sx127x_Modem;

/*!
 * Sx127x modem configuration choices
 */
typedef enum
{
	Bw125Cr45Sf128 = 0,	 ///< Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Default medium range
	Bw500Cr45Sf128,	     ///< Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range
	Bw31_25Cr48Sf512,	   ///< Bw = 31.25 kHz, Cr = 4/8, Sf = 512chips/symbol, CRC on. Slow+long range
	Bw125Cr48Sf4096,     ///< Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on. Slow+long range
  Bw125Cr48Sf1024,     ///< Bw = 125 kHz, Cr = 4/8, Sf = 1024chips/symbol, CRC on.
} ModemConfigChoice;

/*!
 * Sx127x modem configuration registers
 */
typedef struct
{
	uint8_t    reg_1d;   ///< Value for register RH_RF95_REG_1D_MODEM_CONFIG1
	uint8_t    reg_1e;   ///< Value for register RH_RF95_REG_1E_MODEM_CONFIG2
	uint8_t    reg_26;   ///< Value for register RH_RF95_REG_26_MODEM_CONFIG3
} ModemConfig;

/*!
 * Sx127x Parameters and States
 */
typedef struct
{
	Sx127x_Modem        Modem;         ///< Modem of Sx127x (LoRa or FSK)
	Sx127x_State        State;         ///< State of Sx127x
	ModemConfigChoice   CfgChoice;     ///< Modem configuration choice
	Cad_State           CadStt;        ///< CAD status
	float               CenterFreq;    ///< Center Frequency
	uint8_t             PreambleLen;   ///< Preamble length
	uint8_t             TxPower;       ///< Tx Power
	uint8_t             NetworkId;     ///< Network ID
}Radio_Settings;

/*!
 * Sx127x GPIO
 */
typedef struct
{
	Sx127x_GPIOTypeDef  *gpio_port;
	uint16_t            gpio_pin;
}Gpio_t;

/*!
 * Sx127x settings
 */
typedef struct
{
	Sx127x_SPITypeDef   *hspi;         ///< SPI HAL Instance
	Gpio_t              reset_pin;     ///< Resetpin
	Gpio_t              nss_pin;       ///< NSS pin
	Radio_Settings      settings;      ///< Sx127x Settings
}Sx127x_t;

#ifdef __cplusplus
}
#endif
#endif
