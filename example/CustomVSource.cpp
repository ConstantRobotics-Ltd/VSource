#include "CustomVSource.h"
#include "CustomVSourceVersion.h"



namespace
{

/**
 * @brief Convert a parameter value to int. The value comes from the user or
 * from a decoded command: a value that is not a number or does not fit in an
 * int is rejected (its conversion would be undefined).
 * @param value Parameter value.
 * @param result Converted value (not changed if the value is rejected).
 * @return TRUE if converted or FALSE if not.
 */
bool toIntParam(float value, int& result)
{
    // -2^31 and 2^31 are exact float values.
    if (!(value >= -2147483648.0f && value < 2147483648.0f))
        return false;
    result = static_cast<int>(value);
    return true;
}

}



cr::video::CustomVSource::CustomVSource()
{
    // Reset open flag.
    m_params.isOpen = false;

    // Init default output frame.
    m_outputFrame = Frame(1280, 720, Fourcc::BGR24);
}



cr::video::CustomVSource::~CustomVSource()
{

}



std::string cr::video::CustomVSource::getVersion()
{
    return CUSTOM_VSOURCE_VERSION;
}



bool cr::video::CustomVSource::openVSource(std::string& initString)
{
    // Copy init string.
    m_params.source = initString;

    // Set open flag.
    m_params.isOpen = true;

    return true;
}



bool cr::video::CustomVSource::initVSource(VSourceParams& params)
{
    // Copy params.
    m_params = params;

    // Set open flag.
    m_params.isOpen = true;

    return true;
}



bool cr::video::CustomVSource::isVSourceOpen()
{
    return m_params.isOpen;
}



void cr::video::CustomVSource::closeVSource()
{
    // Reset open flag.
    m_params.isOpen = false;
}



bool cr::video::CustomVSource::getFrame(Frame& frame,
                                        [[maybe_unused]] int32_t timeoutMsec)
{
    // Copy frame.
    frame = m_outputFrame;

    return true;
}



bool cr::video::CustomVSource::setParam(VSourceParam id, float value)
{
    // Check param ID.
    switch (id)
    {
    case cr::video::VSourceParam::LOG_LEVEL:
    {
        return toIntParam(value, m_params.logLevel);
    }
    case cr::video::VSourceParam::WIDTH:
    {
        return toIntParam(value, m_params.width);
    }
    case cr::video::VSourceParam::HEIGHT:
    {
        return toIntParam(value, m_params.height);
    }
    case cr::video::VSourceParam::GAIN_MODE:
    {
        return toIntParam(value, m_params.gainMode);
    }
    case cr::video::VSourceParam::GAIN:
    {
        return toIntParam(value, m_params.gain);
    }
    case cr::video::VSourceParam::EXPOSURE_MODE:
    {
        return toIntParam(value, m_params.exposureMode);
    }
    case cr::video::VSourceParam::EXPOSURE:
    {
        return toIntParam(value, m_params.exposure);
    }
    case cr::video::VSourceParam::FOCUS_MODE:
    {
        return toIntParam(value, m_params.focusMode);
    }
    case cr::video::VSourceParam::FOCUS_POS:
    {
        return toIntParam(value, m_params.focusPos);
    }
    case cr::video::VSourceParam::CYCLE_TIME_MKS:
    {
        // Read only param.
        return false;
    }
    case cr::video::VSourceParam::FPS:
    {
        m_params.fps = value;
        return true;
    }
    case cr::video::VSourceParam::IS_OPEN:
    {
        // Read only param.
        return false;
    }
    case cr::video::VSourceParam::ROI_X:
    {
        return toIntParam(value, m_params.roiX);
    }
    case cr::video::VSourceParam::ROI_Y:
    {
        return toIntParam(value, m_params.roiY);
    }
    case cr::video::VSourceParam::ROI_WIDTH:
    {
        return toIntParam(value, m_params.roiWidth);
    }
    case cr::video::VSourceParam::ROI_HEIGHT:
    {
        return toIntParam(value, m_params.roiHeight);
    }
    case cr::video::VSourceParam::CUSTOM_1:
    {
        m_params.custom1 = value;
        return true;
    }
    case cr::video::VSourceParam::CUSTOM_2:
    {
        m_params.custom2 = value;
        return true;
    }
    case cr::video::VSourceParam::CUSTOM_3:
    {
        m_params.custom3 = value;
        return true;
    }
    default:
    {
        return false;
    }
    }
}



