/* Manchester enable = true */
/* Device address = 0 */
/* Modulation format = GFSK */
/* Sync word qualifier mode = 30/32 sync word bits detected */
/* Packet length = 61 */
/* Deviation = 2.883911 */
/* Modulated = true */
/* PA ramping = false */
/* Channel spacing = 26.367188 */
/* Carrier frequency = 901.999649 */
/* TX power = 26 */
/* Preamble count = 4 */
/* CRC autoflush = false */
/* CRC enable = true */
/* Channel number = 0 */
/* RX filter BW = 60.267857 */
/* Packet length mode = Variable packet length mode. Packet length configured by the first byte after sync word */
/* Whitening = false */
/* Address config = No address check */
/* Base frequency = 901.999649 */
/* Data format = Normal mode */
/* Data rate = 2.40111 */
/* PA table */
#define PA_TABLE {0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,}
const int REG_SIZE = 61;
byte Pkt_Buffer[REG_SIZE] = {0};
void RegConfig() {

  CC1101.WriteSingleReg( RADIO_IOCFG0,  0x06 );
  CC1101.WriteSingleReg( RADIO_FIFOTHR, 0x47 );
  CC1101.WriteSingleReg( RADIO_PKTLEN,  0x3D );
  CC1101.WriteSingleReg( RADIO_PKTCTRL0,0x05 );
  CC1101.WriteSingleReg( RADIO_FSCTRL1, 0x06 );
  CC1101.WriteSingleReg( RADIO_FREQ2,   0x21 );
  CC1101.WriteSingleReg( RADIO_FREQ1,   0x68 );
  CC1101.WriteSingleReg( RADIO_FREQ0,   0x4B );
  CC1101.WriteSingleReg( RADIO_MDMCFG4, 0xF6 );
  CC1101.WriteSingleReg( RADIO_MDMCFG3, 0x75 );
  CC1101.WriteSingleReg( RADIO_MDMCFG2, 0x1B );
  CC1101.WriteSingleReg( RADIO_MDMCFG1, 0x20 );
  CC1101.WriteSingleReg( RADIO_MDMCFG0, 0x00 );
  CC1101.WriteSingleReg( RADIO_DEVIATN, 0x06 );
  CC1101.WriteSingleReg( RADIO_MCSM0,   0x18 );
  CC1101.WriteSingleReg( RADIO_FOCCFG,  0x16 );
  //CC1101.WriteSingleReg( RADIO_BSCFG,   0x1C ); //70cm settings
  //CC1101.WriteSingleReg( RADIO_AGCCTRL2,0x47 ); //70cm settings
  //CC1101.WriteSingleReg( RADIO_AGCCTRL0,0xB0 ); //70cm settings
  //CC1101.WriteSingleReg( RADIO_WOREVT1, 0x00 ); //70cm settings
  //CC1101.WriteSingleReg( RADIO_WOREVT0, 0x00 ); //70cm settings
  //CC1101.WriteSingleReg( RADIO_FREND1,  0xB7 ); //70cm settings
  //CC1101.WriteSingleReg( RADIO_WORCTRL, 0xFB ); //Doesn't seem right
  CC1101.WriteSingleReg( RADIO_FSCAL3,  0xE9 );
  CC1101.WriteSingleReg( RADIO_FSCAL2,  0x2A );
  CC1101.WriteSingleReg( RADIO_FSCAL1,  0x00 );
  CC1101.WriteSingleReg( RADIO_FSCAL0,  0x1F );
  //CC1101.WriteSingleReg( RADIO_RCCTRL1, 0x00 ); //70cm settings
  CC1101.WriteSingleReg( RADIO_TEST2,   0x81 );
  CC1101.WriteSingleReg( RADIO_TEST1,   0x35 );
  CC1101.WriteSingleReg( RADIO_TEST0,   0x09 );

  byte paTable[8] = PA_TABLE;
  CC1101.WriteBurstReg( RADIO_PATABLE, paTable, 8 );

}
