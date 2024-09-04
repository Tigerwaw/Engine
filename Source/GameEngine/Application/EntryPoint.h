#pragma once
#include "Application.h"

extern Application* CreateApplication();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

#ifndef _RETAIL
	// Redirect stdout and stderr to the console.
	FILE* consoleOut;
	FILE* consoleErr;
	AllocConsole();
	freopen_s(&consoleOut, "CONOUT$", "w", stdout);  // NOLINT(cert-err33-c)
	setvbuf(consoleOut, nullptr, _IONBF, 1024);  // NOLINT(cert-err33-c)
	freopen_s(&consoleErr, "CONOUT$", "w", stderr);  // NOLINT(cert-err33-c)
	setvbuf(consoleErr, nullptr, _IONBF, 1024);  // NOLINT(cert-err33-c)

	/*
	 * A note about strings:
	 * Strings in C++ are terrible things. They come in a variety of formats which causes issues.
	 * Many modern libraries expect UTF-8 (ImGui, FMOD, FBX SDK, etc) but Windows defaults to UTF-16LE
	 * which is not compatible 1=1.
	 *
	 * To avoid weird problems with compatibility, mangled characters and other problems it is highly
	 * recommended to store everything as UTF-8 encoded strings whenever possible. This means that
	 * when we store i.e. the name of a Model, Level, Sound File, etc we do so in a UTF-8 formatted
	 * std::string, and when we need to communicate with the Windows (or DirectX) API we need to use
	 * std::wstring which represents a UTF-16LE string.
	 *
	 * There are functions available for conversion between these formats in the str namespace in
	 * StringHelpers.h.
	 *
	 * The provided Logging library expects UTF-8 format strings which should provide minimal headaches
	 * for any involved situation. For anything non-unicode (like non swedish signs, accents, etc) you
	 * can just use normal strings as you would anywhere since UTF-8 is backwards compatible with ASCii
	 *
	 * SetConsoleOutputCP(CP_UTF8) tells the Windows Console that we'll output UTF-8. This DOES NOT
	 * affect file output in any way, that's a whole other can of worms. But if you always write and
	 * read your strings in the same format, and always treat them as byte blocks, you'll be fine.
	 */
	SetConsoleOutputCP(CP_UTF8);

	HWND consoleWindow = GetConsoleWindow();
	RECT consoleSize;
	GetWindowRect(consoleWindow, &consoleSize);
	MoveWindow(consoleWindow, consoleSize.left, consoleSize.top, 1280, 720, true);
#endif

    auto app = CreateApplication();
    app->Run();
    delete app;
    return 0;
}