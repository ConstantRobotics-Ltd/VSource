#include "CustomVSource.h"
#include "CustomVSourceVersion.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <new>
#include <string>
#include <utility>
#include "Frame.h"
#include "VSource.h"



namespace
{
/// Limit of float values which can be converted to int. 2^31 is exactly
/// representable as float, so (-LIMIT <= value < LIMIT) is the exact range of
/// int.
constexpr float INT_RANGE_LIMIT{2147483648.0f};
/// Width of the output frame, pixels.
constexpr int OUTPUT_FRAME_WIDTH{1280};
/// Height of the output frame, pixels.
constexpr int OUTPUT_FRAME_HEIGHT{720};
/// Maximum frame width, height and region of interest size, pixels (16K).
constexpr int MAX_FRAME_SIZE{16384};
/// Maximum value of gain, exposure and focus position (see VSourceParam).
constexpr int MAX_LEVEL{65535};
/// Maximum value of the logging mode (see VSourceParam::LOG_LEVEL).
constexpr int MAX_LOG_LEVEL{3};
/// Maximum FPS.
constexpr float MAX_FPS{1000.0f};
/// Maximum length of the source string, bytes (a device, a file name or a
/// URL). The limit bounds the memory spent on a string that comes from outside.
constexpr size_t MAX_SOURCE_LENGTH{4096};
/// Maximum length of the FOURCC string, bytes.
constexpr size_t MAX_FOURCC_LENGTH{16};



/// Convert a param value to int. A conversion of NaN or of a value outside of
/// the int range is undefined behavior, so such values are rejected.
bool toInt(float value, int& result)
{
    // The comparison is false for NaN: NaN is rejected too.
    if (!(value >= -INT_RANGE_LIMIT && value < INT_RANGE_LIMIT))
    {
        return false;
    }
    result = static_cast<int>(value);
    return true;
}



/// Check that the value is inside [minValue, maxValue].
bool isInRange(int value, int minValue, int maxValue)
{
    return value >= minValue && value <= maxValue;
}



/// Convert a param value to int and assign it to the target if it is inside
/// [minValue, maxValue]. The target is not changed if the value is rejected.
bool assignInRange(int& target, float value, int minValue, int maxValue)
{
    int converted = 0;
    if (!toInt(value, converted) || !isInRange(converted, minValue, maxValue))
    {
        return false;
    }
    target = converted;
    return true;
}



/// Check FPS value: a number inside [0, MAX_FPS].
bool isValidFps(float value)
{
    // The comparison is false for NaN: NaN is rejected too.
    return value >= 0.0f && value <= MAX_FPS;
}



/// Check value of a custom param: any finite number.
bool isValidCustom(float value)
{
    return std::isfinite(value);
}



/// Check params. The video source has to validate params that come from
/// outside before use: VSourceParams transports values without a range check.
bool isValidParams(const cr::video::VSourceParams& params)
{
    return isInRange(params.logLevel, 0, MAX_LOG_LEVEL) &&
           params.source.size() <= MAX_SOURCE_LENGTH &&
           params.fourcc.size() <= MAX_FOURCC_LENGTH &&
           isInRange(params.width, 1, MAX_FRAME_SIZE) &&
           isInRange(params.height, 1, MAX_FRAME_SIZE) &&
           isInRange(params.gainMode, 0, 1) &&
           isInRange(params.gain, 0, MAX_LEVEL) &&
           isInRange(params.exposureMode, 0, 1) &&
           isInRange(params.exposure, 0, MAX_LEVEL) &&
           isInRange(params.focusMode, 0, 1) &&
           isInRange(params.focusPos, 0, MAX_LEVEL) &&
           isValidFps(params.fps) &&
           isInRange(params.roiX, 0, MAX_FRAME_SIZE) &&
           isInRange(params.roiY, 0, MAX_FRAME_SIZE) &&
           isInRange(params.roiWidth, 0, MAX_FRAME_SIZE) &&
           isInRange(params.roiHeight, 0, MAX_FRAME_SIZE) &&
           isValidCustom(params.custom1) &&
           isValidCustom(params.custom2) &&
           isValidCustom(params.custom3);
}
}



