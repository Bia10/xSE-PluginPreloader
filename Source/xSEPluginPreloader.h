#pragma once
#include "stdafx.h"
#include "KxDynamicString.h"
#include "SimpleINI.h"

class xSEPP
{
	public:
		enum class LoadStatus
		{
			Loaded,
			LoadedInitialized,

			LoadFailed,
			InitializationFailed,
		};
		using InitPluginFunc = void(__cdecl*)(void);
		using InitTermFunc = void(__cdecl*)(void*, void*);

	private:
		static xSEPP* ms_Instnace;

	public:
		static xSEPP& CreateInstnace();
		static void DestroyInstnace();
		static bool HasInstance()
		{
			return ms_Instnace != NULL;
		}
		static xSEPP& GetInstance()
		{
			return *ms_Instnace;
		}

		static constexpr size_t GetFunctionArraySize()
		{
			return 256;
		}
		static void** GetFunctions();
		static void DelayedLoad(void* p1, void* p2);

	private:
		std::vector<HMODULE> m_LoadedLibraries;
		HMODULE m_OriginalLibrary = NULL;
		InitTermFunc m_initterm_e = NULL;

		CSimpleIniW m_Config;
		KxDynamicString m_OriginalLibraryPath;
		const wchar_t* m_PluginsFolder = NULL;
		FILE* m_Log = NULL;

	private:
		const wchar_t* GetConfigOption(const wchar_t* section, const wchar_t* key, const wchar_t* defaultValue = NULL) const;
		int GetConfigOptionInt(const wchar_t* section, const wchar_t* key, int defaultValue = 0) const;
		bool GetConfigOptionBool(const wchar_t* section, const wchar_t* key, bool defaultValue = false)
		{
			return GetConfigOptionInt(section, key, defaultValue);
		}

		LoadStatus LoadPlugin(const wchar_t* path);
		void LoadPlugins();
		void UnloadPlugins();
		void LogLoadStatus(const wchar_t* path, LoadStatus status) const;
		void CallInitTerm(void* p1, void* p2)
		{
			(*m_initterm_e)(p1, p2);
		}

		void LoadOriginalLibrary();
		void LoadOriginalLibraryFunctions();
		void UnloadOriginalLibrary();
		void ClearOriginalFunctionArray();

		template<class T> T DetourFunctionIAT(T func, const char* nameDLL, const char* nameFunc) const
		{
			uint8_t* base = reinterpret_cast<uint8_t*>(GetModuleHandleW(NULL));
			return reinterpret_cast<T>(NukemDetours::DetourIAT(base, reinterpret_cast<uint8_t*>(func), nameDLL, nameFunc));
		}
		template<class T> T DetourFunction(T func, uintptr_t offset) const
		{
			uint8_t* base = reinterpret_cast<uint8_t*>(GetModuleHandleW(NULL));
			return reinterpret_cast<T>(NukemDetours::DetourFunction(base + offset, reinterpret_cast<uint8_t*>(func)));
		}

	public:
		xSEPP();
		~xSEPP();

	public:
		bool IsOK() const
		{
			return m_OriginalLibrary != NULL;
		}

		template<class ...Args> void Log(const wchar_t* format, Args... args) const
		{
			if (m_Log)
			{
				KxDynamicString string = KxDynamicString::Format(format, std::forward<Args>(args)...);
				fputws(string.data(), m_Log);
				fputws(L"\r\n", m_Log);
				fflush(m_Log);
			}
		}
		KxDynamicString GetOriginalLibraryPath() const;
};
