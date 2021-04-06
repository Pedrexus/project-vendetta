#pragma once

#include "../dx12pch.h"

// bind a vertex buffer to the pipeline, we need to create a 
	// vertex buffer view to the vertex buffer resource

	// A constant buffer is an example of a GPU resource (ID3D12Resource) whose data
	// contents can be referenced in shader programs.

	// constant buffers are usually updated once per frame
	// by the CPU. -> created in the upload heap
namespace InputAssembler
{

	/*
		Vertex may have generic data
	*/
	struct Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};

	static constexpr D3D12_INPUT_ELEMENT_DESC VertexInputLayout[2] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

    // a buffer is a list of elements - e.g.: vertices - we send to the GPU
    // Creates a Default Buffer in the GPU from an intermediate buffer 
    ComPtr<ID3D12Resource> DefaultBuffer(
        ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList,
        const void* initData,
        u64 byteSize,
        ComPtr<ID3D12Resource>& uploadBuffer
    );

	/* 	
		A constant buffer is an example of a GPU resource (ID3D12Resource) whose data
		contents can be referenced in shader programs.

		constant buffers are usually updated once per frame
		by the CPU. -> created in the upload heap
	*/
	struct Constants
	{
        XMFLOAT4X4 worldViewProj;
	};

    template<typename T>
    inline u32 GetConstantBufferByteSize() { return (sizeof(T) + 255) & ~255; }

    template<typename T>
    class UploadBuffer
    {
        ComPtr<ID3D12Resource> m_UploadBuffer;
        u8* m_MappedData = nullptr;

        u64 m_ElementByteSize = 0;
        bool m_IsConstantBuffer = false;

    public:
        inline UploadBuffer(ID3D12Device* device, u64 elementCount, bool isConstantBuffer) :
            m_IsConstantBuffer(isConstantBuffer)
        {
            m_ElementByteSize = sizeof(T);

            // Constant buffer elements need to be multiples of 256 bytes.
            // This is because the hardware can only view constant data 
            // at m*256 byte offsets and of n*256 byte lengths. 
            // typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC {
            // UINT64 OffsetInBytes; // multiple of 256
            // UINT   SizeInBytes;   // multiple of 256
            // } D3D12_CONSTANT_BUFFER_VIEW_DESC;
            if (isConstantBuffer)
                m_ElementByteSize = GetConstantBufferByteSize<T>();

            auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            auto uploadBuffer = CD3DX12_RESOURCE_DESC::Buffer(m_ElementByteSize * elementCount);

            ThrowIfFailed(device->CreateCommittedResource(
                &uploadHeap,
                D3D12_HEAP_FLAG_NONE,
                &uploadBuffer,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&m_UploadBuffer))
            );

            ThrowIfFailed(m_UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedData)));

            // We do not need to unmap until we are done with the resource.  However, we must not write to
            // the resource while it is in use by the GPU (so we must use synchronization techniques).
        }

        UploadBuffer(const UploadBuffer& rhs) = delete;
        UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
        inline ~UploadBuffer()
        {
            if (m_UploadBuffer != nullptr)
                m_UploadBuffer->Unmap(0, nullptr);
            SAFE_DELETE(m_MappedData);
        }

        inline ID3D12Resource* GetResource() const
        {
            return m_UploadBuffer.Get();
        }

        inline void CopyToBuffer(int elementIndex, const T& data)
        {
            // we copy the data to the pointer ->Map() has given us
            memcpy(&m_MappedData[elementIndex * m_ElementByteSize], &data, sizeof(T));
        }

        inline D3D12_CONSTANT_BUFFER_VIEW_DESC GetConstantBufferView() const
        {
            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = GetResource()->GetGPUVirtualAddress();
            auto ConstantBufferByteSize = GetConstantBufferByteSize<Constants>();

            u64 boxCBufIndex = 0; // Offset to the ith object constant buffer in the buffer.
            cbAddress += boxCBufIndex * ConstantBufferByteSize;

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
            cbvDesc.BufferLocation = cbAddress;
            cbvDesc.SizeInBytes = ConstantBufferByteSize;

            return cbvDesc;
        }
    };

}