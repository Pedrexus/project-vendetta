#pragma once

#include "UploadBuffer.h"
#include "../DX12Descriptor.h"

template<typename T>
class ConstantBuffer : public UploadBuffer<T>
{
public:
	Descriptor::Heap cbvHeap;

	inline ConstantBuffer(ID3D12Device* device, u64 elementCount) :
		UploadBuffer<T>(device, elementCount),
		cbvHeap(device, Descriptor::ConstantBuffer::Type)
	{}
	ConstantBuffer(const ConstantBuffer& rhs) = delete;
	ConstantBuffer& operator=(const ConstantBuffer& rhs) = delete;
	virtual ~ConstantBuffer() = default;

	inline u64 CalcBufferByteSize() const override
	{
		// Constant buffers must be a multiple of the minimum hardware
		// allocation size (usually 256 bytes).  So round up to nearest
		// multiple of 256.  We do this by adding 255 and then masking off
		// the lower 2 bytes which store all bits < 256.
		// Example: Suppose byteSize = 300.
		// (300 + 255) & ~255
		// 555 & ~255
		// 0x022B & ~0x00ff
		// 0x022B & 0xff00
		// 0x0200
		// 512
		return (sizeof(T) + 255) & ~255;
	}

	inline D3D12_CONSTANT_BUFFER_VIEW_DESC SpecifyConstantBufferView(u64 elementIndex = 0) const
	{
		auto cbGPUAddress = UploadBuffer<T>::GetResource()->GetGPUVirtualAddress();
		auto cbByteSize = CalcBufferByteSize();

		// Offset to the ith object constant buffer in the buffer.
		cbGPUAddress += elementIndex * cbByteSize;

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = cbGPUAddress;
		cbvDesc.SizeInBytes = cbByteSize;

		return cbvDesc;
	}


};