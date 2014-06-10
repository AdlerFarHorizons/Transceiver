<h1>MasterCC1101/SlaveCC1101</h1>
<h2>Basic example CC1101 Transceiver master/slave units.</h2>
<h3>Lou Nigra<br>
Far Horizons Lab</h3>

Adapted from basic TestRx and TestTx programs.
 
Master transmits a short packet on timer interrupt.

Master waits for a response packet from the slave unit

Master tests for receipt of response.

Slave transmits a short packet upon receipt of a packet from master.

Slave monitors index (first byte) of master packet to see if a packet was missed.

First byte of packet is an index count. 
