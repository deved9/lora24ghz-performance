// print debug info
//#define debug

#include "WiMODLR/WiMODLRHCI_IDs.h"
#include "WiMODLR/WiMODLRHCI.h"
#include "WiMODLR/WMDefs.h"
#include <iostream>
#include <format>
#include <signal.h>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <string>

int main()
{

    // interface init
    TWiMODLRHCI radioIF = TWiMODLRHCI();

    // select port
    std::string comPort = "ttyUSB0";  // hardcoded cause linux always gave it this path because its the only usb plugged in in my case
    if(!radioIF.Open(comPort))
    {
        std:: cout << "Error opening device!\n";
        return 1;
    }

    // get current date and time in JSON format
    std::string filename = "/home/david/" + radioIF.getCurrentDateTimeISO();

    /*
    // Replace JSON specific characters for file name
    std::replace(filename.begin(), filename.end(), '{', '_');
    std::replace(filename.begin(), filename.end(), '}', '_');
    std::replace(filename.begin(), filename.end(), ':', '-');
    std::replace(filename.begin(), filename.end(), '"', '_');
    std::replace(filename.begin(), filename.end(), ' ', '_');
    std::replace(filename.begin(), filename.end(), ',', '_');
    */

    // append extension
    filename += ".csv";

    // create and open CSV file
    std::ofstream csvFile(filename);
    if (!csvFile.is_open()) {
        std::cerr << "Error: Could not create file " << filename << std::endl;
        return 1;
    }

    radioIF.filename = filename;

    // write header to CSV file
    csvFile << "Time,Local Tx Count,Local Rx Count,Peer Tx Count,Peer Rx Count,Local RSSI [dBm],Peer RSSI [dBm],Local SNR [dB],Peer SNR [dB]" << std::endl;

    // write a coment to the CSV file
    csvFile << "# BW=  SF=  CR=  position: " << std::endl;

    csvFile.close();

    // create static link in filesystem to newest measurement - easier to point to
    std::string command = "ln -s -f " + filename + " /home/david/latest_meas";
    system(command.c_str());

    // RLT config
    TWiMODLR_RadioLinkTestConfig conf;
    conf.GroupAddress = 0x10;
    conf.DeviceAddress = 0x2222;
    conf.PacketSize = 15;
    conf.NumPackets = 100;
    conf.TestMode = 1;  // infinite loop

    // generate payload
    UINT8 payload[7];
    UINT8* ptr = &payload[0];

    *ptr++ = conf.GroupAddress;
    HTON16(ptr, conf.DeviceAddress); ptr += 2;
    *ptr++ = conf.PacketSize;
    HTON16(ptr, conf.NumPackets); ptr += 2;
    *ptr++ = conf.TestMode;

    #ifdef debug
    std::cout << "RLT config payload: ";
    for(int i = 0; i < 7; i++)
    {
        std::cout << std::format("{:#x}",payload[i]);
        std::cout << " ";
    }
    std::cout << "\n\n";
    #endif

    // stop any measurement if running
    radioIF.SendHCIMessage(RLT_SAP_ID, RLT_MSG_STOP_REQ,RLT_MSG_STOP_RSP,nullptr,0);
    
    // wait for 10s before starting measurement
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    // start measurement
    radioIF.SendHCIMessage(RLT_SAP_ID, RLT_MSG_START_REQ,RLT_MSG_START_RSP,payload,7);

    // main loop
    while (true) {
        // wait for measurement data from radio, log it
        radioIF.WaitForResponse(RLT_SAP_ID,RLT_MSG_STATUS_IND);
    }

    return 0;
}

