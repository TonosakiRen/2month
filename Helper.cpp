#include "Helper.h"

#include <format>
#include <cassert>

namespace {

    D3D12_RASTERIZER_DESC CreateRasterizerDesc(
        D3D12_FILL_MODE fillMode,
        D3D12_CULL_MODE cullMode,
        BOOL frontCounterClockwise = FALSE,
        INT depthBias = D3D12_DEFAULT_DEPTH_BIAS,
        FLOAT depthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
        FLOAT slopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
        BOOL depthClipEnable = TRUE,
        BOOL multisampleEnable = FALSE,
        BOOL antialiasedLineEnable = FALSE,
        UINT forcedSampleCount = 0,
        D3D12_CONSERVATIVE_RASTERIZATION_MODE conservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF) {
        D3D12_RASTERIZER_DESC rasterizerDesc{};
        rasterizerDesc.FillMode = fillMode;
        rasterizerDesc.CullMode = cullMode;
        rasterizerDesc.FrontCounterClockwise = frontCounterClockwise;
        rasterizerDesc.DepthBias = depthBias;
        rasterizerDesc.DepthBiasClamp = depthBiasClamp;
        rasterizerDesc.SlopeScaledDepthBias = slopeScaledDepthBias;
        rasterizerDesc.DepthClipEnable = depthClipEnable;
        rasterizerDesc.MultisampleEnable = multisampleEnable;
        rasterizerDesc.AntialiasedLineEnable = antialiasedLineEnable;
        rasterizerDesc.ForcedSampleCount = forcedSampleCount;
        rasterizerDesc.ConservativeRaster = conservativeRaster;
        return rasterizerDesc;
    }

    D3D12_BLEND_DESC CreateBlendDesc(BOOL blendEnabled,
        D3D12_BLEND srcBlend, D3D12_BLEND destBlend, D3D12_BLEND_OP blendOp,
        D3D12_BLEND srcBlendAlpha, D3D12_BLEND destBlendAlpha, D3D12_BLEND_OP blendOpAlpha,
        UINT8 renderTargetWriteMask) {
        D3D12_BLEND_DESC blendDesc{};
        blendDesc.IndependentBlendEnable = FALSE;
        blendDesc.RenderTarget[0].BlendEnable = blendEnabled;
        blendDesc.RenderTarget[0].SrcBlend = srcBlend;
        blendDesc.RenderTarget[0].DestBlend = destBlend;
        blendDesc.RenderTarget[0].BlendOp = blendOp;
        blendDesc.RenderTarget[0].SrcBlendAlpha = srcBlendAlpha;
        blendDesc.RenderTarget[0].DestBlendAlpha = destBlendAlpha;
        blendDesc.RenderTarget[0].BlendOpAlpha = blendOpAlpha;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = renderTargetWriteMask;
        return blendDesc;
    }

    D3D12_DEPTH_STENCIL_DESC CreateDepthState(BOOL depthEnabled, D3D12_DEPTH_WRITE_MASK depthWriteMask, D3D12_COMPARISON_FUNC depthFunc) {
        D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
        depthStencilDesc.DepthEnable = depthEnabled;
        depthStencilDesc.DepthWriteMask = depthWriteMask;
        depthStencilDesc.DepthFunc = depthFunc;
        return depthStencilDesc;
    }

}

namespace Helper {

