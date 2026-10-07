#include "VSource.h"
#include "VSourceVersion.h"



// Link namespaces.
using namespace std;
using namespace cr::video;



VSource::~VSource()
{

}



std::string VSource::getVersion()
{
    return VSOURCE_VERSION;
}



VSourceParams &VSourceParams::operator= (const VSourceParams &src)
{
    // Check yourself.
    if (this == &src)
        return *this;

    // Copy params.
    logLevel = src.logLevel;
    source = src.source;
    fourcc = src.fourcc;
    width = src.width;
    height = src.height;
    gainMode = src.gainMode;
    gain = src.gain;
    exposureMode = src.exposureMode;
    exposure = src.exposure;
    focusMode = src.focusMode;
    focusPos = src.focusPos;
    cycleTimeMks = src.cycleTimeMks;
    fps = src.fps;
    isOpen = src.isOpen;
    roiX = src.roiX;
    roiY = src.roiY;
    roiWidth = src.roiWidth;
    roiHeight = src.roiHeight;
    custom1 = src.custom1;
    custom2 = src.custom2;
    custom3 = src.custom3;

    return *this;
}



bool VSourceParams::encode(uint8_t* data, int bufferSize, int& size,
                           VSourceParamsMask* mask)
{
    // Check buffer.
    if (data == nullptr || bufferSize < 7)
    {
        size = 0;
        return false;
    }

    int pos = 0;
    // Header.
    data[0] = 0x02;
    // Encode version.
    data[1] = VSOURCE_MAJOR_VERSION;
    data[2] = VSOURCE_MINOR_VERSION;
    // Prepare mask. Each flag is set after param is copied to buffer.
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    pos = 6;

    if (mask == nullptr)
    {    
        // Encode data.
        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }
        memcpy(&data[pos], &logLevel, 4); pos += 4;
        data[3] = data[3] | static_cast<uint8_t>(128);

        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }
        memcpy(&data[pos], &width, 4); pos += 4;
        data[3] = data[3] | static_cast<uint8_t>(64);
        
        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }
        memcpy(&data[pos], &height, 4); pos += 4;
        data[3] = data[3] | static_cast<uint8_t>(32);

        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }
        memcpy(&data[pos], &gainMode, 4); pos += 4;
        data[3] = data[3] | static_cast<uint8_t>(16);

        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }
        memcpy(&data[pos], &gain, 4); pos += 4;
        data[3] = data[3] | static_cast<uint8_t>(8);

        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }
        memcpy(&data[pos], &exposureMode, 4); pos += 4;
        data[3] = data[3] | static_cast<uint8_t>(4);

        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }
        memcpy(&data[pos], &exposure, 4); pos += 4;
        data[3] = data[3] | static_cast<uint8_t>(2);

        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }
        memcpy(&data[pos], &focusMode, 4); pos += 4;
        data[3] = data[3] | static_cast<uint8_t>(1);

        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }
        memcpy(&data[pos], &focusPos, 4); pos += 4;
        data[4] = data[4] | static_cast<uint8_t>(128);

        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }
        memcpy(&data[pos], &cycleTimeMks, 4); pos += 4;
        data[4] = data[4] | static_cast<uint8_t>(64);

        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }
        memcpy(&data[pos], &fps, 4); pos += 4;
        data[4] = data[4] | static_cast<uint8_t>(32);

        if(bufferSize < pos + 1)
        {
            size = pos;
            return true;
        }
        data[pos] = isOpen == true ? 0x01 : 0x00; pos += 1;
        data[4] = data[4] | static_cast<uint8_t>(16);

        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }
        memcpy(&data[pos], &roiX, 4); pos += 4;
        data[4] = data[4] | static_cast<uint8_t>(8);

        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }
        memcpy(&data[pos], &roiY, 4); pos += 4;
        data[4] = data[4] | static_cast<uint8_t>(4);

        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }
        memcpy(&data[pos], &roiWidth, 4); pos += 4;
        data[4] = data[4] | static_cast<uint8_t>(2);

        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }    
        memcpy(&data[pos], &roiHeight, 4); pos += 4;
        data[4] = data[4] | static_cast<uint8_t>(1);

        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }   
        memcpy(&data[pos], &custom1, 4); pos += 4;
        data[5] = data[5] | static_cast<uint8_t>(128);

        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }   
        memcpy(&data[pos], &custom2, 4); pos += 4;
        data[5] = data[5] | static_cast<uint8_t>(64);

        if(bufferSize < pos + 4)
        {
            size = pos;
            return true;
        }   
        memcpy(&data[pos], &custom3, 4); pos += 4;
        data[5] = data[5] | static_cast<uint8_t>(32);

        size = pos;

        return true;
    }


    if (mask->logLevel && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &logLevel, 4); pos += 4;
        data[3] = data[3] | static_cast<uint8_t>(128);
    }

    if (mask->width && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &width, 4); pos += 4;
        data[3] = data[3] | static_cast<uint8_t>(64);
    }

    if (mask->height && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &height, 4); pos += 4;
        data[3] = data[3] | static_cast<uint8_t>(32);
    }

    if (mask->gainMode && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &gainMode, 4); pos += 4;
        data[3] = data[3] | static_cast<uint8_t>(16);
    }

    if (mask->gain && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &gain, 4); pos += 4;
        data[3] = data[3] | static_cast<uint8_t>(8);
    }

    if (mask->exposureMode && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &exposureMode, 4); pos += 4;
        data[3] = data[3] | static_cast<uint8_t>(4);
    }

    if (mask->exposure && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &exposure, 4); pos += 4;
        data[3] = data[3] | static_cast<uint8_t>(2);
    }

    if (mask->focusMode && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &focusMode, 4); pos += 4;
        data[3] = data[3] | static_cast<uint8_t>(1);
    }

    if (mask->focusPos && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &focusPos, 4); pos += 4;
        data[4] = data[4] | static_cast<uint8_t>(128);
    }

    if (mask->cycleTimeMks && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &cycleTimeMks, 4); pos += 4;
        data[4] = data[4] | static_cast<uint8_t>(64);
    }

    if (mask->fps && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &fps, 4); pos += 4;
        data[4] = data[4] | static_cast<uint8_t>(32);
    }

    if (mask->isOpen && (bufferSize >= pos + 1))
    {
        data[pos] = isOpen == true ? 0x01 : 0x00; pos += 1;
        data[4] = data[4] | static_cast<uint8_t>(16);
    }

    if (mask->roiX && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &roiX, 4); pos += 4;
        data[4] = data[4] | static_cast<uint8_t>(8);
    }

    if (mask->roiY && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &roiY, 4); pos += 4;
        data[4] = data[4] | static_cast<uint8_t>(4);
    }

    if (mask->roiWidth && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &roiWidth, 4); pos += 4;
        data[4] = data[4] | static_cast<uint8_t>(2);
    }

    if (mask->roiHeight && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &roiHeight, 4); pos += 4;
        data[4] = data[4] | static_cast<uint8_t>(1);
    }

    if (mask->custom1 && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &custom1, 4); pos += 4;
        data[5] = data[5] | static_cast<uint8_t>(128);
    }

    if (mask->custom2 && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &custom2, 4); pos += 4;
        data[5] = data[5] | static_cast<uint8_t>(64);
    }

    if (mask->custom3 && (bufferSize >= pos + 4))
    {
        memcpy(&data[pos], &custom3, 4); pos += 4;
        data[5] = data[5] | static_cast<uint8_t>(32);
    }

    size = pos;

    return true;
}



