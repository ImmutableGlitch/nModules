/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TextFormatting.cpp                                              July, 2012
 *  The nModules Project
 *
 *  Formats text strings.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "../nShared/Export.h"
#include "TextFormatting.h"
#include <map>
#include <ctime>

using std::map;

// All registered formatters.
map<LPCSTR, FORMATTINGPROC> g_formatters;


/// <summary>
/// Registers a new text formatter
/// </summary>
EXPORT_CDECL(BOOL) RegisterFormatter(LPCSTR function, FORMATTINGPROC pFormatProc) {
    return FALSE;
}


/// <summary>
/// Unregisters a text formatter
/// </summary>
EXPORT_CDECL(BOOL) UnregisterFormatter(LPCSTR function) {
    return FALSE;
}


/// <summary>
/// Formats a piece of text using the registered text formatters
/// </summary>
EXPORT_CDECL(BOOL) FormatText(LPCWSTR pszSource, size_t cchDest, LPWSTR pszDest) {
    time_t t = time(0);
    struct tm now;
    localtime_s(&now, &t);
    if (wcscmp(pszSource, L"time('HH:mm')") == 0) {
        return SUCCEEDED(StringCchPrintfW(pszDest, cchDest, L"%02d:%02d", now.tm_hour, now.tm_min));
    }
    else {
        return SUCCEEDED(StringCchCopyW(pszDest, cchDest, pszSource));
    }
}
