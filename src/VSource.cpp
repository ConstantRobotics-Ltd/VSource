#include "VSource.h"
#include "VSourceVersion.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <utility>



namespace
{
// Encoded data stores int and float values as raw 4 bytes values in the byte
// order of the host.
static_assert(sizeof(int) == 4, "int must be 4 bytes");
static_assert(sizeof(float) == 4, "float must be 4 bytes");
static_assert(std::numeric_limits<float>::is_iec559, "float must be IEEE 754");

/// Size of an int or float value in the encoded data, bytes.
constexpr int VALUE_SIZE{4};
/// Size of a bool value in the encoded data, bytes.
constexpr int FLAG_SIZE{1};
/// Encoded value of a TRUE bool.
constexpr uint8_t FLAG_TRUE{0x01};
/// Encoded value of a FALSE bool.
constexpr uint8_t FLAG_FALSE{0x00};

/// Data type of an encoded command.
constexpr uint8_t COMMAND_DATA_TYPE{0x00};
/// Data type of an encoded set param command.
constexpr uint8_t SET_PARAM_DATA_TYPE{0x01};
/// Data type of encoded params.
constexpr uint8_t PARAMS_DATA_TYPE{0x02};

/// Position of the ID in an encoded command: behind the data type and the
/// version (major and minor).
constexpr int COMMAND_ID_POS{3};
/// Size of an encoded command: data type, version, command ID.
constexpr int COMMAND_SIZE{COMMAND_ID_POS + VALUE_SIZE};
/// Position of the value in an encoded set param command.
constexpr int SET_PARAM_VALUE_POS{COMMAND_SIZE};
/// Size of an encoded set param command: data type, version, param ID, value.
constexpr int SET_PARAM_COMMAND_SIZE{SET_PARAM_VALUE_POS + VALUE_SIZE};
static_assert(COMMAND_SIZE == 7, "wrong size of encoded command");
static_assert(SET_PARAM_COMMAND_SIZE == 11,
              "wrong size of encoded set param command");

/// Number of VSourceParams fields in the encoded params.
constexpr int FIELDS_NUMBER{19};
/// Number of the fields stored as a bool (FLAG_SIZE): isOpen.
constexpr int FLAG_FIELDS_NUMBER{1};
/// Number of bits in a byte of the mask.
constexpr int BITS_IN_BYTE{8};
/// Bit of the flag of the first field in a byte of the mask: the most
/// significant bit.
constexpr uint8_t FIRST_MASK_BIT{0x80};
/// Position of the mask in encoded params: behind the data type and the
/// version (major and minor).
constexpr int PARAMS_MASK_POS{3};
/// Size of the mask in encoded params: one bit per field, bytes.
constexpr int PARAMS_MASK_SIZE{3};
static_assert(PARAMS_MASK_SIZE * BITS_IN_BYTE >= FIELDS_NUMBER,
              "mask must have a bit for each field");
/// Size of the header of encoded params: data type, version, mask.
constexpr int PARAMS_HEADER_SIZE{PARAMS_MASK_POS + PARAMS_MASK_SIZE};
/// Minimum size of the buffer for encode(...): the header and one byte.
constexpr int PARAMS_MIN_BUFFER_SIZE{PARAMS_HEADER_SIZE + 1};
/// Reserved bits of the mask (the bits behind the flag of the last field).
/// The bits must be zero.
constexpr uint8_t RESERVED_MASK_BITS{static_cast<uint8_t>(
        (1U << (PARAMS_MASK_SIZE * BITS_IN_BYTE - FIELDS_NUMBER)) - 1U)};
/// Maximum size of encoded params (all fields encoded).
constexpr int PARAMS_MAX_SIZE{PARAMS_HEADER_SIZE +
                              (FIELDS_NUMBER - FLAG_FIELDS_NUMBER) *
                              VALUE_SIZE + FLAG_FIELDS_NUMBER * FLAG_SIZE};
static_assert(PARAMS_MAX_SIZE == 79, "wrong size of encoded params");

/// First value of VSourceParam enum.
constexpr int FIRST_PARAM_ID{
        static_cast<int>(cr::video::VSourceParam::LOG_LEVEL)};
/// Last value of VSourceParam enum.
constexpr int LAST_PARAM_ID{
        static_cast<int>(cr::video::VSourceParam::CUSTOM_3)};
static_assert(FIRST_PARAM_ID == 1 && LAST_PARAM_ID == 19,
              "wrong range of VSourceParam");
/// First value of VSourceCommand enum.
constexpr int FIRST_COMMAND_ID{
        static_cast<int>(cr::video::VSourceCommand::RESTART)};
/// Last value of VSourceCommand enum.
constexpr int LAST_COMMAND_ID{
        static_cast<int>(cr::video::VSourceCommand::RESTART)};



/// Location of a VSourceParams field in the encoded data.
struct FieldRef
{
    /// Pointer to the field of a VSourceParams object.
    void* value{nullptr};
    /// Size of the field in the encoded data: VALUE_SIZE or FLAG_SIZE.
    int size{0};
    /// Pointer to the flag of the field in VSourceParamsMask.
    bool cr::video::VSourceParamsMask::* flag{nullptr};
};



/// Get the table of VSourceParams fields in the order they are stored in the
/// encoded data (the same order as the bits of the mask).
std::array<FieldRef, FIELDS_NUMBER> makeFieldTable(
        cr::video::VSourceParams& params)
{
    return {{
        {&params.logLevel, VALUE_SIZE,
         &cr::video::VSourceParamsMask::logLevel},
        {&params.width, VALUE_SIZE,
         &cr::video::VSourceParamsMask::width},
        {&params.height, VALUE_SIZE,
         &cr::video::VSourceParamsMask::height},
        {&params.gainMode, VALUE_SIZE,
         &cr::video::VSourceParamsMask::gainMode},
        {&params.gain, VALUE_SIZE,
         &cr::video::VSourceParamsMask::gain},
        {&params.exposureMode, VALUE_SIZE,
         &cr::video::VSourceParamsMask::exposureMode},
        {&params.exposure, VALUE_SIZE,
         &cr::video::VSourceParamsMask::exposure},
        {&params.focusMode, VALUE_SIZE,
         &cr::video::VSourceParamsMask::focusMode},
        {&params.focusPos, VALUE_SIZE,
         &cr::video::VSourceParamsMask::focusPos},
        {&params.cycleTimeMks, VALUE_SIZE,
         &cr::video::VSourceParamsMask::cycleTimeMks},
        {&params.fps, VALUE_SIZE,
         &cr::video::VSourceParamsMask::fps},
        {&params.isOpen, FLAG_SIZE,
         &cr::video::VSourceParamsMask::isOpen},
        {&params.roiX, VALUE_SIZE,
         &cr::video::VSourceParamsMask::roiX},
        {&params.roiY, VALUE_SIZE,
         &cr::video::VSourceParamsMask::roiY},
        {&params.roiWidth, VALUE_SIZE,
         &cr::video::VSourceParamsMask::roiWidth},
        {&params.roiHeight, VALUE_SIZE,
         &cr::video::VSourceParamsMask::roiHeight},
        {&params.custom1, VALUE_SIZE,
         &cr::video::VSourceParamsMask::custom1},
        {&params.custom2, VALUE_SIZE,
         &cr::video::VSourceParamsMask::custom2},
        {&params.custom3, VALUE_SIZE,
         &cr::video::VSourceParamsMask::custom3}
    }};
}



/// Get the position (in the encoded params) of the mask byte which contains
/// the flag of the field.
size_t maskPos(int field)
{
    return static_cast<size_t>(PARAMS_MASK_POS) +
           static_cast<size_t>(field / BITS_IN_BYTE);
}



/// Get the bit of the flag of the field in its mask byte. The first field of
/// a mask byte has the most significant bit.
uint8_t maskBit(int field)
{
    return static_cast<uint8_t>(FIRST_MASK_BIT >> (field % BITS_IN_BYTE));
}



/// Copy the field to the encoded data. The size of a copy is a constant: the
/// copy is inlined.
void writeField(const FieldRef& field, uint8_t* data)
{
    if (field.size == FLAG_SIZE)
    {
        *data = *static_cast<const bool*>(field.value) ? FLAG_TRUE : FLAG_FALSE;
    }
    else
    {
        std::memcpy(data, field.value, VALUE_SIZE);
    }
}



/// Copy the field from the encoded data. Any non-zero byte is a TRUE bool.
void readField(const FieldRef& field, const uint8_t* data)
{
    if (field.size == FLAG_SIZE)
    {
        *static_cast<bool*>(field.value) = *data != FLAG_FALSE;
    }
    else
    {
        std::memcpy(field.value, data, VALUE_SIZE);
    }
}



/// Set the field to zero.
void clearField(const FieldRef& field)
{
    if (field.size == FLAG_SIZE)
    {
        *static_cast<bool*>(field.value) = false;
    }
    else
    {
        std::memset(field.value, 0, VALUE_SIZE);
    }
}



/// Check that the ID read from a command is a value of VSourceParam enum.
bool isParamId(int id)
{
    return id >= FIRST_PARAM_ID && id <= LAST_PARAM_ID;
}



/// Check that the ID read from a command is a value of VSourceCommand enum.
bool isCommandId(int id)
{
    return id >= FIRST_COMMAND_ID && id <= LAST_COMMAND_ID;
}
}



