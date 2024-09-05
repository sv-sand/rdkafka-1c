#include "Strings.h"

uint32_t Strings::GetLength(const WCHAR_T* Source)
{
    uint32_t res = 0;
    WCHAR_T* tmpShort = (WCHAR_T*)Source;

    while (*tmpShort++)
        ++res;

    return res;
}

/////////////////////////////////////////////////////////////////////////////
// String conversion without memory allocation
// You must allcate memory before for Dest before invoke this methods

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

/////////////////////////////////////////////////////////////////////////////
// String conversion with memory allocation
// You must delete result array after invoke this methods (delete [])

wchar_t* Strings::ToWchar(const char* Source, uint32_t Length)
{
    size_t converted;
    size_t size = Length + 1;
    wchar_t* dest = new wchar_t[size];

    errno_t result = mbstowcs_s(&converted, dest, size, Source, Length);

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

    errno_t result = wcstombs_s(&converted, dest, size, Source, Length);

    return dest;
}

/////////////////////////////////////////////////////////////////////////////
//  Date & Time

const std::string Strings::TimeStamp()
{
    std::chrono::time_point now = std::chrono::high_resolution_clock::now();
    tm current{};

#ifdef WIN32
    time_t time = std::time(NULL);
    localtime_s(&current, &time);
#else
    auto time = std::chrono::system_clock::to_time_t(now);
    current = *std::gmtime(&time);
#endif

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds> (now.time_since_epoch());
    auto nanosecond = duration.count() % 1000000000;

    std::ostringstream stream;
    stream << std::put_time(&current, "%Y-%m-%d %T") << "." << nanosecond;

    return stream.str();
}

std::string Strings::ToString(int64_t timestamp)
{
    int milliseconds = timestamp % 1000;
    time_t seconds = (time_t)(timestamp / 1000);
    
#ifdef WIN32
    tm time = *localtime(&seconds); 
#else
    tm time = *std::gmtime(&seconds);
#endif

    std::ostringstream stream;
    stream << std::put_time(&time, "%Y-%m-%d %T") << "." << milliseconds;
    return stream.str();
}

/////////////////////////////////////////////////////////////////////////////
//  Other methods

std::string Strings::NewUuid()
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    return boost::uuids::to_string(uuid);
}

std::multimap<std::string, std::string> Strings::SplitString(std::string String, std::string PairSeporator, std::string KeyValueSeporator)
{
    std::multimap<std::string, std::string> map;

    std::vector<std::string> pairs;
    boost::algorithm::split(pairs, String, boost::is_any_of(PairSeporator));

    for (auto& pair : pairs)
    {
        std::vector<std::string> keyValue;
        boost::algorithm::split(keyValue, pair, boost::is_any_of(KeyValueSeporator));

        if (keyValue.size() == 2)
        {
            std::string key = keyValue[0];
            std::string value = keyValue[1];
            boost::algorithm::trim_all(key);
            boost::algorithm::trim_all(value);
            map.insert(std::pair<std::string, std::string>(key, value));
        }
    }

    return map;
}
