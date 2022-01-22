#pragma once

#include <d3d12.h>
#include <wrl.h>
using namespace Microsoft::WRL;
#include <memory>

class ConstantBufferBase
{
public:
	ConstantBufferBase(ID3D12Device* device, UINT size);

	UINT get_size() const;
	ID3D12Resource* get_buffer() const;
	ID3D12DescriptorHeap* get_desc_heap() const;
	virtual void* get_raw_data() const = 0;
private:
	// size is uint vs uin64, because const buffer view desc only supports uint. uint64 is for descriptor heaps containing more than simply one thing / const buffer!
	UINT _size;
	ComPtr<ID3D12Resource> _buffer;
	ComPtr<ID3D12DescriptorHeap> _desc_heap;

	ConstantBufferBase(const ConstantBufferBase&);
	ConstantBufferBase(ConstantBufferBase&&);
};

template <typename T, std::size_t Alignment>
constexpr std::size_t get_padding_size()
{
	return Alignment - (sizeof(T) % (Alignment + 1));
}

template <typename T>
class ConstantBuffer : public ConstantBufferBase
{
	static constexpr std::size_t type_size = sizeof(T);
	static constexpr std::size_t required_padding_size = get_padding_size<T, 256>();
	static constexpr std::size_t full_buffer_size = type_size + required_padding_size;

	static_assert(full_buffer_size % 256 == 0, "Constant Buffer size must be 256-byte aligned");
	static_assert(std::is_standard_layout<T>::value, "Constant Buffer must be in standard layout");
public:
	ConstantBuffer(ID3D12Device* device)
		: ConstantBufferBase(device, full_buffer_size)
	{
	}

	void* get_raw_data() const override
	{
		return _data.get();
	}

	T* get_data() const
	{
		return static_cast<T*>(_data.get());
	}

	void set_data(const T& data)
	{
		memcpy(_data.get(), &data, sizeof(T));
	}

private:
	std::unique_ptr<void*> _data;

	ConstantBuffer(const ConstantBuffer<T>&);
	ConstantBuffer(ConstantBuffer<T>&&);
};