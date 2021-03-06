#include "stdafx.h"
#include "xSEPluginPreloader.h"
#include "ScriptExtenderDefinesBase.h"
#include "KxFileFinder.h"

#include "Nukem Detours/Detours.h"
#if _WIN64

namespace NukemDetours = Detours::X64;
#pragma comment(lib, "Nukem Detours/detours x64.lib")

#else

namespace NukemDetours = Detours::X86;
#pragma comment(lib, "Nukem Detours/detours x86.lib")

#endif

//////////////////////////////////////////////////////////////////////////
xSEPP* xSEPP::ms_Instnace = NULL;
xSEPP& xSEPP::CreateInstnace()
{
	if (!HasInstance())
	{
		ms_Instnace = new xSEPP();
	}
	return *ms_Instnace;
}
void xSEPP::DestroyInstnace()
{
	delete ms_Instnace;
	ms_Instnace = NULL;
}

void xSEPP::DelayedLoad(void* p1, void* p2)
{
	xSEPP::GetInstance().Log(L"Loading plugins");
	xSEPP::GetInstance().LoadPlugins();
	xSEPP::GetInstance().CallInitTerm(p1, p2);
}

const wchar_t* xSEPP::GetConfigOption(const wchar_t* section, const wchar_t* key, const wchar_t* defaultValue) const
{
	return m_Config.GetValue(section, key, defaultValue);
}
int xSEPP::GetConfigOptionInt(const wchar_t* section, const wchar_t* key, int defaultValue) const
{
	const wchar_t* value = GetConfigOption(section, key, NULL);
	if (value)
	{
		int valueInt = defaultValue;
		swscanf(value, L"%d", &valueInt);
		return valueInt;
	}
	return defaultValue;
}

xSEPP::LoadStatus xSEPP::LoadPlugin(const wchar_t* path)
{
	Log(L"Trying to load '%s'", path);
	LoadStatus status = LoadStatus::LoadFailed;

	__try
	{
		HMODULE handle = ::LoadLibraryW(path);
		if (handle)
		{
			status = LoadStatus::Loaded;
			m_LoadedLibraries.push_back(handle);

			InitPluginFunc initFunc = reinterpret_cast<InitPluginFunc>(::GetProcAddress(handle, "Initialize"));
			if (initFunc)
			{
				status = LoadStatus::InitializationFailed;
				initFunc();
				status = LoadStatus::LoadedInitialized;
			}
		}
		else
		{
			status = LoadStatus::LoadFailed;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
	return status;
}
void xSEPP::LoadPlugins()
{
	Log(L"Searching '%s' folder for plugins", m_PluginsFolder);

	KxFileFinder finder(m_PluginsFolder, L"*_preload.txt");
	KxFileFinderItem item = finder.FindNext();
	while (finder.IsOK())
	{
		if (item.IsNormalItem() && item.IsFile())
		{
			KxDynamicString pathDLL = item.GetFullPath().before_last(L'_');
			pathDLL += L".dll";

			LoadStatus status = LoadPlugin(pathDLL);
			LogLoadStatus(pathDLL, status);
		}
		item = finder.FindNext();
	}

	Log(L"Loading finished, %zu plugins loaded", m_LoadedLibraries.size());
}
void xSEPP::UnloadPlugins()
{
	Log(L"Unloading plugins");

	for (HMODULE handle : m_LoadedLibraries)
	{
		::FreeLibrary(handle);
	}
	m_LoadedLibraries.clear();
}
void xSEPP::LogLoadStatus(const wchar_t* path, LoadStatus status) const
{
	switch (status)
	{
		case LoadStatus::Loaded:
		{
			Log(L"Plugin '%s' loaded successfully", path);
			break;
		}
		case LoadStatus::LoadedInitialized:
		{
			Log(L"Plugin '%s' loaded successfully, Initialize() executed", path);
			break;
		}
		case LoadStatus::LoadFailed:
		{
			Log(L"Plugin '%s' failed to load", path);
			break;
		}
		case LoadStatus::InitializationFailed:
		{
			Log(L"Plugin '%s' failed to load. Exception was thrown during Initialize() execution", path);
			break;
		}
	};
}

void xSEPP::LoadOriginalLibrary()
{
	KxDynamicString path = GetOriginalLibraryPath();
	Log(L"Loading original library '%s'", path.data());

	ClearOriginalFunctionArray();
	m_OriginalLibrary = ::LoadLibraryW(path);
}
void xSEPP::UnloadOriginalLibrary()
{
	Log(L"Unloading original library");

	FreeLibrary(m_OriginalLibrary);
	ClearOriginalFunctionArray();
}
void xSEPP::ClearOriginalFunctionArray()
{
	for (size_t i = 0; i < GetFunctionArraySize(); i++)
	{
		GetFunctions()[i] = NULL;
	}
}

xSEPP::xSEPP()
	:m_PluginsFolder(L"Data\\" xSE_NAME_W L"\\Plugins")
{
	// Load config
	m_Config.LoadFile(L"xSE PluginPreloader.ini");

	if (const wchar_t* originalLibrary = GetConfigOption(L"General", L"OriginalLibrary"))
	{
		m_OriginalLibraryPath = originalLibrary;
	}

	// Open log
	_wfopen_s(&m_Log, L"xSE PluginPreloader.log", L"wb");
	Log(L"Log opened");

	// Load
	LoadOriginalLibrary();
	if (m_OriginalLibrary)
	{
		LoadOriginalLibraryFunctions();

		Log(L"Overriding '_initterm_e' function for delayed load");
		#if xSE_PLATFORM_F4SE

		m_initterm_e = DetourFunctionIAT(DelayedLoad, "MSVCR110.dll", "_initterm_e");

		#elif xSE_PLATFORM_NVSE

		// 0x00ECCCF0 is address of '_initterm_e' in 'FalloutNV.exe'
		m_initterm_e = DetourFunction(DelayedLoad, 0x00ECCCF0u);

		#endif

		if (m_initterm_e == NULL)
		{
			UnloadOriginalLibrary();
			m_OriginalLibrary = NULL;

			Log(L"Can't override function, terminating");
		}
	}
	else
	{
		Log(L"Can't load original library, terminating");
	}
}
xSEPP::~xSEPP()
{
	if (m_OriginalLibrary)
	{
		UnloadPlugins();
		UnloadOriginalLibrary();
	}

	Log(L"Log closed");
	if (m_Log)
	{
		fclose(m_Log);
	}
}

KxDynamicString xSEPP::GetOriginalLibraryPath() const
{
	if (!m_OriginalLibraryPath.empty())
	{
		return m_OriginalLibraryPath;
	}
	else
	{
		KxDynamicString path;

		#if xSE_PLATFORM_F4SE

		path.resize(MAX_PATH);
		path.resize(::GetSystemDirectoryW(path.data(), MAX_PATH));
		path += L"\\X3DAudio1_7.dll";

		#elif xSE_PLATFORM_NVSE

		path.resize(MAX_PATH);
		path.resize(::GetSystemDirectoryW(path.data(), MAX_PATH));
		path += L"\\DInput8.dll";

		#endif

		return path;
	}
}
