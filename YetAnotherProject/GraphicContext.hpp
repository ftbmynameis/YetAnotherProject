#pragma once

#include <cstdint>
// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using namespace Microsoft::WRL;
#include <d3d12.h>
#include <dxgi1_6.h>

class GraphicContext
{
public:
	GraphicContext(HWND hwnd, UINT width, UINT height);
	void initialize();


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
	
	void setup_render_targets();
};