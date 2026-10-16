#pragma once
#include <cstdint>
#include <string>
#include "Frame.h"
#include "ConfigReader.h"



namespace cr
{
namespace video
{
/**
 * @brief Video source params mask structure.
 *
 * Each flag selects the VSourceParams field with the same name for encoding
 * (see VSourceParams::encode(...)). All flags are TRUE by default.
 */
struct VSourceParamsMask
{
    bool logLevel{true};      ///< Encode logLevel field.
    bool width{true};         ///< Encode width field.
    bool height{true};        ///< Encode height field.
    bool gainMode{true};      ///< Encode gainMode field.
    bool gain{true};          ///< Encode gain field.
    bool exposureMode{true};  ///< Encode exposureMode field.
    bool exposure{true};      ///< Encode exposure field.
    bool focusMode{true};     ///< Encode focusMode field.
    bool focusPos{true};      ///< Encode focusPos field.
    bool cycleTimeMks{true};  ///< Encode cycleTimeMks field.
    bool fps{true};           ///< Encode fps field.
    bool isOpen{true};        ///< Encode isOpen field.
    bool roiX{true};          ///< Encode roiX field.
    bool roiY{true};          ///< Encode roiY field.
    bool roiWidth{true};      ///< Encode roiWidth field.
    bool roiHeight{true};     ///< Encode roiHeight field.
    bool custom1{true};       ///< Encode custom1 field.
    bool custom2{true};       ///< Encode custom2 field.
    bool custom3{true};       ///< Encode custom3 field.
};



/**
 * @brief Video source params structure.
 *
 * The values of the fields are transported (encoded, decoded, read from
 * and written to JSON) without a range check: their meaning belongs to the
 * video source implementation, which must validate them before use.
 *
 * @note The class is not thread-safe: an object must not be accessed from
 * several threads at the same time without external synchronization.
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
    /// Focus mode. Value depends on implementation but it is
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
    float fps{0.0f};
    /// Open flag. false - not open, true - open.
    bool isOpen{false};
    /// Region of interest upper left corner x coordinate.
    int roiX{0};
    /// Region of interest upper left corner y coordinate.
    int roiY{0};
    /// Region of interest width.
    int roiWidth{0};
    /// Region of interest height.
    int roiHeight{0};
    /// Custom parameter. Depends on implementation.
    float custom1{0.0f};
    /// Custom parameter. Depends on implementation.
    float custom2{0.0f};
    /// Custom parameter. Depends on implementation.
    float custom3{0.0f};

    JSON_READABLE(VSourceParams, logLevel, source, fourcc, width, height,
                  gainMode, exposureMode, focusMode, fps, roiX, roiY, roiWidth,
                  roiHeight, custom1, custom2, custom3)

    /**
     * @brief Default constructor.
     */
    VSourceParams() = default;

    /**
     * @brief Copy constructor.
     * @param src Source object.
     */
    VSourceParams(const VSourceParams& src) = default;

    /**
     * @brief Move constructor.
     * @param src Source object.
     */
    VSourceParams(VSourceParams&& src) = default;

    /**
     * @brief Copy assignment operator.
     * @param src Source object.
     * @return Reference to this object.
     */
    VSourceParams& operator= (const VSourceParams& src) = default;

    /**
     * @brief Move assignment operator.
     * @param src Source object.
     * @return Reference to this object.
     */
    VSourceParams& operator= (VSourceParams&& src) = default;

    /**
     * @brief Encode params. The method does not encode source and fourcc
     * fields.
     *
     * The params are encoded in the order of the fields of VSourceParamsMask.
     * A param that does not fit in the buffer is not encoded (its flag in the
     * encoded data is not set). If the mask is nullptr (all params) the
     * encoding stops at the first param that does not fit. Encoded size of all
     * params is 79 bytes.
     *
     * @param data Pointer to data buffer. Must not be nullptr.
     * @param bufferSize Size of data buffer, bytes. Must be at least 7.
     * @param size Size of encoded data, bytes. Set to 0 if the method returns
     * FALSE.
     * @param mask Pointer to parameters mask. If nullptr all params are
     * selected.
     * @return TRUE if params encoded or FALSE if not (data is nullptr or the
     * buffer is smaller than 7 bytes).
     */
    bool encode(uint8_t* data, int bufferSize, int& size,
                VSourceParamsMask* mask = nullptr);

