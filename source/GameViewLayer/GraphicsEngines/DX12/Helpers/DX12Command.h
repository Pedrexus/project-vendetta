#pragma once

#include "../dx12pch.h"

/*
	the GPU recieves commands through a command queue.
	for high performance applications, the goal is to
	keep both CPU and GPU busy (reduce idle time)

	-> keep the queue filled but not overloaded
*/
namespace Command
{
	inline D3D12_COMMAND_QUEUE_DESC SpecifyQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		return queueDesc;
	}

	// Commands sit in a GPU queue until the GPU is ready to process them
	inline void CreateQueue(ID3D12Device* device, ID3D12CommandQueue** queue)
	{
		auto queueDesc = SpecifyQueue();
		ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(queue)));
	}

	// The command list saves each command on a Command Allocator
	inline void CreateAllocator(ID3D12Device* device, ID3D12CommandAllocator** cmdListAlloc)
	{
		ThrowIfFailed(device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT, // DIRECT or BUNDLE (bundle is an optimized version of the command list)
			IID_PPV_ARGS(cmdListAlloc)
		));
	}

	// CPU submits commands to the queue through the Direct3D API using command lists
	inline void CreateList(ID3D12Device* device, ID3D12CommandAllocator* cmdListAlloc, ID3D12GraphicsCommandList** cmdList)
	{
		ThrowIfFailed(device->CreateCommandList(
			0, // node mask: 0 for a single GPU system
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			cmdListAlloc,
			nullptr, // Initial PipelineStateObject should be null
			IID_PPV_ARGS(cmdList)
		));
	}
}