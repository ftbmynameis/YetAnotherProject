#pragma once
#include "GraphicContext.hpp"

#include "d3dx12.h"
#include "helper.hpp"

ComPtr<ID3D12Device> create_device(IDXGIFactory4* factory);
void get_hardware_adapter(IDXGIFactory4* factory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter);
ComPtr<ID3D12CommandQueue> create_command_queue(ID3D12Device* device);
ComPtr<IDXGISwapChain3> create_swap_chain(IDXGIFactory4* factory, HWND hwnd, UINT width, UINT height, UINT buffer_count, ID3D12CommandQueue* command_queue);
ComPtr<ID3D12DescriptorHeap> create_rtv_heap(ID3D12Device* device, UINT frame_count);
ComPtr<ID3D12CommandAllocator> create_command_allocator(ID3D12Device* device);


GraphicContext::GraphicContext(HWND hwnd, UINT width, UINT height)
    : _hwnd(hwnd), _width(width), _height(height)
{
}

void GraphicContext::initialize()
{
#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
        }
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    throw_if_failed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory)));

    _device = create_device(factory.Get());
    _command_queue = create_command_queue(_device.Get());
    _swap_chain = create_swap_chain(factory.Get(), _hwnd, _width, _height, _num_frames, _command_queue.Get());
    _frame_index = _swap_chain->GetCurrentBackBufferIndex();
    _rtv_heap = create_rtv_heap(_device.Get(), _num_frames);
    _rtv_heap_size = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    setup_render_targets();    
    _command_allocator = create_command_allocator(_device.Get());;
}

void GraphicContext::setup_render_targets()
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtv_heap->GetCPUDescriptorHandleForHeapStart());

    for (UINT n = 0; n < _num_frames; n++)
    {
        throw_if_failed(_swap_chain->GetBuffer(n, IID_PPV_ARGS(&_render_targets[n])));
        _device->CreateRenderTargetView(_render_targets[n].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, _rtv_heap_size);
    }
}

ComPtr<ID3D12Device> create_device(IDXGIFactory4* factory)
{
    ComPtr<IDXGIAdapter1> hardwareAdapter;
    get_hardware_adapter(factory, &hardwareAdapter, false);

    ComPtr<ID3D12Device> device;
    throw_if_failed(D3D12CreateDevice(
        hardwareAdapter.Get(),
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&device)
    ));

    return device;
}

void get_hardware_adapter(IDXGIFactory4* factory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter)
{
    *ppAdapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;

    ComPtr<IDXGIFactory6> factory6;
    if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        for (
            UINT adapterIndex = 0;
            SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                adapterIndex,
                requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                IID_PPV_ARGS(&adapter)));
            ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    if (adapter.Get() == nullptr)
    {
        for (UINT adapterIndex = 0; SUCCEEDED(factory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    *ppAdapter = adapter.Detach();
}

ComPtr<ID3D12CommandQueue> create_command_queue(ID3D12Device* device)
{
    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ComPtr<ID3D12CommandQueue> command_queue;
    throw_if_failed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&command_queue)));
    return command_queue;
}

ComPtr<IDXGISwapChain3> create_swap_chain(IDXGIFactory4* factory, HWND hwnd, UINT width, UINT height, UINT buffer_count, ID3D12CommandQueue* command_queue)
{
    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = buffer_count;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;
    throw_if_failed(factory->CreateSwapChainForHwnd(
        command_queue,
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
    ));

    // This sample does not support fullscreen transitions.
    throw_if_failed(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));

    ComPtr<IDXGISwapChain3> swap_chain;
    throw_if_failed(swapChain.As(&swap_chain));
    return swap_chain;
}

ComPtr<ID3D12DescriptorHeap> create_rtv_heap(ID3D12Device* device, UINT frame_count)
{
    // Describe and create a render target view (RTV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = frame_count;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    ComPtr<ID3D12DescriptorHeap> rtv_heap;
    throw_if_failed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtv_heap)));
    return rtv_heap;
}

ComPtr<ID3D12CommandAllocator> create_command_allocator(ID3D12Device* device)
{
    ComPtr<ID3D12CommandAllocator> command_allocator;
    throw_if_failed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator)));
    return command_allocator;
}