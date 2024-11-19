// This file is part of the FidelityFX SDK.
//
// Copyright (C) 2024 Advanced Micro Devices, Inc.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "ffx_provider.h"

// #include "ffx_provider_fsr2.h"
#include "ffx_provider_fsr3upscale.h"
// #include "ffx_provider_framegeneration.h"
// #include "ffx_provider_external.h"

#include <array>
#include <optional>

#ifdef FFX_BACKEND_DX12
#include <d3d12.h>
#include "dx12/ffx_provider_framegenerationswapchain_dx12.h"
#endif // FFX_BACKEND_DX12

#ifdef FFX_BACKEND_VK
#include "vk/ffx_provider_framegenerationswapchain_vk.h"
#endif // FFX_BACKEND_VK

static constexpr ffxProvider* providers[] = {
    &ffxProvider_FSR3Upscale::Instance,
    // &ffxProvider_FSR2::Instance,
    // &ffxProvider_FrameGeneration::Instance,
#ifdef FFX_BACKEND_DX12
    // &ffxProvider_FrameGenerationSwapChain_DX12::Instance,
#endif // FFX_BACKEND_DX12
#ifdef FFX_BACKEND_VK
    // &ffxProvider_FrameGenerationSwapChain_VK::Instance,
#endif // FFX_BACKEND_VK
};

#ifdef _WIN32
static constexpr size_t providerCount = _countof(providers);
#else
static constexpr size_t providerCount = sizeof(providers) / sizeof(providers[0]);
#endif

const ffxProvider* GetffxProvider(ffxStructType_t descType, uint64_t overrideId, void* device)
{

    for (size_t i = 0; i < providerCount; ++i)
    {
        if (providers[i]->GetId() == overrideId || (overrideId == 0 && providers[i]->CanProvide(descType)))
            return providers[i];
    }

    return nullptr;
}

const ffxProvider* GetAssociatedProvider(ffxContext* context)
{
    const InternalContextHeader* hdr = (const InternalContextHeader*)(*context);
    const ffxProvider*        provider = hdr->provider;
    return provider;
}

uint64_t GetProviderCount(ffxStructType_t descType, void* device)
{
    return GetProviderVersions(descType, device, UINT64_MAX, nullptr, nullptr);
}

uint64_t GetProviderVersions(ffxStructType_t descType, void* device, uint64_t capacity, uint64_t* versionIds, const char** versionNames)
{
    uint64_t count = 0;

    for (size_t i = 0; i < providerCount; ++i)
    {
        if (count >= capacity) break;
        if (providers[i]->CanProvide(descType))
        {
            auto index = count;
            count++;
            if (versionIds)
                versionIds[index] = providers[i]->GetId();
            if (versionNames)
                versionNames[index] = providers[i]->GetVersionName();
        }
    }

    return count;
}
