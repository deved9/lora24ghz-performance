//------------------------------------------------------------------------------
//
//	File:		WiMODLRHCI.cpp
//
//	Abstract:	WiMODLR HCI Wrapper Class Implementation
//
//	Version:	0.1
//
//	Date:		02.01.2014
//
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  Include Files
//
//------------------------------------------------------------------------------

#include "WiMODLRHCI.h"
#include "CRC16.h"
#include <chrono>
#include <ios>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <format>
#include <fstream>


//------------------------------------------------------------------------------
//
//  Defines
//
//------------------------------------------------------------------------------

#define SX1272_FOSC             32000000.0

// #define print_res

//------------------------------------------------------------------------------
//
// Data/Radio Link Status Code Strings
//
//------------------------------------------------------------------------------

const TWiMODLRHCI_IDString   WiMODLRHCI_DataLinkStatusStrings[] =
{
    { DATALINK_STATUS_OK,                   "OK" },
    { DATALINK_STATUS_ERROR,                "error" } ,
    { DATALINK_STATUS_CMD_NOT_SUPPORTED,    "command no supported" },
    { DATALINK_STATUS_WRONG_PARAMETER,      "wrong parameter" },
    { DATALINK_STATUS_WRONG_DEVICE_MODE,    "wrong device mode" },
    { DATALINK_STATUS_MEDIA_BUSY,   		"media busy" },
    { DATALINK_STATUS_DEVICE_BUSY,  		"device busy" },
    { DATALINK_STATUS_QUEUE_FULL,           "queue full" },
    { 0, 0 }
};



//------------------------------------------------------------------------------
//
// Device Management Status Code Strings
//
//------------------------------------------------------------------------------

const TWiMODLRHCI_IDString WiMODLRHCI_DeviceMgmtStatusStrings[] =
{
    { DEVMGMT_STATUS_OK,                   "OK" },
    { DEVMGMT_STATUS_ERROR,                "error" } ,
    { DEVMGMT_STATUS_CMD_NOT_SUPPORTED,    "command no supported" },
    { DEVMGMT_STATUS_WRONG_PARAMETER,      "wrong parameter" },
    { DEVMGMT_STATUS_WRONG_DEVICE_MODE,    "wrong device mode" },
    { 0, 0 }
};

//------------------------------------------------------------------------------
//
// Radio Link Test Status Code Strings
//
//------------------------------------------------------------------------------

const TWiMODLRHCI_IDString   WiMODLRHCI_RadioLinkTestStatusStrings[] =
{
    { RLT_STATUS_OK,                   "OK" },
    { RLT_STATUS_ERROR,                "Error" } ,
    { RLT_STATUS_CMD_NOT_SUPPORTED,    "command no supported" },
    { RLT_STATUS_WRONG_PARAMETER,      "wrong parameter" },
    { RLT_STATUS_WRONG_RADIO_MODE,     "wrong radio mode" },
    { 0, 0 }
};

//------------------------------------------------------------------------------
//
// Radio Configuration Strings
//
//------------------------------------------------------------------------------

// RadioModes
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_RadioModes[] =
{
    { WiMODLR_RADIO_CONFIG_RM_STANDARD, "Standard" },
    { WiMODLR_RADIO_CONFIG_RM_ECHO,     "Echo" },
    { WiMODLR_RADIO_CONFIG_RM_SNIFFER,  "Sniffer" },
    { 0, 0 }
};

// Modulations
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_Modulations[] =
{
    { WiMODLR_RADIO_CONFIG_MOD_LORA, "LoRa" },
    { WiMODLR_RADIO_CONFIG_MOD_FSK, "Fsk" },
    { 0, 0 }
};

// Bandwidths
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_Bandwidths[] =
{
    { WiMODLR_RADIO_CONFIG_BW_125kHz, "125 kHz" },
    { WiMODLR_RADIO_CONFIG_BW_250kHz, "250 kHz" },
    { WiMODLR_RADIO_CONFIG_BW_500kHz, "500 kHz" },
    { 0, 0 }
};



// Spreading Factors
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_SFs[] =
{
    { 0, "SF7" },
    { 1, "SF7" },
    { 2, "SF7" },
    { 3, "SF7" },
    { 4, "SF7" },
    { 5, "SF7" },
    { 6, "SF7" },
    { WiMODLR_RADIO_CONFIG_SF7, "SF7" },
    { WiMODLR_RADIO_CONFIG_SF8, "SF8" },
    { WiMODLR_RADIO_CONFIG_SF9, "SF9" },
    { WiMODLR_RADIO_CONFIG_SF10, "SF10" },
    { WiMODLR_RADIO_CONFIG_SF11, "SF11" },
    { WiMODLR_RADIO_CONFIG_SF12, "SF12" },
    { 0, 0 }
};