cr::video::VSource::~VSource() = default;



std::string cr::video::VSource::getVersion()
{
    return VSOURCE_VERSION;
}



bool cr::video::VSourceParams::encode(uint8_t* data, int bufferSize, int& size,
                                      VSourceParamsMask* mask)
{
    // Check buffer.
    if (data == nullptr || bufferSize < PARAMS_MIN_BUFFER_SIZE)
    {
        size = 0;
        return false;
    }

    // Fill header. The flag of a field in the mask is set after the field is
    // copied to the buffer.
    data[0] = PARAMS_DATA_TYPE;
    data[1] = static_cast<uint8_t>(VSOURCE_MAJOR_VERSION);
    data[2] = static_cast<uint8_t>(VSOURCE_MINOR_VERSION);
    std::memset(&data[PARAMS_MASK_POS], 0,
                static_cast<size_t>(PARAMS_MASK_SIZE));

    // Encode fields in the order of the mask bits.
    const auto fields = makeFieldTable(*this);
    int pos = PARAMS_HEADER_SIZE;
    for (int i = 0; i < FIELDS_NUMBER; ++i)
    {
        const FieldRef& field = fields[static_cast<size_t>(i)];

        // Skip the fields excluded by the mask.
        if (mask != nullptr && !(mask->*(field.flag)))
        {
            continue;
        }

        // A field that does not fit in the buffer is not encoded. Without a
        // mask the encoding stops at the first field that does not fit.
        if (bufferSize - pos < field.size)
        {
            if (mask == nullptr)
            {
                break;
            }
            continue;
        }

        writeField(field, &data[pos]);
        pos += field.size;
        data[maskPos(i)] = static_cast<uint8_t>(data[maskPos(i)] |
                                                maskBit(i));
    }

    size = pos;

    return true;
}



