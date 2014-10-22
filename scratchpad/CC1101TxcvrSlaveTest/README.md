<h1>CC1101TxcvrSlaveTest</h1>
<h2>Basic example CC1101 Transceiver slave unit.</h2>
<h3>Lou Nigra<br>
Far Horizons Lab</h3>

Adapted from basic TestRx and TestTx programs.

Transmits a short packet upon receipt of a packet from master.

Monitors index (first byte) of master packet to see if a packet was missed.

First byte of packet is an index count. 