cr::video::CustomVSource::CustomVSource() :
    m_outputFrame(OUTPUT_FRAME_WIDTH, OUTPUT_FRAME_HEIGHT, Fourcc::BGR24)
{

}



cr::video::CustomVSource::~CustomVSource() = default;



std::string cr::video::CustomVSource::getVersion()
{
    return CUSTOM_VSOURCE_VERSION;
}



bool cr::video::CustomVSource::openVSource(std::string& initString)
{
    // Check init string.
    if (initString.size() > MAX_SOURCE_LENGTH)
    {
        return false;
    }

    try
    {
        // All params are set by default, the source is the init string.
        VSourceParams params;
        params.source = initString;
        params.isOpen = true;

        const std::lock_guard<std::mutex> lock(m_mutex);
        m_params = std::move(params);
    }
    catch (const std::bad_alloc&)
    {
        return false;
    }

    return true;
}



bool cr::video::CustomVSource::initVSource(VSourceParams& params)
{
    // Check params.
    if (!isValidParams(params))
    {
        return false;
    }

    try
    {
        // Copy params. Cycle time is a read only param.
        VSourceParams newParams = params;
        newParams.cycleTimeMks = 0;
        newParams.isOpen = true;

        const std::lock_guard<std::mutex> lock(m_mutex);
        m_params = std::move(newParams);
    }
    catch (const std::bad_alloc&)
    {
        return false;
    }

    return true;
}



bool cr::video::CustomVSource::isVSourceOpen()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    return m_params.isOpen;
}



void cr::video::CustomVSource::closeVSource()
{
    // Reset open flag.
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_params.isOpen = false;
}



bool cr::video::CustomVSource::getFrame(Frame& frame, int32_t)
{
    // The output frame is available while the video source is open.
    const std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_params.isOpen)
    {
        return false;
    }

    // Copy frame.
    try
    {
        frame = m_outputFrame;
    }
    catch (const std::bad_alloc&)
    {
        return false;
    }

    return true;
}



