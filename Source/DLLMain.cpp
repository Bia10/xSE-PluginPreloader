#include "stdafx.h"
#include "xSEPluginPreloader.h"

BOOL APIENTRY DllMain(HMODULE module, DWORD event, LPVOID lpReserved)
{
	switch (event)
	{
		case DLL_PROCESS_ATTACH:
		{
			return xSEPP::CreateInstnace().IsOK();
		}
		case DLL_PROCESS_DETACH:
		{
			xSEPP::DestroyInstnace();
			break;
		}
	};
	return TRUE;
}
