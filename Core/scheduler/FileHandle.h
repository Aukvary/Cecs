#ifndef FILE_HANDLE_H
#define FILE_HANDLE_H

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define Rectangle _WinRectangle
#define CloseWindow _WinCloseWindow
#define ShowCursor _WinShowCursor
#define DrawText _WinDrawText
#define LoadImage _WinLoadImage
#define PlaySound _WinPlaySound
#define DrawTextEx _WinDrawTextEx
#include <windows.h>
#undef Rectangle
#undef CloseWindow
#undef ShowCursor
#undef DrawText
#undef LoadImage
#undef PlaySound
#undef DrawTextEx

#define DT_LIB_HANDLE HMODULE
#define DT_LIB_LOAD(name) LoadLibraryA(name)
#define DT_LIB_GET(handle, name) GetProcAddress(handle, name)
#define DT_LIB_CLOSE(handle) FreeLibrary(handle)
#define DT_LIB_NAME(name) name ".dll"

#define DT_EXPORT __declspec(dllexport)
#else
#include <dlfcn.h>
#include <dirent.h>
#include <sys/types.h>

#define DT_LIB_HANDLE void*
#define DT_LIB_LOAD(name) dlopen(name, RTLD_LAZY)
#define DT_LIB_GET(handle, name) dlsym(handle, name)
#define DT_LIB_CLOSE(handle) dlclose(handle)
#define DT_LIB_NAME(name) name ".so"

#define DT_EXPORT __attribute__((visibility("default")))
#endif /*_WIN32*/

#endif /*FILE_HANDLE_H*/
