#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "ConfigReader.h"
#include "VSource.h"
#include "VSourceVersion.h"



static_assert(std::is_abstract<cr::video::VSource>::value,
              "VSource must be an abstract interface class");
static_assert(std::has_virtual_destructor<cr::video::VSource>::value,
              "VSource must have a virtual destructor");
static_assert(!std::is_copy_constructible<cr::video::VSource>::value,
              "VSource must be used only as a base class");
static_assert(std::is_nothrow_move_constructible<
              cr::video::VSourceParams>::value,
              "VSourceParams must have a move constructor");



namespace
{
using cr::video::VSource;
using cr::video::VSourceCommand;
using cr::video::VSourceParam;
using cr::video::VSourceParams;
using cr::video::VSourceParamsMask;

/// Number of fields of VSourceParams in the encoded data.
constexpr int FIELDS_NUMBER{19};
/// Size of the header of the encoded params: data type, version, mask.
constexpr int HEADER_SIZE{6};
/// Size of the encoded params with all fields.
constexpr int FULL_SIZE{79};
/// Size of the encoded set param command.
constexpr int SET_PARAM_SIZE{11};
/// Size of the encoded command.
constexpr int COMMAND_SIZE{7};
/// First value of VSourceParam enum.
constexpr int FIRST_PARAM_ID{1};
/// Last value of VSourceParam enum.
constexpr int LAST_PARAM_ID{19};
/// Value of VSourceCommand::RESTART.
constexpr int RESTART_ID{1};
/// Name of the file for the JSON test.
constexpr const char* JSON_FILE_NAME{"TestVSourceParams.json"};
/// Number of the encode/decode iterations with random masks.
constexpr int MASK_ITERATIONS{3000};
/// Number of damaged samples of the damaged data test.
constexpr int DAMAGED_SAMPLES{40000};
/// Value of the output argument of decodeCommand(...) before the call: the
/// tests check that a rejected command does not change it.
constexpr float COMMAND_SENTINEL_VALUE{123.5f};
/// Value of the command ID output argument of decodeCommand(...) before the
/// call (it is not a value of VSourceCommand enum).
constexpr int COMMAND_SENTINEL_ID{77};
/// Value of the param ID output argument of decodeCommand(...) before the call.
constexpr VSourceParam PARAM_SENTINEL_ID{VSourceParam::FPS};



/// State of the test random generator. The tests use own deterministic
/// generator: results must be reproducible on all platforms.
uint32_t g_randomState{20240915U};



/// Get next random 32-bit value.
uint32_t nextRandom()
{
    g_randomState = g_randomState * 1664525U + 1013904223U;
    return g_randomState;
}



/// Get random number in [0, limit).
int randomInt(int limit)
{
    return static_cast<int>((nextRandom() >> 8) %
                            static_cast<uint32_t>(limit));
}



/// Get random bool.
bool randomBool()
{
    return ((nextRandom() >> 16) & 1U) != 0;
}



/// Get random byte.
uint8_t randomByte()
{
    return static_cast<uint8_t>(nextRandom() >> 24);
}



/// Get random 32 bit pattern.
uint32_t randomBits()
{
    const uint32_t high = nextRandom() & 0xFFFF0000U;
    return high | (nextRandom() >> 16);
}



/// Get random int (any value).
int randomIntValue()
{
    const uint32_t bits = randomBits();
    int value = 0;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}



/// Get random float (any bit pattern: NaN, infinity and denormals too).
float randomFloatValue()
{
    const uint32_t bits = randomBits();
    float value = 0.0f;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}



/// Get bits of a float (floats are compared by bits, NaN included).
uint32_t bitsOf(float value)
{
    uint32_t bits = 0;
    std::memcpy(&bits, &value, sizeof(bits));
    return bits;
}



/// Print a failed check and return false.
bool fail(const char* message)
{
    std::cout << message << std::endl;
    return false;
}



/// Run one test and print the result.
bool runTest(const char* title, bool (*test)())
{
    std::cout << title << ":" << std::endl;
    const bool passed = test();
    std::cout << (passed ? "OK" : "ERROR") << std::endl << std::endl;
    return passed;
}



/// File of a test. The file is removed by the destructor if the test did not
/// remove it (a failed removal is reported).
class TestFile
{
public:
    /// Class constructor.
    explicit TestFile(const char* name) : m_name(name)
    {

    }

    /// Class destructor.
    ~TestFile()
    {
        if (exists() && std::remove(m_name.c_str()) != 0)
        {
            std::cout << "Can't remove file " << m_name << std::endl;
        }
    }

    TestFile(const TestFile&) = delete;
    TestFile(TestFile&&) = delete;
    TestFile& operator= (const TestFile&) = delete;
    TestFile& operator= (TestFile&&) = delete;

    /// Get file name.
    const char* name() const
    {
        return m_name.c_str();
    }

    /// Remove the file. Returns TRUE if the file does not exist after the call.
    bool remove()
    {
        return !exists() || std::remove(m_name.c_str()) == 0;
    }

private:

    /// Check that the file exists.
    bool exists() const
    {
        std::ifstream file(m_name, std::ios::binary);
        return file.is_open();
    }