// Error Codings
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_ECs[] =
{
    { 0, "4/5" },
    { WiMODLR_RADIO_CONFIG_EC_4_5, "4/5" },
    { WiMODLR_RADIO_CONFIG_EC_4_6, "4/6" },
    { WiMODLR_RADIO_CONFIG_EC_4_7, "4/7" },
    { WiMODLR_RADIO_CONFIG_EC_4_8, "4/8" },
    { 0, 0 }
};

// Power Levels
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_PowerLevels[] =
{
    { 0, "5 dBm" },
    { 1, "5 dBm" },
    { 2, "5 dBm" },
    { 3, "5 dBm" },
    { 4, "5 dBm" },
    { 5, "5 dBm" },
    { 6, "6 dBm" },
    { 7, "7 dBm" },
    { 8, "8 dBm" },
    { 9, "9 dBm" },
    { 10, "10 dBm" },
    { 11, "11 dBm" },
    { 12, "12 dBm" },
    { 13, "13 dBm" },
    { 14, "14 dBm" },
    { 15, "15 dBm" },
    { 16, "16 dBm" },
    { 17, "17 dBm" },
    { 18, "18 dBm" },
    { 19, "19 dBm" },
    { 20, "20 dBm" },
    { 21, "21 dBm" },
    { 0, 0 }
};

// Tx Control
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_TxControl[] =
{
    { 0, "Tx Filter off" },
    { 1, "Tx Filter on" },
    { 0, 0 }
};

// Rx Control
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_RxControl[] =
{
    { 0, "Rx off" },
    { 1, "Rx aways on" },
    { 2, "Rx window on" },
    { 0, 0 }
};

// LED Control
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_LEDControl[] =
{
    { 0, "Rx(D3)" },
    { 1, "Tx(D2)" },
    { 2, "Alive(D4)" },
    { 3, "Button(D1)" },
    { 0, 0 }
};

// Radio Options
const TWiMODLRHCI_IDString  WiMODLRHCI_RadioConfig_RadioOptions[] =
{
    { 0, "Ext. Output" },
    { 1, "RTC ON" },
    { 2, "HCI TxInd" },
    { 3, "HCI PowerUp-Ind" },
    { 4, "HCI Button-Ind" },
    { 0, 0 }
};

// Global variables
TWiMODLR_RadioLinkTestStatus last_meas;
// for keeping track of total count
long LTxCount = 0;
long LRxCount = 0;
long PTxCount = 0;
long PRxCount = 0;

//------------------------------------------------------------------------------
//
//  TWiMODLRHCI - Class Constructor
//
//------------------------------------------------------------------------------


TWiMODLRHCI::TWiMODLRHCI()
{
    // register for rx-messages
    ComSlip.RegisterClient(this);

    // pass first RxBuffer and enable receiver/decoder
    ComSlip.SetRxBuffer(&Rx.Message.SapID, (UINT16)WIMODLR_HCI_RX_MESSAGE_SIZE);

    // init counter
    Rx.CRCError = 0;

    // 1000ms timeout for response
    Rx.Timeout = 1000;
}

//------------------------------------------------------------------------------
//
//  ~TWiMODLRHCI - Class Destructor
//
//------------------------------------------------------------------------------

TWiMODLRHCI::~TWiMODLRHCI()
{
    // close comport, if opened
    Close();
}

//------------------------------------------------------------------------------
//
//  Open
//
//  @brief  open comport for communication purpose
//
//------------------------------------------------------------------------------

bool
TWiMODLRHCI::Open(std::string& comPort)
{
    // open serial device
    if (SerialDevice.Open(comPort, Baudrate_115200))
    {
         // pass first RxBuffer and enable receiver/decoder
        ComSlip.SetRxBuffer(&Rx.Message.SapID, (UINT16)WIMODLR_HCI_RX_MESSAGE_SIZE);

        // ok
        return true;
    }
    // error
    return false;
}

//------------------------------------------------------------------------------
//
//  Close
//
//  @brief  close comport
//
//------------------------------------------------------------------------------

bool
TWiMODLRHCI::Close()
{
    // close serial device
    if (SerialDevice.Close())
    {
        // ok
        return true;
    }
    // error
    return false;
}

//------------------------------------------------------------------------------
//
//  Process
//
//  @brief: handle comport receiver path
//
//------------------------------------------------------------------------------

