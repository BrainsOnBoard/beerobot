/************************************************************************************
Filename    :   OculusIntegration.cpp
Content     :   Adds Oculus VR support for the BeeRobot project
Created     :   8th June 2018
Authors     :   James Paul Turner
*************************************************************************************/

// Include DirectX and Oculus SDK
#include "Win32_DirectXAppUtil.h"
#include "OVR_CAPI_D3D.h"


// return true to retry later (e.g. after display lost)
static bool MainLoop(bool retryCreate)
{
    // Initialize these to nullptr here to handle device lost failures cleanly
    std::vector<ID3D11RenderTargetView *> textureChainRtv;
    ovrTextureSwapChain textureChain = nullptr;
    ovrTextureSwapChainDesc textureDesc = {};
    ovrMirrorTexture mirrorTexture = nullptr;
    ovrMirrorTextureDesc mirrorDesc = {};
    ID3D11Texture2D *tex;
    ovrLayerEyeFov layer = {};
    ovrResult result;
    long long frameIndex = 0;
    int msaaRate = 4;
    int textureCount = 0;

    //Camera         * mainCam = nullptr;

    ovrSession session = nullptr;
    ovrGraphicsLuid luid;
    result = ovr_Create(&session, &luid);
    if (!OVR_SUCCESS(result))
        return retryCreate;
    ovrHmdDesc hmdDesc = ovr_GetHmdDesc(session);

    // Setup Device and Graphics
    // Note: the mirror window can be any size, for this sample we use 1/2 the HMD resolution
    if (!DIRECTX.InitDevice(hmdDesc.Resolution.w / 2, hmdDesc.Resolution.h / 2, reinterpret_cast<LUID*>(&luid))) {
        goto Done;
    }

    ovrEyeRenderDesc eyeRenderDesc[2];
    ovrPosef HmdToEyePose[2];

    // Determine the eye render buffer size. 
    ovrRecti eyeRenderViewport;
    ovrSizei texSizeLeft, texSizeRight, bufferSize;
    texSizeLeft = ovr_GetFovTextureSize(session, ovrEye_Left, hmdDesc.DefaultEyeFov[ovrEye_Left], 1.0f);
    texSizeRight = ovr_GetFovTextureSize(session, ovrEye_Right, hmdDesc.DefaultEyeFov[ovrEye_Right], 1.0f);
    bufferSize.w = texSizeLeft.w + texSizeRight.w;
    bufferSize.h = max(texSizeLeft.h, texSizeRight.h);
    eyeRenderViewport.Pos = { 0, 0 };
    eyeRenderViewport.Size = bufferSize;

    // create color texture swap chain
    textureDesc.Type = ovrTexture_2D;
    textureDesc.ArraySize = 1;
    textureDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
    textureDesc.Width = bufferSize.w;
    textureDesc.Height = bufferSize.h;
    textureDesc.MipLevels = 1;
    textureDesc.SampleCount = msaaRate;
    textureDesc.MiscFlags = ovrTextureMisc_DX_Typeless;
    textureDesc.BindFlags = ovrTextureBind_DX_RenderTarget;
    textureDesc.StaticImage = ovrFalse;
    result = ovr_CreateTextureSwapChainDX(session, DIRECTX.Device, &textureDesc, &textureChain);
    if (!OVR_SUCCESS(result)) {
        if (retryCreate) goto Done;
        FATALERROR("Failed to create eye texture.");
    }

    ovr_GetTextureSwapChainLength(session, textureChain, &textureCount);
    textureChainRtv.resize(textureCount);
    for (int i = 0; i < textureCount; ++i)
    {
        ovr_GetTextureSwapChainBufferDX(session, textureChain, i, IID_PPV_ARGS(&tex));
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        rtvDesc.ViewDimension = (msaaRate > 1) ? D3D11_RTV_DIMENSION_TEXTURE2DMS
                                               : D3D11_RTV_DIMENSION_TEXTURE2D;
        HRESULT hr = DIRECTX.Device->CreateRenderTargetView(tex, &rtvDesc, &textureChainRtv[i]);
        VALIDATE((hr == ERROR_SUCCESS), "Error creating render target view");
        tex->Release();
    }

    // Initialize our single full-screen Fov layer.
    layer.Header.Type = ovrLayerType_EyeFov;
    layer.Header.Flags = 0;
    layer.ColorTexture[ovrEye_Left] = textureChain;
    layer.ColorTexture[ovrEye_Right] = textureChain;
    layer.Viewport[ovrEye_Left] = eyeRenderViewport;
    layer.Viewport[ovrEye_Right] = eyeRenderViewport;
    
    // Create a mirror to see on the monitor.
    mirrorDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
    mirrorDesc.Width = DIRECTX.WinSizeW;
    mirrorDesc.Height = DIRECTX.WinSizeH;
    mirrorDesc.MirrorOptions = ovrMirrorOption_Default;
    result = ovr_CreateMirrorTextureWithOptionsDX(session, DIRECTX.Device, &mirrorDesc, &mirrorTexture);    
    if (!OVR_SUCCESS(result)) {
        if (retryCreate) goto Done;
        FATALERROR("Failed to create mirror texture.");
    }

    // Create the camera.
    //mainCam = new Camera(XMVectorSet(0.0f, 0.0f, 5.0f, 0), XMQuaternionIdentity());
    
    // Render loop.
    while (DIRECTX.HandleMessages()) {
        ovrSessionStatus sessionStatus;
        ovr_GetSessionStatus(session, &sessionStatus);
        
        if (sessionStatus.ShouldQuit) {
            // Because the application is requested to quit, should not request retry
            retryCreate = false;
            break;
        }
        
        if (sessionStatus.ShouldRecenter) {
            ovr_RecenterTrackingOrigin(session);
        }

        if (sessionStatus.IsVisible) {
            //XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, -0.05f, 0), mainCam->Rot);
            //XMVECTOR right   = XMVector3Rotate(XMVectorSet(0.05f, 0, 0, 0),  mainCam->Rot);
            //if (DIRECTX.Key['W'] || DIRECTX.Key[VK_UP])      mainCam->Pos = XMVectorAdd(mainCam->Pos, forward);
            //if (DIRECTX.Key['S'] || DIRECTX.Key[VK_DOWN])    mainCam->Pos = XMVectorSubtract(mainCam->Pos, forward);
            //if (DIRECTX.Key['D'])                            mainCam->Pos = XMVectorAdd(mainCam->Pos, right);
            //if (DIRECTX.Key['A'])                            mainCam->Pos = XMVectorSubtract(mainCam->Pos, right);
            //static float Yaw = 0;
            //if (DIRECTX.Key[VK_LEFT])  mainCam->Rot = XMQuaternionRotationRollPitchYaw(0, Yaw += 0.02f, 0);
            //if (DIRECTX.Key[VK_RIGHT]) mainCam->Rot = XMQuaternionRotationRollPitchYaw(0, Yaw -= 0.02f, 0);

            // Call ovr_GetRenderDesc each frame to get the ovrEyeRenderDesc, as the returned values (e.g. HmdToEyePose) may change at runtime.
            eyeRenderDesc[ovrEye_Left] = ovr_GetRenderDesc(session, ovrEye_Left, hmdDesc.DefaultEyeFov[ovrEye_Left]);
            eyeRenderDesc[ovrEye_Right] = ovr_GetRenderDesc(session, ovrEye_Right, hmdDesc.DefaultEyeFov[ovrEye_Right]);
            HmdToEyePose[ovrEye_Left] = eyeRenderDesc[ovrEye_Left].HmdToEyePose;
            HmdToEyePose[ovrEye_Right] = eyeRenderDesc[ovrEye_Right].HmdToEyePose;

            // Get both eye poses simultaneously, with IPD offset already included. 
            double sensorSampleTime;
            ovrPosef EyeRenderPose[2];
            ovr_GetEyePoses(session, frameIndex, ovrTrue, HmdToEyePose, EyeRenderPose, &sensorSampleTime);

            // Update our single full-screen Fov layer.
            layer.Fov[0] = eyeRenderDesc[0].Fov;
            layer.Fov[1] = eyeRenderDesc[1].Fov;
            layer.RenderPose[0] = EyeRenderPose[0];
            layer.RenderPose[1] = EyeRenderPose[1];
            layer.SensorSampleTime = sensorSampleTime;

            // Get, clear and setup the render target for the current frame.
            int textureChainIndex = 0;
            ovr_GetTextureSwapChainCurrentIndex(session, textureChain, &textureChainIndex);
            ID3D11RenderTargetView *renderTargetView = nullptr;
            renderTargetView = textureChainRtv[textureChainIndex];
            DIRECTX.SetAndClearRenderTarget(renderTargetView, (ID3D11DepthStencilView *) NULL);


            // Copy OpenCV matrix to texture.
            ovr_GetMirrorTextureBufferDX(session, mirrorTexture, IID_PPV_ARGS(&tex));

            //DIRECTX.Context->CopyResource(DIRECTX.BackBuffer, tex);

            tex->Release();

            
            
            // Get the pose information in XM format
            //XMVECTOR eyeQuat = XMVectorSet(EyeRenderPose[eye].Orientation.x, EyeRenderPose[eye].Orientation.y,
            //                               EyeRenderPose[eye].Orientation.z, EyeRenderPose[eye].Orientation.w);
            //XMVECTOR eyePos = XMVectorSet(EyeRenderPose[eye].Position.x, EyeRenderPose[eye].Position.y, EyeRenderPose[eye].Position.z, 0);

                        
            DIRECTX.SetViewport((float)eyeRenderViewport.Pos.x, (float)eyeRenderViewport.Pos.y,
                (float)eyeRenderViewport.Size.w, (float)eyeRenderViewport.Size.h);


            
            
            // Commit rendering to the swap chain, and submit frame.
            ovr_CommitTextureSwapChain(session, textureChain);
            ovrLayerHeader *layerHeader = &layer.Header;
            result = ovr_SubmitFrame(session, frameIndex, nullptr, &layerHeader, 1);
            if (!OVR_SUCCESS(result)) {
                goto Done;
            }

            frameIndex++;
        }

        // Render mirror
        tex = nullptr;
        ovr_GetMirrorTextureBufferDX(session, mirrorTexture, IID_PPV_ARGS(&tex));
        DIRECTX.Context->CopyResource(DIRECTX.BackBuffer, tex);
        tex->Release();
        DIRECTX.SwapChain->Present(0, 0);
    }

Done:
    // Release resources
    for (int i = 0; i < (int) textureChainRtv.size(); ++i)
    {
        //Release(textureChainRtv[i]);
        textureChainRtv[i]->Release();
    }
    if (textureChain)
    {
        ovr_DestroyTextureSwapChain(session, textureChain);
    }
    if (mirrorTexture) {
        ovr_DestroyMirrorTexture(session, mirrorTexture);
    }


    //delete mainCam;


    DIRECTX.ReleaseDevice();
    ovr_Destroy(session);

    // Retry on ovrError_DisplayLost
    return retryCreate || (result == ovrError_DisplayLost);
}

//-------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
    // Initializes LibOVR, and the Rift
	ovrInitParams initParams = { ovrInit_RequestVersion | ovrInit_FocusAware, OVR_MINOR_VERSION, NULL, 0, 0 };
    VALIDATE(OVR_SUCCESS(ovr_Initialize(&initParams)), "Failed to initialize libOVR.");

    VALIDATE(DIRECTX.InitWindow(hinst, L"Oculus Robot"), "Failed to open window.");

    DIRECTX.Run(MainLoop);

    ovr_Shutdown();
    return(0);
}
