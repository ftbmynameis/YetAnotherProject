#include "Helper.hpp"

HrException::HrException(HRESULT hr) : std::runtime_error(hr_to_string(hr)), _hr(hr) {}
HRESULT HrException::error() const { return _hr; }

std::string hr_to_string(HRESULT hr)
{
    char s_str[64] = {};
    sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<unsigned int>(hr));
    return std::string(s_str);
}

void throw_if_failed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw HrException(hr);
    }
}

std::wstring get_assets_path()
{
    return get_module_path();
}

std::wstring get_working_dir()
{
    const unsigned int buffer_size = 512;
    WCHAR buffer[buffer_size];
    DWORD size = GetCurrentDirectory(buffer_size, buffer);
    if (size == 0 || size == buffer_size)
    {
        throw std::exception();
    }
    return std::wstring(buffer);
}

std::wstring get_module_path()
{
    const unsigned int buffer_size = 512;
    WCHAR buffer[buffer_size];
    DWORD size = GetModuleFileName(nullptr, buffer, buffer_size);
    if (size == 0 || size == buffer_size)
    {
        throw std::exception();
    }

    // cut off exe file name
    WCHAR* lastSlash = wcsrchr(buffer, L'\\');
    if (lastSlash)
    {
        *(lastSlash + 1) = L'\0';
    }

    return std::wstring(buffer);
}