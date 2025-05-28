//------------------------------------------------------------------------------
//
//	File:		WiMODLRHCI.h
//
//	Abstract:	WiMODLR HCI Wrapper Class Declaration
//
//	Version:	0.1
//
//	Date:		02.01.2014
//
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------

#ifndef WIMODLRHCI_H
#define WIMODLRHCI_H

//------------------------------------------------------------------------------
//
// Include Files
//
//------------------------------------------------------------------------------

#include "WMDefs.h"
#include "WiMODLRHCI_IDs.h"
#include "ComSlip.h"
#include "SerialDevice.h"
#include <string>

//------------------------------------------------------------------------------
//
// General Declaration
//
//------------------------------------------------------------------------------

//#define HIBYTE(w)       (UINT8)((w) >> 8)
//#define LOBYTE(w)       (UINT8)(w)

typedef uint16_t        TWiMODLRResult;

//------------------------------------------------------------------------------
//
// Radio Configuration Defines
//
//------------------------------------------------------------------------------

#define WiMODLR_STORE_INTO_RAM              0
#define WiMODLR_STORE_INTO_NVM              1

#define WiMODLR_RADIO_CONFIG_RM_STANDARD    0
#define WiMODLR_RADIO_CONFIG_RM_ECHO        1
#define WiMODLR_RADIO_CONFIG_RM_SNIFFER     2

#define WiMODLR_RADIO_CONFIG_MOD_LORA       0
#define WiMODLR_RADIO_CONFIG_MOD_FSK        1

#define WiMODLR_RADIO_CONFIG_BW_125kHz      0
#define WiMODLR_RADIO_CONFIG_BW_250kHz      1
#define WiMODLR_RADIO_CONFIG_BW_500kHz      2


#define WiMODLR_RADIO_CONFIG_SF7            7
#define WiMODLR_RADIO_CONFIG_SF8            8
#define WiMODLR_RADIO_CONFIG_SF9            9
#define WiMODLR_RADIO_CONFIG_SF10           10
#define WiMODLR_RADIO_CONFIG_SF11           11
#define WiMODLR_RADIO_CONFIG_SF12           12

#define WiMODLR_RADIO_CONFIG_EC_4_5         1
#define WiMODLR_RADIO_CONFIG_EC_4_6         2
#define WiMODLR_RADIO_CONFIG_EC_4_7         3
#define WiMODLR_RADIO_CONFIG_EC_4_8         4

//------------------------------------------------------------------------------
//
// HCI Message Declaration
//
//------------------------------------------------------------------------------

// message header size: 2 bytes for SapID + MsgID
#define WIMODLR_HCI_MSG_HEADER_SIZE     2

// message payload size
#define WIMODLR_HCI_MSG_PAYLOAD_SIZE    280

// frame check sequence field size: 2 bytes for CRC16
#define WIMODLR_HCI_MSG_FCS_SIZE        2

// visible max. buffer size for lower SLIP layer
#define WIMODLR_HCI_RX_MESSAGE_SIZE     (WIMODLR_HCI_MSG_HEADER_SIZE\
                                         + WIMODLR_HCI_MSG_PAYLOAD_SIZE\
                                         + WIMODLR_HCI_MSG_FCS_SIZE)

//------------------------------------------------------------------------------
//
// HCI Message
//
//------------------------------------------------------------------------------

typedef struct
{
    // Payload Length Information, not transmitted over UART interface !
    UINT16  Length;

    // Service Access Point Identifier
    UINT8   SapID;

    // Message Identifier
    UINT8   MsgID;

    // Payload Field
    UINT8   Payload[WIMODLR_HCI_MSG_PAYLOAD_SIZE];

    // Frame Check Sequence Field
    UINT8   CRC16[WIMODLR_HCI_MSG_FCS_SIZE];

}TWiMODLR_HCIMessage;

//------------------------------------------------------------------------------
//
// Definition of Result/Error Codes
//
//------------------------------------------------------------------------------

typedef enum
{
    WiMODLR_RESULT_OK = 0,
    WiMODLR_RESULT_PAYLOAD_LENGTH_ERROR,
    WiMODLR_RESULT_PAYLOAD_PTR_ERROR,
    WiMODLR_RESULT_TRANMIT_ERROR,
    WiMODLR_RESULT_SLIP_ENCODER_ERROR,
    WiMODLR_RESULT_NO_RESPONSE
}TWiMDLRResultCodes;

//------------------------------------------------------------------------------
//
// Radio Configuration
//
// see WiMODLR HCI Specification Chapter 3.1.5.3
//
//------------------------------------------------------------------------------

typedef struct
{
    UINT8   RadioMode;
    UINT8   GroupAddress;
    UINT8   TxGroupAddress;
    UINT16  DeviceAddress;
    UINT16  TxDeviceAddress;
    UINT8   Modulation;
    UINT32  Frequency;
    UINT8   Bandwidth;
    UINT8   SpreadingFactor;
    UINT8   ErrorCoding;
    UINT8   PowerLevel;
    UINT8   TxControl;
    UINT8   RxControl;
    UINT16  RxWindowTime;
    UINT8   LEDControl;
    UINT8   RadioOptions;
}TWiMODLR_RadioConfig;

typedef struct
{
    UINT8   GroupAddress;
    UINT16  DeviceAddress;
    UINT8   PacketSize;
    UINT16  NumPackets;
    UINT8   TestMode;
}TWiMODLR_RadioLinkTestConfig;

