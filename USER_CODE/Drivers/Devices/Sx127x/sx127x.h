/*!
 * \file      sx127x.h
 *
 * \brief     SX127X driver implementation
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
 
#ifndef SX127X_H
#define SX127X_H

#ifdef __cplusplus
extern "C" {
#endif

#include "./cfg/sx127x_cfg.h"

/*!
 * ============================================================================
 * SX127X Internal registers Address (LoRa Mode, from table 85)
 * ============================================================================
 */
#define SX127X_REG_00_FIFO                                0x00
#define SX127X_REG_01_OP_MODE                             0x01
#define SX127X_REG_02_RESERVED                            0x02
#define SX127X_REG_03_RESERVED                            0x03
#define SX127X_REG_04_RESERVED                            0x04
#define SX127X_REG_05_RESERVED                            0x05
#define SX127X_REG_06_FRF_MSB                             0x06
#define SX127X_REG_07_FRF_MID                             0x07
#define SX127X_REG_08_FRF_LSB                             0x08
#define SX127X_REG_09_PA_CONFIG                           0x09
#define SX127X_REG_0A_PA_RAMP                             0x0a
#define SX127X_REG_0B_OCP                                 0x0b
#define SX127X_REG_0C_LNA                                 0x0c
#define SX127X_REG_0D_FIFO_ADDR_PTR                       0x0d
#define SX127X_REG_0E_FIFO_TX_BASE_ADDR                   0x0e
#define SX127X_REG_0F_FIFO_RX_BASE_ADDR                   0x0f
#define SX127X_REG_10_FIFO_RX_CURRENT_ADDR                0x10
#define SX127X_REG_11_IRQ_FLAGS_MASK                      0x11
#define SX127X_REG_12_IRQ_FLAGS                           0x12
#define SX127X_REG_13_RX_NB_BYTES                         0x13
#define SX127X_REG_14_RX_HEADER_CNT_VALUE_MSB             0x14
#define SX127X_REG_15_RX_HEADER_CNT_VALUE_LSB             0x15
#define SX127X_REG_16_RX_PACKET_CNT_VALUE_MSB             0x16
#define SX127X_REG_17_RX_PACKET_CNT_VALUE_LSB             0x17
#define SX127X_REG_18_MODEM_STAT                          0x18
#define SX127X_REG_19_PKT_SNR_VALUE                       0x19
#define SX127X_REG_1A_PKT_RSSI_VALUE                      0x1a
#define SX127X_REG_1B_RSSI_VALUE                          0x1b
#define SX127X_REG_1C_HOP_CHANNEL                         0x1c
#define SX127X_REG_1D_MODEM_CONFIG1                       0x1d
#define SX127X_REG_1E_MODEM_CONFIG2                       0x1e
#define SX127X_REG_1F_SYMB_TIMEOUT_LSB                    0x1f
#define SX127X_REG_20_PREAMBLE_MSB                        0x20
#define SX127X_REG_21_PREAMBLE_LSB                        0x21
#define SX127X_REG_22_PAYLOAD_LENGTH                      0x22
#define SX127X_REG_23_MAX_PAYLOAD_LENGTH                  0x23
#define SX127X_REG_24_HOP_PERIOD                          0x24
#define SX127X_REG_25_FIFO_RX_BYTE_ADDR                   0x25
#define SX127X_REG_26_MODEM_CONFIG3                       0x26

#define SX127X_REG_2C_RSSIWIDEBAND                        0x2C

#define SX127X_REG_39_SYNCWORD                            0x39

#define SX127X_REG_40_DIO_MAPPING1                        0x40
#define SX127X_REG_41_DIO_MAPPING2                        0x41
#define SX127X_REG_42_VERSION                             0x42

#define SX127X_REG_4B_TCXO                                0x4b
#define SX127X_REG_4D_PA_DAC                              0x4d
#define SX127X_REG_5B_FORMER_TEMP                         0x5b
#define SX127X_REG_61_AGC_REF                             0x61
#define SX127X_REG_62_AGC_THRESH1                         0x62
#define SX127X_REG_63_AGC_THRESH2                         0x63
#define SX127X_REG_64_AGC_THRESH3                         0x64

