/* Deviation = 4.943848 */
/* Device address = 0 */
/* Base frequency = 901.999649 */
/* CRC autoflush = false */
/* Data format = Synchronous serial mode */
/* Modulated = true */
/* RX filter BW = 60.267857 */
/* PA ramping = false */
/* Manchester enable = true */
/* Preamble count = 4 */
/* Modulation format = GFSK */
/* Channel spacing = 26.367188 */
/* CRC enable = false */
/* Channel number = 128 */
/* Carrier frequency = 905.374649 */
/* Address config = No address check */
/* Whitening = false */
/* TX power = 15 */
/* Data rate = 2.40111 */
/* Sync word qualifier mode = No preamble/sync */
/* PA table */
#define PA_TABLE {0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,}
const int REG_SIZE = 61;
byte Pkt_Buffer[REG_SIZE] = {0};
void RegConfig() {

  CC1101.WriteSingleReg( RADIO_IOCFG0,    0x0C );
  CC1101.WriteSingleReg( RADIO_FIFOTHR,   0x47 );
  CC1101.WriteSingleReg( RADIO_PKTLEN,    0x3D );
  CC1101.WriteSingleReg( RADIO_PKTCTRL0,  0x12 );
  CC1101.WriteSingleReg( RADIO_CHANNR,    0x80 );
  CC1101.WriteSingleReg( RADIO_FSCTRL1,   0x06 );
  CC1101.WriteSingleReg( RADIO_FREQ2,     0x21 );
  CC1101.WriteSingleReg( RADIO_FREQ1,     0x68 );
  CC1101.WriteSingleReg( RADIO_FREQ0,     0x4B );
  CC1101.WriteSingleReg( RADIO_MDMCFG4,   0xF6 );
  CC1101.WriteSingleReg( RADIO_MDMCFG3,   0x75 );
  CC1101.WriteSingleReg( RADIO_MDMCFG2,   0x18 );
  CC1101.WriteSingleReg( RADIO_MDMCFG1,   0x20 );
  CC1101.WriteSingleReg( RADIO_MDMCFG0,   0x00 );
  CC1101.WriteSingleReg( RADIO_DEVIATN,   0x14 );
  CC1101.WriteSingleReg( RADIO_MCSM0,     0x18 );
  CC1101.WriteSingleReg( RADIO_FOCCFG,    0x16 );
  CC1101.WriteSingleReg( RADIO_WORCTRL,   0xFB );
  CC1101.WriteSingleReg( RADIO_FSCAL3,    0xE9 );
  CC1101.WriteSingleReg( RADIO_FSCAL2,    0x2A );
  CC1101.WriteSingleReg( RADIO_FSCAL1,    0x00 );
  CC1101.WriteSingleReg( RADIO_FSCAL0,    0x1F );
  CC1101.WriteSingleReg( RADIO_TEST2,     0x81 );
  CC1101.WriteSingleReg( RADIO_TEST1,     0x35 );
  CC1101.WriteSingleReg( RADIO_TEST0,     0x09 );
  CC1101.WriteSingleReg( RADIO_LQI,       0x7F );
  CC1101.WriteSingleReg( RADIO_RSSI,      0x80 );
  CC1101.WriteSingleReg( RADIO_MARCSTATE, 0x01 );
  CC1101.WriteSingleReg( RADIO_VCO_VC_DAC,0x94 );

  byte paTable[8] = PA_TABLE;
  CC1101.WriteBurstReg( RADIO_PATABLE, paTable, 8 );

}
