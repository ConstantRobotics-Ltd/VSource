#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include "VSource.h"



/// Link namesapces.
using namespace cr::video;
using namespace std;



// Copy test.
bool copyTest();

// Encode/decode test.
bool encodeDecodeTest();

/// Encode/decode test with params mask.
bool encodeDecodeWithMaskTest();

/// Encode/decode commands test.
bool encodeDecodeCommandsTest();

/// JSON read/write test.
bool jsonReadWriteTest();

/// Decode of truncated and invalid data.
bool decodeInvalidDataTest();

/// Encode into buffers of every size.
bool encodeBufferSizeTest();

/// Decode of invalid commands.
bool decodeInvalidCommandsTest();

/// Test value (deterministic sequence in [0, 254]).
int testValue();



// Entry point.
int main(void)
{
    cout << "#####################################" << endl;
    cout << "#                                   #" << endl;
    cout << "# VSourceParams test                #" << endl;
    cout << "#                                   #" << endl;
    cout << "#####################################" << endl;
    cout << endl;

    bool allPassed = true;

    cout << "Copy test:" << endl;
    if (copyTest())
        cout << "OK" << endl;
    else
    {
        cout << "ERROR" << endl;
        allPassed = false;
    }
    cout << endl;

    cout << "Encode/Decode test:" << endl;
    if (encodeDecodeTest())
        cout << "OK" << endl;
    else
    {
        cout << "ERROR" << endl;
        allPassed = false;
    }
    cout << endl;

    cout << "Encode/Decode test with params mask:" << endl;
    if (encodeDecodeWithMaskTest())
        cout << "OK" << endl;
    else
    {
        cout << "ERROR" << endl;
        allPassed = false;
    }
    cout << endl;

    cout << "Encode/Decode commands test:" << endl;
    if (encodeDecodeCommandsTest())
        cout << "OK" << endl;
    else
    {
        cout << "ERROR" << endl;
        allPassed = false;
    }
    cout << endl;

    cout << "JSON read/write test:" << endl;
    if (jsonReadWriteTest())
        cout << "OK" << endl;
    else
    {
        cout << "ERROR" << endl;
        allPassed = false;
    }
    cout << endl;

    cout << "Decode invalid data test:" << endl;
    if (decodeInvalidDataTest())
        cout << "OK" << endl;
    else
    {
        cout << "ERROR" << endl;
        allPassed = false;
    }
    cout << endl;

    cout << "Encode buffer size test:" << endl;
    if (encodeBufferSizeTest())
        cout << "OK" << endl;
    else
    {
        cout << "ERROR" << endl;
        allPassed = false;
    }
    cout << endl;

    cout << "Decode invalid commands test:" << endl;
    if (decodeInvalidCommandsTest())
        cout << "OK" << endl;
    else
    {
        cout << "ERROR" << endl;
        allPassed = false;
    }
    cout << endl;

    return allPassed ? 0 : 1;
}