/*!
 * ============================================================================
 * SX127X LoRa bits control definition
 * ============================================================================
 */

// SX127X_REG_01_OP_MODE                             0x01
#define SX127X_OPMODE_LONGRANGEMODE_MASK             0x7F
#define SX127X_LONG_RANGE_MODE                       0x80

#define SX127X_OPMODE_ACCESSSHAREDREG_MASK           0xBF
#define SX127X_ACCESS_SHARED_REG                     0x40

#define SX127X_OPMODE_MASK                           0xF8
#define SX127X_MODE                                  0x07
#define SX127X_MODE_SLEEP                            0x00
#define SX127X_MODE_STDBY                            0x01
#define SX127X_MODE_FSTX                             0x02
#define SX127X_MODE_TX                               0x03
#define SX127X_MODE_FSRX                             0x04
#define SX127X_MODE_RXCONTINUOUS                     0x05
#define SX127X_MODE_RXSINGLE                         0x06
#define SX127X_MODE_CAD                              0x07

// SX127X_REG_09_PA_CONFIG                           0x09
#define SX127X_PA_SELECT                             0x80
#define SX127X_MAX_POWER                             0x70
#define SX127X_OUTPUT_POWER                          0x0f

// SX127X_REG_0A_PA_RAMP                             0x0a
#define SX127X_LOW_PN_TX_PLL_OFF                     0x10
#define SX127X_PA_RAMP                               0x0f
#define SX127X_PA_RAMP_3_4MS                         0x00
#define SX127X_PA_RAMP_2MS                           0x01
#define SX127X_PA_RAMP_1MS                           0x02
#define SX127X_PA_RAMP_500US                         0x03
#define SX127X_PA_RAMP_250US                         0x0
#define SX127X_PA_RAMP_125US                         0x05
#define SX127X_PA_RAMP_100US                         0x06
#define SX127X_PA_RAMP_62US                          0x07
#define SX127X_PA_RAMP_50US                          0x08
#define SX127X_PA_RAMP_40US                          0x09
#define SX127X_PA_RAMP_31US                          0x0a
#define SX127X_PA_RAMP_25US                          0x0b
#define SX127X_PA_RAMP_20US                          0x0c
#define SX127X_PA_RAMP_15US                          0x0d
#define SX127X_PA_RAMP_12US                          0x0e
#define SX127X_PA_RAMP_10US                          0x0f

// SX127X_REG_0B_OCP                                 0x0b
#define SX127X_OCP_ON                                0x20
#define SX127X_OCP_TRIM                              0x1f

// SX127X_REG_0C_LNA                                 0x0c
#define SX127X_LNA_GAIN                              0xe0
#define SX127X_LNA_BOOST                             0x03
#define SX127X_LNA_BOOST_DEFAULT                     0x00
#define SX127X_LNA_BOOST_150PC                       0x11

// SX127X_REG_11_IRQ_FLAGS_MASK                      0x11
#define SX127X_RX_TIMEOUT_MASK                       0x80
#define SX127X_RX_DONE_MASK                          0x40
#define SX127X_PAYLOAD_CRC_ERROR_MASK                0x20
#define SX127X_VALID_HEADER_MASK                     0x10
#define SX127X_TX_DONE_MASK                          0x08
#define SX127X_CAD_DONE_MASK                         0x04
#define SX127X_FHSS_CHANGE_CHANNEL_MASK              0x02
#define SX127X_CAD_DETECTED_MASK                     0x01

// SX127X_REG_12_IRQ_FLAGS                           0x12
#define SX127X_RX_TIMEOUT                            0x80
#define SX127X_RX_DONE                               0x40
#define SX127X_PAYLOAD_CRC_ERROR                     0x20
#define SX127X_VALID_HEADER                          0x10
#define SX127X_TX_DONE                               0x08
#define SX127X_CAD_DONE                              0x04
#define SX127X_FHSS_CHANGE_CHANNEL                   0x02
#define SX127X_CAD_DETECTED                          0x01

