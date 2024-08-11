#include "Strings.h"

uint32_t Strings::GetLength(const WCHAR_T* Source)
{
    uint32_t res = 0;
    WCHAR_T* tmpShort = (WCHAR_T*)Source;

    while (*tmpShort++)
        ++res;

    return res;
}

/*
 * String conversion without memory allocation
 * You must allcate memory before for Dest before invoke this methods
 */

uint32_t Strings::ConvertToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t Length)
{
    size_t size = Length + 1;

    WCHAR_T* tmpShort = *Dest;
    wchar_t* tmpWChar = (wchar_t*)Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, size * sizeof(WCHAR_T));
#ifdef __linux__
    size_t succeed = (size_t)-1;
    size_t f = size * sizeof(wchar_t), t = size * sizeof(WCHAR_T);
    const char* fromCode = sizeof(wchar_t) == 2 ? "UTF-16" : "UTF-32";
    iconv_t cd = iconv_open("UTF-16LE", fromCode);
    if (cd != (iconv_t)-1)
    {
        succeed = iconv(cd, (char**)&tmpWChar, &f, (char**)&tmpShort, &t);
        iconv_close(cd);
        if (succeed != (size_t)-1)
            return (uint32_t)succeed;
    }
#endif //__linux__
    for (; size; --size, ++res, ++tmpWChar, ++tmpShort)
        *tmpShort = (WCHAR_T)*tmpWChar;

    return res - 1;
}

uint32_t Strings::ConvertToWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t Length)
{
    size_t size = Length + 1;

    wchar_t* tmpWChar = *Dest;
    WCHAR_T* tmpShort = (WCHAR_T*)Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, size * sizeof(wchar_t));
#ifdef __linux__
    size_t succeed = (size_t)-1;
    const char* fromCode = sizeof(wchar_t) == 2 ? "UTF-16" : "UTF-32";
    size_t f = size * sizeof(WCHAR_T), t = size * sizeof(wchar_t);
    iconv_t cd = iconv_open("UTF-32LE", fromCode);
    if (cd != (iconv_t)-1)
    {
        succeed = iconv(cd, (char**)&tmpShort, &f, (char**)&tmpWChar, &t);
        iconv_close(cd);
        if (succeed != (size_t)-1)
            return (uint32_t)succeed;
    }
#endif //__linux__
    for (; size; --size, ++res, ++tmpWChar, ++tmpShort)
        *tmpWChar = (wchar_t)*tmpShort;

    return res - 1;
}

/*
 * String conversion with memory allocation
 * You must delete result array after invoke this methods (delete [])
 */

wchar_t* Strings::ToWchar(const char* Source, uint32_t Length)
{
    size_t converted;
    size_t size = Length + 1;
    wchar_t* dest = new wchar_t[size];

    errno_t result = mbstowcs_s(&converted, dest, size, Source, size);
    if (result)
        for (int i = 0; i < size; i++)
            dest[i] = 0;

    return dest;
}

wchar_t* Strings::ToWchar(const WCHAR_T* Source, uint32_t Length)
{
    size_t size = Length + 1;
    wchar_t* dest = new wchar_t[size];

    ConvertToWchar(&dest, Source, Length);

    return dest;
}

WCHAR_T* Strings::ToShortWchar(const char* Source, uint32_t Length)
{
    wchar_t* wcstr = ToWchar(Source, Length);
    WCHAR_T* dest = ToShortWchar(wcstr, Length);

    delete[] wcstr;

    return dest;
}

WCHAR_T* Strings::ToShortWchar(const wchar_t* Source, uint32_t Length)
{
    size_t size = Length + 1;
    WCHAR_T* dest = new WCHAR_T[size];

    ConvertToShortWchar(&dest, Source, Length);

    return dest;
}

char* Strings::ToChar(const wchar_t* Source, uint32_t Length)
{
    size_t converted;
    size_t size = Length + 1;
    char* dest = new char[size];

    errno_t result = wcstombs_s(&converted, dest, size, Source, size);
    if (result)
        for (int i = 0; i < size; i++)
            dest[i] = 0;

    return dest;
}