// Copy test.
bool copyTest()
{
    // Prepare random params.
    VSourceParams in;
    in.source = "alsfghljb";
    in.fourcc = "skdfjhvk";
    in.logLevel = testValue();
    in.cycleTimeMks = testValue();
    in.exposure = testValue();
    in.exposureMode = testValue();
    in.gainMode = testValue();
    in.gain = testValue();
    in.focusMode = testValue();
    in.focusPos = testValue();
    in.fps = static_cast<float>(testValue());
    in.width = testValue();
    in.height = testValue();
    in.isOpen = true;
    in.roiX = testValue();
    in.roiY = testValue();
    in.roiWidth = testValue();
    in.roiHeight = testValue();
    in.custom1 = static_cast<float>(testValue());
    in.custom2 = static_cast<float>(testValue());
    in.custom3 = static_cast<float>(testValue());

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
    if (in.roiX != out.roiX)
    {
        cout << "in.roiX" << endl;
        return false;
    }
    if (in.roiY != out.roiY)
    {
        cout << "in.roiY" << endl;
        return false;
    }
    if (in.roiWidth != out.roiWidth)
    {
        cout << "in.roiWidth" << endl;
        return false;
    }
    if (in.roiHeight != out.roiHeight)
    {
        cout << "in.roiHeight" << endl;
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
    in.source = "source";
    in.fourcc = "skdfjhvk";
    in.logLevel = testValue();
    in.cycleTimeMks = testValue();
    in.exposure = testValue();
    in.exposureMode = testValue();
    in.gainMode = testValue();
    in.gain = testValue();
    in.focusMode = testValue();
    in.focusPos = testValue();
    in.fps = static_cast<float>(testValue());
    in.width = testValue();
    in.height = testValue();
    in.isOpen = true;
    in.roiX = testValue();
    in.roiY = testValue();
    in.roiWidth = testValue();
    in.roiHeight = testValue();
    in.custom1 = static_cast<float>(testValue());
    in.custom2 = static_cast<float>(testValue());
    in.custom3 = static_cast<float>(testValue());

    // Encode data.
    uint8_t data[1024];
    int size = 0;
    in.encode(data, 1024, size);

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
    if (in.roiX != out.roiX)
    {
        cout << "in.roiX" << endl;
        return false;
    }
    if (in.roiY != out.roiY)
    {
        cout << "in.roiY" << endl;
        return false;
    }
    if (in.roiWidth != out.roiWidth)
    {
        cout << "in.roiWidth" << endl;
        return false;
    }
    if (in.roiHeight != out.roiHeight)
    {
        cout << "in.roiHeight" << endl;
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



// Encode/decode commands test.
bool encodeDecodeCommandsTest()
{
    // Encode command.
    uint8_t data[1024];
    int size = 0;
    float outValue = static_cast<float>(testValue() % 20);
    VSource::encodeCommand(data, size, VSourceCommand::RESTART);

    // Decode command.
    VSourceCommand commandId;
    VSourceParam paramId;
    float value = 0.0f;
    if (VSource::decodeCommand(data, size, paramId, commandId, value) != 0)
    {
        cout << "Command not decoded" << endl;
        return false;
    }

    // Checkk ID and value.
    if (commandId != VSourceCommand::RESTART)
    {
        cout << "not a VSourceCommand::RESTART" << endl;
        return false;
    }

    // Encode param.
    outValue = static_cast<float>(testValue() % 20);
    VSource::encodeSetParamCommand(data, size, VSourceParam::EXPOSURE,outValue);

    // Decode command.
    value = 0.0f;
    if (VSource::decodeCommand(data, size, paramId, commandId, value) != 1)
    {
        cout << "Set param command not decoded" << endl;
        return false;
    }

    // Checkk ID and value.
    if (paramId != VSourceParam::EXPOSURE)
    {
        cout << "not a VSourceParam::EXPOSURE" << endl;
        return false;
    }
    if (value != outValue)
    {
        cout << "not equal value" << endl;
        return false;
    }

    return true;
}



/// JSON read/write test.
bool jsonReadWriteTest()
{
    // Prepare random params.
    VSourceParams in;
    in.source = "alsfghljb";
    in.fourcc = "skdfjhvk";
    in.logLevel = testValue();
    in.cycleTimeMks = testValue();
    in.exposure = testValue();
    in.exposureMode = testValue();
    in.gainMode = testValue();
    in.gain = testValue();
    in.focusMode = testValue();
    in.focusPos = testValue();
    in.fps = static_cast<float>(testValue());
    in.width = testValue();
    in.height = testValue();
    in.isOpen = true;
    in.roiX = testValue();
    in.roiY = testValue();
    in.roiWidth = testValue();
    in.roiHeight = testValue();
    in.custom1 = static_cast<float>(testValue());
    in.custom2 = static_cast<float>(testValue());
    in.custom3 = static_cast<float>(testValue());

    // Write params to file.
    cr::utils::ConfigReader inConfig;
    inConfig.set(in, "vSourceParams");
    inConfig.writeToFile("TestVSourceParams.json");

    // Read params from file.
    cr::utils::ConfigReader outConfig;
    if(!outConfig.readFromFile("TestVSourceParams.json"))
    {
        cout << "Can't open config file" << endl;
        return false;
    }

    std::remove("TestVSourceParams.json");

    VSourceParams out;
    if(!outConfig.get(out, "vSourceParams"))
    {
        cout << "Can't read params from file" << endl;
        return false;
    }

    // Compare params.
    bool result = true;
    if (out.source != in.source)
    {
        cout << "in.source" << endl;
        result = false;
    }
    if (out.fourcc != in.fourcc)
    {
        cout << "in.fourcc" << endl;
        result = false;
    }
    if (in.logLevel != out.logLevel)
    {
        cout << "in.logLevel" << endl;
        result = false;
    }
    if (in.exposureMode != out.exposureMode)
    {
        cout << "in.exposureMode" << endl;
        result = false;
    }
    if (in.gainMode != out.gainMode)
    {
        cout << "in.gainMode" << endl;
        result = false;
    }
    if (in.focusMode != out.focusMode)
    {
        cout << "in.focusMode" << endl;
        result = false;
    }
    if (in.fps != out.fps)
    {
        cout << "in.fps" << endl;
        result = false;
    }
    if (in.width != out.width)
    {
        cout << "in.width" << endl;
        result = false;
    }
    if (in.height != out.height)
    {
        cout << "in.height" << endl;
        result = false;
    }
    if (in.roiX != out.roiX)
    {
        cout << "in.roiX" << endl;
        return false;
    }
    if (in.roiY != out.roiY)
    {
        cout << "in.roiY" << endl;
        return false;
    }
    if (in.roiWidth != out.roiWidth)
    {
        cout << "in.roiWidth" << endl;
        return false;
    }
    if (in.roiHeight != out.roiHeight)
    {
        cout << "in.roiHeight" << endl;
        return false;
    }
    if (in.custom1 != out.custom1)
    {
        cout << "in.custom1" << endl;
        result = false;
    }
    if (in.custom2 != out.custom2)
    {
        cout << "in.custom2" << endl;
        result = false;
    }
    if (in.custom3 != out.custom3)
    {
        cout << "in.custom3" << endl;
        result = false;
    }

    return result;
}



/// Encode/decode test with params mask.
bool encodeDecodeWithMaskTest()
{
    // Prepare random params.
    VSourceParams in;
    in.source = "source";
    in.fourcc = "skdfjhvk";
    in.logLevel = testValue();
    in.cycleTimeMks = testValue();
    in.exposure = testValue();
    in.exposureMode = testValue();
    in.gainMode = testValue();
    in.gain = testValue();
    in.focusMode = testValue();
    in.focusPos = testValue();
    in.fps = static_cast<float>(testValue());
    in.width = testValue();
    in.height = testValue();
    in.isOpen = true;
    in.roiX = testValue();
    in.roiY = testValue();
    in.roiWidth = testValue();
    in.roiHeight = testValue();
    in.custom1 = static_cast<float>(testValue());
    in.custom2 = static_cast<float>(testValue());
    in.custom3 = static_cast<float>(testValue());

    // Prepare params mask.
    VSourceParamsMask mask;
    mask.logLevel = true;
    mask.cycleTimeMks = false;
    mask.exposure = true;
    mask.exposureMode = false;
    mask.gainMode = true;
    mask.gain = false;
    mask.focusMode = true;
    mask.focusPos = false;
    mask.fps = true;
    mask.width = false;
    mask.height = true;
    mask.isOpen = false;
    mask.roiX = true;
    mask.roiY = false;
    mask.roiWidth = true;
    mask.roiHeight = false;
    mask.custom1 = true;
    mask.custom2 = false;
    mask.custom3 = true;

    // Encode data.
    uint8_t data[1024];
    int size = 0;
    in.encode(data, 1024, size, &mask);

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
    if (0 != out.cycleTimeMks)
    {
        cout << "in.cycleTimeMks" << endl;
        return false;
    }
    if (in.exposure != out.exposure)
    {
        cout << "in.exposure" << endl;
        return false;
    }
    if (0 != out.exposureMode)
    {
        cout << "in.exposureMode" << endl;
        return false;
    }
    if (in.gainMode != out.gainMode)
    {
        cout << "in.gainMode" << endl;
        return false;
    }
    if (0 != out.gain)
    {
        cout << "in.gain" << endl;
        return false;
    }
    if (in.focusMode != out.focusMode)
    {
        cout << "in.focusMode" << endl;
        return false;
    }
    if (0 != out.focusPos)
    {
        cout << "in.focusPos" << endl;
        return false;
    }
    if (in.fps != out.fps)
    {
        cout << "in.fps" << endl;
        return false;
    }
    if (0 != out.width)
    {
        cout << "in.width" << endl;
        return false;
    }
    if (in.height != out.height)
    {
        cout << "in.height" << endl;
        return false;
    }
    if (false != out.isOpen)
    {
        cout << "in.isOpen" << endl;
        return false;
    }

    if (in.roiX != out.roiX)
    {
        cout << "in.roiX" << endl;
        return false;
    }
    if (0 != out.roiY)
    {
        cout << "in.roiY" << endl;
        return false;
    }
    if (in.roiWidth != out.roiWidth)
    {
        cout << "in.roiWidth" << endl;
        return false;
    }
    if (0 != out.roiHeight)
    {
        cout << "in.roiHeight" << endl;
        return false;
    }
    if (in.custom1 != out.custom1)
    {
        cout << "in.custom1" << endl;
        return false;
    }
    if (0.0f != out.custom2)
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



int testValue()
{
    // Deterministic sequence that visits every value of [0, 254].
    static int value = 0;
    value = (value + 97) % 255;
    return value;
}



/// Params with test values.
static VSourceParams makeTestParams()
{
    VSourceParams in;
    in.logLevel = 1;
    in.width = 1920;
    in.height = 1080;
    in.gainMode = 2;
    in.gain = 3;
    in.exposureMode = 4;
    in.exposure = 5;
    in.focusMode = 6;
    in.focusPos = 7;
    in.cycleTimeMks = 8;
    in.fps = 9.5f;
    in.isOpen = true;
    in.roiX = 10;
    in.roiY = 11;
    in.roiWidth = 12;
    in.roiHeight = 13;
    in.custom1 = 14.5f;
    in.custom2 = 15.5f;
    in.custom3 = 16.5f;
    return in;
}



/// Compare the encoded fields of two params objects.
static bool sameParams(const VSourceParams& a, const VSourceParams& b)
{
    return a.logLevel == b.logLevel && a.width == b.width &&
           a.height == b.height && a.gainMode == b.gainMode &&
           a.gain == b.gain && a.exposureMode == b.exposureMode &&
           a.exposure == b.exposure && a.focusMode == b.focusMode &&
           a.focusPos == b.focusPos && a.cycleTimeMks == b.cycleTimeMks &&
           a.fps == b.fps && a.isOpen == b.isOpen && a.roiX == b.roiX &&
           a.roiY == b.roiY && a.roiWidth == b.roiWidth &&
           a.roiHeight == b.roiHeight && a.custom1 == b.custom1 &&
           a.custom2 == b.custom2 && a.custom3 == b.custom3;
}



/// Decode of truncated and invalid data.
bool decodeInvalidDataTest()
{
    VSourceParams in = makeTestParams();
    uint8_t data[1024];
    int size = 0;
    if (!in.encode(data, 1024, size) || size != 79)
    {
        cout << "Encoded size " << size << endl;
        return false;
    }

    // Every truncated copy (in its own buffer of exactly that size) is
    // rejected and does not change the params.
    for (int length = 0; length < size; ++length)
    {
        std::vector<uint8_t> part(data, data + length);
        VSourceParams out;
        out.width = 777;
        out.source = "keep";
        if (out.decode(part.empty() ? nullptr : part.data(), length))
        {
            cout << "Truncated data decoded, length " << length << endl;
            return false;
        }
        if (out.width != 777 || out.source != "keep")
        {
            cout << "Params changed by invalid data" << endl;
            return false;
        }
    }

    // No data, wrong header, wrong version.
    VSourceParams out;
    if (out.decode(nullptr, 79))
    {
        cout << "Null data decoded" << endl;
        return false;
    }
    uint8_t bad[1024];
    for (int byte = 0; byte < 3; ++byte)
    {
        memcpy(bad, data, static_cast<size_t>(size));
        bad[byte] = static_cast<uint8_t>(bad[byte] + 1);
        if (out.decode(bad, size))
        {
            cout << "Invalid header decoded, byte " << byte << endl;
            return false;
        }
    }

    // The complete data is decoded.
    if (!out.decode(data, size) || !sameParams(in, out))
    {
        cout << "Valid data not decoded" << endl;
        return false;
    }

    return true;
}



/// Encode into buffers of every size.
bool encodeBufferSizeTest()
{
    VSourceParams in = makeTestParams();
    VSourceParamsMask mask;
    mask.gain = false;
    mask.fps = false;
    mask.custom2 = false;

    for (int withMask = 0; withMask < 2; ++withMask)
    {
        for (int bufferSize = 0; bufferSize <= 90; ++bufferSize)
        {
            // Guard bytes behind the buffer must not be written.
            std::vector<uint8_t> buffer(static_cast<size_t>(bufferSize) + 16, 0xA5);
            int size = -1;
            const bool result = in.encode(buffer.data(), bufferSize, size,
                                          withMask == 1 ? &mask : nullptr);
            for (size_t i = static_cast<size_t>(bufferSize); i < buffer.size(); ++i)
            {
                if (buffer[i] != 0xA5)
                {
                    cout << "Written beyond the buffer, size " << bufferSize << endl;
                    return false;
                }
            }
            if (bufferSize < 7)
            {
                if (result || size != 0)
                {
                    cout << "Small buffer accepted " << bufferSize << endl;
                    return false;
                }
                continue;
            }
            if (!result || size < 6 || size > bufferSize)
            {
                cout << "Wrong size " << size << " for buffer " << bufferSize << endl;
                return false;
            }
            // Every encoded field is decoded, every other field is 0.
            VSourceParams out;
            if (!out.decode(buffer.data(), size))
            {
                cout << "Encoded data not decoded, buffer " << bufferSize << endl;
                return false;
            }
            // With the full buffer all selected fields are present.
            if (bufferSize >= 79 && withMask == 0 && !sameParams(in, out))
            {
                cout << "Full buffer: params differ" << endl;
                return false;
            }
        }
    }

    // The data with mask fits exactly into a buffer of its size.
    uint8_t exact[128];
    int fullSize = 0;
    in.encode(exact, 128, fullSize, &mask);
    int size = 0;
    if (!in.encode(exact, fullSize, size, &mask) || size != fullSize)
    {
        cout << "Exact buffer: size " << size << " instead of " << fullSize << endl;
        return false;
    }

    // No buffer.
    size = -1;
    if (in.encode(nullptr, 100, size) || size != 0)
    {
        cout << "Null buffer accepted" << endl;
        return false;
    }

    return true;
}



/// Decode of invalid commands.
bool decodeInvalidCommandsTest()
{
    uint8_t data[64];
    int size = 0;
    VSourceParam paramId = VSourceParam::WIDTH;
    VSourceCommand commandId = VSourceCommand::RESTART;
    float value = 0.0f;

    // A set param command with a wrong size is an error (not a command).
    VSource::encodeSetParamCommand(data, size, VSourceParam::FPS, 30.0f);
    for (int wrongSize : {7, 8, 9, 10, 12, 20})
    {
        if (VSource::decodeCommand(data, wrongSize, paramId, commandId, value) != -1)
        {
            cout << "Set param command of size " << wrongSize << " accepted" << endl;
            return false;
        }
    }
    if (VSource::decodeCommand(data, size, paramId, commandId, value) != 1 ||
        paramId != VSourceParam::FPS || value != 30.0f)
    {
        cout << "Set param command not decoded" << endl;
        return false;
    }

    // Short data, no data, unknown type, wrong version.
    for (int shortSize = 0; shortSize < 7; ++shortSize)
    {
        if (VSource::decodeCommand(data, shortSize, paramId, commandId, value) != -1)
        {
            cout << "Short command accepted" << endl;
            return false;
        }
    }
    if (VSource::decodeCommand(nullptr, 11, paramId, commandId, value) != -1)
    {
        cout << "Null command accepted" << endl;
        return false;
    }
    data[0] = 0x05;
    if (VSource::decodeCommand(data, 11, paramId, commandId, value) != -1)
    {
        cout << "Unknown command type accepted" << endl;
        return false;
    }
    VSource::encodeCommand(data, size, VSourceCommand::RESTART);
    data[1] = static_cast<uint8_t>(data[1] + 1);
    if (VSource::decodeCommand(data, size, paramId, commandId, value) != -1)
    {
        cout << "Wrong version accepted" << endl;
        return false;
    }

    // No buffer for encoding.
    size = 5;
    VSource::encodeCommand(nullptr, size, VSourceCommand::RESTART);
    if (size != 0)
        return false;
    size = 5;
    VSource::encodeSetParamCommand(nullptr, size, VSourceParam::FPS, 1.0f);
    if (size != 0)
        return false;

    return true;
}