// SX127X_REG_18_MODEM_STAT                          0x18
#define SX127X_RX_CODING_RATE                        0xe0
#define SX127X_MODEM_STATUS_CLEAR                    0x10
#define SX127X_MODEM_STATUS_HEADER_INFO_VALID        0x08
#define SX127X_MODEM_STATUS_RX_ONGOING               0x04
#define SX127X_MODEM_STATUS_SIGNAL_SYNCHRONIZED      0x02
#define SX127X_MODEM_STATUS_SIGNAL_DETECTED          0x01

// SX127X_REG_1C_HOP_CHANNEL                         0x1c
#define SX127X_PLL_TIMEOUT                           0x80
#define SX127X_RX_PAYLOAD_CRC_IS_ON                  0x40
#define SX127X_FHSS_PRESENT_CHANNEL                  0x3f

// SX127X_REG_1D_MODEM_CONFIG1                       0x1d
#define SX127X_BW                                    0xc0
#define SX127X_BW_125KHZ                             0x00
#define SX127X_BW_250KHZ                             0x40
#define SX127X_BW_500KHZ                             0x80
#define SX127X_BW_RESERVED                           0xc0
#define SX127X_CODING_RATE                           0x38
#define SX127X_CODING_RATE_4_5                       0x00
#define SX127X_CODING_RATE_4_6                       0x08
#define SX127X_CODING_RATE_4_7                       0x10
#define SX127X_CODING_RATE_4_8                       0x18
#define SX127X_IMPLICIT_HEADER_MODE_ON               0x04
#define SX127X_RX_PAYLOAD_CRC_ON                     0x02
#define SX127X_LOW_DATA_RATE_OPTIMIZE                0x01

// SX127X_REG_1E_MODEM_CONFIG2                       0x1e
#define SX127X_SPREADING_FACTOR                      0xf0
#define SX127X_SPREADING_FACTOR_64CPS                0x60
#define SX127X_SPREADING_FACTOR_128CPS               0x70
#define SX127X_SPREADING_FACTOR_256CPS               0x80
#define SX127X_SPREADING_FACTOR_512CPS               0x90
#define SX127X_SPREADING_FACTOR_1024CPS              0xa0
#define SX127X_SPREADING_FACTOR_2048CPS              0xb0
#define SX127X_SPREADING_FACTOR_4096CPS              0xc0
#define SX127X_TX_CONTINUOUS_MOE                     0x08
#define SX127X_AGC_AUTO_ON                           0x04
#define SX127X_SYM_TIMEOUT_MSB                       0x03

// SX127X_REG_4D_PA_DAC                              0x4d
#define SX127X_PA_DAC_DISABLE                        0x04
#define SX127X_PA_DAC_ENABLE                         0x07


// RegPaConfig
 
#define RF_PACONFIG_PASELECT_MASK                   0x7F
#define RF_PACONFIG_PASELECT_PABOOST                0x80
#define RF_PACONFIG_PASELECT_RFO                    0x00 // Default

#define RF_PACONFIG_MAX_POWER_MASK                  0x8F

#define RF_PACONFIG_OUTPUTPOWER_MASK                0xF0

//RegPaDac

#define RF_PADAC_20DBM_MASK                         0xF8
#define RF_PADAC_20DBM_ON                           0x07
#define RF_PADAC_20DBM_OFF                          0x04  // Default


/*!
 * ============================================================================
 * Public functions prototypes
 * ============================================================================
 */
 
/*!
 * \brief Reset Sx127x Module
 *
 * \param [IN] Sx127x Instance
 *
 * \retval Non-Return
 */
void Sx127x_Reset(Sx127x_t *sx127x);