bool cr::video::VSourceParams::decode(uint8_t* data, int dataSize)
{
    // Check data size (the header must be complete) and data type.
    if (data == nullptr || dataSize < PARAMS_HEADER_SIZE)
    {
        return false;
    }
    if (data[0] != PARAMS_DATA_TYPE)
    {
        return false;
    }

    // Check version. The version (major and minor) is a part of the format.
    if (data[1] != VSOURCE_MAJOR_VERSION || data[2] != VSOURCE_MINOR_VERSION)
    {
        return false;
    }

    // Reserved bits of the mask are zero: a set bit announces a field that
    // this decoder does not know.
    if ((data[PARAMS_MASK_POS + PARAMS_MASK_SIZE - 1] &
         RESERVED_MASK_BITS) != 0)
    {
        return false;
    }

    // Decode into a temporary object: this object is changed only if the
    // whole data is valid. The fields that are not encoded are set to zero.
    VSourceParams result;
    const auto fields = makeFieldTable(result);
    int pos = PARAMS_HEADER_SIZE;
    for (int i = 0; i < FIELDS_NUMBER; ++i)
    {
        const FieldRef& field = fields[static_cast<size_t>(i)];
        const bool encoded = (data[maskPos(i)] & maskBit(i)) != 0;
        if (!encoded)
        {
            clearField(field);
            continue;
        }

        // Check that the field is inside the data.
        if (dataSize - pos < field.size)
        {
            return false;
        }
        readField(field, &data[pos]);
        pos += field.size;
    }

    // Strings are not encoded.
    result.source.clear();
    result.fourcc.clear();
    *this = std::move(result);

    return true;
}



