#pragma once

#include <cstdint>
// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using namespace Microsoft::WRL;
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include "d3dx12.h"

class ConstantBufferBase;

// These functions are only as generic as they need to be for the current use cases. I.e. mostly for a convinient way of wrapping the call with all the parameters into a function.
// be aware of hardcoded flags and types set in descriptor parameters!!

ComPtr<ID3D12Device> create_device(IDXGIFactory4* factory);
void get_hardware_adapter(IDXGIFactory4* factory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter);
ComPtr<ID3D12CommandQueue> create_command_queue(ID3D12Device* device);
ComPtr<IDXGISwapChain3> create_swap_chain(IDXGIFactory4* factory, HWND hwnd, UINT width, UINT height, UINT buffer_count, ID3D12CommandQueue* command_queue);
ComPtr<ID3D12DescriptorHeap> create_rtv_heap(ID3D12Device* device, UINT frame_count);
ComPtr<ID3D12CommandAllocator> create_command_allocator(ID3D12Device* device);
ComPtr<ID3D12Resource> create_commited_resource(ID3D12Device* device, UINT64 width);
ComPtr<ID3D12DescriptorHeap> create_descriptor_heap(ID3D12Device* device, UINT num_heaps, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
void create_constant_buffer_view(ID3D12Device* device, ConstantBufferBase* const_buffer);
// Default root signature initializes a const buffer that is only visible to the vertex shader at slot 0
ComPtr<ID3D12RootSignature> create_default_root_signature(ID3D12Device* device);

// From DXSample(s)
// Assign a name to the object to aid with debugging.
#if defined(_DEBUG) || defined(DBG)
inline void SetName(ID3D12Object* pObject, LPCWSTR name)
{
    pObject->SetName(name);
}
inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index)
{
    WCHAR fullName[50];
    if (swprintf_s(fullName, L"%s[%u]", name, index) > 0)
    {
        pObject->SetName(fullName);
    }
}
#else
inline void SetName(ID3D12Object*, LPCWSTR)
{
}
inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT)
{
}
#endif

// Naming helper for ComPtr<T>.
// Assigns the name of the variable as the name of the object.
// The indexed variant will include the index in the name of the object.
#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)