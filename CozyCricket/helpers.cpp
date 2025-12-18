#include <winnls.h>

const char* PWSTR_to_Char(const wchar_t* wideStr)
{
	int size = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);

	char* buffer = new char[size];

	WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, buffer, size, NULL, NULL);

	return buffer;
}
