                          ____________________

                           KVASER KLINE-J2534

                               Kvaser AB
                          ____________________


                               2021-01-07


Table of Contents
_________________

1. Protocol support
.. 1. Known limitations for CAN FD-based channels
.. 2. Known limitations of ISO 15765 channels
2. Accessing Specific Channels
3. Mixing ISO 15756 and CAN messages
4. Discovery -- device and protocol support
.. 1. Known limitations
5. The Windows Registry
6. Unsupported
7. IO-control
.. 1. `KV_REINIT_CANLIB'
.. 2. `KV_LOG'
8. Timestamps


This is an SAE J2534 DLL that can be used to interface with Kvaser
devices using the pass-thru API.

It is a replacement for several previous dll; `j2534api.dll' for the ISO
9141 and 14230 protocols, and `kvj2534.dll', `kvj2534c.dll', and
`kvj2534i.dll' that together provided support for CAN and ISO 15765
protocols. Their functionality has been merged and expanded upon with
e.g. support for STmin and BS parameters for ISO 15765.

The dll is compliant to j2534-1 DEC2004 and j2534-2 JAN2019.


1 Protocol support
==================

  The dll supports the following protocols defined in J2534-1:

  + `CAN'
  + `CAN FD' (see 1.1)
  + `ISO15765'
  + `ISO9141'
  + `ISO14230'

  The dll also has preliminary support for `ISO15765 on CAN FD' as
  described in J2534-2 (2019).


1.1 Known limitations for CAN FD-based channels
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  Currently only the bit rates of 500k, 1M, 2M, and 4M are supported for
  both arbitration phase and data phase. Further, the sync jump width is
  not guaranteed to satisfy the requirements of J2534-2.

  Verifying that the physical channel actually supports CAN FD is mostly
  left to the user. The physical bus will never be corrupted because of
  opening a non- CAN FD physical channel as a CAN FD channel, but the
  dll will not currently detect this problem on `PassThruConnect()'.

  The process for connecting to a CAN FD-based channel (once a device
  has been opened) is, as per J2534-2:

  1. Call `PassThruConnect()' with a CAN FD protocol id (The
     `FD_CAN_CHx' for the desired CANlib channel. See 2). The
     arbitration-phase bit rate is specified via the `baudrate'
     argument.
     + The Channel is not yet physically connected -- it has not gone
       bus-on.
  2. Set the data-phase bitrate with `PassThruIoctl()', `SET_CONFIG',
     and `FD_CAN_DATA_PHASE_RATE'. *THIS MUST BE DONE EVEN IF THE
     CHANNEL WILL NEVER USE BIT RATE SWITCHING.*
     + The Channel is now physically connect and has gone on-bus.

  For ISO15765 on CAN FD, the maximum allowed `DataSize' is currently
  4099/4100 -- same as for normal ISO15765 -- rather than the 4128 set
  by J2534-2 2019.


1.2 Known limitations of ISO 15765 channels
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  When writing raw CAN messages via an ISO 15765 channel, they use a
  different buffer from the ISO 15765 messages (but do still honor
  STmin). The CAN messages will be processed in parallel, and are likely
  to be sent in the middle of a segmented message transfer. So while the
  transmission of an ISO 15765 is not guaranteed to be sent after the
  completion of previously queued ISO 15765 messages, the individual CAN
  frames of the ISO 15765 message will still be separated by STmin from
  the previous CAN frame sent, whether that frame is part of the same
  conversation or not.

  Flow Control filters do not allow "masking the priority field in a
  29-bit CAN ID as specified in ISO 15765-2 Annex A", as required in
  J2534-1 2004 § 7.2.9.2 .

  ISO 15765 channels have a "Consecutive frame timeout", which in the
  case of `FD_ISO15765_CHx' channels can be set using the `N_CR_MAX'
  configuration parameter (as described in J2534-2 2019). Once this
  timeout has expired, it is possible to start a new reception and
  discarding the timed-out one. But a reception is only discarded if it
  is necessary; if a consecutive frame is received after the timeout has
  occurred but no other reception has started, the consecutive frame
  will be accepted.


2 Accessing Specific Channels
=============================

  For the CAN, ISO 15765, and CAN FD protocols the dll supports
  accessing specific channels, as per J2534-2 §6, Access to Additional
  Channels. These map directly to CANlib's channels.

  Note that J2534-2 defines names starting with one, while CANlib
  numbers channels starting from zero. So CAN_CH1 (0x00009000)
  corresponds to CANlib channel 0, CAN_CH2 to channel 1, etc.

  For the ISO 9141 and 14230 this is not supported; these channels will
  instead connect to the first (lowest-numbered) CANlib channel that
  supports K-line communication.


