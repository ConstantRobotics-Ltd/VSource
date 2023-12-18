#pragma once
#include "VSource.h"



namespace cr
{
namespace video
{
/// Custom video source class.
class CustomVSource: public VSource
{
public:

    /// Class constructor.
    CustomVSource();

    /// Class destructor.
    ~CustomVSource();

    /// Get string of current library version.
    static std::string getVersion();

    /// Open video source.
    bool openVSource(std::string& initString);

    /// Init video source.
    bool initVSource(VSourceParams& params);

    /// Get open status.
    bool isVSourceOpen();

    /// Close video source.
    void closeVSource();

    /// Get new video frame.
    bool getFrame(Frame& frame, int32_t timeoutMsec = 0);

    /// Set video source param.
    bool setParam(VSourceParam id, float value);

    /// Get video source param value.
    float getParam(VSourceParam id);

    /// Get video source params structure.
    void getParams(VSourceParams& params);

    /// Execute command.
    bool executeCommand(VSourceCommand id);

    /// Decode and execute command.
    bool decodeAndExecuteCommand(uint8_t* data, int size);

private:

    /// Video source params.
    VSourceParams m_params;
    /// Output frame.
    Frame m_outputFrame;
};
}
}