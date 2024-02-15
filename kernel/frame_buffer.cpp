#include "frame_buffer.hpp"

#include <string.h>

namespace
{
    int BytesPerPixel(PixelFormat format)
    {
        switch (format)
        {
        case kPixelRGBResv8BitPerColor:
            return 4;

        case kPixelBGRResv8BitPerColor:
            return 4;

        default:
            return -1;
        }
    }

    uint8_t *FrameAddressAt(Vector2D<int> pos, const FrameBufferConfig &config)
    {
        return config.frame_buffer + BytesPerPixel(config.pixel_format) * (config.pixels_per_scan_line * pos.y + pos.x);
    }

    int BytesPerScanLine(const FrameBufferConfig &config)
    {
        return BytesPerPixel(config.pixel_format) * config.pixels_per_scan_line;
    }

    Vector2D<int> FrameBufferSize(const FrameBufferConfig &config)
    {
        return {static_cast<int>(config.horizontal_resolution), static_cast<int>(config.vertical_resolution)};
    }
}

Error FrameBuffer::Initialize(const FrameBufferConfig &config)
{
    config_ = config;

    const auto bytes_per_pixel = BytesPerPixel(config_.pixel_format);
    if (bytes_per_pixel <= 0)
    {
        return MAKE_ERROR(Error::kUnknownPixelFormat);
    }

    if (config_.frame_buffer)
    {
        buffer_.resize(0);
    }
    else
    {
        buffer_.resize(bytes_per_pixel * config_.horizontal_resolution * config_.vertical_resolution);
        config_.frame_buffer = buffer_.data();
        config_.pixels_per_scan_line = config_.horizontal_resolution;
    }

    switch (config_.pixel_format)
    {
    case kPixelRGBResv8BitPerColor:
        writer_ = std::make_unique<RGBResv8BitPerColorPixelWriter>(config_);
        break;

    case kPixelBGRResv8BitPerColor:
        writer_ = std::make_unique<BGRResv8BitPerColorPixelWriter>(config_);
        break;

    default:
        return MAKE_ERROR(Error::kUnknownPixelFormat);
    }

    return MAKE_ERROR(Error::kSuccess);
}

Error FrameBuffer::Copy(Vector2D<int> dest_pos, const FrameBuffer &src, const Rectangle<int> &src_area)
{
    if (config_.pixel_format != src.config_.pixel_format)
    {
        return MAKE_ERROR(Error::kUnknownPixelFormat);
    }

    const auto bytes_per_pixel = BytesPerPixel(config_.pixel_format);
    if (bytes_per_pixel <= 0)
    {
        return MAKE_ERROR(Error::kUnknownPixelFormat);
    }

    const Rectangle<int> src_area_shifted{dest_pos, src_area.size};
    const Rectangle<int> src_outline{dest_pos - src_area.pos, FrameBufferSize(src.config_)};
    const Rectangle<int> dst_outline{{0, 0}, FrameBufferSize(config_)};
    const auto copy_area = dst_outline & src_outline & src_area_shifted;
    const auto src_start_pos = copy_area.pos - (dest_pos - src_area.pos);

    uint8_t *dest_buf = FrameAddressAt(copy_area.pos, config_);
    const uint8_t *src_buf = FrameAddressAt(src_start_pos, src.config_);

    for (int y = 0; y < copy_area.size.y; y++)
    {
        memcpy(dest_buf, src_buf, bytes_per_pixel * copy_area.size.x);
        dest_buf += BytesPerScanLine(config_);
        src_buf += BytesPerScanLine(src.config_);
    }

    return MAKE_ERROR(Error::kSuccess);
}

void FrameBuffer::Move(Vector2D<int> dest_pos, const Rectangle<int> &src)
{
    const auto bytes_per_pixel = BytesPerPixel(config_.pixel_format);
    const auto bytes_per_scan_line = BytesPerScanLine(config_);

    // move up
    if (dest_pos.y < src.pos.y)
    {
        uint8_t *dest_buf = FrameAddressAt(dest_pos, config_);
        const uint8_t *src_buf = FrameAddressAt(src.pos, config_);
        for (int y = 0; y < src.size.y; y++)
        {
            memcpy(dest_buf, src_buf, bytes_per_pixel * src.size.x);
            dest_buf += bytes_per_scan_line;
            src_buf += bytes_per_scan_line;
        }
    }

    // move down
    else
    {
        uint8_t *dest_buf = FrameAddressAt(dest_pos + Vector2D<int>{0, src.size.y - 1}, config_);
        const uint8_t *src_buf = FrameAddressAt(src.pos + Vector2D<int>{0, src.size.y - 1}, config_);
        for (int y = 0; y < src.size.y; y++)
        {
            memcpy(dest_buf, src_buf, bytes_per_pixel * src.size.x);
            dest_buf -= bytes_per_scan_line;
            src_buf -= bytes_per_scan_line;
        }
    }
}
