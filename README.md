![vsource_web_logo](./static/vsource_web_logo.png)



# **VSource interface C++ library**

**v1.8.7**



# Table of contents

- [Overview](#overview)
- [Versions](#versions)
- [Library files](#library-files)
- [Video source interface class description](#video-source-interface-class-description)
  - [VSource class declaration](#vsource-class-declaration)
  - [getVersion method](#getversion-method)
  - [openVSource method](#openvsource-method)
  - [initVSource method](#initvsource-method)
  - [isVSourceOpen method](#isvsourceopen-method)
  - [closeVSource method](#closevsource-method)
  - [getFrame method](#getframe-method)
  - [setParam method](#setparam-method)
  - [getParam method](#getparam-method)
  - [getParams method](#getparams-method)
  - [executeCommand method](#executecommand-method)
  - [encodeSetParamCommand method](#encodesetparamcommand-method)
  - [encodeCommand method](#encodecommand-method)
  - [decodeCommand method](#decodecommand-method)
  - [decodeAndExecuteCommand method](#decodeandexecutecommand-method)
- [Data structures](#data-structures)
  - [VSourceCommand enum](#vsourcecommand-enum)
  - [VSourceParam enum](#vsourceparam-enum)
- [VSourceParams class description](#vsourceparams-class-description)
  - [VSourceParams class declaration](#vsourceparams-class-declaration)
  - [Serialize video source params](#serialize-video-source-params)
  - [Deserialize video source params](#deserialize-video-source-params)
  - [Read params from JSON file and write to JSON file](#read-params-from-json-file-and-write-to-json-file)
- [Portability](#portability)
- [Build and connect to your project](#build-and-connect-to-your-project)
  - [Build stand alone](#build-stand-alone)
  - [CMake options](#cmake-options)
  - [Connect to your project](#connect-to-your-project)
- [How to make custom implementation](#how-to-make-custom-implementation)



# Overview

**VSource** C++ library provides standard interface as well defines data structures and rules for different video source classes (video capture classes). **VSource** interface class doesn't do anything, just provides interface and provides methods to encode/decode commands and encode/decode params. Also **VSource** class provides data structures for video source parameters. Different video source classes inherit interface from **VSource** C++ class. **VSource.h** file contains list of data structures ([VSourceCommand enum](#vsourcecommand-enum), [VSourceParam enum](#vsourceparam-enum) and [VSourceParams class](#vsourceparams-class-description) class). [VSourceParams class](#vsourceparams-class-description) contains video source params and includes methods to encode and decode params.  [VSourceCommand enum](#vsourcecommand-enum) contains IDs of commands supported by **VSource** class. [VSourceParam enum](#vsourceparam-enum) contains IDs of params supported by **VSource** class. All video sources should include params and commands listed in **VSource.h** file. **VSource** interface class depends on [Frame](https://rapidpixel.constantrobotics.com/docs/Service/Frame.html) class (describes video frame data structures) and [ConfigReader](https://rapidpixel.constantrobotics.com/docs/Service/ConfigReader.html) library (provides methods to read/write JSON config files). The repositories of **Frame** and **ConfigReader** are not part of the **VSource** repository (see [Build and connect to your project](#build-and-connect-to-your-project) section). It uses C++17 standard.



# Versions

**Table 1** - Library versions.

| Version | Release date | What's new                                                   |
| ------- | ------------ | ------------------------------------------------------------ |
| 1.0.0   | 13.06.2023   | First version                                                |
| 1.0.2   | 20.06.2023   | - Fixed bugs.<br />- Documentation updated.                  |
| 1.1.0   | 29.06.2023   | - Added new parameters.<br />- Documentation updated.        |
| 1.1.1   | 29.06.2023   | - Added license.<br />- Repository made public.              |
| 1.2.0   | 01.07.2023   | - Added new methods to encode/decode commands.<br />- Tests updated.<br />- Documentation updated. |
| 1.2.1   | 01.07.2023   | - Params description updated in source code.                 |
| 1.3.0   | 05.07.2023   | - VSourceParams class updated (initString replaced by source).<br />- decode(...) method in VSourceParams class updated. |
| 1.3.1   | 06.07.2023   | - gain, exposure and focusPos fields of VSourceParams excluded from JSON reading/writing. |
| 1.3.2   | 06.07.2023   | - Frame library version updated.                             |
| 1.4.0   | 11.07.2023   | - Added VSourceParamsMask structure.<br />- Added params mask in encode(...) method of VSourceParams class. |
| 1.5.0   | 22.09.2023   | - Updated encode(...) and decode(...) methods of VSourceParams.<br />- Added decodeAndExecuteCommand(...) method.<br />- Added example of video source implementation. |
| 1.6.0   | 26.09.2023   | - Signature of getParams(...) method changed.                |
| 1.6.1   | 13.11.2023   | - Frame class updated.                                       |
| 1.7.1   | 14.12.2023   | - Virtual destructor added.<br />- Frame class updated.      |
| 1.8.1   | 18.12.2023   | - Region of interest params included.                        |
| 1.8.2   | 20.04.2024   | - Frame class updated.<br />- ConfigReader class updated.<br />- Documentation updated. |
| 1.8.3   | 20.05.2024   | - Submodules updated.<br />- Documentation updated.          |
| 1.8.4   | 06.07.2024   | - Submodules updated.<br />- CMake updated.                  |
| 1.8.5   | 28.09.2026   | - Submodules updated.<br />- Compiler warnings fixed.        |
| 1.8.6   | 06.10.2026   | - Deep code review.<br />- Frame class updated to v5.1.1, ConfigReader updated to v2.0.1.<br />- Tests extended. |
| 1.8.7   | 16.10.2026   | - Sub-repositories (Frame, ConfigReader) removed: for a stand alone build they must be cloned next to the **VSource** repository.<br />- **VSource::decodeCommand(...)** checks the exact size of a command (7 bytes for COMMAND, 11 bytes for SET_PARAM) and returns -1 for a param ID or a command ID that is not a value of the enum; output arguments are changed only if the command is valid.<br />- **VSourceParams::decode(...)** rejects data with reserved bits of the mask.<br />- **VSource** class: protected default, copy and move special member functions; **VSourceParams** class: move constructor and move assignment operator.<br />- Example: all methods are thread-safe, parameter values and string lengths are validated, **getFrame(...)** returns FALSE if the video source is closed, **openVSource(...)** sets default params.<br />- Compiler warning options, security hardening options and standard C++ mode in CMake files.<br />- Tests extended.<br />- Documentation updated. |



# Library files

The library supplied by source code only. The user would be given a set of files in the form of a CMake project (repository). The repository does not contain the source code of the libraries it depends on (**Frame** and **ConfigReader**). The repository structure is shown below:

```xml
CMakeLists.txt ----------------- Main CMake file of the library.
example ------------------------ Folder with custom video source class.
    CMakeLists.txt ------------- CMake file for example custom video source class.
    CustomVSource.cpp ---------- C++ implementation file.
    CustomVSource.h ------------ Header with CustomVSource class declaration.
    CustomVSourceVersion.h ----- Header file which includes CustomVSource version.
    CustomVSourceVersion.h.in -- CMake service file to generate version file.
static ------------------------- Folder with images of the documentation.
test --------------------------- Folder with test application.
    CMakeLists.txt ------------- CMake file for test application.
    main.cpp ------------------- Source code file of VSource class test application.
src ---------------------------- Folder with source code of the library.
    CMakeLists.txt ------------- CMake file of the library.
    VSource.cpp ---------------- C++ implementation file.
    VSource.h ------------------ Header file which includes VSource class declaration.
    VSourceVersion.h ----------- Header file which includes version of the library.
    VSourceVersion.h.in -------- CMake service file to generate version file.
```



# Video source interface class description



## VSource class declaration

**VSource** interface class declared in **VSource.h** file. Class declaration:

```cpp
namespace cr
{
namespace video
{
/// Video source interface class.
class VSource
{
public:

    /// Class destructor.
    virtual ~VSource();

    /// Get string of current library version.
    static std::string getVersion();

    /// Open video source.
    virtual bool openVSource(std::string& initString) = 0;

    /// Init video source.
    virtual bool initVSource(VSourceParams& params) = 0;

    /// Get open status.
    virtual bool isVSourceOpen() = 0;

    /// Close video source.
    virtual void closeVSource() = 0;

    /// Get new video frame.
    virtual bool getFrame(Frame& frame, int32_t timeoutMsec = 0) = 0;

    /// Set video source param.
    virtual bool setParam(VSourceParam id, float value) = 0;

    /// Get video source param value.
    virtual float getParam(VSourceParam id) = 0;

    /// Get video source params structure.
    virtual void getParams(VSourceParams& params) = 0;

    /// Execute command.
    virtual bool executeCommand(VSourceCommand id) = 0;

    /// Encode set param command.
    static void encodeSetParamCommand(
            uint8_t* data, int& size, VSourceParam id, float value);

    /// Encode command.
    static void encodeCommand(
            uint8_t* data, int& size, VSourceCommand id);

    /// Decode command.
    static int decodeCommand(uint8_t* data,
                             int size,
                             VSourceParam& paramId,
                             VSourceCommand& commandId,
                             float& value);

    /// Decode and execute command.
    virtual bool decodeAndExecuteCommand(uint8_t* data, int size) = 0;

protected:

    /// Default constructor. The class can be used as a base only.
    VSource() = default;

    /// Copy constructor.
    VSource(const VSource& src) = default;

    /// Move constructor.
    VSource(VSource&& src) = default;

    /// Copy assignment operator.
    VSource& operator= (const VSource& src) = default;

    /// Move assignment operator.
    VSource& operator= (VSource&& src) = default;
};
}
}
```



## getVersion method

The **getVersion()** method returns string of current version of **VSource** class. Particular video source class can have it's own **getVersion()** method. Method declaration:

```cpp
static std::string getVersion();
```

Method can be used without **VSource** class instance:

```cpp
std::cout << "VSource class version: " << VSource::getVersion() << std::endl;
```

Console output:

```bash
VSource class version: 1.8.7
```



## openVSource method

The **openVSource(...)** method initializes video source. Instead of **openVSource(...)** method user can call [initVSource(...)](#initvsource-method) method. Method declaration:

```cpp
virtual bool openVSource(std::string& initString) = 0;
```

| Parameter  | Value                                                        |
| ---------- | ------------------------------------------------------------ |
| initString | Initialization string. Format depends on implementation but it is recommended to keep default format: [video device or ID or file];[width];[height];[fourcc]. Example: "/dev/video0;1920;1080;YUYV". |

**Returns:** TRUE if the video source open or FALSE if not.



## initVSource method

The **initVSource(...)** method initializes video source by set of parameters. Instead of **initVSource(...)** method user can call [openVSource(...)](#openvsource-method) method. Method declaration:

```cpp
virtual bool initVSource(VSourceParams& params) = 0;
```

| Parameter | Value                                                        |
| --------- | ------------------------------------------------------------ |
| params    | VSourceParams structure (see [VSourceParams class](#vsourceparams-class-description) description). The video source should set parameters according to params structure. Particular video source may support not all parameters listed in [VSourceParams class](#vsourceparams-class-description). |

**Returns:** TRUE if the video source initialized or FALSE if not.



## isVSourceOpen method

The **isVSourceOpen()** method returns video source initialization status. Initialization status also included in [VSourceParams class](#vsourceparams-class-description). Method declaration:

```cpp
virtual bool isVSourceOpen() = 0;
```

**Returns:** TRUE if the video source open (initialized) or FALSE if not.



## closeVSource method

The **closeVSource()** method closes video source. Method declaration:

```cpp
virtual void closeVSource() = 0;
```



## getFrame method

The **getFrame(...)** method intended to get input video frame. Video source should support auto re-initialization in case connection loss. Method declaration:

```cpp
virtual bool getFrame(Frame& frame, int32_t timeoutMsec = 0) = 0;
```

| Parameter   | Value                                                        |
| ----------- | ------------------------------------------------------------ |
| frame       | Output video frame (see [Frame](https://rapidpixel.constantrobotics.com/docs/Service/Frame.html) class description). Video source class determines output pixel format. Pixel format can be set in [initVSource(...)](#initvsource-method) or [openVSource(...)](#openvsource-method) methods if particular video source supports it. |
| timeoutMsec | Timeout to wait new frame data:<br/>- timeoutMsec == -1 - Method will wait endlessly until new data arrive.<br/>- timeoutMsec == 0  - Method will only check if new data exist.<br/>- timeoutMsec > 0   - Method will wait new data specified time.<br />**Each video source implementation must provide described behavior.** |

**Returns:** TRUE if new data exists and copied or FALSE if not.



## setParam method

The **setParam(...)** method sets new video source parameters value. The particular implementation of the video source must provide thread-safe **setParam(...)** method call. This means that the **setParam(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual bool setParam(VSourceParam id, float value) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| id        | Video source parameter ID according to [VSourceParam enum](#vsourceparam-enum). |
| value     | Video source parameter value.                                |

**Returns:** TRUE if the parameter was set or FALSE if not.



## getParam method

The **getParam(...)** method designed to obtain video source parameter value. The particular implementation of the video source must provide thread-safe **getParam(...)** method call. This means that the **getParam(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual float getParam(VSourceParam id) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| id        | Video source parameter ID according to [VSourceParam enum](#vsourceparam-enum). |

**Returns:** parameter value or -1 if the parameter doesn't exist in particular video source class.



## getParams method

The **getParams(...)** method designed to obtain [VSourceParams](#vsourceparams-class-description) class which includes all video source parameters. The particular implementation of the video source must provide thread-safe **getParams(...)** method call. This means that the **getParams(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual void getParams(VSourceParams& params) = 0;
```

| Parameter | Description                                       |
| --------- | ------------------------------------------------- |
| params    | [VSourceParams](#vsourceparams-class-description) class object. |



## executeCommand method

The **executeCommand(...)** method designed to execute video source command. The particular implementation of the video source must provide thread-safe **executeCommand(...)** method call. This means that the **executeCommand(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual bool executeCommand(VSourceCommand id) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| id        | Video source command ID according to [VSourceCommand enum](#vsourcecommand-enum). |

**Returns:** TRUE if the command was executed or FALSE if not.



## encodeSetParamCommand method

The **encodeSetParamCommand(...)** static method designed to encode command to change any parameter in remote video source. To control video source remotely, the developer has to design his own protocol and according to it encode the command and deliver it over the communication channel. To simplify this, the **VSource** class contains static methods for encoding the control command. The **VSource** class provides two types of commands: a parameter change command (SET_PARAM) and an action command (COMMAND). **encodeSetParamCommand(...)** designed to encode SET_PARAM command. Method declaration:

```cpp
static void encodeSetParamCommand(uint8_t* data, int& size, VSourceParam id, float value);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to data buffer for encoded command. Must have size >= 11 (the method can not check it). If the pointer is nullptr nothing is encoded and size is 0. |
| size      | Size of encoded data. Will be 11 bytes (0 if data is nullptr). |
| id        | Parameter ID according to [VSourceParam enum](#vsourceparam-enum). |
| value     | Parameter value.                                             |

**encodeSetParamCommand(...)** is static and used without **VSource** class instance. This method used on client side (control system). Command encoding example:

```cpp
// Buffer for encoded data.
uint8_t data[11];
// Size of encoded data.
int size = 0;
// Parameter value.
float outValue = 10.0f;
// Encode command.
VSource::encodeSetParamCommand(data, size, VSourceParam::EXPOSURE, outValue);
```



## encodeCommand method

The **encodeCommand(...)** static method designed to encode command to perform action command in remote video source. To control a video source remotely, the developer has to design his own protocol and according to it encode the command and deliver it over the communication channel. To simplify this, the **VSource** class contains static methods for encoding the control command. The **VSource** class provides two types of commands: a parameter change command (SET_PARAM) and an action command (COMMAND). **encodeCommand(...)** designed to encode COMMAND (action command). Method declaration:

```cpp
static void encodeCommand(uint8_t* data, int& size, VSourceCommand id);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to data buffer for encoded command. Must have size >= 7 bytes (the method can not check it). If the pointer is nullptr nothing is encoded and size is 0. |
| size      | Size of encoded data. Will be 7 bytes (0 if data is nullptr). |
| id        | Command ID according to [VSourceCommand enum](#vsourcecommand-enum). |

**encodeCommand(...)** is static and used without **VSource** class instance. This method used on client side (control system). Command encoding example:

```cpp
// Buffer for encoded data.
uint8_t data[7];
// Size of encoded data.
int size = 0;
// Encode command.
VSource::encodeCommand(data, size, VSourceCommand::RESTART);
```



## decodeCommand method

The **decodeCommand(...)** static method designed to decode command on video source side (edge device). Method declaration:

```cpp
static int decodeCommand(uint8_t* data, int size, VSourceParam& paramId, VSourceCommand& commandId, float& value);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to input command. Must not be nullptr (nullptr gives -1). |
| size      | Size of command. Must be exactly 11 bytes for SET_PARAM and exactly 7 bytes for COMMAND (other sizes give -1). |
| paramId   | Parameter ID according to [VSourceParam enum](#vsourceparam-enum). After decoding SET_PARAM command the method will return parameter ID. An ID that is not a value of the enum gives -1. |
| commandId | Command ID according to [VSourceCommand enum](#vsourcecommand-enum). After decoding COMMAND the method will return command ID. An ID that is not a value of the enum gives -1. |
| value     | Parameter value (after decoding SET_PARAM command, 0 after decoding COMMAND). |

**Returns:** **0** - in case decoding COMMAND, **1** - in case decoding SET_PARAM command or **-1** in case errors (null data, wrong size, version, command type or ID). The output arguments (**paramId**, **commandId** and **value**) are changed only if the method returns 0 or 1. The method checks the version of the library which encoded the command: major and minor numbers must be the same.



## decodeAndExecuteCommand method

The **decodeAndExecuteCommand(...)** method decodes and executes command on video source side. The particular implementation of the video source must provide thread-safe **decodeAndExecuteCommand(...)** method call. This means that the **decodeAndExecuteCommand(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual bool decodeAndExecuteCommand(uint8_t* data, int size) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to input command.                                    |
| size      | Size of command. Must be exactly 11 bytes for SET_PARAM and exactly 7 bytes for COMMAND. |

**Returns:** TRUE if command decoded (SET_PARAM or COMMAND) and executed (action command or set param command) or FALSE if the command is not valid or was not executed.



# Data structures

**VSource.h** file defines IDs for parameters (**VSourceParam** enum) and IDs for commands (**VSourceCommand** enum).



## VSourceCommand enum

Enum declaration:

```cpp
enum class VSourceCommand
{
    /// Restart.
    RESTART = 1
};
```

**Table 2** - Video source commands description. Some commands maybe unsupported by particular video source class.

| Command | Description                                  |
| ------- | -------------------------------------------- |
| RESTART | Restart video source (close and open again). |



## VSourceParam enum

Enum declaration:

```cpp
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
```

**Table 3** - Video source params description. Some params maybe unsupported by particular video source class.

| Parameter      | Access       | Description                                                  |
| -------------- | ------------ | ------------------------------------------------------------ |
| LOG_LEVEL      | read / write | Logging mode. Values: 0 - Disable, 1 - Only file, 2 - Only terminal, 3 - File and terminal. |
| WIDTH          | read / write | Frame width. User can set frame width before initialization or after. Some video source classes may set width automatically. |
| HEIGHT         | read / write | Frame height. User can set frame height before initialization or after. Some video source classes may set height automatically. |
| GAIN_MODE      | read / write | Gain mode. Value depends on implementation but it is recommended to keep default values: 0 - Manual control, 1 - Auto. |
| GAIN           | read / write | Gain value. Value: 0(min for particular video source class) - 65535(max for particular video source class). |
| EXPOSURE_MODE  | read / write | Exposure mode. Value depends on implementation but it is recommended to keep default values: 0 - Manual control, 1 - Auto. |
| EXPOSURE       | read / write | Exposure value. Value: 0(min for particular video source class) - 65535(max for particular video source class). |
| FOCUS_MODE     | read / write | Focus mode. Value depends on implementation but it is recommended to keep default values: 0 - Manual control, 1 - Auto. |
| FOCUS_POS      | read / write | Focus position. Value: 0(full near) - 65535(full far).       |
| CYCLE_TIME_MKS | read only    | Video capture cycle time. **VSource** class sets this value automatically. This parameter means time interval between two captured video frame. |
| FPS            | read / write | FPS. User can set frame FPS before initialization or after. Some video source classes may set FPS automatically. |
| IS_OPEN        | read only    | Open flag. 0 - not open, 1 - open.                           |
| ROI_X          | read / write | Region of interest x coordinate.                             |
| ROI_Y          | read / write | Region of interest y coordinate.                             |
| ROI_WIDTH      | read / write | Region of interest width.                                    |
| ROI_HEIGHT     | read / write | Region of interest height.                                   |
| CUSTOM_1       | read / write | Custom parameter. Depends on implementation.                 |
| CUSTOM_2       | read / write | Custom parameter. Depends on implementation.                 |
| CUSTOM_3       | read / write | Custom parameter. Depends on implementation.                 |



# VSourceParams class description



## VSourceParams class declaration

**VSourceParams** class used for video source initialization (**initVSource(...)** method) or to get all actual params ([getParams(...)](#getparams-method) method method). Also **VSourceParams** provide structure to write/read params from JSON files (**JSON_READABLE** macro, see [ConfigReader](https://rapidpixel.constantrobotics.com/docs/Service/ConfigReader.html) class description) and provide methods to encode and decode params. Class declaration:

```cpp
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

    /// Default constructor.
    VSourceParams() = default;

    /// Copy constructor.
    VSourceParams(const VSourceParams& src) = default;

    /// Move constructor.
    VSourceParams(VSourceParams&& src) = default;

    /// Copy assignment operator.
    VSourceParams& operator= (const VSourceParams& src) = default;

    /// Move assignment operator.
    VSourceParams& operator= (VSourceParams&& src) = default;

    /// Encode params.
    bool encode(uint8_t* data, int bufferSize, int& size,
                VSourceParamsMask* mask = nullptr);

    /// Decode params.
    bool decode(uint8_t* data, int dataSize);
};
```

**Table 4** - VSourceParams class fields description.

| Field        | type   | Description                                                  |
| ------------ | ------ | ------------------------------------------------------------ |
| logLevel     | int    | Logging mode. Values: 0 - Disable, 1 - Only file, 2 - Only terminal, 3 - File and terminal. |
| source       | string | Video source: file, video stream, video device, camera num, etc. |
| fourcc       | string | FOURCC: RGB24, BGR24, YUYV, UYVY, GRAY, YUV24, NV12, NV21, YU12, YV12. Value says to video source class which pixel format preferable for output video frame. Particular video source class can ignore this params during initialization. Parameters should be set before initialization. |
| width        | int    | Frame width. User can set frame width before initialization or after. Some video source classes may set width automatically. |
| height       | int    | Frame height. User can set frame height before initialization or after. Some video source classes may set height automatically. |
| gainMode     | int    | Gain mode. Value depends on implementation but it is recommended to keep default values: 0 - Manual control, 1 - Auto. |
| gain         | int    | Gain value. Value: 0(min for particular video source class) - 65535(max for particular video source class). |
| exposureMode | int    | Exposure mode. Value depends on implementation but it is recommended to keep default values: 0 - Manual control, 1 - Auto. |
| exposure     | int    | Exposure value. Value: 0(min for particular video source class) - 65535(max for particular video source class). |
| focusMode    | int    | Focus mode. Value depends on implementation but it is recommended to keep default values: 0 - Manual control, 1 - Auto. |
| focusPos     | int    | Focus position. Value: 0(full near) - 65535(full far).       |
| cycleTimeMks | int    | Video capture cycle time. **VSource** class sets this value automatically. This parameter means time interval between two captured video frame. |
| fps          | float  | FPS. User can set frame FPS before initialization or after. Some video source classes may set FPS automatically. |
| isOpen       | bool   | Open flag. false - not open, true - open.                    |
| roiX         | int    | Region of interest x coordinate.                             |
| roiY         | int    | Region of interest y coordinate.                             |
| roiWidth     | int    | Region of interest width.                                    |
| roiHeight    | int    | Region of interest height.                                   |
| custom1      | float  | Custom parameter. Depends on implementation.                 |
| custom2      | float  | Custom parameter. Depends on implementation.                 |
| custom3      | float  | Custom parameter. Depends on implementation.                 |

**Note:** *VSourceParams class fields listed in Table 4 **must** reflect params set/get by methods setParam(...) and getParam(...).*

**Note:** *The values of the fields are transported (serialized, deserialized, read from and written to JSON files) without a range check: the video source implementation must validate them before use. The **VSourceParams** class is not thread-safe: an object must not be accessed from several threads at the same time without external synchronization.*



## Serialize video source params

**VSourceParams** class provides method **encode(...)** to serialize video source params (fields of VSourceParams class, see Table 4). Serialization of video source params necessary in case when you need to send video source params via communication channels. Method doesn't encode fields: **source** and **fourcc**. Method provides options to exclude particular parameters from serialization. To do this method inserts binary mask (3 bytes) where each bit represents particular parameter and **decode(...)** method recognizes it. Parameters are stored in the order of the fields of **VSourceParamsMask** structure, the first parameter is the most significant bit of the first byte of the mask. Int and float values are stored as 4 bytes values (see [Portability](#portability) section), bool value is stored as 1 byte. Method declaration:

```cpp
bool encode(uint8_t* data, int bufferSize, int& size, VSourceParamsMask* mask = nullptr);
```

| Parameter  | Value                                                        |
| ---------- | ------------------------------------------------------------ |
| data       | Pointer to data buffer. Must not be nullptr.                 |
| bufferSize | Data buffer size, bytes. Must be at least 7 bytes. If buffer size smaller than required, buffer will be filled with fewer parameters: a parameter that does not fit in the buffer is not encoded (its flag in the mask is not set). If the mask is nullptr the encoding stops at the first parameter that does not fit. |
| size       | Size of encoded data. 79 bytes if all parameters are encoded (0 if the method returns FALSE). |
| mask       | Parameters mask - pointer to **VSourceParamsMask** structure. **VSourceParamsMask** (declared in **VSource.h** file) determines flags for each field (parameter) declared in **VSourceParams** class. If the user wants to exclude any parameters from serialization, he can put a pointer to the mask. If the user wants to exclude a particular parameter from serialization, he should set the corresponding flag in the VSourceParamsMask structure to false (all flags are true by default). If the pointer is nullptr all parameters are encoded. |

**Returns:** TRUE if params encoded or FALSE if not (data is nullptr or bufferSize is smaller than 7 bytes).

**VSourceParamsMask** structure declaration:

```cpp
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
```

Example without parameters mask:

```cpp
// Prepare params.
VSourceParams in;
in.source = "any string";
in.logLevel = 0;

// Encode data.
uint8_t data[1024];
int size = 0;
if (!in.encode(data, 1024, size))
{
    std::cout << "Can't encode data" << std::endl;
    return false;
}
std::cout << "Encoded data size: " << size << " bytes" << std::endl;
```

Example with parameters mask:

```cpp
// Prepare params.
VSourceParams in;
in.source = "any string";
in.logLevel = 0;

// Prepare params mask.
VSourceParamsMask mask;
mask.logLevel = false; // Exclude logLevel. Others by default.

// Encode data.
uint8_t data[1024];
int size = 0;
if (!in.encode(data, 1024, size, &mask))
{
    std::cout << "Can't encode data" << std::endl;
    return false;
}
std::cout << "Encoded data size: " << size << " bytes" << std::endl;
```



## Deserialize video source params

**VSourceParams** class provides method **decode(...)** to deserialize video source params (fields of VSourceParams class, see Table 4). Deserialization of video source params necessary in case when you need to receive video source params via communication channels. Method doesn't decode fields: **source** and **fourcc** (they are cleared). Method automatically recognizes which parameters were serialized by **encode(...)** method. The parameters that were not serialized are set to zero. Method declaration:

```cpp
bool decode(uint8_t* data, int dataSize);
```

| Parameter | Value                                                        |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to encoded data. Must not be nullptr. The params are not changed if the data is not valid (null pointer, too short for the parameters its flags announce, wrong data type, version (major and minor numbers must be the same as in **VSource** class) or reserved bits of the mask). Bytes after the last serialized parameter are ignored. |
| dataSize  | Size of data, bytes.                                         |

**Returns:** TRUE if data decoded (deserialized) or FALSE if not.

Example:

```cpp
// Encode data.
VSourceParams in;
uint8_t data[1024];
int size = 0;
if (!in.encode(data, 1024, size))
{
    std::cout << "Can't encode data" << std::endl;
    return false;
}
std::cout << "Encoded data size: " << size << " bytes" << std::endl;

// Decode data.
VSourceParams out;
if (!out.decode(data, size))
{
    std::cout << "Can't decode data" << std::endl;
    return false;
}
```



## Read params from JSON file and write to JSON file

**VSource** library depends on **ConfigReader** library which provides method to read params from JSON file and to write params to JSON file. The JSON contains the fields: **logLevel**, **source**, **fourcc**, **width**, **height**, **gainMode**, **exposureMode**, **focusMode**, **fps**, **roiX**, **roiY**, **roiWidth**, **roiHeight**, **custom1**, **custom2** and **custom3** (the fields **gain**, **exposure**, **focusPos**, **cycleTimeMks** and **isOpen** are not stored in JSON and have default values after reading). All listed fields must be present in the JSON object: if a field is missing or has a wrong type (for example a string instead of a number or a number that does not fit in int) the **get(...)** method of **ConfigReader** returns FALSE and the params object is not changed. Example of writing and reading params to JSON file:

```cpp
// Write params to file.
VSourceParams in;
in.source = "alsfghljb";
in.fourcc = "BGR24";
in.logLevel = 3;
in.width = 640;
in.height = 480;
in.fps = 30.0f;
cr::utils::ConfigReader inConfig;
if (!inConfig.set(in, "vSourceParams") ||
    !inConfig.writeToFile("TestVSourceParams.json"))
{
    std::cout << "Can't write config file" << std::endl;
    return false;
}

// Read params from file.
cr::utils::ConfigReader outConfig;
if (!outConfig.readFromFile("TestVSourceParams.json"))
{
    std::cout << "Can't open config file" << std::endl;
    return false;
}
VSourceParams out;
if (!outConfig.get(out, "vSourceParams"))
{
    std::cout << "Can't read params from config file" << std::endl;
    return false;
}
```

**TestVSourceParams.json** will look like:

```json
{
    "vSourceParams": {
        "logLevel": 3,
        "source": "alsfghljb",
        "fourcc": "BGR24",
        "width": 640,
        "height": 480,
        "gainMode": 1,
        "exposureMode": 1,
        "focusMode": 1,
        "fps": 30.0,
        "roiX": 0,
        "roiY": 0,
        "roiWidth": 0,
        "roiHeight": 0,
        "custom1": 0.0,
        "custom2": 0.0,
        "custom3": 0.0
    }
}
```



# Portability

The library is written in standard C++17 and does not depend on the operating system. It was built and tested with MSVC 2022 (x64 and Win32), GCC 12 and 13, Clang 15 and 18 (x86-64 Linux) and with GCC and Clang 18 cross compilers for aarch64 (run under qemu). The library has no SIMD code. Serialized params and commands store int and float values as raw 4 bytes values in the byte order of the host: the data encoded on a big-endian host can not be decoded on a little-endian host (all platforms listed above are little-endian). The code checks at compile time that int has 4 bytes and float is a 4 bytes IEEE 754 number.



# Build and connect to your project



## Build stand alone

The repository does not contain sub-repositories. The libraries **VSource** depends on (**Frame** and **ConfigReader**) must be cloned into the same folder as **VSource**. The folder structure must be:

```bash
<work folder>
    VSource
    Frame
    ConfigReader
```

Typical commands to build the library, the test application and the example (use the URLs of the repositories provided to you):

```bash
git clone <VSource repository URL>
git clone <Frame repository URL>
git clone <ConfigReader repository URL>
cd VSource
mkdir build
cd build
cmake ..
make
```

On Windows use `cmake --build . --config Release` instead of `make`. If **VSource** is the top-level CMake project, the main CMakeLists.txt file adds the folders ../Frame and ../ConfigReader (in this order) before the library itself. If a folder does not exist CMake stops with an error.



## CMake options

**Table 5** - CMake options of the repository. In the options below ${PARENT} is the prefix defined by the top-level project (empty if **VSource** is the top-level project).

| Option                              | Default                                           | Description                                                  |
| ----------------------------------- | ------------------------------------------------- | ------------------------------------------------------------ |
| ${PARENT}_VSOURCE                   | ON                                                | Build the library.                                           |
| ${PARENT}_VSOURCE_TEST              | ON (stand alone), OFF (included in other project) | Build the test application.                                  |
| ${PARENT}_VSOURCE_EXAMPLE           | ON (stand alone), OFF (included in other project) | Build the example.                                           |
| ${PARENT}_SUBMODULE_CACHE_OVERWRITE | ON                                                | Forced rewriting of the options above in the CMake cache. The top-level project sets it to OFF when it includes **VSource** as a sub-repository. |
| CMAKE_BUILD_TYPE                    | Release (if not specified)                        | Build type.                                                  |



## Connect to your project

If you want connect **VSource** library to your CMake project as source code you can make follow. When **VSource** is included in another project it does not add its dependencies (**Frame** and **ConfigReader**): your project must add them (before or after **VSource**), otherwise the compilation stops with an error (for example "Frame.h: No such file or directory"). For example, if your repository has structure:

```bash
CMakeLists.txt
src
    CMakeList.txt
    yourLib.h
    yourLib.cpp
```

You can add repositories **VSource**, **Frame** and **ConfigReader** as sub-repositories by commands:

```bash
cd <your repository folder>
git submodule add <VSource repository URL> 3rdparty/VSource
git submodule add <Frame repository URL> 3rdparty/Frame
git submodule add <ConfigReader repository URL> 3rdparty/ConfigReader
git submodule update --init --recursive
```

In your repository folder will be created folders **3rdparty/VSource**, **3rdparty/Frame** and **3rdparty/ConfigReader**. Also you can copy the repository folders in **3rdparty** folder of your repository. Structure of your repository:

```bash
CMakeLists.txt
src
    CMakeList.txt
    yourLib.h
    yourLib.cpp
3rdparty
    ConfigReader
    Frame
    VSource
```

Create CMakeLists.txt file in **3rdparty** folder. CMakeLists.txt should contain:

```cmake
cmake_minimum_required(VERSION 3.13)

################################################################################
## 3RD-PARTY
## dependencies for the project
################################################################################
project(3rdparty LANGUAGES CXX)

################################################################################
## SETTINGS
## basic 3rd-party settings before use
################################################################################
# To inherit the top-level architecture when the project is used as a submodule.
SET(PARENT ${PARENT}_YOUR_PROJECT_3RDPARTY)
# Disable self-overwriting of parameters inside included subdirectories.
SET(${PARENT}_SUBMODULE_CACHE_OVERWRITE OFF CACHE BOOL "" FORCE)

################################################################################
## CONFIGURATION
## 3rd-party submodules configuration
################################################################################
SET(${PARENT}_SUBMODULE_FRAME                           ON  CACHE BOOL "" FORCE)
if (${PARENT}_SUBMODULE_FRAME)
    SET(${PARENT}_FRAME                                 ON  CACHE BOOL "" FORCE)
    SET(${PARENT}_FRAME_TEST                            OFF CACHE BOOL "" FORCE)
endif()

SET(${PARENT}_SUBMODULE_CONFIG_READER                   ON  CACHE BOOL "" FORCE)
if (${PARENT}_SUBMODULE_CONFIG_READER)
    SET(${PARENT}_CONFIG_READER                         ON  CACHE BOOL "" FORCE)
    SET(${PARENT}_CONFIG_READER_EXAMPLES                OFF CACHE BOOL "" FORCE)
    SET(${PARENT}_CONFIG_READER_TESTS                   OFF CACHE BOOL "" FORCE)
endif()

SET(${PARENT}_SUBMODULE_VSOURCE                         ON  CACHE BOOL "" FORCE)
if (${PARENT}_SUBMODULE_VSOURCE)
    SET(${PARENT}_VSOURCE                               ON  CACHE BOOL "" FORCE)
    SET(${PARENT}_VSOURCE_TEST                          OFF CACHE BOOL "" FORCE)
    SET(${PARENT}_VSOURCE_EXAMPLE                       OFF CACHE BOOL "" FORCE)
endif()

################################################################################
## INCLUDING SUBDIRECTORIES
## Adding subdirectories according to the 3rd-party configuration
################################################################################
if (${PARENT}_SUBMODULE_FRAME)
    add_subdirectory(Frame)
endif()

if (${PARENT}_SUBMODULE_CONFIG_READER)
    add_subdirectory(ConfigReader)
endif()

if (${PARENT}_SUBMODULE_VSOURCE)
    add_subdirectory(VSource)
endif()
```

File **3rdparty/CMakeLists.txt** adds folders **Frame**, **ConfigReader** and **VSource** (in dependency order) to your project and excludes test applications and examples (VSource class test application and example of custom video source class implementation) from compiling (by default test application and example are excluded from compiling if **VSource** included as sub-repository). Your repository structure will be:

```bash
CMakeLists.txt
src
    CMakeList.txt
    yourLib.h
    yourLib.cpp
3rdparty
    CMakeLists.txt
    ConfigReader
    Frame
    VSource
```

Next you need include folder 3rdparty in main **CMakeLists.txt** file of your repository. Add string at the end of your main **CMakeLists.txt**:

```cmake
add_subdirectory(3rdparty)
```

Next you have to include VSource library in your **src/CMakeLists.txt** file:

```cmake
target_link_libraries(${PROJECT_NAME} VSource)
```

Done!



# How to make custom implementation

The **VSource** class provides only an interface, data structures, and methods for encoding and decoding commands and params. To create your own implementation of the video source, you must include the VSource repository in your project (see [Build and connect to your project](#build-and-connect-to-your-project) section). The catalogue **example** (see [Library files](#library-files) section) includes an example of the design of the custom video source. You must implement all the methods of the VSource interface class. The example implementation is thread-safe (all methods can be called from any thread), validates the values of params and the length of strings that come from outside (the limits are named constants in **CustomVSource.cpp**) and returns FALSE from **getFrame(...)** if the video source is closed. Custom video source class declaration:

```cpp
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
```