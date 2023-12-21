#include "DirectXCommon.h"

#include <dxgi1_6.h>

#include <cassert>
#include <format>

#include "Helper.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")


using namespace Microsoft::WRL;

DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::Initialize() {

	CreateDevice();
	commandQueue_.Create();

	/*descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_RTV].Create(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, SwapChain::kNumBuffers + kMainColorBufferNum)*/;
	descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_RTV].Create(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kRtvHeapDescritorNum);
	descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_DSV].Create(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
	descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kSrvHeapDescritorNum);

}

void DirectXCommon::Shutdown()
{
	commandQueue_.Signal();
	commandQueue_.WaitForGPU();
}

void DirectXCommon::CreateDevice() {

#ifdef _DEBUG

	ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバックレイヤーを有効化する	
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックを行えるようにする
		debugController->SetEnableGPUBasedValidation(TRUE);

	}

#endif

	//DXGIファクトリー
	ComPtr<IDXGIFactory7> factory = nullptr;
	//HRESULTはWIndows系のエラーコードであり、
	//関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	Helper::AssertIfFailed(CreateDXGIFactory(IID_PPV_ARGS(&factory)));
	//初期化の根本的な部分でエラーが出た場合はプログラングが間違っているか、
	//どうにもできない場合が多いのでassertにしておく

	//使用するアダプタ用の変数、最初にnullptrを入れておく
	ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	//良い順にアダプタを頼む
	for (UINT i = 0; factory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i) {
		//アダプターの情報を取得する	
		DXGI_ADAPTER_DESC3 adapterDesc{};
		Helper::AssertIfFailed(useAdapter->GetDesc3(&adapterDesc));
		//ソフトウェアアダプタでなければ採用！
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			Helper::Log(std::format(L"Use Adapter{}\n", adapterDesc.Description));
			break;
		}
		useAdapter = nullptr; //ソフトウェアアダプタの場合は見なかったことにする
	}
	//適切なアダプタが見つからんかったので起動できない
	assert(useAdapter != nullptr);

	//D3D12Deviceの生成
	device_ = nullptr;
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	HRESULT hr;
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		//指定した機能レベルでデバイスが生成できたか確認
		if (SUCCEEDED(hr)) {
			//生成できたのでログ出力を行ってループを抜ける	
			Helper::Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	//デバイスの生成がうまくいかなかったので起動できない
	assert(device_ != nullptr);
	Helper::Log("Complete create D3D12Device!!!\n");//初期化完了のログを出す

	const uint32_t desriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	const uint32_t desriptorSizeRTV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	const uint32_t desriptorSizeDSV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);


#ifdef _DEBUG

	ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//やばいエラー前に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に泊まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に泊まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGIデバックレイヤーとDX12デバッグレイヤーの相方作用バグによるエラーメッセージ
			//https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
#endif

}

DescriptorHandle DirectXCommon::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type) {
	return descriptorHeaps_[type].Allocate();
}
