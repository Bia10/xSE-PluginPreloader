#include "stdafx.h"
#include "xSEPluginPreloader.h"
#include "X3DAudio17.h"
#include "DInput8.h"

#if _WIN64
#define ProxyAPI __declspec(dllexport, noinline) void
#else
#define ProxyAPI __declspec(dllexport, noinline, naked) void
#endif

//////////////////////////////////////////////////////////////////////////
// Call procedures
//////////////////////////////////////////////////////////////////////////
#define LoadOriginalFunc(enumName, name)	ms_OriginalFunctions[enumName::name] = ::GetProcAddress(m_OriginalLibrary, #name);

#if _WIN64

extern "C" void* UnconditionalJumpAddress = NULL;
extern "C" void UnconditionalJump();

#define CallOriginalFunc(enumName, name)	\
UnconditionalJumpAddress = ms_OriginalFunctions[enumName::name];	\
UnconditionalJump();	\

#else

#define CallOriginalFunc(enumName, name)	\
{	\
	void* func = ms_OriginalFunctions[enumName::name];	\
	__asm	\
	{	\
		jmp dword ptr [func]	\
	}	\
}	\

#endif

#define DefineProxyFunc(API, enumName, name)	\
API name()	\
{	\
	CallOriginalFunc(enumName, name);	\
}	\

//////////////////////////////////////////////////////////////////////////
// bink2w64
//////////////////////////////////////////////////////////////////////////
#define LoadFunc_Bink2(name) LoadOriginalFunc(bink2w64, name)
#define CallFunc_Bink2(name) CallOriginalFunc(bink2w64, name)
#define DefineFunc_Bink2(name) DefineProxyFunc(ProxyAPI, bink2w64, name)

//////////////////////////////////////////////////////////////////////////
// X3DAudio17
//////////////////////////////////////////////////////////////////////////
#define LoadFunc_X3DAudio17(name) LoadOriginalFunc(X3DAudio17, name)
#define CallFunc_X3DAudio17(name) CallOriginalFunc(X3DAudio17, name)
#define DefineFunc_X3DAudio17(name) DefineProxyFunc(ProxyAPI, X3DAudio17, name)

//////////////////////////////////////////////////////////////////////////
// DInput8
//////////////////////////////////////////////////////////////////////////
#define LoadFunc_DInput8(name) LoadOriginalFunc(DInput8, name)
#define CallFunc_DInput8(name) CallOriginalFunc(DInput8, name)
#define DefineFunc_DInput8(name) DefineProxyFunc(ProxyAPI, DInput8, name)

//////////////////////////////////////////////////////////////////////////
static void* ms_OriginalFunctions[xSEPP::GetFunctionArraySize()];
void** xSEPP::GetFunctions()
{
	return ms_OriginalFunctions;
}

