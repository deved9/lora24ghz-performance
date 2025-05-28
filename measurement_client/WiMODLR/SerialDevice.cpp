//------------------------------------------------------------------------------
//
//	File:		SerialDevice.cpp
//
//	Abstract:	Serial Device Wrapper Class Implementation
//
//	Version:	0.1
//
//	Date:		09.02.2015
//
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------

#include "SerialDevice.h"
#include <string>
#include <vector>
#include <iostream>
#include <format>


//------------------------------------------------------------------------------
//
//  TSerialDevice - Class Constructor
//
//------------------------------------------------------------------------------

TSerialDevice::TSerialDevice()
{
    // init handle
    ComHandle = INVALID_HANDLE_VALUE;
}

//------------------------------------------------------------------------------
//
//  ~TSerialDevice - Class Destructor
//
//------------------------------------------------------------------------------

TSerialDevice::~TSerialDevice()
{
    // perform close
    Close();
}

//------------------------------------------------------------------------------
//
//  Open
//
//  @brief: open comport
//
//------------------------------------------------------------------------------

bool
TSerialDevice::Open(std::string& comPort, UINT32 baudRate, int bits, UINT8 parity)
{
    if (ComHandle != INVALID_HANDLE_VALUE)
    {
        Close();
    }

    // Open the serial port read/write, with no controlling terminal,
    // and don't wait for a connection.
    // The O_NONBLOCK flag also causes subsequent I/O on the device to
    // be non-blocking.
    // See open(2) ("man 2 open") for details.

    std::string portName = std::string("/dev/") + comPort;
    const char* name  = portName.c_str();
    //const char* ptr = "/dev/tty.usbserial-IMST2"; //name.constData();
    const char* ptr = name;

    ComHandle = ::open(ptr, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (ComHandle != INVALID_HANDLE_VALUE)
    {
        if (::ioctl(ComHandle, TIOCEXCL) != INVALID_HANDLE_VALUE)
        {
            // Get the current options and save them so we can restore the
            // default settings later.
            struct termios _originalTTYAttrs;
            struct termios options;

            if (::tcgetattr(ComHandle, &_originalTTYAttrs) != INVALID_HANDLE_VALUE)
            {
                // The serial port attributes such as timeouts and baud rate are set by
                // modifying the termios structure and then calling tcsetattr to
                // cause the changes to take effect. Note that the
                // changes will not take effect without the tcsetattr() call.
                // See tcsetattr(4) ("man 4 tcsetattr") for details.
                options = _originalTTYAttrs;
                // Print the current input and output baud rates.
                // See tcsetattr(4) ("man 4 tcsetattr") for details.
                cfmakeraw(&options);
                options.c_cc[VMIN] = 1;
                options.c_cc[VTIME] = 10;
                // The baud rate, word length, and handshake options can be set as follows:
                cfsetspeed(&options, baudRate);           // Set 57600 baud
                // cfsetspeed(&options, B115200);           // Set 115200 baud
                // Disable parity (even parity if PARODD
                options.c_cflag &= ~(CSTOPB | CRTSCTS | PARENB );
                // add flags for parity and word size
                options.c_cflag |= (CSIZE | parity);
                options.c_cflag |= (bits|CLOCAL|CREAD);               // Use n bit words
                options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
                options.c_iflag &= ~(IXOFF | IXON | INPCK | OPOST);
                // Cause the new options to take effect immediately.
                if (tcsetattr(ComHandle, TCSANOW, &options) != INVALID_HANDLE_VALUE)
                {
                    return true;
                }
            }
        }
        Close();
    }

    return false;
}
//------------------------------------------------------------------------------
//
//  Close
//
//  @brief: close comport
//
//------------------------------------------------------------------------------
bool
TSerialDevice::Close()
{

    if(ComHandle != INVALID_HANDLE_VALUE)
    {
        ::close(ComHandle);

        ComHandle = INVALID_HANDLE_VALUE;
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
//
//  SendData
//
//  @brief  send a block of characters
//
//------------------------------------------------------------------------------

bool
TSerialDevice::SendData(UINT8* data, int txLength)
{

    if(ComHandle == INVALID_HANDLE_VALUE)
        return false;

    #ifdef debug
    std::cout << "Sending data: \n";
    std::cout << "Length: " << txLength << std::endl;
    std::cout << "Payload: \n";
    for(int i = 0; i < txLength; i++)
    {
        std::cout << std::format("{:#x}", data[i]);
        std::cout << " ";
    }
    std::cout << "\n\n";
    #endif

    size_t  numTxBytes = ::write(ComHandle, data, txLength);

    if (numTxBytes == (size_t)txLength)
    {
        return true;
    }

    return false;

}

//------------------------------------------------------------------------------
//
//  ReadData
//
//  @brief  read a block of characters
//
//------------------------------------------------------------------------------

int
TSerialDevice::ReadData(UINT8* rxBuffer, int bufferSize)
{
    // handle ok ?
    if(ComHandle == INVALID_HANDLE_VALUE){
        std::cout << "Invalid handle" << std::endl;
        return 0;
    }

    ssize_t  numRxBytes = ::read(ComHandle, rxBuffer, bufferSize);
    if(numRxBytes > 0)
        return (int)numRxBytes;


    return 0;
}


//------------------------------------------------------------------------------
//
//  GetComPorts
//
//  @brief  return list of available comports
//
//------------------------------------------------------------------------------

/*
int TSerialDevice::GetComPorts(std::vector<std::string>& portList)
{
    QDir myDir("/dev");

    QStringList filters;

    filters.append("tty*");

    portList = myDir.entryList (filters, QDir::System);	// filter tty and write to portlist

    return portList.count();
}
*/


//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------

