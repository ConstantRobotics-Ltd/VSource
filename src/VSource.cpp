#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include "VideoSourceV4L2.h"
#include "VideoSourceV4L2Version.h"



// Link namespaces.
using namespace std;
using namespace cr::vsource;



/// Get class version.
std::string VideoSourceV4L2::getVersion()
{
    return VIDEO_SOURCE_V4L2_VERSION;
}



/// Class constructor.
VideoSourceV4L2::VideoSourceV4L2()
{
    // Reset buffers.
    memset(m_buffers, 0, 256 * sizeof(buffer));
}



/// Class destructor.
VideoSourceV4L2::~VideoSourceV4L2()
{
    // Close device.
    if (m_fd != -1)
        ::close(m_fd);
}



/// Close video source.
void VideoSourceV4L2::close()
{
    // Reset flag.
    m_isOpen.store(false);

    // Close device.
    if (m_fd != -1)
        ::close(m_fd);
}



/// Open video source.
bool VideoSourceV4L2::open(std::string& initString)
{
    // Close device if open.
    close();

    // Split init string into substrings.
    std::string str = initString;
    std::vector<std::string> strings = splitString(str, ';');

    // Check number of strings.
    if (strings.size() != 4)
        return false;

    // Get frame size.
    try {
        m_width = std::stoi(strings[1]);
        m_height = std::stoi(strings[2]);
    } catch(...) {
       return false;
    }

    // Get pixel format.
    if (strings[3] == "RGB24")
        m_fourcc = ValidFourccCodes::RGB24;
    else if (strings[3] == "BGR24")
        m_fourcc = ValidFourccCodes::BGR24;
    else if (strings[3] == "Y800")
        m_fourcc = ValidFourccCodes::Y800;
    else if (strings[3] == "NV12")
        m_fourcc = ValidFourccCodes::NV12;
    else if (strings[3] == "YUY2")
        m_fourcc = ValidFourccCodes::YUY2;
    else if (strings[3] == "UYVY")
        m_fourcc = ValidFourccCodes::UYVY;
    else
        return false;

    // Open device.
    m_name = strings[0];
    m_fd = ::open(m_name.c_str(), O_RDWR | O_NONBLOCK);
    if (m_fd == -1)
        return false;

    // Init device.
    if (!initDevice(m_width, m_height, m_fourcc))
        return false;

    // Set open flag.
    m_isOpen.store(true);

	return true;
}



/// Check open status.
bool VideoSourceV4L2::isOpen()
{
    return m_isOpen.load();
}



/// Get video frame.
bool VideoSourceV4L2::getFrame(Frame& frame, int32_t timeoutMsec)
{
    // Check if the device not open.
    if (!m_isOpen.load())
    {
        // Unmap buffers.
        for (int n = 0; n < m_numBuffers; ++n)
            munmap(m_buffers[n].start, m_buffers[n].length);

        // Close device.
        if (m_fd != -1)
            ::close(m_fd);

        // Open device.
        m_fd = ::open(m_name.c_str(), O_RDWR | O_NONBLOCK);
        if (m_fd == -1)
        {
            m_fd = -1;
            m_isOpen.store(false);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return false;
        }

        // Init device.
        if (!initDevice(m_width, m_height, m_fourcc))
        {
            m_isOpen.store(false);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return false;
        }

        // Set open flag.
        m_isOpen.store(true);
    }

    // Wait device.
    fd_set fds;
    struct timeval tv;
    int r;
    FD_ZERO(&fds);
    FD_SET(m_fd, &fds);
    // Determine timeout.
    if (timeoutMsec > 999)
    {
        tv.tv_sec = timeoutMsec / 1000;
        tv.tv_usec = timeoutMsec % 1000;
        r = select(m_fd + 1, &fds, NULL, NULL, &tv);
    }
    else
    {
        tv.tv_sec = 0;
        tv.tv_usec = timeoutMsec * 1000;
        r = select(m_fd + 1, &fds, NULL, NULL, &tv);
    }

    // Check result of wait device.
    if (-1 == r)
    {
        if (EINTR == errno)
            return false;

        // Set not open flag.
        m_isOpen.store(false);
        return false;
    }

    // Check timeout.
    if (0 == r)
    {
        // Set not open flag.
        m_isOpen.store(false);
        return false;
    }

    // Prepare buffer.
    struct v4l2_buffer buf;
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if (-1 == xioctl(m_fd, VIDIOC_DQBUF, &buf))
    {
        switch (errno)
        {
        case EAGAIN:

            // Set not open flag.
            m_isOpen.store(false);
            return false;
        case EIO:
            return false;
        default:
            // Set not open flag.
            m_isOpen.store(false);
            return false;
        }
    }

    // Check buffer index.
    if (buf.index > m_numBuffers)
    {
        // Set not open flag.
        m_isOpen.store(false);
        return false;
    }

    // Copy image data.
    if (frame.width != m_width || frame.height != m_height ||
        frame.fourcc != (uint32_t)m_fourcc)
    {
        frame.release();
        frame = Frame(m_width, m_height, (uint32_t)m_fourcc);
    }
    memcpy(frame.data, m_buffers[buf.index].start, buf.bytesused);

    if (-1 == xioctl(m_fd, VIDIOC_QBUF, &buf))
    {
        // Set not open flag.
        m_isOpen.store(false);
        return false;
    }

    return true;
}



