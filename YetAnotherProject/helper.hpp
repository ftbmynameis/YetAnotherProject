#pragma once
#include <stdexcept>
#include <winerror.h>
#include <Windows.h>

// From: https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/Samples/UWP/D3D1211On12/src/DXSampleHelper.h
class HrException : public std::runtime_error
{
public:
    HrException(HRESULT hr);
    HRESULT error() const;
private:
    const HRESULT _hr;
};

std::string hr_to_string(HRESULT hr);
bool throw_if_failed(HRESULT hr);
std::wstring get_assets_path();
std::wstring get_working_dir();
std::wstring get_module_path();