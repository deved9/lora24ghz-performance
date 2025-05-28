//------------------------------------------------------------------------------
//
//	File:		SerialDevice.h
//
//	Abstract:	Serial Device Wrapper Class Declaration
//
//	Version:	0.1
//
//	Date:		09.02.2015
//
//	Disclaimer:	This example code is provided by IMST GmbH on an "AS IS" basis
//				without any warranties.
//
//------------------------------------------------------------------------------

#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

#include <string>

#include <errno.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define INVALID_HANDLE_VALUE    -1

#define Baudrate_9600       9600
#define Baudrate_115200     115200
#define DataBits_7          CS7
#define DataBits_8          CS8
#define Parity_Even         PARENB
#define Parity_None         0


#include "WMDefs.h"
#include <string>

//------------------------------------------------------------------------------
//
// TSerialDevice Class Declaration
//
//------------------------------------------------------------------------------

class TSerialDevice
{
public:
                TSerialDevice();
                ~TSerialDevice();

    bool        Open(std::string& comPort, UINT32 baudRate, int bits = DataBits_8, UINT8 parity = Parity_None);
    bool        Close();

    bool        SendData(UINT8* data, int txLength);
    int         ReadData(UINT8* rxBuffer, int bufferSize);

private:

    int     ComHandle;

};

#endif // SERIALDEVICE_H

//------------------------------------------------------------------------------
// end of file
//------------------------------------------------------------------------------

