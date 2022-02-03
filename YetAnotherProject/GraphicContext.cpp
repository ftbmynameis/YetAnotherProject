#include "GraphicContext.hpp"

#include <d3dcompiler.h>

#include "d3dx12.h"
#include "helper.hpp"
#include "Vertex.hpp"
#include "d3d12_helper.hpp"

#include <DirectXMath.h>

using namespace DirectX;

GraphicContext::GraphicContext(HWND hwnd, UINT width, UINT height)
    : _hwnd(hwnd),
    _width(width),
    _height(height),
    _assets_folder_path(get_assets_path()),
    _aspect_ratio(static_cast<float>(width) / static_cast<float>(height)),
    _viewport_rect{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height) },
    _scissor_rect{ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) },
    _fence_values{0, 0},
    _vertex_buffer_view(),
    _rtv_heap_size(0),
    _frame_index(0),
    _fence_event(nullptr),
    _const_buffer(nullptr),
    _const_buffer_data()
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
    _command_allocator[0] = create_command_allocator(_device.Get());
    _command_allocator[1] = create_command_allocator(_device.Get());

    setup_triangle_assets();
}

void GraphicContext::exit()
{
    wait_for_gpu();

    CloseHandle(_fence_event);
}

void GraphicContext::setup_triangle_assets()
{
    _root_signature = create_default_root_signature(_device.Get());

    // Create the pipeline state, which includes compiling and loading shaders.
    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
        // Enable better shader debugging with the graphics debugging tools.
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif
        auto ps_shader_path = _assets_folder_path + L"\\" + L"ps_shader.hlsl";
        auto vs_shader_path = _assets_folder_path + L"\\" + L"vs_shader.hlsl";
        ComPtr<ID3DBlob> error_blob = nullptr;
        auto vs_compile_hr = D3DCompileFromFile(vs_shader_path.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &error_blob);

        if (FAILED(vs_compile_hr))
        {
            if (error_blob)
            {
                OutputDebugStringA((char*)error_blob->GetBufferPointer());
                error_blob->Release();
            }

            throw_if_failed(vs_compile_hr);
        }

        error_blob = nullptr;
        auto ps_compile_blob = D3DCompileFromFile(ps_shader_path.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &error_blob);

        if (FAILED(ps_compile_blob))
        {
            if (error_blob)
            {
                OutputDebugStringA((char*)error_blob->GetBufferPointer());
                error_blob->Release();
            }

            throw_if_failed(ps_compile_blob);
        }

        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = _root_signature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        throw_if_failed(_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pipeline_state)));
    }

    // Create the command list.
    throw_if_failed(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _command_allocator[0].Get(), _pipeline_state.Get(), IID_PPV_ARGS(&_command_list)));

    // Command lists are created in the recording state, but there is nothing
    // to record yet. The main loop expects it to be closed, so close it now.
    throw_if_failed(_command_list->Close());

    // Create the vertex buffer.
    {
        float multiplier = 1.0f;
        float z_val = 0.5f;
        // Define the geometry for a triangle.
        SimpleVertex triangleVertices[] =
        {
            { { 0.0f, 5.0f * multiplier, z_val }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { { 5.0f * multiplier, -5.0f * multiplier, z_val }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            { { -5.0f * multiplier, -5.0f * multiplier, z_val }, { 0.0f, 0.0f, 1.0f, 1.0f } }
        };

        const UINT vertexBufferSize = sizeof(triangleVertices);

        // Note: using upload heaps to transfer static data like vert buffers is not 
        // recommended. Every time the GPU needs it, the upload heap will be marshalled 
        // over. Please read up on Default Heap usage. An upload heap is used here for 
        // code simplicity and because there are very few verts to actually transfer.
        auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto resource_desc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
        throw_if_failed(_device->CreateCommittedResource(
            &heap_properties,
            D3D12_HEAP_FLAG_NONE,
            &resource_desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&_vertex_buffer)));

        // Copy the triangle data to the vertex buffer.
        UINT8* pVertexDataBegin;
        CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
        throw_if_failed(_vertex_buffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
        memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
        _vertex_buffer->Unmap(0, nullptr);

        // Initialize the vertex buffer view.
        _vertex_buffer_view.BufferLocation = _vertex_buffer->GetGPUVirtualAddress();
        _vertex_buffer_view.StrideInBytes = sizeof(SimpleVertex);
        _vertex_buffer_view.SizeInBytes = vertexBufferSize;
    }

    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        throw_if_failed(_device->CreateFence(_fence_values[_frame_index], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));
        _fence_values[_frame_index]++;

        // Create an event handle to use for frame synchronization.
        _fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (_fence_event == nullptr)
        {
            throw_if_failed(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    _const_buffer = std::make_unique<ConstantBuffer<BasicConstBufferData> >(_device.Get());

    // Wait for the command list to execute; we are reusing the same command 
    // list in our main loop but for now, we just want to wait for setup to 
    // complete before continuing.
    wait_for_gpu();
}

void GraphicContext::setup_triangle_rendering()
{
    throw_if_failed(_command_allocator[_frame_index]->Reset());
    throw_if_failed(_command_list->Reset(_command_allocator[_frame_index].Get(), _pipeline_state.Get()));

    // Set necessary state.
    _command_list->SetGraphicsRootSignature(_root_signature.Get());

    // setup const buffer
    ID3D12DescriptorHeap* ppHeaps[] = { _const_buffer->get_desc_heap() };
    _command_list->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    _command_list->SetGraphicsRootDescriptorTable(0, _const_buffer->get_desc_heap()->GetGPUDescriptorHandleForHeapStart());

    _command_list->RSSetViewports(1, &_viewport_rect);
    _command_list->RSSetScissorRects(1, &_scissor_rect);

    // Indicate that the back buffer will be used as a render target.
    auto resource_barrier = CD3DX12_RESOURCE_BARRIER::Transition(_render_targets[_frame_index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    _command_list->ResourceBarrier(1, &resource_barrier);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtv_heap->GetCPUDescriptorHandleForHeapStart(), _frame_index, _rtv_heap_size);
    _command_list->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // Record commands.
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    _command_list->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    _command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _command_list->IASetVertexBuffers(0, 1, &_vertex_buffer_view);
    _command_list->DrawInstanced(3, 1, 0, 0);

    // Indicate that the back buffer will now be used to present.
    auto resource_barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(_render_targets[_frame_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    _command_list->ResourceBarrier(1, &resource_barrier2);

    throw_if_failed(_command_list->Close());
}

float g_ft_acc = 0.0f;

// Somehow M_PI is not defined even tho intellisense recognizes it with use math defines :(
const float g_pi_value = 3.14159265358979323846f;

const float g_fov = g_pi_value * 0.75f;
const float g_aspect = 800.0f / 600.0f;
const float g_near_z = 0.1f;
const float g_far_z = 10.0f;

vec3f g_eye = vec3f(4.0f, 3.0f, -3.0f);
vec3f g_at = vec3f(0.0f, 0.0f, 0.0f);
vec3f g_up = vec3f(0.0f, 1.0f, 0.0f);

mat4f g_view = mat::look_at(g_eye, g_at, g_up);
mat4f g_proj = mat::proj(g_fov, g_aspect, g_near_z, g_far_z);

XMVECTOR vec_to_xmvec(const vec3f& vec)
{
    const auto xmfloat = XMFLOAT3(&vec.data[0]);
    return XMLoadFloat3(&xmfloat);
}

void GraphicContext::triangle_render(float frametime)
{
    // DirectX::XMFLOAT4X4 float4x4;
    // const auto mvp = mat::translate(vec3f(0, 0, 60));
    // const auto dx_mvp = DirectX::XMMatrixTranslation(0, 0, 60);
    // const auto mvp = mat::look_at(g_eye, g_at, g_up);
    // const auto dx_mvp = XMMatrixLookAtLH(vec_to_xmvec(g_eye), vec_to_xmvec(g_at), vec_to_xmvec(g_up));
    /*const auto mvp = mat::proj(g_fov, g_aspect, g_near_z, g_far_z) * mat::look_at(g_eye, g_at, g_up);
    const auto dx_mvp = XMMatrixLookAtLH(vec_to_xmvec(g_eye), vec_to_xmvec(g_at), vec_to_xmvec(g_up)) * XMMatrixPerspectiveFovLH(g_fov, g_aspect, g_near_z, g_far_z);
    XMStoreFloat4x4(&float4x4, XMMatrixTranspose(dx_mvp));
    memcpy(_const_buffer_data.dx_world_view_proj, &float4x4, sizeof(float4x4));*/

    const auto mvp = mat::proj(g_fov, g_aspect, g_near_z, g_far_z) * mat::look_at(g_eye, g_at, g_up);
    memcpy(_const_buffer_data.world_view_proj, &mvp[0][0], sizeof(mvp));
    _const_buffer->update_buffer_data(_const_buffer_data);

    // Record all the commands we need to render the scene into the command list.
    setup_triangle_rendering();

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { _command_list.Get() };
    _command_queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    throw_if_failed(_swap_chain->Present(1, 0));

    move_to_next_frame();    
}

// Wait for pending GPU work to complete.
void GraphicContext::wait_for_gpu()
{
    // Schedule a Signal command in the queue.
    throw_if_failed(_command_queue->Signal(_fence.Get(), _fence_values[_frame_index]));

    // Wait until the fence has been processed.
    throw_if_failed(_fence->SetEventOnCompletion(_fence_values[_frame_index], _fence_event));
    WaitForSingleObjectEx(_fence_event, INFINITE, FALSE);

    // Increment the fence value for the current frame.
    _fence_values[_frame_index]++;
}

// Prepare to render the next frame.
void GraphicContext::move_to_next_frame()
{
    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = _fence_values[_frame_index];
    throw_if_failed(_command_queue->Signal(_fence.Get(), currentFenceValue));

    // Update the frame index.
    _frame_index = _swap_chain->GetCurrentBackBufferIndex();

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (_fence->GetCompletedValue() < _fence_values[_frame_index])
    {
        throw_if_failed(_fence->SetEventOnCompletion(_fence_values[_frame_index], _fence_event));
        WaitForSingleObjectEx(_fence_event, INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    _fence_values[_frame_index] = currentFenceValue + 1;
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