typedef struct
{
    UINT8   TestStatus = 0;
    UINT32  LTxCount = 0;
    UINT32  LRxCount = 0;
    UINT32  PTxCount = 0;
    UINT32  PRxCount = 0;
    INT16  LocalRSSI = 0;
    INT16  PeerRSSI = 0;
    INT8   LocalSNR = 0;
    INT8   PeerSNR = 0;
}TWiMODLR_RadioLinkTestStatus;

//------------------------------------------------------------------------------
//
// ID String Table Item
//
//------------------------------------------------------------------------------

typedef struct
{
    // ID Code
    UINT8       ID;

    // ID String
    const char* IDString;

}TWiMODLRHCI_IDString;

//------------------------------------------------------------------------------
//
// TWiMODLRHCIClient Class Declaration
//
//------------------------------------------------------------------------------

class TWiMODLRHCIClient
{
    public:
                        TWiMODLRHCIClient() {}
    virtual             ~TWiMODLRHCIClient() {}

    // define debug callback function
    virtual void        evRadio_ShowMessage(const std::string& /* prefix */, const std::string& /* msg */) {}

    // define handler for received unreliable messages
    virtual void        evRadioLink_RxUMessage(const TWiMODLR_HCIMessage& /* rxMsg */) {}
};

//------------------------------------------------------------------------------
//
// TWiMODLRHCI Class Declaration
//
//------------------------------------------------------------------------------

class TWiMODLRHCI : public TWiMODLRHCIClient
{
    public:
                        TWiMODLRHCI();
                        ~TWiMODLRHCI();

    void                RegisterClient(TWiMODLRHCIClient* client) { Client = client; }

    // connection handling
    bool                Open(std::string& comPort);
    bool                Close();
    void                Process();

    // device management commands
    TWiMODLRResult      PingRequest();
    TWiMODLRResult      FactoryReset();
    TWiMODLRResult      GetRadioConfiguration(TWiMODLR_RadioConfig& config, UINT8& status);
    TWiMODLRResult      SetRadioConfiguration(TWiMODLR_RadioConfig& config, UINT8 destMemory, UINT8& status);

    //void                ConvertRadioConfiguration(TKeyValueList& list, const TWiMODLR_RadioConfig& config);
    const char*         GetDeviceMgmtStatusString(UINT8 status);

    // radio link services
    TWiMODLRResult      SendURadioMessage(UINT8* txMessage, UINT16 length, UINT8& status);
    //void                ConvertRadioRxMessage(TKeyValueList& list, const TWiMODLR_HCIMessage& rxMsg);


    const char*         GetRadioLinkStatusString(UINT8 status);

    // other helper functions
    void                U32TimeToString(std::string& timeString, UINT32 time, bool isoFormat = true);
    UINT32              GetFrequencyFromConfig(UINT32 regConfig);
    const char*         GetStringFromTable(const TWiMODLRHCI_IDString* table, UINT8 id);
    std::string         GetCombinedStringFromTable(const TWiMODLRHCI_IDString* table, UINT8 id, int numBits);


    TWiMODLRResult      SendHCIMessage(UINT8 sapId, UINT8 msgID, UINT8 rxMsgID, UINT8* payload = 0, UINT16 length = 0);

    // receiver functions
    bool                WaitForResponse(UINT8 rxSapID, UINT8 rxMsgID);
    UINT8*              ProcessRxMessage(UINT8* rxBuffer, UINT16 length);
    
    // receiver struct
    typedef struct
    {
        // flag indicating that a respons eis expected
        bool        Active;
        // flag indicating response successfully received
        bool        Done;
        // SAP ID of expected response
        UINT8       SapID;
        // Msg ID  of expected response
        UINT8       MsgID;
        // reserve one Rx-Message-Buffer
        TWiMODLR_HCIMessage Message;
        // CRC error counter
        int         CRCError;
        // Timeout (~1000ms)
        int         Timeout;
        // reserve one rx-buffer for recepton of SLIP encoded octet sequence
        UINT8       Buffer[512];
    }TReceiver;
    
    // receiver instance
    TReceiver           Rx;

    // measurement filename
    std::string filename;

    std::string         getCurrentDateTimeISO       ();

    private:

    // transmit functions
    TWiMODLRResult      PostMessage(UINT8 sapId, UINT8 msgID, UINT8* payload = 0, UINT16 length = 0);
    TWiMODLRResult      SendPacket(UINT8* txData, UINT16 length);

    // dispatcher functions
    void                DispatchRxMessage           (TWiMODLR_HCIMessage& rxMsg);
    void                DispatchDeviceMgmtMessage   (TWiMODLR_HCIMessage& rxMsg);
    void                DispatchRadioLinkMessage    (TWiMODLR_HCIMessage& rxMsg);
    void                DispatchRadioLinkTestMessage(TWiMODLR_HCIMessage& rxMsg);

    // data storage functions
    void                printMesuredData            (TWiMODLR_RadioLinkTestStatus& data);
    void                writeDataToFile             (std::string& filename,TWiMODLR_RadioLinkTestStatus& data);


    // debug support
    void                ShowMessage(const std::string& msg, const TWiMODLR_HCIMessage& rxMsg);
    void                ShowMessage(const char* msg);


    private:
    // reserve one Tx-Message-Buffer
    TWiMODLR_HCIMessage TxMessage;

    // reserve one tx-buffer for transmission of SLIP encoded octet sequence
    UINT8               TxBuffer[512];

    // SLIP communication layer instance
    TComSlip            ComSlip;

    // Serial Device (Comport Abstraction)
    TSerialDevice       SerialDevice;

    TWiMODLRHCIClient*  Client;
};

#endif // WIMODLRHCI_H

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