    /// File name.
    std::string m_name;
};



/// Description of a field of VSourceParams in the encoded data. The table of
/// the fields is a reference written independently of the library code: the
/// position of the flag of each field in the mask is given explicitly.
struct FieldInfo
{
    /// Name of the field.
    const char* name{""};
    /// Size of the field in the encoded data: 4 (int, float) or 1 (bool).
    int size{0};
    /// Pointer to the int field or nullptr.
    int VSourceParams::* intField{nullptr};
    /// Pointer to the float field or nullptr.
    float VSourceParams::* floatField{nullptr};
    /// Pointer to the bool field or nullptr.
    bool VSourceParams::* boolField{nullptr};
    /// Pointer to the flag of the field in the mask structure.
    bool VSourceParamsMask::* flag{nullptr};
    /// Index of the byte (in the encoded data) with the flag of the field.
    size_t maskByte{0};
    /// Bit of the flag of the field in the byte.
    uint8_t maskBit{0};
};



/// Create description of an int field.
FieldInfo intInfo(const char* name, int VSourceParams::* field,
                  bool VSourceParamsMask::* flag, size_t maskByte,
                  uint8_t maskBit)
{
    FieldInfo info;
    info.name = name;
    info.size = 4;
    info.intField = field;
    info.flag = flag;
    info.maskByte = maskByte;
    info.maskBit = maskBit;
    return info;
}



/// Create description of a float field.
FieldInfo floatInfo(const char* name, float VSourceParams::* field,
                    bool VSourceParamsMask::* flag, size_t maskByte,
                    uint8_t maskBit)
{
    FieldInfo info;
    info.name = name;
    info.size = 4;
    info.floatField = field;
    info.flag = flag;
    info.maskByte = maskByte;
    info.maskBit = maskBit;
    return info;
}



/// Create description of a bool field.
FieldInfo boolInfo(const char* name, bool VSourceParams::* field,
                   bool VSourceParamsMask::* flag, size_t maskByte,
                   uint8_t maskBit)
{
    FieldInfo info;
    info.name = name;
    info.size = 1;
    info.boolField = field;
    info.flag = flag;
    info.maskByte = maskByte;
    info.maskBit = maskBit;
    return info;
}



/// Get the table of the fields in the order they are stored in the data.
const std::array<FieldInfo, FIELDS_NUMBER>& fieldTable()
{
    static const std::array<FieldInfo, FIELDS_NUMBER> table{{
        intInfo("logLevel", &VSourceParams::logLevel,
                &VSourceParamsMask::logLevel, 3, 128),
        intInfo("width", &VSourceParams::width,
                &VSourceParamsMask::width, 3, 64),
        intInfo("height", &VSourceParams::height,
                &VSourceParamsMask::height, 3, 32),
        intInfo("gainMode", &VSourceParams::gainMode,
                &VSourceParamsMask::gainMode, 3, 16),
        intInfo("gain", &VSourceParams::gain,
                &VSourceParamsMask::gain, 3, 8),
        intInfo("exposureMode", &VSourceParams::exposureMode,
                &VSourceParamsMask::exposureMode, 3, 4),
        intInfo("exposure", &VSourceParams::exposure,
                &VSourceParamsMask::exposure, 3, 2),
        intInfo("focusMode", &VSourceParams::focusMode,
                &VSourceParamsMask::focusMode, 3, 1),
        intInfo("focusPos", &VSourceParams::focusPos,
                &VSourceParamsMask::focusPos, 4, 128),
        intInfo("cycleTimeMks", &VSourceParams::cycleTimeMks,
                &VSourceParamsMask::cycleTimeMks, 4, 64),
        floatInfo("fps", &VSourceParams::fps,
                  &VSourceParamsMask::fps, 4, 32),
        boolInfo("isOpen", &VSourceParams::isOpen,
                 &VSourceParamsMask::isOpen, 4, 16),
        intInfo("roiX", &VSourceParams::roiX,
                &VSourceParamsMask::roiX, 4, 8),
        intInfo("roiY", &VSourceParams::roiY,
                &VSourceParamsMask::roiY, 4, 4),
        intInfo("roiWidth", &VSourceParams::roiWidth,
                &VSourceParamsMask::roiWidth, 4, 2),
        intInfo("roiHeight", &VSourceParams::roiHeight,
                &VSourceParamsMask::roiHeight, 4, 1),
        floatInfo("custom1", &VSourceParams::custom1,
                  &VSourceParamsMask::custom1, 5, 128),
        floatInfo("custom2", &VSourceParams::custom2,
                  &VSourceParamsMask::custom2, 5, 64),
        floatInfo("custom3", &VSourceParams::custom3,
                  &VSourceParamsMask::custom3, 5, 32)
    }};
    return table;
}



/// Compare a field of two objects (floats by bits).
bool sameField(const FieldInfo& field, const VSourceParams& a,
               const VSourceParams& b)
{
    if (field.intField != nullptr)
    {
        return a.*(field.intField) == b.*(field.intField);
    }
    if (field.floatField != nullptr)
    {
        return bitsOf(a.*(field.floatField)) == bitsOf(b.*(field.floatField));
    }
    return a.*(field.boolField) == b.*(field.boolField);
}



/// Set a field to zero.
void zeroField(const FieldInfo& field, VSourceParams& params)
{
    if (field.intField != nullptr)
    {
        params.*(field.intField) = 0;
    }
    else if (field.floatField != nullptr)
    {
        params.*(field.floatField) = 0.0f;
    }
    else
    {
        params.*(field.boolField) = false;
    }
}



/// Copy a field from one object to another.
void copyField(const FieldInfo& field, const VSourceParams& from,
               VSourceParams& to)
{
    if (field.intField != nullptr)
    {
        to.*(field.intField) = from.*(field.intField);
    }
    else if (field.floatField != nullptr)
    {
        to.*(field.floatField) = from.*(field.floatField);
    }
    else
    {
        to.*(field.boolField) = from.*(field.boolField);
    }
}



/// Append a field to the data in the format of the encoded params.
void appendField(const FieldInfo& field, const VSourceParams& params,
                 std::vector<uint8_t>& data)
{
    if (field.boolField != nullptr)
    {
        data.push_back((params.*(field.boolField)) ? 0x01 : 0x00);
        return;
    }
    uint8_t bytes[4];
    if (field.intField != nullptr)
    {
        const int value = params.*(field.intField);
        std::memcpy(bytes, &value, sizeof(bytes));
    }
    else
    {
        const float value = params.*(field.floatField);
        std::memcpy(bytes, &value, sizeof(bytes));
    }
    data.insert(data.end(), bytes, bytes + sizeof(bytes));
}



/// Compare all fields of two objects (the strings are not compared). Prints
/// the names of the different fields.
bool sameFields(const VSourceParams& a, const VSourceParams& b)
{
    bool same = true;
    for (const FieldInfo& field : fieldTable())
    {
        if (!sameField(field, a, b))
        {
            std::cout << "Field " << field.name << " differs" << std::endl;
            same = false;
        }
    }
    return same;
}



/// Compare all fields and the strings of two objects.
bool sameParams(const VSourceParams& a, const VSourceParams& b)
{
    bool same = sameFields(a, b);
    if (a.source != b.source)
    {
        std::cout << "Field source differs" << std::endl;
        same = false;
    }
    if (a.fourcc != b.fourcc)
    {
        std::cout << "Field fourcc differs" << std::endl;
        same = false;
    }
    return same;
}



/// Make params with random values (floats can be NaN, infinity or denormal).
VSourceParams makeRandomParams()
{
    VSourceParams params;
    params.source = "source " + std::to_string(randomInt(1000));
    params.fourcc = "fourcc " + std::to_string(randomInt(1000));
    params.logLevel = randomIntValue();
    params.width = randomIntValue();
    params.height = randomIntValue();
    params.gainMode = randomIntValue();
    params.gain = randomIntValue();
    params.exposureMode = randomIntValue();
    params.exposure = randomIntValue();
    params.focusMode = randomIntValue();
    params.focusPos = randomIntValue();
    params.cycleTimeMks = randomIntValue();
    params.fps = randomFloatValue();
    params.isOpen = randomBool();
    params.roiX = randomIntValue();
    params.roiY = randomIntValue();
    params.roiWidth = randomIntValue();
    params.roiHeight = randomIntValue();
    params.custom1 = randomFloatValue();
    params.custom2 = randomFloatValue();
    params.custom3 = randomFloatValue();
    return params;
}



/// Make params with the values different from the default values of every
/// field. The object is used to check that a rejected call changes nothing.
VSourceParams makeSentinel()
{
    VSourceParams params;
    params.source = "keep source";
    params.fourcc = "keep fourcc";
    params.logLevel = 11;
    params.width = 12;
    params.height = 13;
    params.gainMode = 14;
    params.gain = 15;
    params.exposureMode = 16;
    params.exposure = 17;
    params.focusMode = 18;
    params.focusPos = 19;
    params.cycleTimeMks = 20;
    params.fps = 21.5f;
    params.isOpen = true;
    params.roiX = 22;
    params.roiY = 23;
    params.roiWidth = 24;
    params.roiHeight = 25;
    params.custom1 = 26.5f;
    params.custom2 = 27.5f;
    params.custom3 = 28.5f;
    return params;
}



/// Make a mask with random flags.
VSourceParamsMask makeRandomMask()
{
    VSourceParamsMask mask;
    for (const FieldInfo& field : fieldTable())
    {
        mask.*(field.flag) = randomBool();
    }
    return mask;
}



/// Make a mask with all flags set to FALSE.
VSourceParamsMask makeEmptyMask()
{
    VSourceParamsMask mask;
    for (const FieldInfo& field : fieldTable())
    {
        mask.*(field.flag) = false;
    }
    return mask;
}



/// Reference encoder: the data that VSourceParams::encode(...) must produce.
/// The code does not use the library encoder.
/// @param params Params.
/// @param mask Mask or nullptr (all fields).
/// @param bufferSize Size of the buffer.
std::vector<uint8_t> referenceEncode(const VSourceParams& params,
                                     const VSourceParamsMask* mask,
                                     int bufferSize)
{
    std::vector<uint8_t> data{0x02, VSOURCE_MAJOR_VERSION,
                              VSOURCE_MINOR_VERSION, 0x00, 0x00, 0x00};
    for (const FieldInfo& field : fieldTable())
    {
        if (mask != nullptr && !(mask->*(field.flag)))
        {
            continue;
        }

        // A field that does not fit is not encoded. Without the mask the
        // encoding stops at the first field that does not fit.
        if (bufferSize - static_cast<int>(data.size()) < field.size)
        {
            if (mask == nullptr)
            {
                break;
            }
            continue;
        }
        appendField(field, params, data);
        data[field.maskByte] = static_cast<uint8_t>(data[field.maskByte] |
                                                    field.maskBit);
    }
    return data;
}



/// Make the object that decoding of the data must give: encoded fields are
/// equal to the fields of the source params, other fields are zero, strings
/// are empty.
VSourceParams referenceDecode(const VSourceParams& source,
                              const std::vector<uint8_t>& data)
{
    VSourceParams result;
    result.source.clear();
    result.fourcc.clear();
    for (const FieldInfo& field : fieldTable())
    {
        if ((data[field.maskByte] & field.maskBit) != 0)
        {
            copyField(field, source, result);
        }
        else
        {
            zeroField(field, result);
        }
    }
    return result;
}



/// Encode params with all fields (the data is checked by the callers).
std::vector<uint8_t> encodeAll(VSourceParams& params)
{
    std::vector<uint8_t> data(1024, 0);
    int size = 0;
    if (!params.encode(data.data(), 1024, size))
    {
        size = 0;
    }
    data.resize(static_cast<size_t>(size));
    return data;
}



/// Decode a copy of the data held in a buffer of exactly that size (a read
/// behind the data is found by sanitizers).
bool decodeCopy(VSourceParams& out, const uint8_t* data, size_t size)
{
    std::vector<uint8_t> buffer(data, data + size);
    return out.decode(buffer.empty() ? nullptr : buffer.data(),
                      static_cast<int>(size));
}



/// Decode a command from a buffer of exactly the size of the command.
int decodeCommandCopy(const std::vector<uint8_t>& command,
                      VSourceParam& paramId, VSourceCommand& commandId,
                      float& value)
{
    std::vector<uint8_t> copy = command;
    return VSource::decodeCommand(copy.empty() ? nullptr : copy.data(),
                                  static_cast<int>(copy.size()), paramId,
                                  commandId, value);
}



/// Make a set param command (the layout is written here, it is independent of
/// the library code).
std::vector<uint8_t> makeSetParamCommand(int paramId, float value)
{
    std::vector<uint8_t> data{0x01, VSOURCE_MAJOR_VERSION,
                              VSOURCE_MINOR_VERSION};
    uint8_t bytes[4];
    std::memcpy(bytes, &paramId, sizeof(bytes));
    data.insert(data.end(), bytes, bytes + sizeof(bytes));
    std::memcpy(bytes, &value, sizeof(bytes));
    data.insert(data.end(), bytes, bytes + sizeof(bytes));
    return data;
}



/// Make a command (the layout is written here, it is independent of the
/// library code).
std::vector<uint8_t> makeCommand(int commandId)
{
    std::vector<uint8_t> data{0x00, VSOURCE_MAJOR_VERSION,
                              VSOURCE_MINOR_VERSION};
    uint8_t bytes[4];
    std::memcpy(bytes, &commandId, sizeof(bytes));
    data.insert(data.end(), bytes, bytes + sizeof(bytes));
    return data;
}



/// Check that the bytes behind the data of a buffer have the fill value.
bool isFilled(const std::vector<uint8_t>& buffer, size_t from, uint8_t fill)
{
    return std::all_of(buffer.begin() + static_cast<std::ptrdiff_t>(from),
                       buffer.end(),
                       [fill](uint8_t value) { return value == fill; });
}



/// Decode the command and check the result. The output arguments must not
/// change if the command is rejected.
bool checkCommand(const std::vector<uint8_t>& command, int expected,
                  const char* what)
{
    VSourceParam paramId = PARAM_SENTINEL_ID;
    VSourceCommand commandId = static_cast<VSourceCommand>(COMMAND_SENTINEL_ID);
    float value = COMMAND_SENTINEL_VALUE;
    const int result = decodeCommandCopy(command, paramId, commandId, value);
    if (result != expected)
    {
        std::cout << what << ": result " << result << " instead of "
                  << expected << std::endl;
        return false;
    }
    if (result == -1 && (paramId != PARAM_SENTINEL_ID ||
                         static_cast<int>(commandId) != COMMAND_SENTINEL_ID ||
                         value != COMMAND_SENTINEL_VALUE))
    {
        std::cout << what << ": outputs changed by a rejected command"
                  << std::endl;
        return false;
    }
    return true;
}



/// Copy test: copy and move constructors and assignment operators.
bool copyTest()
{
    const VSourceParams original = makeRandomParams();

    // Copy constructor and copy assignment.
    const VSourceParams copy(original);
    if (!sameParams(original, copy))
    {
        return fail("Copy constructor");
    }
    VSourceParams assigned;
    assigned = original;
    if (!sameParams(original, assigned))
    {
        return fail("Copy assignment");
    }

    // Assignment to itself.
    VSourceParams self = original;
    const VSourceParams& alias = self;
    self = alias;
    if (!sameParams(original, self))
    {
        return fail("Assignment to itself");
    }

    // Move constructor and move assignment.
    VSourceParams source1 = original;
    const VSourceParams moved(std::move(source1));
    if (!sameParams(original, moved))
    {
        return fail("Move constructor");
    }
    VSourceParams source2 = original;
    VSourceParams moveAssigned;
    moveAssigned = std::move(source2);
    if (!sameParams(original, moveAssigned))
    {
        return fail("Move assignment");
    }

    // Default values.
    const VSourceParams defaults;
    if (defaults.logLevel != 0 || defaults.source != "/dev/video0" ||
        defaults.fourcc != "YUYV" || defaults.width != 1920 ||
        defaults.height != 1080 || defaults.gainMode != 1 ||
        defaults.gain != 0 || defaults.exposureMode != 1 ||
        defaults.exposure != 1 || defaults.focusMode != 1 ||
        defaults.focusPos != 0 || defaults.cycleTimeMks != 0 ||
        defaults.fps != 0.0f || defaults.isOpen || defaults.roiX != 0 ||
        defaults.roiY != 0 || defaults.roiWidth != 0 ||
        defaults.roiHeight != 0 || defaults.custom1 != 0.0f ||
        defaults.custom2 != 0.0f || defaults.custom3 != 0.0f)
    {
        return fail("Default values");
    }

    // Mask: all flags are TRUE by default.
    const VSourceParamsMask defaultMask;
    for (const FieldInfo& field : fieldTable())
    {
        if (!(defaultMask.*(field.flag)))
        {
            std::cout << "Mask flag " << field.name << std::endl;
            return false;
        }
    }

    return true;
}



/// Encode/decode test: all fields, the layout of the data.
bool encodeDecodeTest()
{
    for (int i = 0; i < 200; ++i)
    {
        VSourceParams in = makeRandomParams();

        // Encode data and compare with the reference encoder.
        std::vector<uint8_t> data(1024, 0xCC);
        int size = -1;
        if (!in.encode(data.data(), 1024, size))
        {
            return fail("Can't encode data");
        }
        if (size != FULL_SIZE)
        {
            std::cout << "Encoded data size " << size << std::endl;
            return false;
        }
        const std::vector<uint8_t> expected = referenceEncode(in, nullptr,
                                                              1024);
        if (expected.size() != static_cast<size_t>(FULL_SIZE) ||
            std::memcmp(expected.data(), data.data(), expected.size()) != 0)
        {
            return fail("Encoded data differs from the reference");
        }
        if (data[0] != 0x02 || data[1] != VSOURCE_MAJOR_VERSION ||
            data[2] != VSOURCE_MINOR_VERSION || data[3] != 0xFF ||
            data[4] != 0xFF || data[5] != 0xE0)
        {
            return fail("Header of the encoded data");
        }

        // Nothing is written behind the encoded data.
        if (!isFilled(data, static_cast<size_t>(size), 0xCC))
        {
            return fail("Data written behind the encoded data");
        }

        // Decode data: strings are cleared.
        VSourceParams out = makeSentinel();
        if (!decodeCopy(out, data.data(), static_cast<size_t>(size)))
        {
            return fail("Can't decode data");
        }
        if (!out.source.empty() || !out.fourcc.empty())
        {
            return fail("source and fourcc must be cleared");
        }
        if (!sameFields(in, out))
        {
            return false;
        }
    }

    return true;
}



/// Encode and decode with random masks (every fourth call without mask).
bool randomMasksRoundTrip()
{
    for (int i = 0; i < MASK_ITERATIONS; ++i)
    {
        VSourceParams in = makeRandomParams();
        VSourceParamsMask mask = makeRandomMask();
        const bool useMask = (i % 4) != 0;
        std::vector<uint8_t> data(1024, 0xCC);
        int size = -1;
        if (!in.encode(data.data(), 1024, size, useMask ? &mask : nullptr))
        {
            return fail("Can't encode data");
        }
        const std::vector<uint8_t> expected =
                referenceEncode(in, useMask ? &mask : nullptr, 1024);
        if (static_cast<size_t>(size) != expected.size() ||
            std::memcmp(expected.data(), data.data(), expected.size()) != 0)
        {
            std::cout << "Encoded data differs from the reference, size "
                      << size << std::endl;
            return false;
        }
        if (!isFilled(data, expected.size(), 0xCC))
        {
            return fail("Data written behind the encoded data");
        }

        // Decode: selected fields are equal, other fields are zero.
        VSourceParams out = makeSentinel();
        if (!decodeCopy(out, data.data(), static_cast<size_t>(size)))
        {
            return fail("Can't decode data");
        }
        if (!sameParams(referenceDecode(in, expected), out))
        {
            return false;
        }
    }
    return true;
}



/// Encode and decode with a mask that selects no field and every single field.
bool singleFieldRoundTrip()
{
    VSourceParams in = makeRandomParams();
    const VSourceParamsMask none = makeEmptyMask();
    for (int single = -1; single < FIELDS_NUMBER; ++single)
    {
        VSourceParamsMask mask = none;
        int expectedSize = HEADER_SIZE;
        if (single >= 0)
        {
            const FieldInfo& field = fieldTable()[static_cast<size_t>(single)];
            mask.*(field.flag) = true;
            expectedSize += field.size;
        }
        std::vector<uint8_t> data(64, 0xCC);
        int size = -1;
        if (!in.encode(data.data(), 64, size, &mask) || size != expectedSize)
        {
            std::cout << "Single field " << single << ": size " << size
                      << std::endl;
            return false;
        }
        VSourceParams out = makeSentinel();
        if (!decodeCopy(out, data.data(), static_cast<size_t>(size)) ||
            !sameParams(referenceDecode(in, data), out))
        {
            std::cout << "Single field " << single << std::endl;
            return false;
        }
    }
    return true;
}



/// Encode/decode test with params mask.
bool encodeDecodeWithMaskTest()
{
    return randomMasksRoundTrip() && singleFieldRoundTrip();
}



/// Encode, check and decode a set param command.
bool setParamCommandRoundTrip(int paramId, float value)
{
    std::vector<uint8_t> data(16, 0xCC);
    int size = -1;
    VSource::encodeSetParamCommand(data.data(), size,
                                   static_cast<VSourceParam>(paramId), value);
    if (size != SET_PARAM_SIZE)
    {
        std::cout << "Size of set param command " << size << std::endl;
        return false;
    }
    const std::vector<uint8_t> expected = makeSetParamCommand(paramId, value);
    if (std::memcmp(expected.data(), data.data(), expected.size()) != 0)
    {
        return fail("Set param command differs from the reference");
    }
    if (!isFilled(data, expected.size(), 0xCC))
    {
        return fail("Data written behind the command");
    }

    VSourceParam outParam = VSourceParam::LOG_LEVEL;
    VSourceCommand outCommand = VSourceCommand::RESTART;
    float outValue = -5.0f;
    data.resize(static_cast<size_t>(size));
    if (decodeCommandCopy(data, outParam, outCommand, outValue) != 1)
    {
        return fail("Set param command not decoded");
    }
    if (static_cast<int>(outParam) != paramId ||
        bitsOf(outValue) != bitsOf(value))
    {
        return fail("Wrong decoded param ID or value");
    }
    return true;
}



/// Encode, check and decode the command.
bool commandRoundTrip()
{
    std::vector<uint8_t> data(16, 0xCC);
    int size = -1;
    VSource::encodeCommand(data.data(), size, VSourceCommand::RESTART);
    if (size != COMMAND_SIZE)
    {
        std::cout << "Size of command " << size << std::endl;
        return false;
    }
    const std::vector<uint8_t> expected = makeCommand(RESTART_ID);
    if (std::memcmp(expected.data(), data.data(), expected.size()) != 0)
    {
        return fail("Command differs from the reference");
    }
    if (!isFilled(data, expected.size(), 0xCC))
    {
        return fail("Data written behind the command");
    }
    VSourceParam paramId = VSourceParam::LOG_LEVEL;
    VSourceCommand commandId = static_cast<VSourceCommand>(0);
    float value = -5.0f;
    data.resize(static_cast<size_t>(size));
    if (decodeCommandCopy(data, paramId, commandId, value) != 0)
    {
        return fail("Command not decoded");
    }
    if (commandId != VSourceCommand::RESTART || value != 0.0f ||
        paramId != VSourceParam::LOG_LEVEL)
    {
        return fail("Wrong decoded command ID or value");
    }
    return true;
}



/// Encode/decode commands test.
bool encodeDecodeCommandsTest()
{
    // Set param commands: every ID, special and random values.
    const float values[] = {0.0f, -0.0f, 1.0f, -1.5f, 65535.0f, 1.0e30f,
                            std::numeric_limits<float>::infinity(),
                            -std::numeric_limits<float>::infinity(),
                            std::numeric_limits<float>::quiet_NaN(),
                            std::numeric_limits<float>::denorm_min(),
                            std::numeric_limits<float>::max(),
                            std::numeric_limits<float>::lowest()};
    const int specialValues = static_cast<int>(sizeof(values) /
                                               sizeof(values[0]));
    for (int paramId = FIRST_PARAM_ID; paramId <= LAST_PARAM_ID; ++paramId)
    {
        for (int i = 0; i < 2 * specialValues; ++i)
        {
            const float value = i < specialValues ? values[i] :
                                                    randomFloatValue();
            if (!setParamCommandRoundTrip(paramId, value))
            {
                return false;
            }
        }
    }

    return commandRoundTrip();
}



/// Replace the value of a key of the "vSourceParams" object of a JSON text
/// written by ConfigReader (a compact text, the value ends with ',' or '}').
std::string withValue(const std::string& text, const std::string& key,
                      const std::string& value)
{
    const std::string prefix = "\"" + key + "\":";
    const size_t start = text.find(prefix);
    if (start == std::string::npos)
    {
        return text;
    }
    const size_t valueStart = start + prefix.size();
    size_t valueEnd = valueStart;
    bool inString = false;
    while (valueEnd < text.size() && (inString || (text[valueEnd] != ',' &&
                                                   text[valueEnd] != '}')))
    {
        if (inString && text[valueEnd] == '\\')
        {
            // Escaped character of a string.
            ++valueEnd;
        }
        else if (text[valueEnd] == '"')
        {
            inString = !inString;
        }
        ++valueEnd;
    }
    return text.substr(0, valueStart) + value + text.substr(valueEnd);
}



/// Make the params that reading of the JSON written for the params must give:
/// the JSON contains logLevel, source, fourcc, width, height, gainMode,
/// exposureMode, focusMode, fps, roiX, roiY, roiWidth, roiHeight, custom1,
/// custom2 and custom3. The other fields (gain, exposure, focusPos,
/// cycleTimeMks and isOpen) have default values after reading.
VSourceParams makeJsonParams(const VSourceParams& in)
{
    VSourceParams expected;
    expected.source = in.source;
    expected.fourcc = in.fourcc;
    expected.logLevel = in.logLevel;
    expected.width = in.width;
    expected.height = in.height;
    expected.gainMode = in.gainMode;
    expected.exposureMode = in.exposureMode;
    expected.focusMode = in.focusMode;
    expected.fps = in.fps;
    expected.roiX = in.roiX;
    expected.roiY = in.roiY;
    expected.roiWidth = in.roiWidth;
    expected.roiHeight = in.roiHeight;
    expected.custom1 = in.custom1;
    expected.custom2 = in.custom2;
    expected.custom3 = in.custom3;
    return expected;
}



/// Invalid JSON documents are rejected and do not change the params.
bool rejectsInvalidJson(const std::string& text)
{
    // A missing field, a field of a wrong type, a value out of the range of
    // int, not a JSON, a missing object.
    const std::string invalid[] = {
        "{\"vSourceParams\":{}}",
        "{}",
        "not a json",
        "",
        "{\"vSourceParams\":[]}",
        "{\"vSourceParams\":5}",
        withValue(text, "width", "\"text\""),
        withValue(text, "width", "null"),
        withValue(text, "width", "1e30"),
        withValue(text, "width", "4294967296"),
        withValue(text, "fps", "\"1.5\""),
        withValue(text, "source", "5")
    };
    for (const std::string& document : invalid)
    {
        cr::utils::ConfigReader config;
        VSourceParams target = makeSentinel();
        if (config.readFromString(document) &&
            config.get(target, "vSourceParams"))
        {
            std::cout << "Invalid document accepted: " << document
                      << std::endl;
            return false;
        }
        if (!sameParams(target, makeSentinel()))
        {
            return fail("Params changed by an invalid document");
        }
    }
    return true;
}



/// Read params from JSON and write to JSON test.
bool jsonReadWriteTest()
{
    // Write params to file.
    VSourceParams in = makeSentinel();
    in.source = "source with ; space \" and quote";
    in.fourcc = "BGR24";
    TestFile file(JSON_FILE_NAME);
    cr::utils::ConfigReader inConfig;
    if (!inConfig.set(in, "vSourceParams"))
    {
        return fail("Can't set params");
    }
    if (!inConfig.writeToFile(file.name()))
    {
        return fail("Can't write config file");
    }

    // Read params from file.
    cr::utils::ConfigReader outConfig;
    if (!outConfig.readFromFile(file.name()))
    {
        return fail("Can't open config file");
    }
    if (!file.remove())
    {
        return fail("Can't remove config file");
    }
    VSourceParams out;
    if (!outConfig.get(out, "vSourceParams"))
    {
        return fail("Can't read params from file");
    }
    const VSourceParams expected = makeJsonParams(in);
    if (!sameParams(expected, out))
    {
        return false;
    }

    // Write to string and read from string.
    std::string text;
    if (!inConfig.writeToString(text))
    {
        return fail("Can't write config to string");
    }
    cr::utils::ConfigReader textConfig;
    VSourceParams fromText;
    if (!textConfig.readFromString(text) ||
        !textConfig.get(fromText, "vSourceParams") ||
        !sameParams(expected, fromText))
    {
        return fail("Can't read params from string");
    }

    return rejectsInvalidJson(text);
}



/// Check that null data and small sizes are rejected and do not change the
/// params.
bool rejectsSmallAndNullData()
{
    uint8_t byte = 0;
    const int sizes[] = {std::numeric_limits<int>::min(), -100, -1, 0, 1, 2, 3,
                         4, 5};
    for (const int size : sizes)
    {
        VSourceParams out = makeSentinel();
        if (out.decode(&byte, size) || out.decode(nullptr, size) ||
            !sameParams(out, makeSentinel()))
        {
            std::cout << "Size " << size << " accepted" << std::endl;
            return false;
        }
    }

    VSourceParams out = makeSentinel();
    if (out.decode(nullptr, FULL_SIZE) ||
        out.decode(nullptr, std::numeric_limits<int>::max()) ||
        !sameParams(out, makeSentinel()))
    {
        return fail("Null data accepted");
    }
    return true;
}



/// Check that every truncation of the data (full data and random masks) is
/// rejected: the data has a flag for each of its fields.
bool rejectsTruncatedData()
{
    for (int i = 0; i < 60; ++i)
    {
        VSourceParams in = makeRandomParams();
        VSourceParamsMask mask = makeRandomMask();
        std::vector<uint8_t> data(1024, 0);
        int size = 0;
        if (!in.encode(data.data(), 1024, size, i == 0 ? nullptr : &mask))
        {
            return fail("Can't encode data");
        }
        for (int length = 0; length < size; ++length)
        {
            VSourceParams out = makeSentinel();
            if (decodeCopy(out, data.data(), static_cast<size_t>(length)))
            {
                std::cout << "Truncated data decoded, length " << length
                          << " of " << size << std::endl;
                return false;
            }
            if (!sameParams(out, makeSentinel()))
            {
                return fail("Params changed by truncated data");
            }
        }
    }
    return true;
}



/// Check that every wrong value of the data type, the major and the minor
/// version and every reserved bit of the mask is rejected.
bool rejectsInvalidHeader(const std::vector<uint8_t>& data)
{
    for (size_t index = 0; index < 3; ++index)
    {
        for (int value = 0; value < 256; ++value)
        {
            if (value == data[index])
            {
                continue;
            }
            std::vector<uint8_t> bad = data;
            bad[index] = static_cast<uint8_t>(value);
            VSourceParams out = makeSentinel();
            if (decodeCopy(out, bad.data(), bad.size()) ||
                !sameParams(out, makeSentinel()))
            {
                std::cout << "Header byte " << index << " = " << value
                          << " accepted" << std::endl;
                return false;
            }
        }
    }

    for (int bit = 0; bit < 5; ++bit)
    {
        std::vector<uint8_t> bad = data;
        bad[5] = static_cast<uint8_t>(bad[5] | (1U << bit));
        VSourceParams out = makeSentinel();
        if (decodeCopy(out, bad.data(), bad.size()) ||
            !sameParams(out, makeSentinel()))
        {
            std::cout << "Reserved bit " << bit << " accepted" << std::endl;
            return false;
        }
    }
    return true;
}



/// Check that the data with bytes after the last field and non-canonical
/// values of a boolean is decoded: the extra bytes are ignored and any
/// non-zero byte is TRUE.
bool acceptsTrailingBytesAndBooleans(VSourceParams& in,
                                     const std::vector<uint8_t>& data)
{
    for (const size_t extra : {size_t{1}, size_t{4}, size_t{100}})
    {
        std::vector<uint8_t> longer = data;
        longer.resize(data.size() + extra, 0xAB);
        VSourceParams out = makeSentinel();
        if (!decodeCopy(out, longer.data(), longer.size()) ||
            !sameFields(in, out))
        {
            return fail("Data with bytes after the last field");
        }
    }

    VSourceParamsMask isOpenOnly = makeEmptyMask();
    isOpenOnly.isOpen = true;
    std::vector<uint8_t> flagData(16, 0);
    int flagSize = 0;
    if (!in.encode(flagData.data(), 16, flagSize, &isOpenOnly) ||
        flagSize != HEADER_SIZE + 1)
    {
        return fail("Can't encode isOpen");
    }
    for (int value = 0; value < 256; ++value)
    {
        flagData[HEADER_SIZE] = static_cast<uint8_t>(value);
        VSourceParams out = makeSentinel();
        if (!decodeCopy(out, flagData.data(), static_cast<size_t>(flagSize)) ||
            out.isOpen != (value != 0))
        {
            std::cout << "Boolean byte " << value << std::endl;
            return false;
        }
    }
    return true;
}



/// Check that the header only data and the complete data are decoded.
bool acceptsHeaderOnlyAndCompleteData(const VSourceParams& in,
                                      const std::vector<uint8_t>& data)
{
    const std::vector<uint8_t> header{0x02, VSOURCE_MAJOR_VERSION,
                                      VSOURCE_MINOR_VERSION, 0, 0, 0};
    VSourceParams empty = makeSentinel();
    if (!decodeCopy(empty, header.data(), header.size()) ||
        !sameParams(referenceDecode(in, header), empty))
    {
        return fail("Header only data");
    }

    VSourceParams out = makeSentinel();
    if (!decodeCopy(out, data.data(), data.size()) || !sameFields(in, out))
    {
        return fail("Valid data not decoded");
    }
    return true;
}



/// Decode of truncated and invalid data.
bool decodeInvalidDataTest()
{
    VSourceParams in = makeRandomParams();
    const std::vector<uint8_t> data = encodeAll(in);
    if (data.size() != static_cast<size_t>(FULL_SIZE))
    {
        return fail("Can't encode data");
    }

    return rejectsSmallAndNullData() && rejectsTruncatedData() &&
           rejectsInvalidHeader(data) &&
           acceptsTrailingBytesAndBooleans(in, data) &&
           acceptsHeaderOnlyAndCompleteData(in, data);
}



/// Encode into a buffer of the given size and check the result.
bool checkEncodeToBuffer(VSourceParams& in, VSourceParamsMask& mask,
                         bool useMask, int bufferSize)
{
    VSourceParamsMask* maskPtr = useMask ? &mask : nullptr;

    // Guard bytes behind the buffer must not be written.
    std::vector<uint8_t> buffer(static_cast<size_t>(bufferSize) + 16, 0xA5);
    int size = -1;
    const bool result = in.encode(buffer.data(), bufferSize, size, maskPtr);
    if (bufferSize < HEADER_SIZE + 1)
    {
        // Rejected: size is 0, nothing is written.
        if (!isFilled(buffer, 0, 0xA5))
        {
            return fail("Data written by a rejected call");
        }
        if (result || size != 0)
        {
            std::cout << "Small buffer accepted " << bufferSize << std::endl;
            return false;
        }
        return true;
    }

    if (!isFilled(buffer, static_cast<size_t>(bufferSize), 0xA5))
    {
        std::cout << "Written behind the buffer, size " << bufferSize
                  << std::endl;
        return false;
    }
    const std::vector<uint8_t> expected =
            referenceEncode(in, maskPtr, bufferSize);
    if (!result || static_cast<size_t>(size) != expected.size() ||
        std::memcmp(expected.data(), buffer.data(), expected.size()) != 0)
    {
        std::cout << "Wrong data for buffer " << bufferSize << ", size "
                  << size << std::endl;
        return false;
    }

    // The encoded data is decoded: encoded fields are equal.
    VSourceParams out = makeSentinel();
    if (!decodeCopy(out, buffer.data(), static_cast<size_t>(size)) ||
        !sameParams(referenceDecode(in, expected), out))
    {
        std::cout << "Encoded data not decoded, buffer " << bufferSize
                  << std::endl;
        return false;
    }

    // The data fits exactly into the buffer of its size. The data that has the
    // header only (6 bytes) does not: the minimum size of the buffer is 7
    // bytes.
    std::vector<uint8_t> exact(static_cast<size_t>(size), 0);
    int exactSize = -1;
    const bool exactResult = in.encode(exact.data(), size, exactSize, maskPtr);
    if (size > HEADER_SIZE)
    {
        if (!exactResult || exactSize != size ||
            std::memcmp(exact.data(), buffer.data(),
                        static_cast<size_t>(size)) != 0)
        {
            return fail("Encoding into a buffer of the exact size");
        }
    }
    else if (exactResult || exactSize != 0)
    {
        return fail("Encoding into a buffer of 6 bytes");
    }
    return true;
}



/// Encode into buffers of every size.
bool encodeBufferSizeTest()
{
    for (int i = 0; i < 30; ++i)
    {
        VSourceParams in = makeRandomParams();
        VSourceParamsMask mask = makeRandomMask();
        for (int withMask = 0; withMask < 2; ++withMask)
        {
            for (int bufferSize = 0; bufferSize <= FULL_SIZE + 10; ++bufferSize)
            {
                if (!checkEncodeToBuffer(in, mask, withMask == 1, bufferSize))
                {
                    return false;
                }
            }
        }
    }

    // No buffer.
    VSourceParams in;
    int size = -1;
    if (in.encode(nullptr, 100, size) || size != 0)
    {
        return fail("Null buffer accepted");
    }
    size = -1;
    VSourceParamsMask mask;
    if (in.encode(nullptr, 0, size, &mask) || size != 0)
    {
        return fail("Null buffer accepted (mask)");
    }

    return true;
}



/// Check that commands of a wrong size are rejected: commands have a fixed
/// size.
bool rejectsWrongCommandSizes(const std::vector<uint8_t>& setParam,
                              const std::vector<uint8_t>& command)
{
    for (size_t size = 0; size <= 64; ++size)
    {
        if (size != static_cast<size_t>(SET_PARAM_SIZE))
        {
            std::vector<uint8_t> data = setParam;
            data.resize(size, 0);
            if (!checkCommand(data, -1, "Set param command of a wrong size"))
            {
                return false;
            }
        }
        if (size != static_cast<size_t>(COMMAND_SIZE))
        {
            std::vector<uint8_t> data = command;
            data.resize(size, 0);
            if (!checkCommand(data, -1, "Command of a wrong size"))
            {
                return false;
            }
        }
    }

    // No data, negative sizes.
    VSourceParam paramId = PARAM_SENTINEL_ID;
    VSourceCommand commandId = static_cast<VSourceCommand>(COMMAND_SENTINEL_ID);
    float value = COMMAND_SENTINEL_VALUE;
    std::vector<uint8_t> copy = setParam;
    if (VSource::decodeCommand(nullptr, SET_PARAM_SIZE, paramId, commandId,
                               value) != -1 ||
        VSource::decodeCommand(copy.data(), -1, paramId, commandId, value)
        != -1 ||
        VSource::decodeCommand(copy.data(),
                               std::numeric_limits<int>::min(), paramId,
                               commandId, value) != -1 ||
        VSource::decodeCommand(nullptr, 0, paramId, commandId, value) != -1)
    {
        return fail("Null data or negative size accepted");
    }
    return true;
}



/// Check that every wrong data type and every wrong version is rejected.
bool rejectsWrongCommandHeader(const std::vector<uint8_t>& setParam,
                               const std::vector<uint8_t>& command)
{
    // Every value except 0 (command) and 1 (set param) is a wrong data type.
    for (int type = 2; type < 256; ++type)
    {
        std::vector<uint8_t> data = setParam;
        data[0] = static_cast<uint8_t>(type);
        std::vector<uint8_t> shortData = command;
        shortData[0] = static_cast<uint8_t>(type);
        if (!checkCommand(data, -1, "Unknown data type") ||
            !checkCommand(shortData, -1, "Unknown data type (command size)"))
        {
            return false;
        }
    }

    // Every wrong value of the major and the minor version.
    for (size_t index = 1; index <= 2; ++index)
    {
        for (int version = 0; version < 256; ++version)
        {
            if (version == setParam[index])
            {
                continue;
            }
            std::vector<uint8_t> data = setParam;
            data[index] = static_cast<uint8_t>(version);
            std::vector<uint8_t> shortData = command;
            shortData[index] = static_cast<uint8_t>(version);
            if (!checkCommand(data, -1, "Wrong version") ||
                !checkCommand(shortData, -1, "Wrong version (command)"))
            {
                return false;
            }
        }
    }
    return true;
}



/// Check that only the values of the enums are accepted as IDs.
bool checksCommandIds()
{
    const int paramIds[] = {std::numeric_limits<int>::min(), -1000, -1, 0,
                            FIRST_PARAM_ID, FIRST_PARAM_ID + 1,
                            LAST_PARAM_ID - 1, LAST_PARAM_ID,
                            LAST_PARAM_ID + 1, 20, 21, 255, 256, 1000,
                            std::numeric_limits<int>::max()};
    for (const int paramId : paramIds)
    {
        const bool valid = paramId >= FIRST_PARAM_ID &&
                           paramId <= LAST_PARAM_ID;
        const std::vector<uint8_t> data = makeSetParamCommand(paramId, 2.5f);
        if (!checkCommand(data, valid ? 1 : -1, "Set param ID"))
        {
            return false;
        }

        // The decoded param ID and value, the command ID is not changed.
        VSourceParam outParam = PARAM_SENTINEL_ID;
        VSourceCommand outCommand =
                static_cast<VSourceCommand>(COMMAND_SENTINEL_ID);
        float outValue = COMMAND_SENTINEL_VALUE;
        if (valid &&
            (decodeCommandCopy(data, outParam, outCommand, outValue) != 1 ||
             static_cast<int>(outParam) != paramId || outValue != 2.5f ||
             static_cast<int>(outCommand) != COMMAND_SENTINEL_ID))
        {
            return fail("Wrong outputs of a set param command");
        }
    }

    const int commandIds[] = {std::numeric_limits<int>::min(), -1, 0,
                              RESTART_ID, RESTART_ID + 1, 3, 255, 256, 1000,
                              std::numeric_limits<int>::max()};
    for (const int commandId : commandIds)
    {
        const bool valid = commandId == RESTART_ID;
        if (!checkCommand(makeCommand(commandId), valid ? 0 : -1,
                          "Command ID"))
        {
            return false;
        }
    }
    return true;
}



/// Check that the encoding of commands without a buffer gives size 0 and
/// that the encoding into buffers of the exact size works.
bool checksCommandBuffers(const std::vector<uint8_t>& setParam,
                          const std::vector<uint8_t>& command)
{
    int size = 5;
    VSource::encodeCommand(nullptr, size, VSourceCommand::RESTART);
    if (size != 0)
    {
        return fail("encodeCommand with null buffer");
    }
    size = 5;
    VSource::encodeSetParamCommand(nullptr, size, VSourceParam::FPS, 1.0f);
    if (size != 0)
    {
        return fail("encodeSetParamCommand with null buffer");
    }

    std::vector<uint8_t> exactSetParam(static_cast<size_t>(SET_PARAM_SIZE));
    VSource::encodeSetParamCommand(exactSetParam.data(), size,
                                   VSourceParam::FPS, 30.0f);
    std::vector<uint8_t> exactCommand(static_cast<size_t>(COMMAND_SIZE));
    int commandSize = 0;
    VSource::encodeCommand(exactCommand.data(), commandSize,
                           VSourceCommand::RESTART);
    if (size != SET_PARAM_SIZE || commandSize != COMMAND_SIZE ||
        exactSetParam != setParam || exactCommand != command)
    {
        return fail("Encoding into buffers of the exact size");
    }
    return true;
}



/// Decode of invalid commands.
bool decodeInvalidCommandsTest()
{
    const std::vector<uint8_t> setParam = makeSetParamCommand(
            static_cast<int>(VSourceParam::FPS), 30.0f);
    const std::vector<uint8_t> command = makeCommand(RESTART_ID);

    // Valid commands.
    if (!checkCommand(setParam, 1, "Set param command") ||
        !checkCommand(command, 0, "Command"))
    {
        return false;
    }

    return rejectsWrongCommandSizes(setParam, command) &&
           rejectsWrongCommandHeader(setParam, command) &&
           checksCommandIds() && checksCommandBuffers(setParam, command);
}



/// Damage a copy of the data by replacing bytes with probability 0.1 %, 0.5 %
/// or 2 % per byte.
void replaceBytes(std::vector<uint8_t>& data)
{
    const int permille[] = {1, 5, 20};
    const int rate = permille[randomInt(3)];
    for (uint8_t& value : data)
    {
        if (randomInt(1000) < rate)
        {
            value = randomByte();
        }
    }
    // At least one byte in one of four cases.
    if (!data.empty() && randomInt(4) == 0)
    {
        data[static_cast<size_t>(randomInt(static_cast<int>(data.size())))] =
                randomByte();
    }
}



/// Damage a copy of the data by duplicating a piece or by swapping two
/// pieces.
void duplicateOrSwapPieces(std::vector<uint8_t>& data)
{
    if (data.size() < 4)
    {
        return;
    }
    const size_t half = data.size() / 2;
    const size_t from = static_cast<size_t>(randomInt(static_cast<int>(half)));
    const size_t length = 1 + static_cast<size_t>(
                randomInt(static_cast<int>(half)));
    if (randomBool())
    {
        const std::vector<uint8_t> piece(
                    data.begin() + static_cast<std::ptrdiff_t>(from),
                    data.begin() + static_cast<std::ptrdiff_t>(from + length));
        data.insert(data.end(), piece.begin(), piece.end());
        return;
    }
    const size_t to = half + static_cast<size_t>(randomInt(
                static_cast<int>(data.size() - half - length + 1)));
    for (size_t i = 0; i < length && to + i < data.size(); ++i)
    {
        std::swap(data[from + i], data[to + i]);
    }
}



/// Damage a copy of the data: replace bytes, truncate, duplicate a piece,
/// reorder pieces or append bytes.
std::vector<uint8_t> damage(const std::vector<uint8_t>& original)
{
    std::vector<uint8_t> data = original;
    switch (randomInt(6))
    {
    case 0:
    case 1:
    case 2:
    {
        replaceBytes(data);
        break;
    }
    case 3:
    {
        // Truncate to 10 %, 50 % or 90 % of the size.
        const int percent[] = {10, 50, 90};
        data.resize(data.size() * static_cast<size_t>(percent[randomInt(3)]) /
                    100);
        break;
    }
    case 4:
    {
        duplicateOrSwapPieces(data);
        break;
    }
    default:
    {
        // Append random bytes.
        const int count = 1 + randomInt(16);
        for (int i = 0; i < count; ++i)
        {
            data.push_back(randomByte());
        }
        break;
    }
    }
    return data;
}



/// Check decoding of damaged params: the result is deterministic, a rejected
/// call changes nothing and accepted data gives an object that is encoded and
/// decoded back to the same object. Returns 1 if the data was accepted, 0 if
/// rejected and -1 if a check failed.
int checkDamagedParams(const std::vector<uint8_t>& data)
{
    VSourceParams first = makeSentinel();
    VSourceParams second = makeSentinel();
    const bool firstResult = decodeCopy(first, data.data(), data.size());
    const bool secondResult = decodeCopy(second, data.data(), data.size());
    if (firstResult != secondResult || !sameParams(first, second))
    {
        fail("Decoding of damaged params is not deterministic");
        return -1;
    }
    if (!firstResult)
    {
        if (!sameParams(first, makeSentinel()))
        {
            fail("Params changed by damaged data");
            return -1;
        }
        return 0;
    }

    // The decoded object is encoded and decoded back.
    const std::vector<uint8_t> encoded = encodeAll(first);
    VSourceParams again = makeSentinel();
    if (encoded.size() != static_cast<size_t>(FULL_SIZE) ||
        !decodeCopy(again, encoded.data(), encoded.size()) ||
        !sameFields(first, again))
    {
        fail("Decoded params are not encoded and decoded back");
        return -1;
    }
    return 1;
}



/// Check decoding of a damaged command: the result and the outputs are
/// deterministic and valid. Returns 1 if the data was accepted, 0 if rejected
/// and -1 if a check failed.
int checkDamagedCommand(const std::vector<uint8_t>& data)
{
    VSourceParam param1 = PARAM_SENTINEL_ID;
    VSourceParam param2 = PARAM_SENTINEL_ID;
    VSourceCommand command1 = static_cast<VSourceCommand>(COMMAND_SENTINEL_ID);
    VSourceCommand command2 = static_cast<VSourceCommand>(COMMAND_SENTINEL_ID);
    float value1 = COMMAND_SENTINEL_VALUE;
    float value2 = COMMAND_SENTINEL_VALUE;
    const int result1 = decodeCommandCopy(data, param1, command1, value1);
    const int result2 = decodeCommandCopy(data, param2, command2, value2);
    if (result1 != result2 || param1 != param2 || command1 != command2 ||
        bitsOf(value1) != bitsOf(value2))
    {
        fail("Decoding of damaged command is not deterministic");
        return -1;
    }
    if (result1 == -1)
    {
        if (param1 != PARAM_SENTINEL_ID ||
            static_cast<int>(command1) != COMMAND_SENTINEL_ID ||
            value1 != COMMAND_SENTINEL_VALUE)
        {
            fail("Outputs changed by a damaged command");
            return -1;
        }
        return 0;
    }
    if ((result1 != 0 && result1 != 1) ||
        (result1 == 1 && (static_cast<int>(param1) < FIRST_PARAM_ID ||
                          static_cast<int>(param1) > LAST_PARAM_ID)) ||
        (result1 == 0 && command1 != VSourceCommand::RESTART))
    {
        fail("Wrong result or ID of a damaged command");
        return -1;
    }
    return 1;
}



/// Damaged data test: decoding of damaged params and commands.
bool damagedDataTest()
{
    // Valid samples of params (all fields and random masks) and commands.
    std::vector<std::vector<uint8_t>> samples;
    for (int i = 0; i < 40; ++i)
    {
        VSourceParams in = makeRandomParams();
        VSourceParamsMask mask = makeRandomMask();
        std::vector<uint8_t> data(1024, 0);
        int size = 0;
        if (!in.encode(data.data(), 1024, size, i == 0 ? nullptr : &mask))
        {
            return fail("Can't encode data");
        }
        data.resize(static_cast<size_t>(size));
        samples.push_back(data);
    }
    const size_t paramSamples = samples.size();
    for (int paramId = FIRST_PARAM_ID; paramId <= LAST_PARAM_ID; ++paramId)
    {
        samples.push_back(makeSetParamCommand(paramId, randomFloatValue()));
    }
    samples.push_back(makeCommand(RESTART_ID));

    int accepted = 0;
    for (int i = 0; i < DAMAGED_SAMPLES; ++i)
    {
        const size_t index = static_cast<size_t>(
                    randomInt(static_cast<int>(samples.size())));
        const std::vector<uint8_t> data = damage(samples[index]);
        const int result = index < paramSamples ? checkDamagedParams(data) :
                                                  checkDamagedCommand(data);
        if (result < 0)
        {
            return false;
        }
        accepted += result;
    }
    std::cout << DAMAGED_SAMPLES << " damaged samples, " << accepted
              << " accepted" << std::endl;

    return true;
}



/// Version test: the version string is made of the version macros.
bool versionTest()
{
    const std::string expected = std::to_string(VSOURCE_MAJOR_VERSION) + "." +
                                 std::to_string(VSOURCE_MINOR_VERSION) + "." +
                                 std::to_string(VSOURCE_PATCH_VERSION);
    std::cout << "VSource version: " << VSource::getVersion() << std::endl;
    return VSource::getVersion() == expected &&
           std::string(VSOURCE_VERSION) == expected;
}
}



