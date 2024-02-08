#pragma once

#include <cstdint>
#include <array>

#include "error.hpp"

namespace pci
{
    const uint16_t kConfigAddress = 0xcf8;
    const uint16_t kConfigData = 0x0cfc;

    struct ClassCode
    {
        uint8_t base, sub, interface;

        bool Match(uint8_t b)
        {
            return b == base;
        }
        bool Match(uint8_t b, uint8_t s)
        {
            return Match(b) && s == sub;
        }
        bool Match(uint8_t b, uint8_t s, uint8_t i)
        {
            return Match(b, s) && i == interface;
        }
    };

    struct Device
    {
        uint8_t bus, device, function, header_type;
        ClassCode class_code;
    };

    void WriteAddress(uint32_t address);
    void WriteData(uint32_t value);
    uint32_t ReadData();

    uint16_t ReadVendorId(uint8_t bus, uint8_t device, uint8_t function);
    uint16_t ReadDeviceId(uint8_t bus, uint8_t device, uint8_t function);
    uint8_t ReadHeaderType(uint8_t bus, uint8_t device, uint8_t function);
    ClassCode ReadClassCode(uint8_t bus, uint8_t device, uint8_t function);

    inline uint16_t ReadVendorId(const Device &d)
    {
        return ReadVendorId(d.bus, d.device, d.function);
    }

    uint32_t ReadConfReg(const Device &d, uint8_t reg_addr);

    void WriteConfReg(const Device &d, uint8_t reg_addr, uint32_t value);

    /**
     * - 32-24: 無効
     * - 23-16: サブオーディネイトバス番号
     * - 15-8: セカンダリバス番号
     * - 7:0 リビジョン番号
     */
    uint32_t ReadBusNumbers(uint8_t bus, uint8_t device, uint8_t function);

    bool IsSingleFunctionDevice(uint8_t header_type);

    inline std::array<Device, 32> devices;
    inline int num_device;

    Error ScanAllBus();

    constexpr uint8_t CalcBarAddress(unsigned int bar_index)
    {
        return 0x10 + 4 * bar_index;
    }

    WithError<uint64_t> ReadBar(Device &device, unsigned int bar_index);
} // namespace pci