/// Set video source param.
bool VideoSourceV4L2::setParam(VideoSourceParam id, float value)
{
    // Check property ID.
    switch (id)
    {
    // Frame width.
    case VideoSourceParam::WIDTH:
        // Must be set during initialization.
        return false;
    // Frame height.
    case VideoSourceParam::HEIGHT:
        // Must be set during initialization.
        return false;
    // FPS.
    case VideoSourceParam::FPS:
        // Will be max FPS.
        return false;
    // Brightness mode: 0 - off, 1 - mode.
    case VideoSourceParam::BRIGHTNESS_MODE:
    {
        struct v4l2_control ctrl ={0};
        ctrl.id = V4L2_CID_AUTOBRIGHTNESS;
        ctrl.value = (int)value == 0 ? 0 : 1;
        if (-1 == xioctl(m_fd, VIDIOC_S_CTRL, &ctrl))
            return false;
        return true;
    }
    // Brightness.
    case VideoSourceParam::BRIGHTNESS:

        return false;
    // Contrast.
    case VideoSourceParam::CONTRAST:
        return false;
    // Gain mode.
    case VideoSourceParam::GAIN_MODE:
        return false;
    // Gain.
    case VideoSourceParam::GAIN:
        return false;
    // Exposure mode.
    case VideoSourceParam::EXPOSURE_MODE:
    {
        // Manual mode.
        if ((int)value == 0)
        {
            struct v4l2_control ctrl ={0};
            ctrl.id = V4L2_CID_EXPOSURE_AUTO;
            ctrl.value = V4L2_EXPOSURE_MANUAL;
            return xctrl(ctrl);
        }
        // Auto mode.
        else if ((int)value == 1)
        {
            struct v4l2_control ctrl ={0};
            ctrl.id = V4L2_CID_EXPOSURE_AUTO;
            ctrl.value = V4L2_EXPOSURE_AUTO;
            return xctrl(ctrl);
        }
        // Shutter priority.
        else if ((int)value == 2)
        {
            struct v4l2_control ctrl ={0};
            ctrl.id = V4L2_CID_EXPOSURE_AUTO;
            ctrl.value = V4L2_EXPOSURE_SHUTTER_PRIORITY;
            return xctrl(ctrl);
        }
        // Aperture priority.
        else if ((int)value == 3)
        {
            struct v4l2_control ctrl ={0};
            ctrl.id = V4L2_CID_EXPOSURE_AUTO;
            ctrl.value = V4L2_EXPOSURE_APERTURE_PRIORITY;
            return xctrl(ctrl);
        }
        else
        {
            return false;
        }
        return false;
    }
    // Exposure time.
    case VideoSourceParam::EXPOSURE:
    {
        // Check value.
        if ((int)value < 0 || (int)value > 1000000)
            return false;

        struct v4l2_control ctrl ={0};
        ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
        ctrl.value = (int)value / 100;
        return xctrl(ctrl);
    }
    // Sharpening mode.
    case VideoSourceParam::SHARPENING_MODE:
        return false;
    // Sharpening.
    case VideoSourceParam::SHARPENING:
        return false;
    // Polarity.
    case VideoSourceParam::POLARITY:
        return false;
    // Print debug info.
    case VideoSourceParam::PRINT_DEBUG_INFO:
        // Check parameter value.
        if ((int)value == 0)
            m_printDebugInfo.store(false);
        else
            m_printDebugInfo.store(true);
        return true;
    // Focus mode.
    case VideoSourceParam::FOCUS_MODE:
    {
        struct v4l2_control ctrl ={0};
        ctrl.id = V4L2_CID_FOCUS_AUTO;
        ctrl.value = (int)value == 0 ? 0 : 1;
        return xctrl(ctrl);
    }
    // Focus absolute position.
    case VideoSourceParam::FOCUS_ABSOLUTE:
    {
        struct v4l2_control ctrl ={0};
        ctrl.id = V4L2_CID_FOCUS_ABSOLUTE;
        ctrl.value = (int)value;
        return xctrl(ctrl);
    }
    default:
        return false;
    }

    return false;
}



