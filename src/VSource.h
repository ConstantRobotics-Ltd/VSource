#pragma once
#include <atomic>
#include <thread>
#include <mutex>
#include <cstring>
#include <vector>
#include <condition_variable>
#include "VideoSource.h"



namespace cr
{
namespace vsource
{
/**
 * @brief Video source class based on V4L2 API.
*/
class VideoSourceV4L2 : public VideoSource
{
public:

    /**
     * @brief Get string of current version of library.
     * @return String of current library version.
     */
    static std::string getVersion();

    /**
     * @brief Class constructor.
     */
    VideoSourceV4L2();

    /**
     * @brief Class destructor.
     */
    ~VideoSourceV4L2();

    /**
     * @brief Open video source.
     * @param initString Initialization string. Init string can be:
     * 1. <path or camera num>;<width or -1>;<height or -1>;<FOURCC>
     * 2. <path or camera num>
     * <path> can be path to file or rtsp/rtp stream.
     * @return TRUE if the video source open or FALSE.
     */
    bool open(std::string& initString);

    /**
     * @brief Get connection status.
     * @return TRUE if video source open or FALSE.
     */
    bool isOpen();

    /**
     * @brief Get new video frame.
     * @param frame Frame object to copy new data.
     * @param timeoutMsec Timeout to wait new frame data:
     * timeoutMs == -1 - Method will wait endlessly until new data arrive.
     * timeoutMs == 0  - Method will only check if new data exist.
     * timeoutMs > 0   - Method will wait new data specified time.
     * @return TRUE if new data exist and copied or FALSE.
     */
    bool getFrame(Frame& frame, int32_t timeoutMsec = 0);

    /**
     * @brief Close video source.
     */
    void close();

    /**
     * @brief Set video source param.
     * @param id Parameter ID according to camera specification.
     * @param value Parameter value to set.
     * @return TRUE if property was set of FALSE.
     */
    bool setParam(VideoSourceParam id, float value);

    /**
     * @brief Get video source parameter value.
     * @param id Parameter ID according to camera specification.
     * @return Parameter value or -1.
     */
    float getParam(VideoSourceParam id);

    /**
     * @brief Execute command.
     * @param id Command ID according to specification.
     * @param arg Command argument.
     * @return TRUE if the command accepted or FALSE.
     */
    bool executeCommand(VideoSourceCommand id, float arg = 0);

private:

    /// Video data buffer structure.
    struct buffer
    {
        void *start;
        size_t length;
    };

    /// File descriptor.
    int m_fd{-1};
    /// Flag of open video source.
    std::atomic<bool> m_isOpen{false};
    /// Read video thread.
    std::thread m_readThread;
    /// Frame ID.
    uint32_t m_frameId{0};
    /// Video data buffers.
    struct buffer m_buffers[256];
    /// Number of buffers.
    int m_numBuffers{0};
    /// Frame width.
    int m_width{0};
    /// Frame height.
    int m_height{0};
    /// Pixel format.
    ValidFourccCodes m_fourcc{ValidFourccCodes::BGR24};
    /// Device name.
    std::string m_name{""};
    /// Print debug info flag.
    std::atomic<bool> m_printDebugInfo{false};


    /**
     * @brief Split string by symbol.
     * @param str String.
     * @param symbol Symbol.
     * @return Vector of strings.
     */
    std::vector<std::string> splitString(std::string& str, char symbol);

    /**
     * @brief Init device.
     * @param width Frame width.
     * @param height Frame height.
     * @param fourcc FOURCC code.
     * @return TRUE if the device is init or FALSE.
     */
    bool initDevice(int width, int height, ValidFourccCodes fourcc);

    /**
     * @brief xioctl Request for to device.
     * @param fh Device descriptor.
     * @param request Type of request.
     * @param arg Arguments.
     * @return -1 in case errors or result.
     */
    int xioctl(int fh, int request, void *arg);

    /**
     * @brief Set property value.
     * @param control Control structure.
     * @return (-1) - no such control;
     *         (-2) - control not set;
     *         (-3) - internal error;
     *         (0) - OK.
     */
    bool xctrl(struct v4l2_control control);

};

}
}