void
TWiMODLRHCI::Process()
{
    //std::cout << "Entering Process()\n";
    // read data from comport
    int numRxBytes = SerialDevice.ReadData(Rx.Buffer, sizeof(Rx.Buffer));
    //std::cout << numRxBytes << std::endl;
    // bytes received ?
    if (numRxBytes > 0)
    {
        // yes, pass to SLIP Decoder
        // Complete SLIP messages will be forwared via callback to
        // callback function "ProcessRxMessage" (see Receiver section)
        ComSlip.DecodeData(Rx.Buffer, numRxBytes);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  Device Management Services
//
//  @brief: this section includes Device Management services
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  PingRequest
//
//  @brief: send ping to check communication link
//
//------------------------------------------------------------------------------

TWiMODLRResult
TWiMODLRHCI::PingRequest()
{
    // send HCI message and wait for response
    return SendHCIMessage(DEVMGMT_SAP_ID, DEVMGMT_MSG_PING_REQ, DEVMGMT_MSG_PING_RSP);
}

//------------------------------------------------------------------------------
//
//  FactoryReset
//
//  @brief: restore factory settings
//
//------------------------------------------------------------------------------

TWiMODLRResult
TWiMODLRHCI::FactoryReset()
{
    // send HCI message and wait for response
    return SendHCIMessage(DEVMGMT_SAP_ID, DEVMGMT_MSG_RESET_RADIO_CONFIG_REQ, DEVMGMT_MSG_RESET_RADIO_CONFIG_RSP);
}

//------------------------------------------------------------------------------
//
//  GetRadioConfiguration
//
//  @brief: get radio configuration
//
//------------------------------------------------------------------------------

TWiMODLRResult
TWiMODLRHCI::GetRadioConfiguration(TWiMODLR_RadioConfig& config, UINT8& status)
{
    // send message and wait for response
    TWiMODLRResult result = SendHCIMessage(DEVMGMT_SAP_ID, DEVMGMT_MSG_GET_RADIO_CONFIG_REQ, DEVMGMT_MSG_GET_RADIO_CONFIG_RSP);

    // message sent && response received ?
    if (result == WiMODLR_RESULT_OK)
    {
        // yes, return response status
        status = Rx.Message.Payload[0];

        // status ok ? -> config valid
        if(status == DEVMGMT_STATUS_OK)
        {
            // set pointer to start of radio configuration field
            UINT8* ptr = &Rx.Message.Payload[1];

            // deserialize data
            config.RadioMode        = *ptr++;
            config.GroupAddress     = *ptr++;
            config.TxGroupAddress   = *ptr++;
            config.DeviceAddress    = NTOH16(ptr); ptr += 2;
            config.TxDeviceAddress  = NTOH16(ptr); ptr += 2;
            config.Modulation       = *ptr++;
            config.Frequency        = NTOH24(ptr); ptr += 3;
            config.Bandwidth        = *ptr++;
            config.SpreadingFactor  = *ptr++;
            config.ErrorCoding      = *ptr++;
            config.PowerLevel       = *ptr++;
            config.TxControl        = *ptr++;
            config.RxControl        = *ptr++;
            config.RxWindowTime     = NTOH16(ptr); ptr += 2;
            config.LEDControl       = *ptr++;
            config.RadioOptions     = *ptr++;
        }

        //ok, HCI response message received
        return WiMODLR_RESULT_OK;
    }

    // return result
    return result;
}

//------------------------------------------------------------------------------
//
//  SetRadioConfiguration
//
//  @brief: set radio configuration
//
//------------------------------------------------------------------------------

TWiMODLRResult
TWiMODLRHCI::SetRadioConfiguration(TWiMODLR_RadioConfig& config, UINT8 destMemory, UINT8& status)
{
    UINT8 payload[1+21];

    // set destination memory (RAM / NVM(EEPROM))
    payload[0] = destMemory;

    // set pointer to start of radio configuration field
    UINT8* ptr = &payload[1];

    // serialize data
    *ptr++ = config.RadioMode;
    *ptr++ = config.GroupAddress;
    *ptr++ = config.TxGroupAddress;
    HTON16(ptr, config.DeviceAddress); ptr += 2;
    HTON16(ptr, config.TxDeviceAddress); ptr += 2;
    *ptr++ = config.Modulation;
    HTON24(ptr, config.Frequency); ptr+= 3;
    *ptr++ = config.Bandwidth;
    *ptr++ = config.SpreadingFactor;
    *ptr++ = config.ErrorCoding;
    *ptr++ = config.PowerLevel;
    *ptr++ = config.TxControl;
    *ptr++ = config.RxControl;
    HTON16(ptr, config.RxWindowTime); ptr += 2;
    *ptr++ = config.LEDControl;
    *ptr++ = config.RadioOptions;

    // send message and wait for response
    TWiMODLRResult result = SendHCIMessage(DEVMGMT_SAP_ID, DEVMGMT_MSG_SET_RADIO_CONFIG_REQ, DEVMGMT_MSG_SET_RADIO_CONFIG_RSP, payload, sizeof(payload));
    if(result == WiMODLR_RESULT_OK)
    {
        // return status
        status = Rx.Message.Payload[0];

        return WiMODLR_RESULT_OK;
    }
    return result;
}
//------------------------------------------------------------------------------
//
//  ConvertRadioConfiguration
//
//  @brief: convert radio configuration into human readble key-value-list
//
//------------------------------------------------------------------------------
/*
void
TWiMODLRHCI::ConvertRadioConfiguration(TKeyValueList& list, const TWiMODLR_RadioConfig& config)
{
    // Radio Mode
    list.AddKeyValue("RadioMode", GetStringFromTable(WiMODLRHCI_RadioConfig_RadioModes, config.RadioMode));

    // Addresses on RF Link
    list.AddHexKeyValue("GroupAddress",     config.GroupAddress);
    list.AddHexKeyValue("TxGroupAddress",   config.TxGroupAddress);
    list.AddHexKeyValue("DeviceAddress",    config.DeviceAddress);
    list.AddHexKeyValue("TxDeviceAddress",  config.TxDeviceAddress);

    // Modulation
    list.AddKeyValue("Modulation", GetStringFromTable(WiMODLRHCI_RadioConfig_Modulations, config.Modulation));

    // Carrier Frequency
    UINT32 freq = GetFrequencyFromConfig(config.Frequency);
    list.AddKeyValue("Frequency", QString("%1 Hz").arg(freq));

    // Bandwidth
    list.AddKeyValue("Bandwidth", GetStringFromTable(WiMODLRHCI_RadioConfig_Bandwidths, config.Bandwidth));

    // SF
    list.AddKeyValue("SpreadingFactor", GetStringFromTable(WiMODLRHCI_RadioConfig_SFs, config.SpreadingFactor));

    // Error Coding
    list.AddKeyValue("ErrorcCoding", GetStringFromTable(WiMODLRHCI_RadioConfig_ECs, config.ErrorCoding));

    // Power Level
    list.AddKeyValue("PowerLevel", GetStringFromTable(WiMODLRHCI_RadioConfig_PowerLevels, config.PowerLevel));

    // Tx Control
    list.AddKeyValue("TxControl", GetStringFromTable(WiMODLRHCI_RadioConfig_TxControl, config.TxControl));

    // Rx Control
    list.AddKeyValue("RxControl", GetStringFromTable(WiMODLRHCI_RadioConfig_RxControl, config.RxControl));

    // Rx Window
    list.AddKeyValue("RxWindowTime", QString("%1 ms").arg((UINT16)config.RxWindowTime));

    // LED Control
    list.AddKeyValue("LEDControl", GetCombinedStringFromTable(WiMODLRHCI_RadioConfig_LEDControl, config.LEDControl, 4));

    // Radio Options
    list.AddKeyValue("RadioOptions", GetCombinedStringFromTable(WiMODLRHCI_RadioConfig_RadioOptions, config.RadioOptions, 5));
}
*/
//------------------------------------------------------------------------------
//
//  GetDeviceMgmtStatusString
//
//  @brief  convert device manamgement status code to string
//
//------------------------------------------------------------------------------

const char*
TWiMODLRHCI::GetDeviceMgmtStatusString(UINT8 status)
{
    return GetStringFromTable(WiMODLRHCI_DeviceMgmtStatusStrings, status);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  Radio Link Services
//
//  @brief: this section includes Radio Link services
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  SendURadioMessage
//
//  @brief: send unrelaible radio message
//
//------------------------------------------------------------------------------

TWiMODLRResult
TWiMODLRHCI::SendURadioMessage(UINT8* txMessage, UINT16 length, UINT8& status)
{
    // send message and wait for response
    TWiMODLRResult result = SendHCIMessage(DATALINK_SAP_ID, DATALINK_MSG_SEND_URADIO_MSG_REQ, DATALINK_MSG_SEND_URADIO_MSG_RSP, txMessage, length);

    // message sent && response received ?
    if (result == WiMODLR_RESULT_OK)
    {
        // yes, return response status
        status = Rx.Message.Payload[0];

        //ok, HCI response message received
        return WiMODLR_RESULT_OK;
    }

    // return result
    return result;
}

//------------------------------------------------------------------------------
//
//  ConvertRadioMessage
//
//  @brief: convert radio message into human readable key-value-list
//
//------------------------------------------------------------------------------
/*
void
TWiMODLRHCI::ConvertRadioRxMessage(TKeyValueList& list, const TWiMODLR_HCIMessage& rxMsg)
{
    // get radio packet format
    UINT8 format = rxMsg.Payload[0];
    list.AddHexKeyValue("Format", format);

    // get destination group address
    list.AddHexKeyValue("DstGroupAddress:",rxMsg.Payload[1]);

    // get destination device address
    UINT16 val = NTOH16(&rxMsg.Payload[2]);
    list.AddHexKeyValue("DstDeviceAddress:",val);

    // get source group address
    list.AddHexKeyValue("SrcGroupAddress:",rxMsg.Payload[4]);

    // get source device address
    val = NTOH16(&rxMsg.Payload[5]);
    list.AddHexKeyValue("SrcDeviceAddress:",val);

    // get radio message payload (msg length - x bytes header)
    int numPayloadBytes = rxMsg.Length - DATALINK_MSG_HEADER_SIZE;

    // extended HCI output fomat attched ?
    if (format & 0x01)
    {
        // correct  by additional footer bytes
        numPayloadBytes -= DATALINK_MSG_FOOTER_SIZE;
    }

    // add payload
    if(numPayloadBytes > 0)
    {
        list.AddHexKeyValue("Payload",&rxMsg.Payload[DATALINK_MSG_HEADER_SIZE], numPayloadBytes);
    }

    // extended HCI output ?
    if(format & 0x01)
    {
        int extOffset = DATALINK_MSG_HEADER_SIZE + numPayloadBytes;

        INT16  rssi = NTOH16(&rxMsg.Payload[extOffset]);
        INT8   snr  = rxMsg.Payload[extOffset + 2];
        UINT32 time = NTOH32(&rxMsg.Payload[extOffset + 3]);

        QString timeString;
        U32TimeToString(timeString, time, true);

        list.AddKeyValue("RSSI", QString("%1 dBm").arg((float)rssi,0,'f',2));
        list.AddKeyValue("SNR", QString("SNR:%1 dB").arg((float)snr,0,'f',2));
        list.AddKeyValue("RxTime", timeString);
    }
}
*/
//------------------------------------------------------------------------------
//
//  GetRadioLinkStatusString
//
//  @brief  convert radiolink status code to string
//
//------------------------------------------------------------------------------

const char*
TWiMODLRHCI::GetRadioLinkStatusString(UINT8 status)
{
    return GetStringFromTable(WiMODLRHCI_DataLinkStatusStrings, status);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  HCI Message functions
//
//  @brief: this section includes functions for HCI message handling
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  SendHCIMessage
//
//  @brief: this function send a HCI message to a radio module and waits
//          for a response
//
//------------------------------------------------------------------------------

TWiMODLRResult
TWiMODLRHCI::SendHCIMessage(UINT8 dstSapID, UINT8 msgID, UINT8 rxMsgID, UINT8* payload, UINT16 length)
{
    // send message
    TWiMODLRResult result = PostMessage(dstSapID, msgID, payload, length);

    #ifdef debug
    std::cout << "Send result: " << result << std::endl;
    #endif
    
    // message sent ?
    if (result == WiMODLR_RESULT_OK)
    {
        // yes, wait for response from radio
        if (WaitForResponse(dstSapID, rxMsgID))
        {
            return WiMODLR_RESULT_OK;
        }
        return WiMODLR_RESULT_NO_RESPONSE;
    }
    // return error
    return result;
}

//------------------------------------------------------------------------------
//
//  PostMessage
//
//  @brief  send a HCI message (with or without payload)
//
//------------------------------------------------------------------------------

TWiMODLRResult
TWiMODLRHCI::PostMessage(UINT8 sapID, UINT8 msgID, UINT8* payload, UINT16 length)
{
    // 1. check parameter
    //
    // 1.1 check length
    //
    if(length > WIMODLR_HCI_MSG_PAYLOAD_SIZE)
    {
        return WiMODLR_RESULT_PAYLOAD_LENGTH_ERROR;
    }
    // 1.2 check payload ptr
    //
    if(length && !payload)
    {
        return WiMODLR_RESULT_PAYLOAD_PTR_ERROR;
    }

    // 2.  init TxMessage
    //
    // 2.1 init SAP ID
    //
    TxMessage.SapID = sapID;

    // 2.2 init Msg ID
    //
    TxMessage.MsgID = msgID;

    // 2.3 copy payload, if present
    //
    if(payload && length)
    {
        UINT8*  dstPtr  = TxMessage.Payload;
        int     n       = (int)length;

        // copy bytes
        while(n--)
            *dstPtr++ = *payload++;
    }

    // 3. Calculate CRC16 over header and optional payload
    //
    UINT16 crc16 = CRC16_Calc(&TxMessage.SapID, length + WIMODLR_HCI_MSG_HEADER_SIZE, CRC16_INIT_VALUE);

    // 3.1 get 1's complement
    //
    crc16 = ~crc16;

    // 3.2 attach CRC16 and correct length, lobyte first
    //
    TxMessage.Payload[length++] = LOBYTE(crc16);
    TxMessage.Payload[length++] = HIBYTE(crc16);

    // 4. forward message to SLIP layer
    //    - start transmission with SAP ID
    //    - correct length by header size

    return SendPacket(&TxMessage.SapID, length + WIMODLR_HCI_MSG_HEADER_SIZE);
}

//------------------------------------------------------------------------------
//
//  SendPacket
//
//  @brief: send packet as SLIP stream via serial device
//
//------------------------------------------------------------------------------

TWiMODLRResult
TWiMODLRHCI::SendPacket(UINT8* txData, UINT16 length)
{
    #ifdef debug
    std::cout << "Sending packet" <<std::endl;
    #endif

    // call SLIP encoder
    int txLength = ComSlip.EncodeData(TxBuffer, sizeof(TxBuffer), txData, length);

    // stream ok ?
    if (txLength > 0)
    {
        // send SLIP stream via serial device
        if (SerialDevice.SendData(TxBuffer, txLength))
            return WiMODLR_RESULT_OK;
        else
            return WiMODLR_RESULT_TRANMIT_ERROR;
    }
    return WiMODLR_RESULT_SLIP_ENCODER_ERROR;
}

//------------------------------------------------------------------------------
//
//  WaitForResponse
//
//  @brief: wait for response HCI message
//
//------------------------------------------------------------------------------
bool
TWiMODLRHCI::WaitForResponse(UINT8 rxSapID, UINT8 rxMsgID)
{
    // init receiver struct
    Rx.Active = true;
    Rx.Done   = false;
    Rx.SapID  = rxSapID;
    Rx.MsgID  = rxMsgID;

    // Get the starting time point
    auto start = std::chrono::steady_clock::now();

    // wwit for response ~1000ms
    while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() <= Rx.Timeout)
    {
        // call receiver path
        Process();

        // response received  ?
        if(Rx.Done)
        {
            // clear flag
            Rx.Active = false;

            // ok
            #ifdef debug
            std::cout << "Got Response" << std::endl;
            #endif

            return true;
        }
    }

    #ifdef debug
    std::cout << "Timed out" << std::endl;
    #endif
    
    // clear flag
    Rx.Active = false;

    // error - timeout
    return false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  Section Receiver Functions
//
//  @brief: this section includes functions that handle received HCI messages
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  ProcessRxMessage
//
//  @brief: handle received SLIP message
//
//------------------------------------------------------------------------------

UINT8*
TWiMODLRHCI::ProcessRxMessage(UINT8* rxBuffer, UINT16 length)
{
    #ifdef debug
    std::cout << "Entering ProcessRxMessage" << std::endl;
    #endif
    
    // 1. check CRC
    if (CRC16_Check(rxBuffer, length, CRC16_INIT_VALUE))
    {
        // 2. check min length, 2 bytes for SapID + MsgID + 2 bytes CRC16
        if(length >= (WIMODLR_HCI_MSG_HEADER_SIZE + WIMODLR_HCI_MSG_FCS_SIZE))
        {
            // 3. Hack: since only one RxMessage buffer is used,
            //          rxBuffer must point to RxMessage.SapId, thus
            //          memcpy to RxMessage structure is not needed here

            // add length
            Rx.Message.Length = length - (WIMODLR_HCI_MSG_HEADER_SIZE + WIMODLR_HCI_MSG_FCS_SIZE);
            
            #ifdef debug
            std::cout << "Length: " << Rx.Message.Length << std::endl;
            std::cout << "Payload: \n";
            for(int i = 0; i < Rx.Message.Length; i++)
            {
                std::cout << std::format("{:#x}", Rx.Message.Payload[i]);
                std::cout << " ";
            }
            std::cout << "\n\n";
            #endif

            // dispatch completed RxMessage
            DispatchRxMessage(Rx.Message);
        }
    }
    else
    {
        // handle CRC error
        Rx.CRCError++;

        ShowMessage("CRC Error");
    }

    // return same buffer again, keep receiver enabled
    return &Rx.Message.SapID;
}

//------------------------------------------------------------------------------
//
//  DispatchRxMessage
//
//  @brief: dispatch CRC checked HCI message
//
//------------------------------------------------------------------------------

void
TWiMODLRHCI::DispatchRxMessage(TWiMODLR_HCIMessage& rxMsg)
{
    #ifdef debug
    std::cout << "Entering DispatchRxMessage" << std::endl;
    #endif
    
    // 1. test if a response message is expected
    if(Rx.Active)
    {
        // expected response received ?
        if ((rxMsg.SapID == Rx.SapID) && (rxMsg.MsgID == Rx.MsgID))
        {
            #ifdef debug
            std::cout << "SAPID: " << std::format("{:#x}", rxMsg.SapID) << ", MsgID: " << std::format("{:#x}", rxMsg.MsgID) << std::endl;
            #endif
            
            // yes
            Rx.Done = true;
        }
    }

    // 2. forward received messages to corresponding SAP
    switch(rxMsg.SapID)
    {
        case    DEVMGMT_SAP_ID:
                DispatchDeviceMgmtMessage(rxMsg);
                break;

        case    DATALINK_SAP_ID:
                DispatchRadioLinkMessage(rxMsg);
                break;

        case    RLT_SAP_ID:
                DispatchRadioLinkTestMessage(rxMsg);
                break;
    }
}

//------------------------------------------------------------------------------
//
//  DisaptchDeviceMgmtMessage
//
//  @brief: dispatch Device Management Access Point messages here
//
//------------------------------------------------------------------------------

void
TWiMODLRHCI::DispatchDeviceMgmtMessage(TWiMODLR_HCIMessage& rxMsg)
{
    switch(rxMsg.MsgID)
    {
        case    DEVMGMT_MSG_PING_RSP:
                // handle ping response here
                break;

        default:
                // handle unsupported MsgIDs here
                ShowMessage("warning - unsupported DeviceMgmt message received", rxMsg);
                break;
    }
}

//------------------------------------------------------------------------------
//
//  DispatchRadioLinkMessage
//
//  @brief: dispatch Radio Link Access Point messages here
//
//------------------------------------------------------------------------------

void
TWiMODLRHCI::DispatchRadioLinkMessage(TWiMODLR_HCIMessage& rxMsg)
{
    switch(rxMsg.MsgID)
    {
        case    DATALINK_MSG_RECV_URADIO_MSG_IND:
                ShowMessage("Unreliable RadioLink message received", rxMsg);
                if (Client)
                {
                    // notify client
                    Client->evRadioLink_RxUMessage(rxMsg);
                }
                break;

        default:
                // handle unsupported MsgIDs here
                ShowMessage("warning - unsupported RadioLink message received", rxMsg);
                break;
    }
}

//------------------------------------------------------------------------------
//
//  DispatchRadioLinkMessage
//
//  @brief: dispatch Radio Link Access Point messages here
//
//------------------------------------------------------------------------------

void
TWiMODLRHCI::DispatchRadioLinkTestMessage(TWiMODLR_HCIMessage& rxMsg)
{
    #ifdef debug
    std::cout << "Entered DispatchRadioLintTestMessage" << std::endl;
    #endif

    switch(rxMsg.MsgID)
    {
        case    RLT_MSG_START_RSP:
                #ifdef debug
                std::cout << "Radio Link Test Start Response: " << GetStringFromTable(WiMODLRHCI_RadioLinkTestStatusStrings,rxMsg.Payload[0]) << std::endl;
                #endif
                break; 

        case    RLT_MSG_STOP_RSP:
                #ifdef debug
                std::cout << "Radio Link Test Stop Response: " << GetStringFromTable(WiMODLRHCI_RadioLinkTestStatusStrings,rxMsg.Payload[0]) << std::endl;
                #endif
                break;

        case    RLT_MSG_STATUS_IND:
                // set pointer to start of radio configuration field
                UINT8* ptr = &Rx.Message.Payload[0];

                //if(*ptr == 1)
                //    break;

                // deserialize data
                TWiMODLR_RadioLinkTestStatus meas;
                meas.TestStatus = *ptr++;
                meas.LTxCount = NTOH16(ptr); ptr += 2;
                meas.LRxCount = NTOH16(ptr); ptr += 2;
                meas.PTxCount = NTOH16(ptr); ptr += 2;
                meas.PRxCount = NTOH16(ptr); ptr += 2;
                meas.LocalRSSI = NTOH16(ptr); ptr += 2;
                meas.PeerRSSI = NTOH16(ptr); ptr += 2;
                meas.LocalSNR = *ptr++;
                meas.PeerSNR = *ptr;

                if(meas.LTxCount == 1)
                {
                    // measurement reset after 100
                    LTxCount += meas.LTxCount;
                    LRxCount += meas.LRxCount;
                    PTxCount += meas.PTxCount;
                    PRxCount += meas.PRxCount;
                }
                else
                {
                    // keep track of packet counts, including packet loss
                    LTxCount += meas.LTxCount - last_meas.LTxCount;
                    LRxCount += meas.LRxCount - last_meas.LRxCount;
                    PTxCount += meas.PTxCount - last_meas.PTxCount;
                    PRxCount += meas.PRxCount - last_meas.PRxCount;
                }


                last_meas = meas;

                meas.LTxCount = LTxCount;
                meas.LRxCount = LRxCount;
                meas.PTxCount = PTxCount;
                meas.PRxCount = PRxCount;

                #ifdef print_res
                printMesuredData(meas);
                #endif

                writeDataToFile(filename, meas);

                break;
    }
}

// print all data to std::cout
#ifdef print_res
void TWiMODLRHCI::printMesuredData(TWiMODLR_RadioLinkTestStatus& data)
{
    std::cout << "Link status: " << std::to_string(data.LTxCount) << std::endl;
    std::cout << "Local Tx Count: " << std::to_string(data.LTxCount) << std::endl;
    std::cout << "Local Rx Count: " << std::to_string(data.LRxCount) << std::endl;
    std::cout << "Peer Tx Count: " << std::to_string(data.PTxCount) << std::endl;
    std::cout << "Peer Rx Count: " << std::to_string(data.PRxCount) << std::endl;
    std::cout << "Local RSSI: " << std::to_string(data.LocalRSSI) << " dBm" << std::endl;
    std::cout << "Peer RSSI: " << std::to_string(data.PeerRSSI) << "dBm" << std::endl;
    std::cout << "Local SNR: " << std::to_string(data.LocalSNR) << "dBm" << std::endl;
    std::cout << "Peer SNR: " << std::to_string(data.PeerSNR) << "dBm" << std::endl << std::endl;
}
#endif

// write measured data to a CSV file
void TWiMODLRHCI::writeDataToFile(std::string& filename,TWiMODLR_RadioLinkTestStatus& data)
{
    // Open CSV file in append mode to add new data
    std::ofstream csvFile(filename, std::ios::app);
    if (!csvFile.is_open()) {
        std::cerr << "Error: Could not open the file." << std::endl;
        return;
    }

    // Write data to the CSV file in a comma-separated format
    csvFile << getCurrentDateTimeISO() << ","
            << std::to_string(data.LTxCount) << ","
            << std::to_string(data.LRxCount) << ","
            << std::to_string(data.PTxCount) << ","
            << std::to_string(data.PRxCount) << ","
            << std::to_string(data.LocalRSSI) << ","
            << std::to_string(data.PeerRSSI) << ","
            << std::to_string(data.LocalSNR) << ","
            << std::to_string(data.PeerSNR) << std::endl;

    // Close the file
    csvFile.close();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  Debug Functions
//
//  @brief: this section includes some debug functions
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  ShowMessage
//
//  @brief: show debug message
//
//------------------------------------------------------------------------------

void
TWiMODLRHCI::ShowMessage(const std::string& strMsg, const TWiMODLR_HCIMessage& msg)
{
    std::cout << strMsg << std::endl;
}
//------------------------------------------------------------------------------
//
//  ShowMessage
//
//  @brief: show debug message
//
//------------------------------------------------------------------------------

void
TWiMODLRHCI::ShowMessage(const char* string)
{
    std::cout << string << std::endl;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
//  Helper Functions
//
//  @brief: this section includes some helper functions
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  U32TimeToString
//
//  @brief: convert target time to time string
//
//------------------------------------------------------------------------------

#define RTC_GET_SECONDS(t)	((t)       & 0x3F)
#define RTC_GET_MINUTES(t)	((t >>  6) & 0x3F)
#define RTC_GET_MONTHS(t)	((t >> 12) & 0x0F)

#define RTC_GET_HOURS(t)	((t >> 16) & 0x1F)
#define RTC_GET_DAYS(t)		((t >> 21) & 0x1F)
#define RTC_GET_YEARS(t)   (((t >> 26) & 0x3F) + 2000)

void
TWiMODLRHCI::U32TimeToString(std::string& timeString, UINT32 time, bool isoFormat)
{
    UINT8   seconds = RTC_GET_SECONDS(time);
    UINT8   minutes = RTC_GET_MINUTES(time);
    UINT8   months  = RTC_GET_MONTHS(time);
    UINT8   hours   = RTC_GET_HOURS(time);
    UINT8   days    = RTC_GET_DAYS(time);
    UINT16  years   = RTC_GET_YEARS(time);

    std::ostringstream oss;
    oss << std::setfill('0');  // Set fill character to '0'

    if (isoFormat) {
        oss << years << '-'
            << std::setw(2) << static_cast<int>(months) << '-'
            << std::setw(2) << static_cast<int>(days) << ' '
            << std::setw(2) << static_cast<int>(hours) << ':'
            << std::setw(2) << static_cast<int>(minutes) << ':'
            << std::setw(2) << static_cast<int>(seconds);
    } else {
        oss << std::setw(2) << static_cast<int>(days) << '.'
            << std::setw(2) << static_cast<int>(months) << '.'
            << years << ' '
            << std::setw(2) << static_cast<int>(hours) << ':'
            << std::setw(2) << static_cast<int>(minutes) << ':'
            << std::setw(2) << static_cast<int>(seconds);
    }

    timeString = oss.str();
}


//------------------------------------------------------------------------------
//
//  GetFrequencyFromConfig
//
//  @brief: convert frequency register configuration to frequency value in [Hz]
//
//------------------------------------------------------------------------------

UINT32
TWiMODLRHCI::GetFrequencyFromConfig(UINT32 regConfig)
{
    double fstep    = SX1272_FOSC / double(1 << 19);

    double freq     = (regConfig * fstep);

    return (UINT32)freq;
}

//------------------------------------------------------------------------------
//
//  GetStringFromTable
//
//  @brief: convert ID into human readable string
//
//------------------------------------------------------------------------------

const char*
TWiMODLRHCI::GetStringFromTable(const TWiMODLRHCI_IDString* table, UINT8 id)
{
    while(table->IDString)
    {
        if (table->ID == id)
            return table->IDString;

        table++;
    }
    return "Unsupported ID";
}

//------------------------------------------------------------------------------
//
//  GetCombinedStringFromTable
//
//  @brief: convert bit settings into human readable string
//
//------------------------------------------------------------------------------

std::string
TWiMODLRHCI::GetCombinedStringFromTable(const TWiMODLRHCI_IDString* table, UINT8 id, int numBits)
{
    std::string string;

    UINT8 mask = 0x01;
    for(int i = 0; i < numBits; i++)
    {
        if (id & mask)
        {
            string.append(GetStringFromTable(table, i));
            string.append("/");
        }
        mask <<= 1;
    }

    string.erase(string.size() - 1, 1);
    if (string == "")
        string = "off";

    return string;
}

// Returns current timestamp in ISO 8601 time format
std::string 
TWiMODLRHCI::getCurrentDateTimeISO() {
    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    // Convert time_point to a tm structure
    std::tm tm = *std::localtime(&now_c);

    // Get milliseconds
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    // Format tm into string
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S.");
    oss << std::setfill('0') << std::setw(3) << milliseconds.count(); // Add milliseconds with zero-padding to three digits

    return oss.str();
}
//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------
