#pragma once

#include <cstdint>
// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using namespace Microsoft::WRL;
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include "d3dx12.h"

class GraphicContext
{
public:
	GraphicContext(HWND hwnd, UINT width, UINT height);
	void initialize();

	void setup_triangle_assets();
	void setup_triangle_rendering();

	void triangle_render();

private:
	static const uint8_t _num_frames = 2;
	HWND _hwnd;
	UINT _width;
	UINT _height;

	ComPtr<ID3D12Device> _device;
	ComPtr<ID3D12CommandQueue> _command_queue;
	ComPtr<IDXGISwapChain3> _swap_chain;
	UINT _frame_index;
	ComPtr<ID3D12DescriptorHeap> _rtv_heap;
	UINT _rtv_heap_size;
	ComPtr<ID3D12Resource> _render_targets[_num_frames];
	ComPtr<ID3D12GraphicsCommandList> _command_list;
	ComPtr<ID3D12CommandAllocator> _command_allocator;

	ComPtr<ID3D12RootSignature> _root_signature;
	ComPtr<ID3D12PipelineState> _pipeline_state;

	std::wstring _assets_folder_path;
	float _aspect_ratio;

	ComPtr<ID3D12Resource> _vertex_buffer;
	D3D12_VERTEX_BUFFER_VIEW _vertex_buffer_view;

	HANDLE _fence_event;
	ComPtr<ID3D12Fence> _fence;
	UINT64 _fence_value;

	CD3DX12_VIEWPORT _viewport_rect;
	CD3DX12_RECT _scissor_rect;
	
	void wait_for_previous_frame();
	void setup_render_targets();
};