bool cr::video::CustomVSource::setParam(VSourceParam id, float value)
{
    const std::lock_guard<std::mutex> lock(m_mutex);

    // Check param ID.
    switch (id)
    {
    case VSourceParam::LOG_LEVEL:
    {
        return assignInRange(m_params.logLevel, value, 0, MAX_LOG_LEVEL);
    }
    case VSourceParam::WIDTH:
    {
        return assignInRange(m_params.width, value, 1, MAX_FRAME_SIZE);
    }
    case VSourceParam::HEIGHT:
    {
        return assignInRange(m_params.height, value, 1, MAX_FRAME_SIZE);
    }
    case VSourceParam::GAIN_MODE:
    {
        return assignInRange(m_params.gainMode, value, 0, 1);
    }
    case VSourceParam::GAIN:
    {
        return assignInRange(m_params.gain, value, 0, MAX_LEVEL);
    }
    case VSourceParam::EXPOSURE_MODE:
    {
        return assignInRange(m_params.exposureMode, value, 0, 1);
    }
    case VSourceParam::EXPOSURE:
    {
        return assignInRange(m_params.exposure, value, 0, MAX_LEVEL);
    }
    case VSourceParam::FOCUS_MODE:
    {
        return assignInRange(m_params.focusMode, value, 0, 1);
    }
    case VSourceParam::FOCUS_POS:
    {
        return assignInRange(m_params.focusPos, value, 0, MAX_LEVEL);
    }
    case VSourceParam::CYCLE_TIME_MKS:
    {
        // Read only param.
        return false;
    }
    case VSourceParam::FPS:
    {
        if (!isValidFps(value))
        {
            return false;
        }
        m_params.fps = value;
        return true;
    }
    case VSourceParam::IS_OPEN:
    {
        // Read only param.
        return false;
    }
    case VSourceParam::ROI_X:
    {
        return assignInRange(m_params.roiX, value, 0, MAX_FRAME_SIZE);
    }
    case VSourceParam::ROI_Y:
    {
        return assignInRange(m_params.roiY, value, 0, MAX_FRAME_SIZE);
    }
    case VSourceParam::ROI_WIDTH:
    {
        return assignInRange(m_params.roiWidth, value, 0, MAX_FRAME_SIZE);
    }
    case VSourceParam::ROI_HEIGHT:
    {
        return assignInRange(m_params.roiHeight, value, 0, MAX_FRAME_SIZE);
    }
    case VSourceParam::CUSTOM_1:
    {
        if (!isValidCustom(value))
        {
            return false;
        }
        m_params.custom1 = value;
        return true;
    }
    case VSourceParam::CUSTOM_2:
    {
        if (!isValidCustom(value))
        {
            return false;
        }
        m_params.custom2 = value;
        return true;
    }
    case VSourceParam::CUSTOM_3:
    {
        if (!isValidCustom(value))
        {
            return false;
        }
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
    const std::lock_guard<std::mutex> lock(m_mutex);

    // Check param ID.
    switch (id)
    {
    case VSourceParam::LOG_LEVEL:
    {
        return static_cast<float>(m_params.logLevel);
    }
    case VSourceParam::WIDTH:
    {
        return static_cast<float>(m_params.width);
    }
    case VSourceParam::HEIGHT:
    {
        return static_cast<float>(m_params.height);
    }
    case VSourceParam::GAIN_MODE:
    {
        return static_cast<float>(m_params.gainMode);
    }
    case VSourceParam::GAIN:
    {
        return static_cast<float>(m_params.gain);
    }
    case VSourceParam::EXPOSURE_MODE:
    {
        return static_cast<float>(m_params.exposureMode);
    }
    case VSourceParam::EXPOSURE:
    {
        return static_cast<float>(m_params.exposure);
    }
    case VSourceParam::FOCUS_MODE:
    {
        return static_cast<float>(m_params.focusMode);
    }
    case VSourceParam::FOCUS_POS:
    {
        return static_cast<float>(m_params.focusPos);
    }
    case VSourceParam::CYCLE_TIME_MKS:
    {
        return static_cast<float>(m_params.cycleTimeMks);
    }
    case VSourceParam::FPS:
    {
        return m_params.fps;
    }
    case VSourceParam::IS_OPEN:
    {
        return m_params.isOpen ? 1.0f : 0.0f;
    }
    case VSourceParam::ROI_X:
    {
        return static_cast<float>(m_params.roiX);
    }
    case VSourceParam::ROI_Y:
    {
        return static_cast<float>(m_params.roiY);
    }
    case VSourceParam::ROI_WIDTH:
    {
        return static_cast<float>(m_params.roiWidth);
    }
    case VSourceParam::ROI_HEIGHT:
    {
        return static_cast<float>(m_params.roiHeight);
    }
    case VSourceParam::CUSTOM_1:
    {
        return m_params.custom1;
    }
    case VSourceParam::CUSTOM_2:
    {
        return m_params.custom2;
    }
    case VSourceParam::CUSTOM_3:
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
    // Copy params. The copy of the strings can fail only if there is no memory.
    const std::lock_guard<std::mutex> lock(m_mutex);
    try
    {
        VSourceParams copy = m_params;
        params = std::move(copy);
    }
    catch (const std::bad_alloc&)
    {
        // The method returns nothing: the output params are not changed.
        return;
    }
}



bool cr::video::CustomVSource::executeCommand(VSourceCommand id)
{
    // Check command ID.
    switch (id)
    {
    case VSourceCommand::RESTART:
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
    // Decode command. The decoder checks the size, the version and the IDs.
    VSourceCommand commandId = VSourceCommand::RESTART;
    VSourceParam paramId = VSourceParam::CUSTOM_1;
    float value = 0.0f;
    switch (VSource::decodeCommand(data, size, paramId, commandId, value))
    {
    // COMMAND.
    case 0:
    {
        // Execute command.
        return executeCommand(commandId);
    }
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
