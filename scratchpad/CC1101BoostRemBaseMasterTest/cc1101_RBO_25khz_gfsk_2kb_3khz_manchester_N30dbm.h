/* Data rate = 2.00224 */
/* Base frequency = 446.412018 */
/* RX filter BW = 58.035714 */
/* CRC autoflush = false */
/* Whitening = false */
/* Device address = 0 */
/* Data format = Normal mode */
/* Manchester enable = true */
/* Modulation format = GFSK */
/* PA ramping = false */
/* Deviation = 2.975464 */
/* Preamble count = 4 */
/* Channel spacing = 25.390625 */
/* CRC enable = true */
/* Modulated = true */
/* Packet length mode = Variable packet length mode. Packet length configured by the first byte after sync word */
/* Carrier frequency = 446.412018 */
/* Channel number = 0 */
/* TX power = -30 */
/* Packet length = 61 */
/* Sync word qualifier mode = 30/32 sync word bits detected */
/* Address config = No address check */
/* PA table */
#define PA_TABLE {0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,}
const int REG_SIZE = 61;
byte Pkt_Buffer[REG_SIZE] = {0};
void RegConfig() {

  CC1101.WriteSingleReg( RADIO_IOCFG0,  0x06 );
  CC1101.WriteSingleReg( RADIO_FIFOTHR, 0x47 );
  CC1101.WriteSingleReg( RADIO_PKTLEN,  0x3D );
  CC1101.WriteSingleReg( RADIO_PKTCTRL0,0x05 );
  CC1101.WriteSingleReg( RADIO_FSCTRL1, 0x0C );
  CC1101.WriteSingleReg( RADIO_FREQ2,   0x11 );
  CC1101.WriteSingleReg( RADIO_FREQ1,   0x2B );
  CC1101.WriteSingleReg( RADIO_FREQ0,   0x71 );
  CC1101.WriteSingleReg( RADIO_MDMCFG4, 0xF6 );
  CC1101.WriteSingleReg( RADIO_MDMCFG3, 0x43 );
  CC1101.WriteSingleReg( RADIO_MDMCFG2, 0x1B );
  CC1101.WriteSingleReg( RADIO_MDMCFG1, 0x20 );
  CC1101.WriteSingleReg( RADIO_MDMCFG0, 0x00 );
  CC1101.WriteSingleReg( RADIO_DEVIATN, 0x07 );
  CC1101.WriteSingleReg( RADIO_MCSM0,   0x18 );
  CC1101.WriteSingleReg( RADIO_FOCCFG,  0x1D );
  CC1101.WriteSingleReg( RADIO_BSCFG,   0x1C );
  CC1101.WriteSingleReg( RADIO_AGCCTRL2,0x47 );
  CC1101.WriteSingleReg( RADIO_AGCCTRL0,0xB0 );
  CC1101.WriteSingleReg( RADIO_WOREVT1, 0x00 );
  CC1101.WriteSingleReg( RADIO_WOREVT0, 0x00 );
  CC1101.WriteSingleReg( RADIO_FREND1,  0xB7 );
  CC1101.WriteSingleReg( RADIO_FSCAL3,  0xE9 );
  CC1101.WriteSingleReg( RADIO_FSCAL2,  0x2A );
  CC1101.WriteSingleReg( RADIO_FSCAL1,  0x00 );
  CC1101.WriteSingleReg( RADIO_FSCAL0,  0x1F );
  CC1101.WriteSingleReg( RADIO_RCCTRL1, 0x00 );
  CC1101.WriteSingleReg( RADIO_TEST2,   0x81 );
  CC1101.WriteSingleReg( RADIO_TEST1,   0x35 );
  CC1101.WriteSingleReg( RADIO_TEST0,   0x09 );

  byte paTable[8] = PA_TABLE;
  CC1101.WriteBurstReg( RADIO_PATABLE, paTable, 8 );

}