/// Get video source parameter value.
float VideoSourceV4L2::getParam(VideoSourceParam id)
{
    // Check property ID.
    switch (id)
    {
    /// Frame width.
    case VideoSourceParam::WIDTH:
        return (float)m_width;
    /// Frame height.
    case VideoSourceParam::HEIGHT:
        return (float)m_height;
    /// FPS.
    case VideoSourceParam::FPS:
        return -1.0f;
    /// Brightness.
    case VideoSourceParam::BRIGHTNESS:
        return -1.0f;
    /// Contrast.
    case VideoSourceParam::CONTRAST:
        return -1.0f;
    /// Gain mode.
    case VideoSourceParam::GAIN_MODE:
        return -1.0f;
    /// Gain.
    case VideoSourceParam::GAIN:
        return -1.0f;
    /// Exposure mode.
    case VideoSourceParam::EXPOSURE_MODE:
        return -1.0f;
    /// Exposure time.
    case VideoSourceParam::EXPOSURE:
        return -1.0f;
    /// Sharpening mode.
    case VideoSourceParam::SHARPENING_MODE:
        return -1.0f;
    /// Sharpening.
    case VideoSourceParam::SHARPENING:
        return -1.0f;
    /// Polarity.
    case VideoSourceParam::POLARITY:
        return -1.0f;
    default:
        return -1.0f;
    }

    return -1.0f;
}






/// Split content by symbol.
std::vector<std::string> VideoSourceV4L2::splitString(
        std::string& str, char symbol)
{
    // Init variables.
    std::stringstream stream(str);
    std::string segment;
    std::vector<std::string> strings;

    // Split into segments.
    while(std::getline(stream, segment, symbol))
    {
        if (segment != "")
            strings.push_back(segment);
    }

    // Return result.
    return strings;
}