    /**
     * @brief Decode params encoded by encode(...) method.
     *
     * The method checks data type, version, reserved bits of the mask and the
     * size of the data before reading each field. The fields that are not
     * encoded are set to zero. The source and fourcc fields are not decoded:
     * they are cleared. Bytes after the last encoded field are ignored. Values
     * of the fields are not checked against any range.
     *
     * @param data Pointer to data. Must not be nullptr.
     * @param dataSize Size of data, bytes.
     * @return TRUE if params decoded or FALSE if not. If FALSE is returned the
     * object is not changed.
     */
    bool decode(uint8_t* data, int dataSize);
};



/**
 * @brief Enum of video source params.
 *
 * @note CUSTOM_3 is the last value: VSource::decodeCommand(...) uses it to
 * check the param ID of a command.
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
    /// [read/write] Region of interest upper left corner x coordinate.
    ROI_X,
    /// [read/write] Region of interest upper left corner y coordinate.
    ROI_Y,
    /// [read/write] Region of interest width.
    ROI_WIDTH,
    /// [read/write] Region of interest height.
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
 *
 * @note RESTART is the last value: VSource::decodeCommand(...) uses it to
 * check the ID of a command.
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
     * Default format: [video device or ID or file];[width];[height];[fourcc]
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
     * @note The implementation must be thread-safe.
     * @param id Parameter ID.
     * @param value Parameter value to set.
     * @return TRUE if the param was set or FALSE if not.
     */
    virtual bool setParam(VSourceParam id, float value) = 0;

    /**
     * @brief Get video source param value.
     * @note The implementation must be thread-safe.
     * @param id Parameter ID.
     * @return Parameter value or -1 if the param is not supported.
     */
    virtual float getParam(VSourceParam id) = 0;

    /**
     * @brief Get video source params structure.
     * @note The implementation must be thread-safe.
     * @param params Video source params class object.
     */
    virtual void getParams(VSourceParams& params) = 0;

    /**
     * @brief Execute command.
     * @note The implementation must be thread-safe.
     * @param id Command ID.
     * @return TRUE if the command accepted or FALSE if not.
     */
    virtual bool executeCommand(VSourceCommand id) = 0;

    /**
     * @brief Encode set param command.
     * @param data Pointer to data buffer. Must have size >= 11 (the method can
     * not check it). If nullptr the method does nothing except size = 0.
     * @param size Size of encoded data (0 if there is no buffer). Always 11
     * if the buffer is not nullptr.
     * @param id Parameter id.
     * @param value Parameter value.
     */
    static void encodeSetParamCommand(
            uint8_t* data, int& size, VSourceParam id, float value);

    /**
     * @brief Encode command.
     * @param data Pointer to data buffer. Must have size >= 7 (the method can
     * not check it). If nullptr the method does nothing except size = 0.
     * @param size Size of encoded data (0 if there is no buffer). Always 7 if
     * the buffer is not nullptr.
     * @param id Command ID.
     */
    static void encodeCommand(
            uint8_t* data, int& size, VSourceCommand id);

    /**
     * @brief Decode command.
     *
     * The method checks the version, the exact size of the command (7 bytes
     * for COMMAND, 11 bytes for SET_PARAM) and the ID (an ID outside the
     * VSourceCommand or VSourceParam enum is an error). The output arguments
     * are changed only if the method returns 0 or 1.
     *
     * @param data Pointer to command data. Must not be nullptr.
     * @param size Size of data, bytes.
     * @param paramId Output parameter ID (set param command).
     * @param commandId Output command ID (command).
     * @param value Output param value (0 for a command).
     * @return 0 - command decoded, 1 - set param command decoded, -1 - error.
     */
    static int decodeCommand(uint8_t* data,
                             int size,
                             VSourceParam& paramId,
                             VSourceCommand& commandId,
                             float& value);

    /**
     * @brief Decode and execute command.
     * @note The implementation must be thread-safe.
     * @param data Pointer to command data.
     * @param size Size of data.
     * @return TRUE if the command was decoded and executed (or the parameter
     * was set) or FALSE if not.
     */
    virtual bool decodeAndExecuteCommand(uint8_t* data, int size) = 0;

protected:

    /**
     * @brief Default constructor. The class can be used as a base only.
     */
    VSource() = default;

    /**
     * @brief Copy constructor.
     * @param src Source object.
     */
    VSource(const VSource& src) = default;

    /**
     * @brief Move constructor.
     * @param src Source object.
     */
    VSource(VSource&& src) = default;

    /**
     * @brief Copy assignment operator.
     * @param src Source object.
     * @return Reference to this object.
     */
    VSource& operator= (const VSource& src) = default;

    /**
     * @brief Move assignment operator.
     * @param src Source object.
     * @return Reference to this object.
     */
    VSource& operator= (VSource&& src) = default;
};

}
}
