#pragma once
#include "Frame.h"
#include "ConfigReader.h"



namespace cr
{
namespace video
{
/**
 * @brief Video source params structure.
 */
class VSourceParams
{
public:
    /// Log level: 0 - Disable, 1 - Console, 2 - File, 3 - Console and file.
    int logLevel{0};
    /// Video source name.
    std::string source{"/dev/video0"};
    /// FOURCC: RGB24, BGR24, YUYV, UYVY, GRAY, YUV24, NV12, NV21, YU12, YV12.
    std::string fourcc{"YUYV"};
    /// Frame width. 0 - will be set automatically.
    int width{1920};
    /// Frame height. 0 - will be set automatically.
    int height{1080};
    /// Gain mode. Depends on implementation. Default: 0 - Manual, 1 - Auto.
    int gainMode{1};
    /// Gain value in case manual gain mode. Value: 0(min) - 65535(max).
    int gain{0};
    /// Exposure mode. Depends on implementation. Default: 0 - Manual, 1 - Auto.
    int exposureMode{1};
    /// Exposure value in case manual exposure mode. Value: 0(min) - 65535(max).
    int exposure{1};
    /// Focus mode. Depends on implementation. Default: 0 - Manual, 1 - Auto.
    int focusMode{1};
    /// Focus position. Value: 0(full near) - 65535(full far).
    int focusPos{0};
    /// Cycle processing time microsecconds.
    int cycleTimeMks{0};
    /// FPS. 0 - will be set automatically.
    float fps{0};
    /// Open flag.
    bool isOpen{false};
    /// Custom param 1. Depends on implementation.
    float custom1{0.0f};
    /// Custom param 2. Depends on implementation.
    float custom2{0.0f};
    /// Custom param 3. Depends on implementation.
    float custom3{0.0f};

    JSON_READABLE(VSourceParams,
                  logLevel,
                  source,
                  fourcc,
                  width,
                  height,
                  gainMode,
                  gain,
                  exposureMode,
                  exposure,
                  focusMode,
                  focusPos,
                  fps,
                  custom1,
                  custom2,
                  custom3);

    /**
     * @brief operator =
     * @param src Source object.
     * @return VSourceParams obect.
     */
    VSourceParams& operator= (const VSourceParams& src);

    /**
     * @brief Encode params. The method doesn't encode params:
     * source and fourcc.
     * @param data Pointer to data buffer.
     * @param size Size of data.
     */
    void encode(uint8_t* data, int& size);

    /**
     * @brief Decode params. The method doesn't decode params:
     * source and fourcc.
     * @param data Pointer to data.
     * @return TRUE is params decoded or FALSE if not.
     */
    bool decode(uint8_t* data, int size);
};



/**
 * @brief Enum of video source params.
 */
enum class VSourceParam
{
    /// [read/write] Log level:
    /// 0 - Disable, 1 - Console, 2 - File, 3 - Console and file.
    LOG_LEVEL = 1,
    /// [read/write] Frame width.
    WIDTH,
    /// [read/write] Frame height.
    HEIGHT,
    /// [read/write] Gain mode. Depends on implementation.
    /// Default: 0 - Manual, 1 - Auto.
    GAIN_MODE,
    /// [read/write] Gain value in case manual gain mode.
    /// Value: 0(min) - 65535(max).
    GAIN,
    /// [read/write] Exposure mode. Depends on implementation.
    /// Default: 0 - Manual, 1 - Auto.
    EXPOSURE_MODE,
    /// [read/write] Exposure value in case manual exposure mode.
    /// Value: 0(min) - 65535(max).
    EXPOSURE,
    /// [read/write] Focus mode. Depends on implementation.
    /// Default: 0 - Manual, 1 - Auto.
    FOCUS_MODE,
    /// [read/write] Focus position.
    /// Value: 0(full near) - 65535(full far).
    FOCUS_POS,
    /// [read only] Cycle processing time microsecconds.
    CYCLE_TIME_MKS,
    /// [read/write]  FPS. 0 - will be set automatically.
    FPS,
    /// Open flag. 0 - not open, 1 - open.
    IS_OPEN,
    /// Custom parameter. Depends on implementation.
    CUSTOM_1,
    /// Custom parameter. Depends on implementation.
    CUSTOM_2,
    /// Custom parameter. Depends on implementation.
    CUSTOM_3
};



/**
 * @brief Video source commands.
 */
enum class VSourceCommand
{
    /// Restart.
    RESTART = 1
};



/**
 * @brief Video source interface class.
 */
class VSource
{
public:

    /**
     * @brief Get string of current library version.
     * @return String of current library version.
     */
    static std::string getVersion();

    /**
     * @brief Open video source. All params will be set by default.
     * @param initString Init string. Format depends on implementation.
     * Default format: <video device or ID or file>;<width>;<height>;<fourcc>
     * @return TRUE if the video source open or FALSE if not.
     */
    virtual bool openVSource(std::string& initString) = 0;

    /**
     * @brief Init video source. All params will be set according to structure.
     * @param params Video source parameters structure.
     * @return TRUE if the video source init or FALSE if not.
     */
    virtual bool initVSource(VSourceParams& params) = 0;

    /**
     * @brief Get open status.
     * @return TRUE if video source open or FALSE if not.
     */
    virtual bool isVSourceOpen() = 0;

    /**
     * @brief Close video source.
     */
    virtual void closeVSource() = 0;

    /**
     * @brief Get new video frame.
     * @param frame Frame object to copy new data.
     * @param timeoutMsec Timeout to wait new frame data:
     * timeoutMs == -1 - Method will wait endlessly until new data arrive.
     * timeoutMs == 0  - Method will only check if new data exist.
     * timeoutMs > 0   - Method will wait new data specified time.
     * @return TRUE if new data exist and copied or FALSE if not.
     */
    virtual bool getFrame(Frame& frame, int32_t timeoutMsec = 0) = 0;

    /**
     * @brief Set video source param.
     * @param id Parameter ID.
     * @param value Parameter value to set.
     * @return TRUE if property was set of FALSE.
     */
    virtual bool setParam(VSourceParam id, float value) = 0;

    /**
     * @brief Get video source parameter value.
     * @param id Parameter ID according to camera specification.
     * @return Parameter value or -1.
     */
    virtual float getParam(VSourceParam id) = 0;

    /**
     * @brief Get video source params structure.
     * @return Video source parameters structure.
     */
    virtual VSourceParams getParams() = 0;

    /**
     * @brief Execute command.
     * @param id Command ID .
     * @return TRUE if the command accepted or FALSE if not.
     */
    virtual bool executeCommand(VSourceCommand id) = 0;
};

}
}