/*!
 * \brief Read register of Sx127x
 *
 * \param [IN] Sx127x Instance
 * \param [IN] Register address
 * \param [OUT] Register value
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx127x_ReadReg(Sx127x_t *sx127x, uint8_t addr, uint8_t *reg_value);

/*!
 * \brief Write register of Sx127x
 *
 * \param [IN] Sx127x Instance
 * \param [IN] Register address
 * \param [IN] Value to write
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx127x_WriteReg(Sx127x_t *sx127x, uint8_t addr, uint8_t reg_value);

/*!
 * \brief Set operation mode of Sx127x
 *
 * \param [IN] Sx127x Instance
 * \param [IN] Operation mode
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx127x_SetOpMode(Sx127x_t *sx127x, uint8_t opMode);

/*!
 * \brief Set Standby mode 
 *
 * \param [IN] Sx127x Instance
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx127x_SetModeStdby(Sx127x_t *sx127x);

/*!
 * \brief Set Sleep mode 
 *
 * \param [IN] Sx127x Instance
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx127x_SetModeSleep(Sx127x_t *sx127x);

/*!
 * \brief Set Tx mode 
 *
 * \param [IN] Sx127x Instance
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx127x_SetModeTx(Sx127x_t *sx127x);

/*!
 * \brief Set Rx mode 
 *
 * \param [IN] Sx127x Instance
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx127x_SetModeRx(Sx127x_t *sx127x);

/*!
 * \brief Set Channel activity detection mode 
 *
 * \param [IN] Sx127x Instance
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx127x_SetModeCAD(Sx127x_t *sx127x);

/*!
 * \brief Set modem configuration 
 *
 * \param [IN] Sx127x Instance
 * \param [IN] Modem configuration choice
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx127x_SetModemConfig(Sx127x_t *sx127x, ModemConfigChoice index);

/*!
 * \brief Set center frequency 
 *
 * \param [IN] Sx127x Instance
 * \param [IN] Center frequency
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx127x_SetFrequency(Sx127x_t *sx127x, float centre);

/*!
 * \brief Set Tx Power 
 *
 * \param [IN] Sx127x Instance
 * \param [IN] Tx Power
 * \param [IN] Using RFO or not
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx127x_SetTxPower(Sx127x_t *sx127x, int8_t power, Std_SwitchType useRFO);

///*!
// * \brief Set Tx Power 
// *
// * \param [IN] Sx127x Instance
// * \param [IN] Tx Power
// *
// * \retval STD_OK or STD_NOT_OK
// */
//Std_ReturnType Sx127x_SetTxPower(Sx127x_t *sx127x, int8_t power);

/*!
 * \brief Set Network ID 
 *
 * \param [IN] Sx127x Instance
 * \param [IN] Network ID
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx127x_SetNetworkId(Sx127x_t *sx127x, uint8_t network_id);

/*!
 * \brief Wait for empty channel with timeout 
 *
 * \param [IN] Sx127x Instance
 * \param [IN] Back-off time
 * \param [IN] Timeout
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx127x_Wait4FreeChannel(Sx127x_t *sx127x, uint16_t backoff_time, uint16_t timeout);

/*!
 * \brief Send Data 
 *
 * \param [IN] Sx127x Instance
 * \param [IN] Data to send
 * \param [IN] Length of data
 * \param [IN] Timeout
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx127x_SendData(Sx127x_t *sx127x, const uint8_t* data, uint16_t len, uint16_t timeout);

/*!
 * \brief Receive Data 
 *
 * \param [IN] Sx127x Instance
 * \param [OUT] Buffer to contain received data
 * \param [OUT] Length of data
 * \param [IN] Timeout
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx127x_ReceiveData(Sx127x_t *sx127x, uint8_t *rx_buffer, uint8_t *len_rx, uint16_t timeout);

/*!
 * \brief Init Sx127x Module 
 *
 * \param [IN] Sx127x Instance
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx127x_InitModule(Sx127x_t *sx127x);


/*!
 * \brief Sx127x random seed generator 
 *
 * \param [IN] Sx127x Instance
 * \param [OUT] Random seed
 *
 * \retval STD_OK or STD_NOT_OK
 */
Std_ReturnType Sx1276_RandomSeed(Sx127x_t *sx127x, uint32_t *rnd_seed);


#ifdef __cplusplus
}
#endif
#endif /* SX127X_H */
