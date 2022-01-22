#include "ConstantBuffer.hpp"

#include "d3d12_helper.hpp"

ConstantBufferBase::ConstantBufferBase(ID3D12Device* device, UINT size)
	: _size(size)
{
	_buffer = create_commited_resource(device, size);
	_desc_heap = create_descriptor_heap(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	create_constant_buffer_view(device, this);
}

UINT ConstantBufferBase::get_size() const
{
	return _size;
}

ID3D12Resource* ConstantBufferBase::get_buffer() const
{
	return _buffer.Get();
}

ID3D12DescriptorHeap* ConstantBufferBase::get_desc_heap() const
{
	return _desc_heap.Get();
}