bool VSourceParams::decode(uint8_t* data, int dataSize)
{
    // Check data.
    if (data == nullptr || dataSize < 6)
        return false;

    // Check header.
    if (data[0] != 0x02)
        return false;

    // Decode version.
    if (data[1] != VSOURCE_MAJOR_VERSION)
        return false;
    if (data[2] != VSOURCE_MINOR_VERSION)
        return false;

    // The params are decoded into a copy: the object is not changed if the
    // data is not valid.
    VSourceParams params;

    // Position where actual data stars after masks.
    int pos = 6;

    if ((data[3] & static_cast<uint8_t>(128)) == static_cast<uint8_t>(128))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.logLevel, &data[pos], 4); pos += 4;
    }
    else
    {
        params.logLevel = 0;
    }
    if ((data[3] & static_cast<uint8_t>(64)) == static_cast<uint8_t>(64))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.width, &data[pos], 4); pos += 4;
    }
    else
    {
        params.width = 0;
    }
    if ((data[3] & static_cast<uint8_t>(32)) == static_cast<uint8_t>(32))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.height, &data[pos], 4); pos += 4;
    }
    else
    {
        params.height = 0;
    }
    if ((data[3] & static_cast<uint8_t>(16)) == static_cast<uint8_t>(16))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.gainMode, &data[pos], 4); pos += 4;
    }
    else
    {
        params.gainMode = 0;
    }
    if ((data[3] & static_cast<uint8_t>(8)) == static_cast<uint8_t>(8))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.gain, &data[pos], 4); pos += 4;
    }
    else
    {
        params.gain = 0;
    }
    if ((data[3] & static_cast<uint8_t>(4)) == static_cast<uint8_t>(4))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.exposureMode, &data[pos], 4); pos += 4;
    }
    else
    {
        params.exposureMode = 0;
    }
    if ((data[3] & static_cast<uint8_t>(2)) == static_cast<uint8_t>(2))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.exposure, &data[pos], 4); pos += 4;
    }
    else
    {
        params.exposure = 0;
    }
    if ((data[3] & static_cast<uint8_t>(1)) == static_cast<uint8_t>(1))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.focusMode, &data[pos], 4); pos += 4;
    }
    else
    {
        params.focusMode = 0;
    }


    if ((data[4] & static_cast<uint8_t>(128)) == static_cast<uint8_t>(128))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.focusPos, &data[pos], 4); pos += 4;
    }
    else
    {
        params.focusPos = 0;
    }
    if ((data[4] & static_cast<uint8_t>(64)) == static_cast<uint8_t>(64))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.cycleTimeMks, &data[pos], 4); pos += 4;
    }
    else
    {
        params.cycleTimeMks = 0;
    }
    if ((data[4] & static_cast<uint8_t>(32)) == static_cast<uint8_t>(32))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.fps, &data[pos], 4); pos += 4;
    }
    else
    {
        params.fps = 0.0f;
    }
    if ((data[4] & static_cast<uint8_t>(16)) == static_cast<uint8_t>(16))
    {
        if (dataSize < pos + 1)
            return false;
        params.isOpen = data[pos] == 0x00 ? false : true; pos += 1;
    }
    else
    {
        params.isOpen = false;
    }
    if ((data[4] & static_cast<uint8_t>(8)) == static_cast<uint8_t>(8))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.roiX, &data[pos], 4); pos += 4;
    }
    else
    {
        params.roiX = 0;
    }
    if ((data[4] & static_cast<uint8_t>(4)) == static_cast<uint8_t>(4))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.roiY, &data[pos], 4); pos += 4;
    }
    else
    {
        params.roiY = 0;
    }
    if ((data[4] & static_cast<uint8_t>(2)) == static_cast<uint8_t>(2))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.roiWidth, &data[pos], 4); pos += 4;
    }
    else
    {
        params.roiWidth = 0;
    }  
    if ((data[4] & static_cast<uint8_t>(1)) == static_cast<uint8_t>(1))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.roiHeight, &data[pos], 4); pos += 4;
    }
    else
    {
        params.roiHeight = 0;
    }  
    if ((data[5] & static_cast<uint8_t>(128)) == static_cast<uint8_t>(128))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.custom1, &data[pos], 4); pos += 4;
    }
    else
    {
        params.custom1 = 0.0f;
    }
    if ((data[5] & static_cast<uint8_t>(64)) == static_cast<uint8_t>(64))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.custom2, &data[pos], 4); pos += 4;
    }
    else
    {
        params.custom2 = 0.0f;
    }
    if ((data[5] & static_cast<uint8_t>(32)) == static_cast<uint8_t>(32))
    {
        if (dataSize < pos + 4)
            return false;
        memcpy(&params.custom3, &data[pos], 4);
    }
    else
    {
        params.custom3 = 0.0f;
    }

    params.source = "";
    params.fourcc = "";
    *this = params;

    return true;
}



