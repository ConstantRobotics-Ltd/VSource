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

    /// Video source params.
    VSourceParams m_params;
    /// Output frame.
    Frame m_outputFrame;
};
}
}