/// Init device.
bool VideoSourceV4L2::initDevice(int width, int height, ValidFourccCodes fourcc)
{
    // Get video device capabilities.
    struct v4l2_capability cap;
    memset(&cap, 0, sizeof(cap));
    if (-1 == xioctl(m_fd, VIDIOC_QUERYCAP, &cap))
    {
        if (m_printDebugInfo.load())
        cout << "[" << __FILE__ << "][" << __LINE__ << "] " <<
        "VIDIOC_QUERYCAP error" << endl;
        return false;
    }

    // Check video capture deivice type.
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        if (m_printDebugInfo.load())
        cout << "[" << __FILE__ << "][" << __LINE__ << "] " <<
        "Not video device" << endl;
        return false;
    }

    // Check streaming support.
    if (!(cap.capabilities & V4L2_CAP_STREAMING))
    {
        if (m_printDebugInfo.load())
        cout << "[" << __FILE__ << "][" << __LINE__ << "] " <<
        "No streaming support" << endl;
        return false;
    }

    // Set new parameters (size and pixel format).
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width  = width;
    fmt.fmt.pix.height = height;
    switch (fourcc)
    {
    case ValidFourccCodes::BGR24:
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
        break;
    case ValidFourccCodes::RGB24:
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
        break;
    case ValidFourccCodes::NV12:
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
        break;
    case ValidFourccCodes::UYVY:
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
        break;
    case ValidFourccCodes::YUV1:
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
        break;
    case ValidFourccCodes::YUY2:
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        break;
    case ValidFourccCodes::Y800:
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
        break;
    default:
        if (m_printDebugInfo.load())
        cout << "[" << __FILE__ << "][" << __LINE__ << "] " <<
        "Not supported pixel format" << endl;
        return false;
    }
    if (-1 == xioctl(m_fd, VIDIOC_S_FMT, &fmt))
    {
        if (m_printDebugInfo.load())
        cout << "[" << __FILE__ << "][" << __LINE__ << "] " <<
        "Picture parameters not set" << endl;
        return false;
    }

    // Request buffers.
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = 2; // We want to get 4 buffers.
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (-1 == xioctl(m_fd, VIDIOC_REQBUFS, &req))
    {
        if (m_printDebugInfo.load())
        cout << "[" << __FILE__ << "][" << __LINE__ << "] " <<
        "Request buffers error" << endl;
        return false;
    }

    // Check real number of buffers.
    if (req.count < 1)
    {
        if (m_printDebugInfo.load())
        cout << "[" << __FILE__ << "][" << __LINE__ << "] " <<
        "Incorrect number of buffers" << endl;
        return false;
    }

    // Reset buffers.
    memset(m_buffers, 0, 256 * sizeof(buffer));

    // Remember number of buffers.
    m_numBuffers = req.count;

    // Map buffers. We get pointer to data from driver.
    for (int n = 0; n < req.count; ++n)
    {
        // Query buffer.
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n;
        if (-1 == xioctl(m_fd, VIDIOC_QUERYBUF, &buf))
            return false;

        // Set size of buffer.
        m_buffers[n].length = buf.length;
        m_buffers[n].start =
            mmap(NULL /* start anywhere */,
            buf.length,
            PROT_READ | PROT_WRITE /* required */,
            MAP_SHARED /* recommended */,
            m_fd,
            buf.m.offset);

        // Check result.
        if (MAP_FAILED == m_buffers[n].start)
        {
            if (m_printDebugInfo.load())
            cout << "[" << __FILE__ << "][" << __LINE__ << "] " <<
            "Buffer " << n << " mapping error" << endl;
            return false;
        }
    }

    for (int i = 0; i < m_numBuffers; ++i)
    {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (-1 == xioctl(m_fd, VIDIOC_QBUF, &buf))
        {
            if (m_printDebugInfo.load())
            cout << "[" << __FILE__ << "][" << __LINE__ << "] " <<
            "Buffer " << i << " not quered" << endl;
            return false;
        }
    }

    // Turn on stream.
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(m_fd, VIDIOC_STREAMON, &type))
    {
        if (m_printDebugInfo.load())
        cout << "[" << __FILE__ << "][" << __LINE__ << "] " <<
        "Stream not turned ON" << endl;
        return false;
    }

    return true;
}



/// Process ioctl.
int VideoSourceV4L2::xioctl(int fh, int request, void *arg)
{
    int r;
    do {
        r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);
    return r;
}



/// Execute command.
bool VideoSourceV4L2::executeCommand(VideoSourceCommand id, float arg)
{
    // No supported command yet.
    return false;
}



/// Set property value.
bool VideoSourceV4L2::xctrl(struct v4l2_control control)
{
    // Query control.
    struct v4l2_queryctrl queryctrl = {0};
    queryctrl.id = control.id;
    if (-1 == ioctl(m_fd, VIDIOC_QUERYCTRL, &queryctrl))
    {
        if (errno != EINVAL)
        {
            if (m_printDebugInfo.load())
            cout << "[" << __FILE__ << "][" << __LINE__ << "] " <<
            "VIDIOC_QUERYCTRL error" << endl;
            return false;
        }
        else
        {
            if (m_printDebugInfo.load())
            cout << "[" << __FILE__ << "][" << __LINE__ << "] " <<
            "Control not supported" << endl;
            return false;
        }
    }

    // Set control.
    if (-1 == xioctl(m_fd, VIDIOC_S_CTRL, &control))
    {
        if (m_printDebugInfo.load())
        cout << "[" << __FILE__ << "][" << __LINE__ << "] " <<
        "Control not set" << endl;
        return false;
    }

    return true;
}












