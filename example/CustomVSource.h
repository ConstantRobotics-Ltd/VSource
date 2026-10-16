#pragma once
#include <cstdint>
#include <mutex>
#include <string>
#include "Frame.h"
#include "VSource.h"



namespace cr
{
namespace video
{
/// Custom video source class. All methods are thread-safe: they can be called
/// from any thread.
class CustomVSource: public VSource
{
public:

    /// Class constructor.
    CustomVSource();

    /// Class destructor.
    ~CustomVSource() override;

    /// Copy constructor is deleted (the class owns a mutex).
    CustomVSource(const CustomVSource&) = delete;

    /// Move constructor is deleted (the class owns a mutex).
    CustomVSource(CustomVSource&&) = delete;

    /// Copy assignment operator is deleted (the class owns a mutex).
    CustomVSource& operator= (const CustomVSource&) = delete;

    /// Move assignment operator is deleted (the class owns a mutex).
    CustomVSource& operator= (CustomVSource&&) = delete;

    /// Get string of current library version.
    static std::string getVersion();

    /// Open video source.
    bool openVSource(std::string& initString) override;

    /// Init video source.
    bool initVSource(VSourceParams& params) override;

    /// Get open status.
    bool isVSourceOpen() override;

    /// Close video source.
    void closeVSource() override;

    /// Get new video frame.
    bool getFrame(Frame& frame, int32_t timeoutMsec = 0) override;

    /// Set video source param.
    bool setParam(VSourceParam id, float value) override;

    /// Get video source param value.
    float getParam(VSourceParam id) override;

    /// Get video source params structure.
    void getParams(VSourceParams& params) override;

    /// Execute command.
    bool executeCommand(VSourceCommand id) override;

    /// Decode and execute command.
    bool decodeAndExecuteCommand(uint8_t* data, int size) override;

private:

    /// Video source params. Protected by m_mutex.
    VSourceParams m_params;
    /// Output frame. Protected by m_mutex.
    Frame m_outputFrame;
    /// Mutex to protect m_params and m_outputFrame.
    std::mutex m_mutex;
};
}
}