void cr::video::VSource::encodeSetParamCommand(
        uint8_t* data, int& size, VSourceParam id, float value)
{
    // Check buffer.
    if (data == nullptr)
    {
        size = 0;
        return;
    }

    // Fill header.
    data[0] = 0x01;
    data[1] = VSOURCE_MAJOR_VERSION;
    data[2] = VSOURCE_MINOR_VERSION;

    // Fill data.
    int paramId = static_cast<int>(id);
    memcpy(&data[3], &paramId, 4);
    memcpy(&data[7], &value, 4);
    size = 11;
}



void cr::video::VSource::encodeCommand(uint8_t* data,
                                   int& size,
                                   cr::video::VSourceCommand id)
{
    // Check buffer.
    if (data == nullptr)
    {
        size = 0;
        return;
    }

    // Fill header.
    data[0] = 0x00;
    data[1] = VSOURCE_MAJOR_VERSION;
    data[2] = VSOURCE_MINOR_VERSION;

    // Fill data.
    int commandId = static_cast<int>(id);
    memcpy(&data[3], &commandId, 4);
    size = 7;
}



int cr::video::VSource::decodeCommand(uint8_t* data,
                                  int size,
                                  cr::video::VSourceParam& paramId,
                                  cr::video::VSourceCommand& commandId,
                                  float& value)
{
    // Check data and size.
    if (data == nullptr || size < 7)
        return -1;

    // Check version.
    if (data[1] != VSOURCE_MAJOR_VERSION || data[2] != VSOURCE_MINOR_VERSION)
        return -1;

    // Extract data.
    int id = 0;
    memcpy(&id, &data[3], 4);
    value = 0.0f;


    // Check command type.
    if (data[0] == 0x00)
    {
        commandId = static_cast<VSourceCommand>(id);
        return 0;
    }
    else if (data[0] == 0x01)
    {
        // Check size.
        if (size != 11)
            return -1;

        paramId = static_cast<VSourceParam>(id);
        memcpy(&value, &data[7], 4);
        return 1;
    }

    return -1;
}