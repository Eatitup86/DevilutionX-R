#include "utils/file_util.h"
#include "utils/log.hpp"

#include <algorithm>
#include <string>

#include <SDL.h>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#if defined(_WIN64) || defined(_WIN32)
// Suppress definitions of `min` and `max` macros by <windows.h>:
#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if _POSIX_C_SOURCE >= 200112L || defined(_BSD_SOURCE) || defined(__APPLE__)
#include <sys/stat.h>
#include <unistd.h>
#else
#include <cstdio>
#endif

namespace devilution {

bool FileExists(const char *path)
{
#if _POSIX_C_SOURCE >= 200112L || defined(_BSD_SOURCE) || defined(__APPLE__)
	return ::access(path, F_OK) == 0;
#else
	FILE *file = std::fopen(path, "rb");
	if (file == NULL)
		return false;
	std::fclose(file);
	return true;
#endif
}

bool GetFileSize(const char *path, std::uintmax_t *size)
{
#if defined(_WIN64) || defined(_WIN32)
	WIN32_FILE_ATTRIBUTE_DATA attr;
	int path_utf16_size = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
	wchar_t *path_utf16 = new wchar_t[path_utf16_size];
	if (MultiByteToWideChar(CP_UTF8, 0, path, -1, path_utf16, path_utf16_size) != path_utf16_size) {
		delete[] path_utf16;
		return false;
	}
	if (!GetFileAttributesExW(path_utf16, GetFileExInfoStandard, &attr)) {
		delete[] path_utf16;
		return false;
	}
	delete[] path_utf16;
	*size = (attr.nFileSizeHigh) << (sizeof(attr.nFileSizeHigh) * 8) | attr.nFileSizeLow;
	return true;
#else
	struct ::stat statResult;
	if (::stat(path, &statResult) == -1)
		return false;
	*size = static_cast<uintmax_t>(statResult.st_size);
	return true;
#endif
}

bool ResizeFile(const char *path, std::uintmax_t size)
{
#if defined(_WIN64) || defined(_WIN32)
	LARGE_INTEGER lisize;
	lisize.QuadPart = static_cast<LONGLONG>(size);
	if (lisize.QuadPart < 0) {
		return false;
	}
	int path_utf16_size = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
	wchar_t *path_utf16 = new wchar_t[path_utf16_size];
	if (MultiByteToWideChar(CP_UTF8, 0, path, -1, path_utf16, path_utf16_size) != path_utf16_size) {
		delete[] path_utf16;
		return false;
	}
	HANDLE file = ::CreateFileW(path_utf16, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	delete[] path_utf16;
	if (file == INVALID_HANDLE_VALUE) {
		return false;
	} else if (::SetFilePointerEx(file, lisize, NULL, FILE_BEGIN) == 0 || ::SetEndOfFile(file) == 0) {
		::CloseHandle(file);
		return false;
	}
	::CloseHandle(file);
	return true;
#elif _POSIX_C_SOURCE >= 200112L || defined(_BSD_SOURCE) || defined(__APPLE__)
	return ::truncate(path, static_cast<off_t>(size)) == 0;
#else
	static_assert(false, "truncate not implemented for the current platform");
#endif
}

void RemoveFile(const char *lpFileName)
{
	std::string name = lpFileName;
	std::replace(name.begin(), name.end(), '\\', '/');
	FILE *f = fopen(name.c_str(), "r+");
	if (f != nullptr) {
		fclose(f);
		remove(name.c_str());
		f = nullptr;
		Log("Removed file: {}", name);
	} else {
		Log("Failed to remove file: {}", name);
	}
}

} // namespace devilution
