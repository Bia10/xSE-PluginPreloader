#pragma once
#include "stdafx.h"
#include "KxDynamicString.h"

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

	private:
		std::vector<HMODULE> m_LoadedLibraries;
		HMODULE m_OriginalLibrary = NULL;

		const wchar_t* m_PluginsFolder = NULL;
		FILE* m_Log = NULL;

	private:
		LoadStatus LoadPlugin(const wchar_t* path);
		void LoadPlugins();
		void UnloadPlugins();
		void LogLoadStatus(const wchar_t* path, LoadStatus status) const;

		void LoadOriginalLibrary();
		void LoadOriginalLibraryFunctions();
		void UnloadOriginalLibrary();
		void ClearOriginalFunctionArray();

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