3 Mixing ISO 15756 and CAN messages
===================================

  The dll has support for J2534-2 §8, Mixed Format Frames on a CAN
  Network. However, ISO 15765 and CAN messages are currently not
  synchronized.


4 Discovery -- device and protocol support
==========================================

  The dll has support for j2534-2 §23, Discovery Mechanism.

  The following pieces of device information are supported:

  + `ISO9141_SUPPORTED'
  + `ISO14230_SUPPORTED'
  + `CAN_SUPPORTED'
  + `ISO15765_SUPPORTED'
  + `FD_CAN_SUPPORTED'
  + `FD_ISO15765_SUPPORTED'
  + `ISO9141_SIMULTANEOUS'
  + `ISO14230_SIMULTANEOUS'
  + `CAN_SIMULTANEOUS'
  + `ISO15765_SIMULTANEOUS'
  + `FD_CAN_SIMULTANEOUS'
  + `FD_ISO15765_SIMULTANEOUS'

  All protocol information for CAN and ISO 15765 (non- pin select
  versions) are supported except `DESIRED_DATA_RATE' and
  `MAX_ISO15765_WFT_MAX'.

  The dll does not have a fixed limit for every limit that can be
  queried with `GET_PROTOCOL_INFO'; several of the `MAX_xxx' parameters
  will return the largest value of `unsigned long' to indicate this.


4.1 Known limitations
~~~~~~~~~~~~~~~~~~~~~

  When reporting the number of channels available for a specific
  protocol, the dll will use the value from `canGetNumberOfChannels()'
  -- this is not necessarily the number of channels that support that
  specific protocol. This is partially because the dll keeps the
  ordering of channel numbers from CANlib and the first two devices that
  support CAN are not necessarily CAN_CH1 and CAN_CH2.


5 The Windows Registry
======================

  The dll uses five keys from the registry:

  + `SOFTWARE\PassThruSupport.04.04\Kvaser CAN\Logfile2534' (`REG_SZ')
  + `SOFTWARE\PassThruSupport.04.04\Kvaser CAN\EnableLog' (`REG_DWORD')
  + `SOFTWARE\PassThruSupport.04.04\Kvaser CAN\CAN channel'
    (`REG_DWORD')
  + `SOFTWARE\PassThruSupport.04.04\Kvaser CAN\canOPEN_ACCEPT_VIRTUAL'
    (`REG_DWORD')
  + `SOFTWARE\PassThruSupport.04.04\Kvaser CAN\share canlib channels'
    (`REG_DWORD')

  If `EnableLog' is not `0', then the dll will put its log at the path
  given by `Logfile2534'.

  `CAN channel' on the other hand specifies the default channel
  number. If a specific channel number is not specified as per 2 when
  calling `PassThruConnect()', you will be connected to the CANlib
  channel number given in the `CAN channel' registry key. This applies
  to both the `CAN' and `ISO15765' protocols.

  `canOPEN_ACCEPT_VIRTUAL' will allow you to use the J2534 dll to
  connect to CANlib's "virtual channels", as well as the physical
  ones. This is supported on CAN and CAN FD based channels, not on
  K-line based ones.

  If `SOFTWARE\PassThruSupport.04.04\Kvaser CAN\share canlib channels'
  is missing or set to zero, the dll use the CANlib flag
  `canOPEN_REQUIRE_INIT_ACCESS' when opening internal CANlib handles to
  make sure that the dll is able to set the bitrate supplied to
  `PassThruConnect()'. This prevents calling `PassThruConnect()' for the
  same channel number multiple times, and means that `PassThruConnect()'
  will fail if another program is using the specified channel number
  (with init-access). But the `canOPEN_REQUIRE_INIT_ACCESS' flag is
  needed to ensure that channels are using the bitrate supplied by the
  user, and that the CANlib channels keep using that bitrate. If instead
  the `share canlib channels' key is set to e.g. one, no such checks are
  made and it is up to the user to make sure that the channel keeps
  functioning as expected, but doing so does allow multiple PassThru
  channels and/or other programs to "share" a single CANlib channel.

  Note that all these registry keys are read when the dll is initialized
  during the first call to `PassThruOpen()'.


6 Unsupported
=============

  + Functions
    PassThruSetProgrammingVoltage()
          Kvaser interfaces are not J1962 connectors and therefore this
          function does not apply.


