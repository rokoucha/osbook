#pragma once

#include <cstdio>
#include <array>

class Error
{
public:
    enum Code
    {
        kSuccess,
        kFull,
        kEmpty,
        kNoEnoughMemory,
        kIndexOutOfRange,
        kHostControllerNotHalted,
        kInvalidSlotID,
        kPortNotConnected,
        kInvalidEndpointNumber,
        kTransferRingNotSet,
        kAlreadyAllocated,
        kNotImplemented,
        kInvalidDescriptor,
        kBufferTooSmall,
        kUnknownDevice,
        kNoCorrespondingSetupStage,
        kTransferFailed,
        kInvalidPhase,
        kUnknownXHCISpeedID,
        kNoWaiter,
        kLasstOfCode,
    };

private:
    Code code_;
    int line_;
    const char *file_;

    static constexpr std::array code_names_ = {
        "kSuccess",
        "kFull",
        "kEmpty",
        "kNoEnoughMemory",
        "kIndexOutOfRange",
        "kHostContollerNotHalted",
        "kInvalidSlotID",
        "kPortNotConnected",
        "kInvalidEndpointNumber",
        "kTransferRingNotSet",
        "kAlreadyAllocated",
        "kNotImplemented",
        "kInvalidDescriptor",
        "kBufferTooSmall",
        "kUnknownDevice",
        "kNoCorrespondingSetupStage",
        "kTransferFailed",
        "kInvalidPhase",
        "kUnknownXHCISpeedID",
        "kNoWriter",
    };
    static_assert(Error::Code::kLasstOfCode == code_names_.size());

public:
    Error(Code code, const char *file, int line)
        : code_{code}, line_{line}, file_{file}
    {
    }

    Code Cause() const
    {
        return this->code_;
    }

    operator bool() const
    {
        return this->code_ != kSuccess;
    }

    const char *Name() const
    {
        return code_names_[static_cast<int>(this->code_)];
    }

    const char *File()
    {
        return this->file_;
    }

    int Line()
    {
        return this->line_;
    }
};

#define MAKE_ERROR(code) Error((code), __FILE__, __LINE__)

template <class T>
struct WithError
{
    T value;
    Error error;
};