void xSEPP::LoadOriginalLibraryFunctions()
{
	#if xSE_PLATFORM_F4SE

	LoadFunc_X3DAudio17(X3DAudioCalculate);
	LoadFunc_X3DAudio17(X3DAudioInitialize);

	#elif xSE_PLATFORM_NVSE

	LoadFunc_DInput8(DirectInput8Create);
	LoadFunc_DInput8(DllCanUnloadNow);
	LoadFunc_DInput8(DllGetClassObject);
	LoadFunc_DInput8(DllRegisterServer);
	LoadFunc_DInput8(DllUnregisterServer);

	#endif

	#if 0
	LoadFunc_Bink2(BinkBufferBlit);
	LoadFunc_Bink2(BinkBufferCheckWinPos);
	LoadFunc_Bink2(BinkBufferClear);
	LoadFunc_Bink2(BinkBufferClose);
	LoadFunc_Bink2(BinkBufferGetDescription);
	LoadFunc_Bink2(BinkBufferGetError);
	LoadFunc_Bink2(BinkBufferLock);
	LoadFunc_Bink2(BinkBufferOpen);
	LoadFunc_Bink2(BinkBufferSetDirectDraw);
	LoadFunc_Bink2(BinkBufferSetHWND);
	LoadFunc_Bink2(BinkBufferSetOffset);
	LoadFunc_Bink2(BinkBufferSetResolution);
	LoadFunc_Bink2(BinkBufferSetScale);
	LoadFunc_Bink2(BinkBufferUnlock);
	LoadFunc_Bink2(BinkCheckCursor);
	LoadFunc_Bink2(BinkClose);
	LoadFunc_Bink2(BinkCloseTrack);
	LoadFunc_Bink2(BinkControlBackgroundIO);
	LoadFunc_Bink2(BinkControlPlatformFeatures);
	LoadFunc_Bink2(BinkCopyToBuffer);
	LoadFunc_Bink2(BinkCopyToBufferRect);
	LoadFunc_Bink2(BinkDDSurfaceType);
	LoadFunc_Bink2(BinkDX8SurfaceType);
	LoadFunc_Bink2(BinkDX9SurfaceType);
	LoadFunc_Bink2(BinkDoFrame);
	LoadFunc_Bink2(BinkDoFrameAsync);
	LoadFunc_Bink2(BinkDoFrameAsyncWait);
	LoadFunc_Bink2(BinkDoFramePlane);
	LoadFunc_Bink2(BinkFreeGlobals);
	LoadFunc_Bink2(BinkGetError);
	LoadFunc_Bink2(BinkGetFrameBuffersInfo);
	LoadFunc_Bink2(BinkGetKeyFrame);
	LoadFunc_Bink2(BinkGetPalette);
	LoadFunc_Bink2(BinkGetPlatformInfo);
	LoadFunc_Bink2(BinkGetRealtime);
	LoadFunc_Bink2(BinkGetRects);
	LoadFunc_Bink2(BinkGetSummary);
	LoadFunc_Bink2(BinkGetTrackData);
	LoadFunc_Bink2(BinkGetTrackID);
	LoadFunc_Bink2(BinkGetTrackMaxSize);
	LoadFunc_Bink2(BinkGetTrackType);
	LoadFunc_Bink2(BinkGoto);
	LoadFunc_Bink2(BinkIsSoftwareCursor);
	LoadFunc_Bink2(BinkLogoAddress);
	LoadFunc_Bink2(BinkNextFrame);
	LoadFunc_Bink2(BinkOpen);
	LoadFunc_Bink2(BinkOpenDirectSound);
	LoadFunc_Bink2(BinkOpenMiles);
	LoadFunc_Bink2(BinkOpenTrack);
	LoadFunc_Bink2(BinkOpenWaveOut);
	LoadFunc_Bink2(BinkOpenWithOptions);
	LoadFunc_Bink2(BinkOpenXAudio2);
	LoadFunc_Bink2(BinkPause);
	LoadFunc_Bink2(BinkRegisterFrameBuffers);
	LoadFunc_Bink2(BinkRequestStopAsyncThread);
	LoadFunc_Bink2(BinkRestoreCursor);
	LoadFunc_Bink2(BinkService);
	LoadFunc_Bink2(BinkSetError);
	LoadFunc_Bink2(BinkSetFileOffset);
	LoadFunc_Bink2(BinkSetFrameRate);
	LoadFunc_Bink2(BinkSetIO);
	LoadFunc_Bink2(BinkSetIOSize);
	LoadFunc_Bink2(BinkSetMemory);
	LoadFunc_Bink2(BinkSetPan);
	LoadFunc_Bink2(BinkSetSimulate);
	LoadFunc_Bink2(BinkSetSoundOnOff);
	LoadFunc_Bink2(BinkSetSoundSystem);
	LoadFunc_Bink2(BinkSetSoundSystem2);
	LoadFunc_Bink2(BinkSetSoundTrack);
	LoadFunc_Bink2(BinkSetSpeakerVolumes);
	LoadFunc_Bink2(BinkSetVideoOnOff);
	LoadFunc_Bink2(BinkSetVolume);
	LoadFunc_Bink2(BinkSetWillLoop);
	LoadFunc_Bink2(BinkShouldSkip);
	LoadFunc_Bink2(BinkStartAsyncThread);
	LoadFunc_Bink2(BinkUseTelemetry);
	LoadFunc_Bink2(BinkUseTmLite);
	LoadFunc_Bink2(BinkWait);
	LoadFunc_Bink2(BinkWaitStopAsyncThread);
	LoadFunc_Bink2(RADTimerRead);
	#endif
}

