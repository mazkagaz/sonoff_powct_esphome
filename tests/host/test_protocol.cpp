#include <doctest/doctest.h>

#include "cse7761_core_protocol.h"

using namespace cse7761_core;

TEST_CASE("write frame for a zero value has no payload") {
    uint8_t frame[5] = {};
    CHECK(build_write_frame(0x00, 0x0000, frame) == 2);
    CHECK(frame[0] == 0xA5);
    CHECK(frame[1] == 0x00);
}

TEST_CASE("write frame uses one byte below 0xFF") {
    uint8_t frame[5] = {};
    CHECK(build_write_frame(0x00, 0x0012, frame) == 4);
    CHECK(frame[0] == 0xA5);
    CHECK(frame[1] == 0x00);
    CHECK(frame[2] == 0x12);
    CHECK(frame[3] == 0x48);  // ~(0xA5 + 0x00 + 0x12)
}

TEST_CASE("write frame uses two bytes for 0xFF and larger values") {
    uint8_t frame[5] = {};
    CHECK(build_write_frame(0x00, 0x00FF, frame) == 5);
    CHECK(frame[0] == 0xA5);
    CHECK(frame[1] == 0x00);
    CHECK(frame[2] == 0x00);
    CHECK(frame[3] == 0xFF);
    CHECK(frame[4] == 0x5B);  // ~(0xA5 + 0x00 + 0x00 + 0xFF)

    CHECK(build_write_frame(0x00, 0x1234, frame) == 5);
    CHECK(frame[2] == 0x12);
    CHECK(frame[3] == 0x34);
    CHECK(frame[4] == 0x14);  // ~(0xA5 + 0x00 + 0x12 + 0x34)
}

TEST_CASE("read frame validation accepts a valid checksum") {
    const uint8_t data[] = {0x12, 0x34};
    CHECK(validate_read_frame(0x00, data, 2, 0x14));  // ~(0xA5 + 0x00 + 0x12 + 0x34)
    CHECK_FALSE(validate_read_frame(0x00, data, 2, 0x15));
}

TEST_CASE("big-endian decoding") {
    const uint8_t data24[] = {0x12, 0x34, 0x56};
    const uint8_t data32[] = {0x12, 0x34, 0x56, 0x78};
    CHECK(decode_be(data24, 3) == 0x123456);
    CHECK(decode_be(data32, 4) == 0x12345678);
}

TEST_CASE("24-bit sign extension") {
    CHECK(sign_extend_24(0x7FFFFF) == 8388607);
    CHECK(sign_extend_24(0x800000) == -8388608);
    CHECK(sign_extend_24(0xFFFFFF) == -1);
}

TEST_CASE("checksum is the one's complement of the sum") {
    const uint8_t frame[] = {0xA5, 0x00, 0x12};
    CHECK(checksum(frame, 3) == 0x48);
}
