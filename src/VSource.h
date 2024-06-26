#pragma once
#include "Frame.h"
#include "ConfigReader.h"



namespace cr
{
namespace video
{
/**
 * @brief Video source params mask structure.
 */
struct VSourceParamsMask
{
    bool logLevel{true};
    bool width{true};
    bool height{true};
    bool gainMode{true};
    bool gain{true};
    bool exposureMode{true};
    bool exposure{true};
    bool focusMode{true};
    bool focusPos{true};
    bool cycleTimeMks{true};
    bool fps{true};
    bool isOpen{true};
    bool roiX{true};
    bool roiY{true};
    bool roiWidth{true};
    bool roiHeight{true};
    bool custom1{true};
    bool custom2{true};
    bool custom3{true};
};



/**
 * @brief Video source params structure.
 */
class VSourceParams
{
public:
    /// Logging mode. Values: 0 - Disable, 1 - Only file,
    /// 2 - Only terminal, 3 - File and terminal.
    int logLevel{0};
    /// Video source: file, video stream, video device, camera num, etc.
    std::string source{"/dev/video0"};
    /// FOURCC: RGB24, BGR24, YUYV, UYVY, GRAY, YUV24, NV12, NV21, YU12, YV12.
    /// Value says to video source class which pixel format preferable for
    /// output video frame. Particular video source class can ignore this params
    /// during initialization. Parameters should be set before initialization.
    std::string fourcc{"YUYV"};
    /// Frame width. User can set frame width before initialization
    /// or after. Some video source classes may set width automatically.
    int width{1920};
    /// Frame height. User can set frame height before
    /// initialization or after. Some video source classes may set height
    /// automatically.
    int height{1080};
    /// Gain mode. Value depends on implementation but it is
    /// recommended to keep default values: 0 - Manual control, 1 - Auto.
    int gainMode{1};
    /// Gain value. Value: 0(min for particular video source class)
    /// - 65535(max for particular video source class).
    int gain{0};
    /// Exposure mode. Value depends on implementation but it is
    /// recommended to keep default values: 0 - Manual control, 1 - Auto.
    int exposureMode{1};
    /// Exposure value. Value: 0(min for particular video source
    /// class) - 65535(max for particular video source class).
    int exposure{1};
    /// Focus mode. Focus mode. Value depends on implementation but it is
    /// recommended to keep default values: 0 - Manual control, 1 - Auto.
    int focusMode{1};
    /// Focus position. Value: 0(full near) - 65535(full far).
    int focusPos{0};
    /// Video capture cycle time. **VSource** class sets this value
    /// automatically. This parameter means time interval between two captured
    /// video frame.
    int cycleTimeMks{0};
    /// FPS. User can set frame FPS before initialization or after.
    /// Some video source classes may set FPS automatically.
    float fps{0};
    /// Open flag. 0 - not open, 1 - open.
    bool isOpen{false};
    /// Region of intrest upper left corner x coordinate.
    int roiX{0};
    /// Region of intrest upper left corner y coordinate.
    int roiY{0};
    /// Region of intrest width.
    int roiWidth{0};
    /// Region of intrest heigth.
    int roiHeight{0};
    /// Custom parameter. Depends on implementation.
    float custom1{0.0f};
    /// Custom parameter. Depends on implementation.
    float custom2{0.0f};
    /// Custom parameter. Depends on implementation.
    float custom3{0.0f};

    JSON_READABLE(VSourceParams, logLevel, source, fourcc, width, height,
                  gainMode, exposureMode, focusMode, fps, roiX, roiY, roiWidth,
                  roiHeight, custom1, custom2, custom3);

    /**
     * @brief operator =
     * @param src Source object.
     * @return VSourceParams obect.
     */
    VSourceParams& operator= (const VSourceParams& src);

    /**
     * @brief Encode params. The method doesn't encode params:
     * source and fourcc fields.
     * @param data Pointer to data buffer.
     * @param bufferSize Size of data biffer.
     * @param size Size of data.
     * @param mask Pointer to parameters mask.
     * @return TRUE if params encoded or FALSE if not.
     */
    bool encode(uint8_t* data, int bufferSize, int& size,
                VSourceParamsMask* mask = nullptr);

