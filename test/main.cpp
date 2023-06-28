#include <iostream>
#include "VSource.h"



/// Link namesapces.
using namespace cr::video;
using namespace std;



// Copy test.
bool copyTest();

// Encode/decode test.
bool encodeDecodeTest();



// Entry point.
int main(void)
{
    cout << "#####################################" << endl;
    cout << "#                                   #" << endl;
    cout << "# VSourceParams test                #" << endl;
    cout << "#                                   #" << endl;
    cout << "#####################################" << endl;
    cout << endl;

    cout << "Copy test:" << endl;
    if (copyTest())
        cout << "OK" << endl;
    else
        cout << "ERROR" << endl;
    cout << endl;

    cout << "Encode/Decode test:" << endl;
    if (encodeDecodeTest())
        cout << "OK" << endl;
    else
        cout << "ERROR" << endl;
    cout << endl;



    return 1;
}



// Copy test.
bool copyTest()
{
    // Prepare random params.
    VSourceParams in;
    in.source = "alsfghljb";
    in.fourcc = "skdfjhvk";
    in.logLevel = rand() % 255;
    in.cycleTimeMks = rand() % 255;
    in.exposure = rand() % 255;
    in.exposureMode = rand() % 255;
    in.gainMode = rand() % 255;
    in.gain = rand() % 255;
    in.focusMode = rand() % 255;
    in.focusPos = rand() % 255;
    in.fps = rand() % 255;
    in.width = rand() % 255;
    in.height = rand() % 255;
    in.isOpen = true;
    in.custom1 = rand() % 255;
    in.custom2 = rand() % 255;
    in.custom3 = rand() % 255;

    // Copy params.
    VSourceParams out = in;

    // Compare params.
    if (in.source != out.source)
    {
        cout << "in.source" << endl;
        return false;
    }
    if (in.fourcc != out.fourcc)
    {
        cout << "in.fourcc" << endl;
        return false;
    }
    if (in.logLevel != out.logLevel)
    {
        cout << "in.logLevel" << endl;
        return false;
    }
    if (in.cycleTimeMks != out.cycleTimeMks)
    {
        cout << "in.cycleTimeMks" << endl;
        return false;
    }
    if (in.exposure != out.exposure)
    {
        cout << "in.exposure" << endl;
        return false;
    }
    if (in.exposureMode != out.exposureMode)
    {
        cout << "in.exposureMode" << endl;
        return false;
    }
    if (in.gainMode != out.gainMode)
    {
        cout << "in.gainMode" << endl;
        return false;
    }
    if (in.gain != out.gain)
    {
        cout << "in.gain" << endl;
        return false;
    }
    if (in.focusMode != out.focusMode)
    {
        cout << "in.focusMode" << endl;
        return false;
    }
    if (in.focusPos != out.focusPos)
    {
        cout << "in.focusPos" << endl;
        return false;
    }
    if (in.fps != out.fps)
    {
        cout << "in.fps" << endl;
        return false;
    }
    if (in.width != out.width)
    {
        cout << "in.width" << endl;
        return false;
    }
    if (in.height != out.height)
    {
        cout << "in.height" << endl;
        return false;
    }
    if (in.isOpen != out.isOpen)
    {
        cout << "in.isOpen" << endl;
        return false;
    }
    if (in.custom1 != out.custom1)
    {
        cout << "in.custom1" << endl;
        return false;
    }
    if (in.custom2 != out.custom2)
    {
        cout << "in.custom2" << endl;
        return false;
    }
    if (in.custom3 != out.custom3)
    {
        cout << "in.custom3" << endl;
        return false;
    }

    return true;
}



// Encode/decode test.
bool encodeDecodeTest()
{
    // Prepare random params.
    VSourceParams in;
    in.source = "alsfghljb";
    in.fourcc = "skdfjhvk";
    in.logLevel = rand() % 255;
    in.cycleTimeMks = rand() % 255;
    in.exposure = rand() % 255;
    in.exposureMode = rand() % 255;
    in.gainMode = rand() % 255;
    in.gain = rand() % 255;
    in.focusMode = rand() % 255;
    in.focusPos = rand() % 255;
    in.fps = rand() % 255;
    in.width = rand() % 255;
    in.height = rand() % 255;
    in.isOpen = true;
    in.custom1 = rand() % 255;
    in.custom2 = rand() % 255;
    in.custom3 = rand() % 255;

    // Encode data.
    uint8_t data[1024];
    int size = 0;
    in.encode(data, size);

    cout << "Encoded data size: " << size << " bytes" << endl;

    // Decode data.
    VSourceParams out;
    if (!out.decode(data, size))
    {
        cout << "Can't decode data" << endl;
        return false;
    }

    // Compare params.
    if (out.source != "")
    {
        cout << "in.source" << endl;
        return false;
    }
    if (out.fourcc != "")
    {
        cout << "in.fourcc" << endl;
        return false;
    }
    if (in.logLevel != out.logLevel)
    {
        cout << "in.logLevel" << endl;
        return false;
    }
    if (in.cycleTimeMks != out.cycleTimeMks)
    {
        cout << "in.cycleTimeMks" << endl;
        return false;
    }
    if (in.exposure != out.exposure)
    {
        cout << "in.exposure" << endl;
        return false;
    }
    if (in.exposureMode != out.exposureMode)
    {
        cout << "in.exposureMode" << endl;
        return false;
    }
    if (in.gainMode != out.gainMode)
    {
        cout << "in.gainMode" << endl;
        return false;
    }
    if (in.gain != out.gain)
    {
        cout << "in.gain" << endl;
        return false;
    }
    if (in.focusMode != out.focusMode)
    {
        cout << "in.focusMode" << endl;
        return false;
    }
    if (in.focusPos != out.focusPos)
    {
        cout << "in.focusPos" << endl;
        return false;
    }
    if (in.fps != out.fps)
    {
        cout << "in.fps" << endl;
        return false;
    }
    if (in.width != out.width)
    {
        cout << "in.width" << endl;
        return false;
    }
    if (in.height != out.height)
    {
        cout << "in.height" << endl;
        return false;
    }
    if (in.isOpen != out.isOpen)
    {
        cout << "in.isOpen" << endl;
        return false;
    }
    if (in.custom1 != out.custom1)
    {
        cout << "in.custom1" << endl;
        return false;
    }
    if (in.custom2 != out.custom2)
    {
        cout << "in.custom2" << endl;
        return false;
    }
    if (in.custom3 != out.custom3)
    {
        cout << "in.custom3" << endl;
        return false;
    }

    return true;
}














