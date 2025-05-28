//------------------------------------------------------------------------------
//
//	File:		ComSlip.cpp
//
//	Abstract:	SLIP Wrapper Class Declaration
//
//	Version:	0.1
//
//	Date:		09.02.2015
//
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------

#ifndef COMSLIP_H
#define COMSLIP_H

//------------------------------------------------------------------------------
//
// Include Files
//
//------------------------------------------------------------------------------

#include "WMDefs.h"

class TWiMODLRHCI;

//------------------------------------------------------------------------------
//
// General Definitions
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// Class Declaration
//
//------------------------------------------------------------------------------

class TComSlipClient
{
    public:
                    TComSlipClient() {}
    virtual         ~TComSlipClient() {}
};

class TComSlip
{
    public:
                    TComSlip();

    void            RegisterClient(TWiMODLRHCI* client) { RxClient = client; }

    int             EncodeData(UINT8* dstBuffer, UINT16 dstBufferSize, UINT8* srcBuffer, UINT16 length);

    bool            SetRxBuffer(UINT8*  rxBuffer, UINT16 rxbufferSize);

    void            DecodeData(UINT8* rxData, UINT16 length);

    private:

    void            StoreTxByte(UINT8 txByte);
    void            StoreRxByte(UINT8 rxByte);

    private:

    // receiver/decoder state
    int             RxState;

    // rx buffer index
    UINT16          RxIndex;

    // size of RxBuffer
    UINT16          RxBufferSize;

    // pointer to RxBuffer
    UINT8*          RxBuffer;

    // client for received messages
    TWiMODLRHCI* RxClient;

    // pointer to Txbuffer
    UINT8*          TxBuffer;

    // size of TxBuffer
    UINT16          TxBufferSize;

    // tx buffer index
    UINT16          TxIndex;
};

#endif // COMSLIP_H

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