7 IO-control
============

  The following IOCTL values are supported by each protocol:

  + CAN
    + `GET_CONFIG'
    + `SET_CONFIG'
    + `CLEAR_TX_BUFFER'
    + `CLEAR_RX_BUFFER'
    + `CLEAR_MSG_FILTERS'
    + `CLEAR_PERIODIC_MSGS'
  + CAN FD
    + `GET_CONFIG'
    + `SET_CONFIG'
    + `CLEAR_TX_BUFFER'
    + `CLEAR_RX_BUFFER'
    + `CLEAR_MSG_FILTERS'
    + `CLEAR_PERIODIC_MSGS'
  + ISO 15765
    + `GET_CONFIG'
    + `SET_CONFIG'
    + `CLEAR_TX_BUFFER'
    + `CLEAR_RX_BUFFER'
    + `CLEAR_MSG_FILTERS'
    + `CLEAR_PERIODIC_MSGS'
  + ISO 9141/14230
    + `GET_CONFIG'
    + `SET_CONFIG'
    + `CLEAR_TX_BUFFER' [1]
    + `CLEAR_RX_BUFFER' [1]
    + `FIVE_BAUD_INIT'
    + `FAST_INIT'
    + `CLEAR_PERIODIC_MSGS'
    + `CLEAR_MSG_FILTERS' [1]

  The following config values (ioctl `GET_CONFIG' / `SET_CONFIG') are
  supported:

  + CAN
    + `LOOPBACK'
    + `DATA_RATE'
  + CAN FD
    + `LOOPBACK'
    + `DATA_RATE'
    + `FD_CAN_DATA_PHASE_RATE'
  + ISO 15765
    + `LOOPBACK'
    + `DATA_RATE'
    + `CAN_MIXED_FORMAT'
    + `ISO15765_BS'
    + `ISO15765_STMIN'
    + `BS_TX'
    + `STMIN_TX'
  + ISO 15765 on CAN FD
    + `DATA_RATE'
    + `FD_CAN_DATA_PHASE_RATE'
    + `FD_ISO15765_TX_DATA_LENGTH'
    + `N_CR_MAX'
    + `ISO15765_PAD_VALUE'
    + `LOOPBACK'
    + `CAN_MIXED_FORMAT'
    + `STMIN_TX'
    + `BS_TX'
    + `ISO15765_STMIN'
    + `ISO15765_BS'

  + ISO 9141/14230
    + All config parameters are passed on to the vehicle.

  The following Kvaser-specific values are also supported:

   KV_REINIT_CANLIB  0x10000 
   KV_LOG            0x10001 


7.1 `KV_REINIT_CANLIB'
~~~~~~~~~~~~~~~~~~~~~~

  Reinitialize CANlib (the underlying library used by the J2534 dll).

  This IoctlID must not be used while any channel is currently connected
  (`PassThruConnect()' has been called without then calling
  `PassThruDisconnect()' on the channel ID). Doing so causes undefined
  behavior -- /ALL OPEN CHANNELS WILL FAIL CATASTROPHICALLY/.

  Using this Ioctl rescans for connected devices; if you plugged in a
  new device after calling `PassThruOpen()' then that device will not be
  recognized by the j2534 dll until you have called `PassThruIoctl()'
  with the `KV_REINIT_CANLIB' IoctlID.

  Parameters:

  `ChannelID'
        The device ID returned by `PassThruOpen()'.
  `IoctlID'
        `KV_REINIT_CANLIB'.
  `pInput'
        A null-pointer, unused by this ioctl ID.
  `pOutput'
        A null-pointer, unused by this ioctl ID.


7.2 `KV_LOG'
~~~~~~~~~~~~

  Write a custom message to the log file.

  Parameters:

  `ChannelID'
        The device ID returned by `PassThruOpen()'.
  `IoctlID'
        `KV_LOG'.
  `pInput'
        A null-terminated string with the message that will be logged
        (should not contain new-lines).
  `pOutput'
        A null-pointer, unused by this ioctl ID.


8 Timestamps
============

  The timestamps returned by the dll (in `PASSTHRU_MSG' structs) are
  currently expressed in microseconds since the first timestamp
  needed. This means that the first message returned by the dll will
  have a timestamp of `0'.

  Also, the dll treats CANlib as if it were the hardware layer;
  timestamps are from when the dll reads or writes messages to/from
  CANlib.



Footnotes
_________

[1] Currently has no effect
