#pragma once

#include <dx12pch.h>

namespace DXGI
{
	namespace Factory
	{
		ComPtr<IDXGIFactory> CreateWithDebugLayer()
		{
			ComPtr<IDXGIFactory> dxgiFactory = {};

#ifdef _DEBUG
			UINT CREATE_FACTORY_FLAGS = DXGI_CREATE_FACTORY_DEBUG;
#else
			UINT CREATE_FACTORY_FLAGS = 0;
#endif
			ThrowIfFailed(CreateDXGIFactory2(CREATE_FACTORY_FLAGS, IID_PPV_ARGS(&dxgiFactory)));
			return dxgiFactory;
		}
	}

}

namespace Display
{
	inline std::vector<IDXGIOutput*> GetAdapterOutputs(IDXGIAdapter* adapter)
	{
		UINT i = 0;
		IDXGIOutput* output = nullptr;
		std::vector<IDXGIOutput*> outputList;
		while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
		{
			outputList.push_back(output);
			++i;
		}

		return outputList;
	}

	inline std::vector<DXGI_MODE_DESC1> GetOutputModes(IDXGIOutput1* output, DXGI_FORMAT backBufferFormat)
	{
		UINT count = 0;
		UINT flags = 0;

		// Call with nullptr to get list count.
		output->GetDisplayModeList1(backBufferFormat, flags, &count, nullptr);

		std::vector<DXGI_MODE_DESC1> modeList(count);
		output->GetDisplayModeList1(backBufferFormat, flags, &count, &modeList[0]);

		return modeList;
	}

	typedef std::vector<IDXGIAdapter4*> AdapterVector;

	inline AdapterVector GetAdaptersOrderedByPerformance(IDXGIFactory6* dxgiFactory)
	{
		UINT i = 0;
		IDXGIAdapter4* adapter = nullptr;
		AdapterVector adapterVector;
		while (dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND)
		{
			adapterVector.push_back(adapter);
			++i;
		}

		return adapterVector;
	}

	inline void LogInformation(IDXGIFactory* dxgiFactory, DXGI_FORMAT backBufferFormat)
	{
		std::string text = "\n";
		for (auto& adapter : GetAdaptersOrderedByPerformance(static_cast<IDXGIFactory6*>(dxgiFactory)))
		{
			DXGI_ADAPTER_DESC3 adapterDesc;
			adapter->GetDesc3(&adapterDesc);
			text += fmt::format("Adapter {}: {}\n", adapterDesc.DeviceId, Convert::wide2str(adapterDesc.Description));
			for (auto& output : Display::GetAdapterOutputs(adapter))
			{
				DXGI_OUTPUT_DESC outputDesc;
				output->GetDesc(&outputDesc);
				text += fmt::format("Output: {}\n", Convert::wide2str(outputDesc.DeviceName));
				for (auto& mode : Display::GetOutputModes(static_cast<IDXGIOutput1*>(output), backBufferFormat))
					text += fmt::format("Mode: {}x{}\t{}Hz\n", mode.Width, mode.Height, mode.RefreshRate.Numerator / mode.RefreshRate.Denominator);
			}
		}
		LOG("Graphics", text);
	}

	// TODO: inline DXGI_MODE_DESC1 GetOutputInfo
}


