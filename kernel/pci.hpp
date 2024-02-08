#pragma once

#include <cstdint>
#include <array>

#include "error.hpp"

namespace pci
{
    const uint16_t kConfigAddress = 0xcf8;
    const uint16_t kConfigData = 0x0cfc;

    void WriteAddress(uint32_t address);
    void WriteData(uint32_t value);
    uint32_t ReadData();

    uint16_t ReadVendorId(uint8_t bus, uint8_t device, uint8_t function);
    uint16_t ReadDeviceId(uint8_t bus, uint8_t device, uint8_t function);
    uint8_t ReadHeaderType(uint8_t bus, uint8_t device, uint8_t function);

    /**
     * - 32-24: ベースクラス
     * - 23-16: サブクラス
     * - 15-8: インターフェース
     * - 7-0: リビジョン
     */
    uint32_t ReadClassCode(uint8_t bus, uint8_t device, uint8_t function);

    /**
     * - 32-24: 無効
     * - 23-16: サブオーディネイトバス番号
     * - 15-8: セカンダリバス番号
     * - 7:0 リビジョン番号
     */
    uint32_t ReadBusNumbers(uint8_t bus, uint8_t device, uint8_t function);

    bool IsSingleFunctionDevice(uint8_t header_type);

    struct Device
    {
        uint8_t bus, device, function, header_type;
    };

    inline std::array<Device, 32> devices;
    inline int num_device;

    Error ScanAllBus();
} // namespace pci
