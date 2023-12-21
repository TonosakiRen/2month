#include "ShaderManager.h"

#include <cassert>
#include <format>
#include <filesystem>
#include <fstream>

#include "Helper.h"

#pragma comment(lib,"dxcompiler.lib")

using namespace Microsoft::WRL;

namespace {

    std::wstring profiles[ShaderManager::kNumTypes] = {
        L"vs_6_0",
        L"ps_6_0",
        L"gs_6_0",
        L"hs_6_0",
        L"ds_6_0",
        L"cs_6_0"
    };

}

ShaderManager* ShaderManager::GetInstance() {
    static ShaderManager instance;
    return &instance;
}

void ShaderManager::Initialize() {
    Helper::AssertIfFailed(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils_.GetAddressOf())));
    Helper::AssertIfFailed(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(compiler_.GetAddressOf())));
    Helper::AssertIfFailed(utils_->CreateDefaultIncludeHandler(includeHandler_.GetAddressOf()));
}

Microsoft::WRL::ComPtr<IDxcBlob> ShaderManager::Compile(const std::wstring& path, Type type) {
    return Compile(path, profiles[type]);
}

Microsoft::WRL::ComPtr<IDxcBlob> ShaderManager::Compile(const std::wstring& path, const std::wstring& profile) {

	std::wstring directionary = L"./Resources/shaders/" + path;
	//これからシェーダーをコンパイルする旨をログに出す
	Helper::Log(Helper::ConvertString(std::format(L"Begin CompileShader,path:{},profile:{}\n", path, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	Helper::AssertIfFailed(utils_->LoadFile(directionary.c_str(), nullptr, &shaderSource));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8の文字コードであることを通知

	LPCWSTR arguments[] = {
		path.c_str(),//コンパイル対象のhlslファイル名
		L"-E",L"main",//エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile.c_str(),//ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",//デバック用の情報を埋め込む
		L"-Od",//最適化を外しておく
		L"-Zpr",//メモリレイアウト行優先
	};

	//実際にshaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	Helper::AssertIfFailed(compiler_->Compile(
		&shaderSourceBuffer,//読み込んだファイル
		arguments,//コンパイルオプション
		_countof(arguments),//コンパイルオプションの数
		includeHandler_.Get(),//includeが含まれたもろもろ
		IID_PPV_ARGS(&shaderResult)//コンパイル結果

	));

	//警告・エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		std::string error = shaderError->GetStringPointer();
		Helper::Log(error);
		//警告・エラーダメ絶対
		assert(false);
	}

	//コンパイル結果から実行用のバイナリを取得
	IDxcBlob* shaderBlob = nullptr;
	Helper::AssertIfFailed(shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr));
	//成功したログを出す
	Helper::Log(Helper::ConvertString(std::format(L"Compile Succeeded, path:{} , profile:{}\n", path, profile)));
	//もう使わないリソース
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
}
