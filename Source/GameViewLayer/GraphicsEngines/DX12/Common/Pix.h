#pragma once

#include <dx12pch.h>

#if defined(_PIX_H_) || defined(_PIX3_H_)

// Scoped PIX event.
class PixEvent
{
	bool mHasEnded = false;

	ID3D12GraphicsCommandList* mCommandList;
	ID3D12CommandQueue* mCommandQueue;

	inline u32 GetPixColor(_In_ XMVECTOR color)
	{
		auto c3 = Vector3(color * 256);
		return PIX_COLOR((u8) c3.x, (u8) c3.y, (u8) c3.z);
	}

public:
	PixEvent(_In_ ID3D12GraphicsCommandList* pCommandList, _In_ XMVECTOR color, PCWSTR pFormat) noexcept
		: mCommandList(pCommandList), mCommandQueue(nullptr)
	{
		PIXBeginEvent(pCommandList, GetPixColor(color), pFormat);
	}

	PixEvent(_In_ ID3D12CommandQueue* pCommandQueue, _In_ XMVECTOR color, PCWSTR pFormat) noexcept
		: mCommandList(nullptr), mCommandQueue(pCommandQueue)
	{
		PIXBeginEvent(pCommandQueue, GetPixColor(color), pFormat);
	}

	PixEvent(_In_ XMVECTOR color, PCWSTR pFormat) noexcept
		: mCommandList(nullptr), mCommandQueue(nullptr)
	{
		PIXBeginEvent(GetPixColor(color), pFormat);
	}

	inline void End()
	{
		if (!mHasEnded)
		{
			if (mCommandList)
				PIXEndEvent(mCommandList);
			else if (mCommandQueue)
				PIXEndEvent(mCommandQueue);
			else
				PIXEndEvent();
		}

		mHasEnded = true;
	}

	~PixEvent()
	{
		End();
	}
};

#endif