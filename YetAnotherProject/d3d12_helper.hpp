#pragma once

#include <cstdint>
// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using namespace Microsoft::WRL;
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include "d3dx12.h"

ComPtr<ID3D12Device> create_device(IDXGIFactory4* factory);
void get_hardware_adapter(IDXGIFactory4* factory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter);
ComPtr<ID3D12CommandQueue> create_command_queue(ID3D12Device* device);
ComPtr<IDXGISwapChain3> create_swap_chain(IDXGIFactory4* factory, HWND hwnd, UINT width, UINT height, UINT buffer_count, ID3D12CommandQueue* command_queue);
ComPtr<ID3D12DescriptorHeap> create_rtv_heap(ID3D12Device* device, UINT frame_count);
ComPtr<ID3D12CommandAllocator> create_command_allocator(ID3D12Device* device);