void cr::video::VSource::encodeSetParamCommand(
        uint8_t* data, int& size, VSourceParam id, float value)
{
    // Check buffer.
    if (data == nullptr)
    {
        size = 0;
        return;
    }

    // Fill header.
    data[0] = SET_PARAM_DATA_TYPE;
    data[1] = static_cast<uint8_t>(VSOURCE_MAJOR_VERSION);
    data[2] = static_cast<uint8_t>(VSOURCE_MINOR_VERSION);

    // Fill data.
    const int paramId = static_cast<int>(id);
    std::memcpy(&data[COMMAND_ID_POS], &paramId, VALUE_SIZE);
    std::memcpy(&data[SET_PARAM_VALUE_POS], &value, VALUE_SIZE);
    size = SET_PARAM_COMMAND_SIZE;
}



void cr::video::VSource::encodeCommand(
        uint8_t* data, int& size, VSourceCommand id)
{
    // Check buffer.
    if (data == nullptr)
    {
        size = 0;
        return;
    }

    // Fill header.
    data[0] = COMMAND_DATA_TYPE;
    data[1] = static_cast<uint8_t>(VSOURCE_MAJOR_VERSION);
    data[2] = static_cast<uint8_t>(VSOURCE_MINOR_VERSION);

    // Fill data.
    const int commandId = static_cast<int>(id);
    std::memcpy(&data[COMMAND_ID_POS], &commandId, VALUE_SIZE);
    size = COMMAND_SIZE;
}



int cr::video::VSource::decodeCommand(
        uint8_t* data, int size, VSourceParam& paramId,
        VSourceCommand& commandId, float& value)
{
    // Check data and size of the shortest command.
    if (data == nullptr || size < COMMAND_SIZE)
    {
        return -1;
    }

    // Check version. The version (major and minor) is a part of the format.
    if (data[1] != VSOURCE_MAJOR_VERSION || data[2] != VSOURCE_MINOR_VERSION)
    {
        return -1;
    }

    // Extract ID.
    int rawId = 0;
    std::memcpy(&rawId, &data[COMMAND_ID_POS], VALUE_SIZE);

    // Check command type, exact size of the command and the ID. The output
    // arguments are changed only if the command is valid.
    if (data[0] == COMMAND_DATA_TYPE)
    {
        if (size != COMMAND_SIZE || !isCommandId(rawId))
        {
            return -1;
        }
        commandId = static_cast<VSourceCommand>(rawId);
        value = 0.0f;
        return 0;
    }
    if (data[0] == SET_PARAM_DATA_TYPE)
    {
        if (size != SET_PARAM_COMMAND_SIZE || !isParamId(rawId))
        {
            return -1;
        }
        paramId = static_cast<VSourceParam>(rawId);
        std::memcpy(&value, &data[SET_PARAM_VALUE_POS], VALUE_SIZE);
        return 1;
    }

    return -1;
}
