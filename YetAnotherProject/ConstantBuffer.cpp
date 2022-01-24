#include "ConstantBuffer.hpp"

#include "Helper.hpp"
#include "d3d12_helper.hpp"

ConstantBufferBase::ConstantBufferBase(ID3D12Device* device, UINT size)
	: _size(size)
{
	_buffer = create_commited_resource(device, size);
	_desc_heap = create_descriptor_heap(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	create_constant_buffer_view(device, this);

	// Map and initialize the constant buffer. We don't unmap this until the
	// app closes. Keeping things mapped for the lifetime of the resource is okay.
	CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
	throw_if_failed(_buffer->Map(0, &readRange, reinterpret_cast<void**>(&_buffer_begin)));
	// memcpy(_buffer_begin, &m_constantBufferData, sizeof(m_constantBufferData));
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