float cr::video::CustomVSource::getParam(VSourceParam id)
{
    // Check param ID.
    switch (id)
    {
    case cr::video::VSourceParam::LOG_LEVEL:
    {
        return static_cast<float>(m_params.logLevel);
    }
    case cr::video::VSourceParam::WIDTH:
    {
        return static_cast<float>(m_params.width);
    }
    case cr::video::VSourceParam::HEIGHT:
    {
        return static_cast<float>(m_params.height);
    }
    case cr::video::VSourceParam::GAIN_MODE:
    {
        return static_cast<float>(m_params.gainMode);
    }
    case cr::video::VSourceParam::GAIN:
    {
        return static_cast<float>(m_params.gain);
    }
    case cr::video::VSourceParam::EXPOSURE_MODE:
    {
        return static_cast<float>(m_params.exposureMode);
    }
    case cr::video::VSourceParam::EXPOSURE:
    {
        return static_cast<float>(m_params.exposure);
    }
    case cr::video::VSourceParam::FOCUS_MODE:
    {
        return static_cast<float>(m_params.focusMode);
    }
    case cr::video::VSourceParam::FOCUS_POS:
    {
        return static_cast<float>(m_params.focusPos);
    }
    case cr::video::VSourceParam::CYCLE_TIME_MKS:
    {
        return static_cast<float>(m_params.cycleTimeMks);
    }
    case cr::video::VSourceParam::FPS:
    {
        return m_params.fps;
    }
    case cr::video::VSourceParam::IS_OPEN:
    {
        return m_params.isOpen ? 1.0f : 0.0f;
    }
    case cr::video::VSourceParam::ROI_X:
    {
        return static_cast<float>(m_params.roiX);
    }
    case cr::video::VSourceParam::ROI_Y:
    {
        return static_cast<float>(m_params.roiY);
    }
    case cr::video::VSourceParam::ROI_WIDTH:
    {
        return static_cast<float>(m_params.roiWidth);
    }
    case cr::video::VSourceParam::ROI_HEIGHT:
    {
        return static_cast<float>(m_params.roiHeight);
    }
    case cr::video::VSourceParam::CUSTOM_1:
    {
        return m_params.custom1;
    }
    case cr::video::VSourceParam::CUSTOM_2:
    {
        return m_params.custom2;
    }
    case cr::video::VSourceParam::CUSTOM_3:
    {
        return m_params.custom3;
    }
    default:
    {
        return -1.0f;
    }
    }
}



void cr::video::CustomVSource::getParams(VSourceParams& params)
{
    params = m_params;
}



bool cr::video::CustomVSource::executeCommand(VSourceCommand id)
{
    // Check command ID.
    switch (id)
    {
    case cr::video::VSourceCommand::RESTART:
    {
        return true;
    }
    default:
    {
        return false;
    }
    }
}



bool cr::video::CustomVSource::decodeAndExecuteCommand(uint8_t* data, int size)
{
    // Decode command.
    VSourceCommand commandId = VSourceCommand::RESTART;
    VSourceParam paramId = VSourceParam::CUSTOM_1;
    float value = 0.0f;
    switch (VSource::decodeCommand(data, size, paramId, commandId, value))
    {
    // COMMAND.
    case 0:
        // Executte command.
        return executeCommand(commandId);
    // SET_PARAM.
    case 1:
    {
        // Set param.
        return setParam(paramId, value);
    }
    default:
    {
        return false;
    }
    }
}