    const D3D12_RASTERIZER_DESC RasterizerDefault = CreateRasterizerDesc(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK);
    const D3D12_RASTERIZER_DESC RasterizerNoCull = CreateRasterizerDesc(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
    const D3D12_RASTERIZER_DESC RasterizerWireframe = CreateRasterizerDesc(D3D12_FILL_MODE_WIREFRAME, D3D12_CULL_MODE_NONE);

    const D3D12_BLEND_DESC BlendNoColorWrite =
        CreateBlendDesc(FALSE,
            D3D12_BLEND_SRC_ALPHA,
            D3D12_BLEND_INV_SRC_ALPHA,
            D3D12_BLEND_OP_ADD,
            D3D12_BLEND_ONE,
            D3D12_BLEND_INV_SRC_ALPHA,
            D3D12_BLEND_OP_ADD,
            0);
    const D3D12_BLEND_DESC BlendDisable =
        CreateBlendDesc(FALSE,
            D3D12_BLEND_SRC_ALPHA,
            D3D12_BLEND_INV_SRC_ALPHA,
            D3D12_BLEND_OP_ADD,
            D3D12_BLEND_ONE,
            D3D12_BLEND_ZERO,
            D3D12_BLEND_OP_ADD,
            D3D12_COLOR_WRITE_ENABLE_ALL);     // ブレンド無効
    const D3D12_BLEND_DESC BlendAlpha =
        CreateBlendDesc(TRUE,
            D3D12_BLEND_SRC_ALPHA,
            D3D12_BLEND_INV_SRC_ALPHA,
            D3D12_BLEND_OP_ADD,
            D3D12_BLEND_ONE,
            D3D12_BLEND_ZERO,
            D3D12_BLEND_OP_ADD,
            D3D12_COLOR_WRITE_ENABLE_ALL);     // アルファ合成
    const D3D12_BLEND_DESC BlendMultiply =
        CreateBlendDesc(TRUE,
            D3D12_BLEND_ZERO,
            D3D12_BLEND_SRC_COLOR,
            D3D12_BLEND_OP_ADD,
            D3D12_BLEND_ONE,
            D3D12_BLEND_ZERO,
            D3D12_BLEND_OP_ADD,
            D3D12_COLOR_WRITE_ENABLE_ALL);     // 乗算合成
    const D3D12_BLEND_DESC BlendAdditive =
        CreateBlendDesc(TRUE,
            D3D12_BLEND_SRC_ALPHA,
            D3D12_BLEND_ONE,
            D3D12_BLEND_OP_ADD,
            D3D12_BLEND_ONE,
            D3D12_BLEND_ZERO,
            D3D12_BLEND_OP_ADD,
            D3D12_COLOR_WRITE_ENABLE_ALL);    // 加算合成
    const D3D12_BLEND_DESC BlendSubtract =
        CreateBlendDesc(TRUE,
            D3D12_BLEND_SRC_ALPHA,
            D3D12_BLEND_ONE,
            D3D12_BLEND_OP_REV_SUBTRACT,
            D3D12_BLEND_ONE,
            D3D12_BLEND_ZERO,
            D3D12_BLEND_OP_ADD,
            D3D12_COLOR_WRITE_ENABLE_ALL);    // 減算合成

    const D3D12_DEPTH_STENCIL_DESC DepthStateDisabled = CreateDepthState(FALSE, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_ALWAYS);
    const D3D12_DEPTH_STENCIL_DESC DepthStateReadWrite = CreateDepthState(TRUE, D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC_LESS_EQUAL);


    DXGI_FORMAT GetBaseFormat(DXGI_FORMAT format) {
        switch (format) {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            return DXGI_FORMAT_R8G8B8A8_TYPELESS;

        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            return DXGI_FORMAT_B8G8R8A8_TYPELESS;

        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            return DXGI_FORMAT_B8G8R8X8_TYPELESS;

        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
            return DXGI_FORMAT_R32G8X24_TYPELESS;

        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
            return DXGI_FORMAT_R32_TYPELESS;

        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
            return DXGI_FORMAT_R24G8_TYPELESS;

        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
            return DXGI_FORMAT_R16_TYPELESS;

        default:
            break;
        }
        return format;
    }

    DXGI_FORMAT GetUAVFormat(DXGI_FORMAT format) {
        switch (format) {
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM;

        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            return DXGI_FORMAT_B8G8R8A8_UNORM;

        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            return DXGI_FORMAT_B8G8R8X8_UNORM;

        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_R32_FLOAT:
            return DXGI_FORMAT_R32_FLOAT;

#ifdef _DEBUG
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        case DXGI_FORMAT_D16_UNORM:
            assert(false);
#endif // _DEBUG

        default:
            break;
        }
        return format;
    }

    DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format) {
        switch (format) {
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
            return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
            return DXGI_FORMAT_D32_FLOAT;

        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;

        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
            return DXGI_FORMAT_D16_UNORM;

        default:
            break;
        }
        return format;
    }

    DXGI_FORMAT GetDepthFormat(DXGI_FORMAT format) {
        switch (format) {
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
            return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
            return DXGI_FORMAT_R32_FLOAT;

        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
            return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
            return DXGI_FORMAT_R16_UNORM;

        default:
            break;
        }
        return DXGI_FORMAT_UNKNOWN;
    }

    DXGI_FORMAT GetStencilFormat(DXGI_FORMAT format) {
        switch (format) {
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
            return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;

        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
            return DXGI_FORMAT_X24_TYPELESS_G8_UINT;

        default:
            break;
        }
        return DXGI_FORMAT_UNKNOWN;
    }

    size_t GetBytePerPixel(DXGI_FORMAT format) {
        switch (format) {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
            return 16;

        case DXGI_FORMAT_R32G32B32_TYPELESS:
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R32G32B32_UINT:
        case DXGI_FORMAT_R32G32B32_SINT:
            return 12;

        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R32G32_TYPELESS:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
            return 8;

        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R16G16_TYPELESS:
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        case DXGI_FORMAT_R8G8_B8G8_UNORM:
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            return 4;

        case DXGI_FORMAT_R8G8_TYPELESS:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R16_SINT:
        case DXGI_FORMAT_B5G6R5_UNORM:
        case DXGI_FORMAT_B5G5R5A1_UNORM:
        case DXGI_FORMAT_A8P8:
        case DXGI_FORMAT_B4G4R4A4_UNORM:
            return 2;

        case DXGI_FORMAT_R8_TYPELESS:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT:
        case DXGI_FORMAT_A8_UNORM:
        case DXGI_FORMAT_P8:
            return 1;

        default:
            break;
        }
        return 0;
    }

    void AssertIfFailed(HRESULT hr)
    {
        assert(SUCCEEDED(hr));
    }

    void Log(const std::string& message) {
        OutputDebugStringA(message.c_str());
    }
    void Log(const std::wstring& message) {
        OutputDebugStringW(message.c_str());
    }
    std::wstring ConvertString(const std::string& str) {
        if (str.empty()) {
            return std::wstring();
        }
        auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>
            (&str[0]), static_cast<int>(str.size()), NULL, 0);
        if (sizeNeeded == 0) {
            return std::wstring();
        }
        std::wstring result(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]),
            static_cast<int>(str.size()), &result[0], sizeNeeded);
        return result;
    }

    std::string ConvertString(const std::wstring& str) {
        if (str.empty()) {
            return std::string();
        }
        auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>
            (str.size()), NULL, 0, NULL, NULL);
        if (sizeNeeded == 0) {
            return std::string();
        }
        std::string result(sizeNeeded, 0);
        WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()),
            result.data(), sizeNeeded, NULL, NULL);
        return result;
    }

}