//////////////////////////////////////////////////////////////////////////
extern "C"
{
	#if xSE_PLATFORM_F4SE

	DefineFunc_X3DAudio17(X3DAudioCalculate);
	DefineFunc_X3DAudio17(X3DAudioInitialize);

	#elif xSE_PLATFORM_NVSE

	DefineFunc_DInput8(DirectInput8Create);
	DefineFunc_DInput8(DllCanUnloadNow);
	DefineFunc_DInput8(DllGetClassObject);
	DefineFunc_DInput8(DllRegisterServer);
	DefineFunc_DInput8(DllUnregisterServer);

	#endif

	#if 0
	DefineFunc_Bink2(BinkBufferBlit);
	DefineFunc_Bink2(BinkBufferCheckWinPos);
	DefineFunc_Bink2(BinkBufferClear);
	DefineFunc_Bink2(BinkBufferClose);
	DefineFunc_Bink2(BinkBufferGetDescription);
	DefineFunc_Bink2(BinkBufferGetError);
	DefineFunc_Bink2(BinkBufferLock);
	DefineFunc_Bink2(BinkBufferOpen);
	DefineFunc_Bink2(BinkBufferSetDirectDraw);
	DefineFunc_Bink2(BinkBufferSetHWND);
	DefineFunc_Bink2(BinkBufferSetOffset);
	DefineFunc_Bink2(BinkBufferSetResolution);
	DefineFunc_Bink2(BinkBufferSetScale);
	DefineFunc_Bink2(BinkBufferUnlock);
	DefineFunc_Bink2(BinkCheckCursor);
	DefineFunc_Bink2(BinkClose);
	DefineFunc_Bink2(BinkCloseTrack);
	DefineFunc_Bink2(BinkControlBackgroundIO);
	DefineFunc_Bink2(BinkControlPlatformFeatures);
	DefineFunc_Bink2(BinkCopyToBuffer);
	DefineFunc_Bink2(BinkCopyToBufferRect);
	DefineFunc_Bink2(BinkDDSurfaceType);
	DefineFunc_Bink2(BinkDX8SurfaceType);
	DefineFunc_Bink2(BinkDX9SurfaceType);
	DefineFunc_Bink2(BinkDoFrame);
	DefineFunc_Bink2(BinkDoFrameAsync);
	DefineFunc_Bink2(BinkDoFrameAsyncWait);
	DefineFunc_Bink2(BinkDoFramePlane);
	DefineFunc_Bink2(BinkFreeGlobals);
	DefineFunc_Bink2(BinkGetError);
	DefineFunc_Bink2(BinkGetFrameBuffersInfo);
	DefineFunc_Bink2(BinkGetKeyFrame);
	DefineFunc_Bink2(BinkGetPalette);
	DefineFunc_Bink2(BinkGetPlatformInfo);
	DefineFunc_Bink2(BinkGetRealtime);
	DefineFunc_Bink2(BinkGetRects);
	DefineFunc_Bink2(BinkGetSummary);
	DefineFunc_Bink2(BinkGetTrackData);
	DefineFunc_Bink2(BinkGetTrackID);
	DefineFunc_Bink2(BinkGetTrackMaxSize);
	DefineFunc_Bink2(BinkGetTrackType);
	DefineFunc_Bink2(BinkGoto);
	DefineFunc_Bink2(BinkIsSoftwareCursor);
	DefineFunc_Bink2(BinkLogoAddress);
	DefineFunc_Bink2(BinkNextFrame);
	DefineFunc_Bink2(BinkOpen);
	DefineFunc_Bink2(BinkOpenDirectSound);
	DefineFunc_Bink2(BinkOpenMiles);
	DefineFunc_Bink2(BinkOpenTrack);
	DefineFunc_Bink2(BinkOpenWaveOut);
	DefineFunc_Bink2(BinkOpenWithOptions);
	DefineFunc_Bink2(BinkOpenXAudio2);
	DefineFunc_Bink2(BinkPause);
	DefineFunc_Bink2(BinkRegisterFrameBuffers);
	DefineFunc_Bink2(BinkRequestStopAsyncThread);
	DefineFunc_Bink2(BinkRestoreCursor);
	DefineFunc_Bink2(BinkService);
	DefineFunc_Bink2(BinkSetError);
	DefineFunc_Bink2(BinkSetFileOffset);
	DefineFunc_Bink2(BinkSetFrameRate);
	DefineFunc_Bink2(BinkSetIO);
	DefineFunc_Bink2(BinkSetIOSize);
	DefineFunc_Bink2(BinkSetMemory);
	DefineFunc_Bink2(BinkSetPan);
	DefineFunc_Bink2(BinkSetSimulate);
	DefineFunc_Bink2(BinkSetSoundOnOff);
	DefineFunc_Bink2(BinkSetSoundSystem);
	DefineFunc_Bink2(BinkSetSoundSystem2);
	DefineFunc_Bink2(BinkSetSoundTrack);
	DefineFunc_Bink2(BinkSetSpeakerVolumes);
	DefineFunc_Bink2(BinkSetVideoOnOff);
	DefineFunc_Bink2(BinkSetVolume);
	DefineFunc_Bink2(BinkSetWillLoop);
	DefineFunc_Bink2(BinkShouldSkip);
	DefineFunc_Bink2(BinkStartAsyncThread);
	DefineFunc_Bink2(BinkUseTelemetry);
	DefineFunc_Bink2(BinkUseTmLite);
	DefineFunc_Bink2(BinkWait);
	DefineFunc_Bink2(BinkWaitStopAsyncThread);
	DefineFunc_Bink2(RADTimerRead);
	#endif
};