    /**
     * @brief Decode params. The method doesn't decode params:
     * source and fourcc fields.
     * @param data Pointer to data.
     * @param dataSize Size of data.
     * @return TRUE is params decoded or FALSE if not.
     */
    bool decode(uint8_t* data, int dataSize);
};



/**
 * @brief Enum of video source params.
 */
enum class VSourceParam
{
    /// [read/write] Logging mode. Values: 0 - Disable, 1 - Only file,
    /// 2 - Only terminal, 3 - File and terminal.
    LOG_LEVEL = 1,
    /// [read/write] Frame width. User can set frame width before initialization
    /// or after. Some video source classes may set width automatically.
    WIDTH,
    /// [read/write] Frame height. User can set frame height before
    /// initialization or after. Some video source classes may set height
    /// automatically.
    HEIGHT,
    /// [read/write] Gain mode. Value depends on implementation but it is
    /// recommended to keep default values: 0 - Manual control, 1 - Auto.
    GAIN_MODE,
    /// [read/write] Gain value. Value: 0(min for particular video source class)
    /// - 65535(max for particular video source class).
    GAIN,
    /// [read/write] Exposure mode. Value depends on implementation but it is
    /// recommended to keep default values: 0 - Manual control, 1 - Auto.
    EXPOSURE_MODE,
    /// [read/write] Exposure value. Value: 0(min for particular video source
    /// class) - 65535(max for particular video source class).
    EXPOSURE,
    /// [read/write] Focus mode. Value depends on implementation but it is
    /// recommended to keep default values: 0 - Manual control, 1 - Auto.
    FOCUS_MODE,
    /// [read/write] Focus position. Value: 0(full near) - 65535(full far).
    FOCUS_POS,
    /// [read only] Video capture cycle time. **VSource** class sets this value
    /// automatically. This parameter means time interval between two captured
    /// video frame.
    CYCLE_TIME_MKS,
    /// [read/write] FPS. User can set frame FPS before initialization or after.
    /// Some video source classes may set FPS automatically.
    FPS,
    /// [read only] Open flag. 0 - not open, 1 - open.
    IS_OPEN,
    /// Region of intrest upper left corner x coordinate.
    ROI_X,
    /// Region of intrest upper left corner y coordinate.
    ROI_Y,
    /// Region of intrest width.
    ROI_WIDTH,
    /// Region of intrest height.
    ROI_HEIGHT,
    /// [read/write] Custom parameter. Depends on implementation.
    CUSTOM_1,
    /// [read/write] Custom parameter. Depends on implementation.
    CUSTOM_2,
    /// [read/write] Custom parameter. Depends on implementation.
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
     * @brief Class destructor.
     */
    virtual ~VSource();

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
     * timeoutMsec == -1 - Method will wait endlessly until new data arrive.
     * timeoutMsec == 0  - Method will only check if new data exist.
     * timeoutMsec > 0   - Method will wait new data specified time.
     * @return TRUE if new video frame exist and copied or FALSE if not.
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
     * @brief Get video source param value.
     * @param id Parameter ID.
     * @return Parameter value or -1.
     */
    virtual float getParam(VSourceParam id) = 0;

    /**
     * @brief Get video source params structure.
     * @param params Video source params class object.
     */
    virtual void getParams(VSourceParams& params) = 0;

    /**
     * @brief Execute command.
     * @param id Command ID.
     * @return TRUE if the command accepted or FALSE if not.
     */
    virtual bool executeCommand(VSourceCommand id) = 0;

    /**
     * @brief Encode set param command.
     * @param data Pointer to data buffer. Must have size >= 11.
     * @param size Size of encoded data.
     * @param id Parameter id.
     * @param value Parameter value.
     */
    static void encodeSetParamCommand(
            uint8_t* data, int& size, VSourceParam id, float value);

    /**
     * @brief Encode command.
     * @param data Pointer to data buffer. Must have size >= 11.
     * @param size Size of encoded data.
     * @param id Command ID.
     */
    static void encodeCommand(
            uint8_t* data, int& size, VSourceCommand id);

    /**
     * @brief Decode command.
     * @param data Pointer to command data.
     * @param size Size of data.
     * @param paramId Output command ID.
     * @param commandId Output command ID.
     * @param value Param or command value.
     * @return 0 - command decoded, 1 - set param command decoded, -1 - error.
     */
    static int decodeCommand(uint8_t* data,
                             int size,
                             VSourceParam& paramId,
                             VSourceCommand& commandId,
                             float& value);

    /**
     * @brief Decode and execute command.
     * @param data Pointer to command data.
     * @param size Size of data.
     * @return 0 - command decoded, 1 - set param command decoded, -1 - error.
     */
    virtual bool decodeAndExecuteCommand(uint8_t* data, int size) = 0;
};

}
}