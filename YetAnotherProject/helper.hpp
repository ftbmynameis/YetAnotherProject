#pragma once
#include <stdexcept>
#include <winerror.h>

// From: https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/Samples/UWP/D3D1211On12/src/DXSampleHelper.h
inline std::string hr_to_string(HRESULT hr)
{
    char s_str[64] = {};
    sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<unsigned int>(hr));
    return std::string(s_str);
}

class HrException : public std::runtime_error
{
public:
    HrException(HRESULT hr) : std::runtime_error(hr_to_string(hr)), _hr(hr) {}
    HRESULT error() const { return _hr; }
private:
    const HRESULT _hr;
};

inline void throw_if_failed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw HrException(hr);
    }
}