/// Entry point.
int main()
{
    std::cout << "#####################################" << std::endl;
    std::cout << "#                                   #" << std::endl;
    std::cout << "# VSource test                      #" << std::endl;
    std::cout << "#                                   #" << std::endl;
    std::cout << "#####################################" << std::endl;
    std::cout << std::endl;

    // Every test runs even if a previous test failed.
    bool allPassed = true;
    allPassed = runTest("Copy test", copyTest) && allPassed;
    allPassed = runTest("Encode/Decode test", encodeDecodeTest) && allPassed;
    allPassed = runTest("Encode/Decode test with params mask",
                        encodeDecodeWithMaskTest) && allPassed;
    allPassed = runTest("Encode/Decode commands test",
                        encodeDecodeCommandsTest) && allPassed;
    allPassed = runTest("JSON read/write test", jsonReadWriteTest) &&
                allPassed;
    allPassed = runTest("Decode invalid data test", decodeInvalidDataTest) &&
                allPassed;
    allPassed = runTest("Encode buffer size test", encodeBufferSizeTest) &&
                allPassed;
    allPassed = runTest("Decode invalid commands test",
                        decodeInvalidCommandsTest) && allPassed;
    allPassed = runTest("Damaged data test", damagedDataTest) && allPassed;
    allPassed = runTest("Version test", versionTest) && allPassed;

    std::cout << (allPassed ? "ALL TESTS PASSED" : "TESTS FAILED")
              << std::endl;
    return allPassed ? 0 : 1;
}
