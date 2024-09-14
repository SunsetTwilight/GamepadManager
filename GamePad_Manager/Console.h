#pragma once
#include <Windows.h>

BOOL InitConsole()
{
	if (AttachConsole(ATTACH_PARENT_PROCESS) == 0) {
		//ErrorMessaget((LPTSTR)L"AttachConsole");
	}

	if (AllocConsole() == 0) {
		//ErrorMessage((LPTSTR)L"AllocConsole");
	}

	FILE* fp = NULL;
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);

	return TRUE;
}
BOOL CleanUpConsole()
{
	if (FreeConsole() == 0) {
		//ErrorMessage((LPTSTR)L"FreeConsole");
	}

	return TRUE;
}