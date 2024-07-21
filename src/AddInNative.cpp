#include "AddInNative.h"

#ifdef WIN32
#pragma setlocale("ru-RU")
#endif

static const wchar_t *g_PropNames[] = {
    L"ComponentVersion",
    L"RdKafkaVersion",
    L"LogFile",
    L"LogLevel",
    L"OperationTimeout",
    L"ErrorDescription",
};
static const wchar_t *g_PropNamesRu[] = {
    L"ВерсияКомпоненты",
    L"ВерсияRdKafka",
    L"ЛогФайл",
    L"УровеньЛогирования",
    L"ТаймаутОпераций",
    L"ОписаниеОшибки",
};

static const wchar_t *g_MethodNames[] = {
    L"EnableLogging",
    L"SetConfigProperty",
    L"InitProducer",
    L"Produce",
    L"StopProducer",
    L"InitConsumer",
    L"Consume",
    L"GetMessageData",
    L"GetMessageMetadata",
    L"StopConsumer",
};
static const wchar_t *g_MethodNamesRu[] = {
    L"ВключитьЛогирование",
    L"УстановитьПараметр",
    L"ИнициализироватьПродюсера",
    L"Отправить",
    L"ОстановитьПродюсера",
    L"ИнициализироватьКонсюмера",
    L"Прочитать",
    L"ДанныеСообщения",
    L"МетаданныеСообщения",
    L"ОстановитьКонсюмера",
};

static const WCHAR_T g_kClassNames[] = u"Connector"; //"|OtherClass1|OtherClass2";
static IAddInDefBase *pAsyncEvent = NULL;

static AppCapabilities g_capabilities = eAppCapabilitiesInvalid;
static std::u16string s_names(g_kClassNames);

Loger::Levels LogLevelFromString(std::string Level);
std::string LogLevelToString(Loger::Levels Level);

uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, size_t len = 0);
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len = 0);
uint32_t getLenShortWcharStr(const WCHAR_T* Source);
wchar_t* convToWchar(const char* Source);
wchar_t* convToWchar(const WCHAR_T* Source);
WCHAR_T* convToShortWchar(const char* Source);
WCHAR_T* convToShortWchar(const wchar_t* Source);
char* convToChar(const wchar_t* source);

/////////////////////////////////////////////////////////////////////////////
// IComponentBase

long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface)
{
    if(!*pInterface)
    {
        *pInterface= new CAddInNative;
        return (long)*pInterface;
    }
    return 0;
}

AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities)
{
    g_capabilities = capabilities;
    return eAppCapabilitiesLast;
}

AttachType GetAttachType()
{
    return eCanAttachAny;
}

long DestroyObject(IComponentBase** pIntf)
{
    if(!*pIntf)
        return -1;

    delete *pIntf;
    *pIntf = 0;
    return 0;
}

const WCHAR_T* GetClassNames()
{
    return s_names.c_str();
} 

/////////////////////////////////////////////////////////////////////////////
// CAddInNative

CAddInNative::CAddInNative()
{
    m_iMemory = nullptr; 
    m_iConnect = nullptr;
    rdk1c = nullptr;
}

CAddInNative::~CAddInNative()
{
    delete rdk1c;
}

bool CAddInNative::Init(void* pConnection)
{ 
    setlocale(LC_ALL, "ru_RU");

    rdk1c = new RdKafka1C();
    m_iConnect = (IAddInDefBase*)pConnection;
    return m_iConnect != nullptr;
}

long CAddInNative::GetInfo()
{ 
    // Component should put supported component technology version 
    // This component supports 2.0 version
    return 2000; 
}

void CAddInNative::Done()
{
    
}

bool CAddInNative::setMemManager(void* mem)
{
    m_iMemory = (IMemoryManager*)mem;
    return m_iMemory != 0;
}

/////////////////////////////////////////////////////////////////////////////
// ILanguageExtenderBase

bool CAddInNative::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{ 
    return ToShortWchar(wsExtensionName, EXTENSION_NAME) > 0;
}

long CAddInNative::GetNProps()
{ 
    // You may delete next lines and add your own implementation code here
    return ePropLast;
}

long CAddInNative::FindProp(const WCHAR_T* wsPropName)
{ 
    long plPropNum = -1;
    wchar_t* propName = 0;

    ::convFromShortWchar(&propName, wsPropName);
    plPropNum = findName(g_PropNames, propName, ePropLast);

    if (plPropNum == -1)
        plPropNum = findName(g_PropNamesRu, propName, ePropLast);

    delete[] propName;

    return plPropNum;
}

const WCHAR_T* CAddInNative::GetPropName(long lPropNum, long lPropAlias)
{ 
    if (lPropNum >= ePropLast)
        return NULL;

    wchar_t *wsCurrentName = NULL;
    WCHAR_T *wsPropName = NULL;
    
    switch(lPropAlias)
    {
    case 0: // First language
        wsCurrentName = (wchar_t*)g_PropNames[lPropNum];
        break;
    case 1: // Second language
        wsCurrentName = (wchar_t*)g_PropNamesRu[lPropNum];
        break;
    default:
        return NULL;
    }
    
    ToShortWchar(&wsPropName, wsCurrentName);

    return wsPropName;
}

bool CAddInNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{ 
    switch(lPropNum)
    {
    case ePropComponentVersion:
        SetVariant(pvarPropVal, COMPONENT_VERSION);
        return true;

    case ePropRdKafkaVersion:
        SetVariant(pvarPropVal, rdk1c->RdKafkaVersion());
        return true;

    case ePropLogFile:
        SetVariant(pvarPropVal, rdk1c->GetCurrentLogFile());
        return true;

    case ePropLogLevel:
        SetVariant(pvarPropVal, GetLogLevel());
        return true;

    case ePropOperationTimeout:
        SetVariant(pvarPropVal, rdk1c->OperationTimeout);
        return true;

    case ePropErrorDescription:
        SetVariant(pvarPropVal, rdk1c->ErrorDescription());
        return true;

    }

    return false;
}

bool CAddInNative::SetPropVal(const long lPropNum, tVariant *varPropVal)
{ 
    std::string logLevel;
    Loger::Levels level;
    
    switch (lPropNum)
    {
    case ePropOperationTimeout:
        rdk1c->OperationTimeout = TV_INT(varPropVal);
        return true;
    
    case ePropLogLevel:
        return SetLogLevel(TV_WSTR(varPropVal));
    }
    
    return false;
}

bool CAddInNative::IsPropReadable(const long lPropNum)
{ 
    switch(lPropNum)
    { 
    case ePropComponentVersion:
    case ePropRdKafkaVersion:
    case ePropLogFile:
    case ePropLogLevel:
    case ePropOperationTimeout:
    case ePropErrorDescription:
        return true;
    }

    return false;
}

bool CAddInNative::IsPropWritable(const long lPropNum)
{
    switch (lPropNum)
    {
    case ePropLogLevel:
    case ePropOperationTimeout:
        return true;
    }

    return false;
}

long CAddInNative::GetNMethods()
{ 
    return eMethLast;
}

long CAddInNative::FindMethod(const WCHAR_T* wsMethodName)
{ 
    long plMethodNum = -1;
    wchar_t* name = 0;

    ::convFromShortWchar(&name, wsMethodName);

    plMethodNum = findName(g_MethodNames, name, eMethLast);

    if (plMethodNum == -1)
        plMethodNum = findName(g_MethodNamesRu, name, eMethLast);

    delete[] name;

    return plMethodNum;
}

const WCHAR_T* CAddInNative::GetMethodName(const long lMethodNum, const long lMethodAlias)
{ 
    if (lMethodNum >= eMethLast)
        return NULL;

    WCHAR_T* wsMethodName = NULL;
    wchar_t *wsCurrentName = NULL;
    
    switch(lMethodAlias)
    {
    case 0: // First language
        wsCurrentName = (wchar_t*)g_MethodNames[lMethodNum];
        break;
    case 1: // Second language
        wsCurrentName = (wchar_t*)g_MethodNamesRu[lMethodNum];
        break;
    default: 
        return 0;
    }

    ToShortWchar(&wsMethodName, wsCurrentName);

    return wsMethodName;
}

long CAddInNative::GetNParams(const long lMethodNum)
{ 
    switch(lMethodNum)
    { 
    case eMethEnableLogging:
        return 2;
    case eMethSetConfigProperty:
        return 2;
    case eMethInitProducer:
        return 2;
    case eMethProduce:
        return 3;
    case eMethInitConsumer:
        return 3;
    }
    
    return 0;
}

bool CAddInNative::GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant *pvarParamDefValue)
{ 
    TV_VT(pvarParamDefValue)= VTYPE_EMPTY;

    return false;
} 

bool CAddInNative::HasRetVal(const long lMethodNum)
{ 
    switch(lMethodNum)
    { 
    case eMethEnableLogging:
    case eMethInitProducer:
    case eMethProduce:
    case eMethStopProducer:
    case eMethInitConsumer:
    case eMethConsume:
    case eMethGetMessageData:
    case eMethGetMessageMetadata:
    case eMethStopConsumer:
        return true;
    }

    return false;
}

bool CAddInNative::CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray)
{ 
    switch (lMethodNum)
    {
    case eMethSetConfigProperty:        
        return SetConfigProperty(paParams, lSizeArray);
    }

    return false;
}

bool CAddInNative::CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{ 
    switch(lMethodNum)
    {
    case eMethEnableLogging:        
        return EnableLogging(pvarRetValue, paParams, lSizeArray);

    case eMethInitProducer:        
        return InitProducer(pvarRetValue, paParams, lSizeArray);

    case eMethProduce:
        return Produce(pvarRetValue, paParams, lSizeArray);

    case eMethStopProducer:
        return StopProducer(pvarRetValue, paParams, lSizeArray);

    case eMethInitConsumer:
        return InitConsumer(pvarRetValue, paParams, lSizeArray);

    case eMethConsume:
        return Consume(pvarRetValue, paParams, lSizeArray);

    case eMethGetMessageData:
        return GetMessageData(pvarRetValue, paParams, lSizeArray);

    case eMethGetMessageMetadata:
        return GetMessageMetadata(pvarRetValue, paParams, lSizeArray);

    case eMethStopConsumer:
        return StopConsumer(pvarRetValue, paParams, lSizeArray);

    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////
// UserLanguageBase

void ADDIN_API CAddInNative::SetUserInterfaceLanguageCode(const WCHAR_T * lang)
{
    m_userLang.assign(lang);
}

/////////////////////////////////////////////////////////////////////////////
// LocaleBase

void CAddInNative::SetLocale(const WCHAR_T* loc)
{
#if !defined( __linux__ ) && !defined(__APPLE__)
    _wsetlocale(LC_ALL, (wchar_t*)loc);
#else
    //We convert in char* char_locale
    //also we establish locale
    //setlocale(LC_ALL, char_locale);
#endif
}

void CAddInNative::addError(uint32_t wcode, const wchar_t* source, const wchar_t* descriptor, long code)
{
    if (!m_iConnect)
        return;

    WCHAR_T* err = nullptr;
    WCHAR_T* descr = nullptr;

    ::convToShortWchar(&err, source);
    ::convToShortWchar(&descr, descriptor);

    m_iConnect->AddError(wcode, err, descr, code);

    delete[] err;
    delete[] descr;
}

long CAddInNative::findName(const wchar_t* names[], const wchar_t* name, const uint32_t size) const
{
    long ret = -1;
    for (uint32_t i = 0; i < size; i++)
    {
        if (!wcscmp(names[i], name))
        {
            ret = i;
            break;
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
// Additional

bool CAddInNative::SetLogLevel(WCHAR_T* pwstrLogLevel)
{
    std::string stringLevel = ToString(pwstrLogLevel);
    Loger::Levels level;

    try
    {
        level = LogLevelFromString(stringLevel);
    }
    catch (std::string e)
    {
        wchar_t* error = convToWchar(e.c_str());
        addError(ADDIN_E_VERY_IMPORTANT, EXTENSION_NAME, error, -1);
        delete[] error;
        return false;
    }

    rdk1c->SetLogerLevel(level);
}

std::string CAddInNative::GetLogLevel()
{
    return LogLevelToString(rdk1c->GetLogerLevel());
}

bool CAddInNative::SetConfigProperty(tVariant* paParams, const long lSizeArray)
{
    if (lSizeArray != 2 || !paParams || TV_VT(paParams) != VTYPE_PWSTR)
    {
        addError(ADDIN_E_VERY_IMPORTANT, EXTENSION_NAME, L"Invalid parameters", -1);
        return false;
    }
    
    std::string paramName = ToString(paParams[0].pwstrVal);
    std::string paramValue = ToString(paParams[1].pwstrVal);

    rdk1c->SetConfigProperty(paramName, paramValue);

    return true;
}

bool CAddInNative::EnableLogging(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    if (lSizeArray != 2 || !paParams || TV_VT(paParams) != VTYPE_PWSTR)
    {
        addError(ADDIN_E_VERY_IMPORTANT, EXTENSION_NAME, L"Invalid parameters", -1);
        return false;
    }

    std::string directory = ToString(paParams[0].pwstrVal);
    std::string stringLevel = ToString(paParams[1].pwstrVal);

    Loger::Levels level;
    try
    {
        level = LogLevelFromString(stringLevel);
    }
    catch (std::string e)
    {
        wchar_t* error = convToWchar(e.c_str());
        addError(ADDIN_E_VERY_IMPORTANT, EXTENSION_NAME, error, -1);
        delete[] error;
        return false;
    }

    TV_VT(pvarRetValue) = VTYPE_BOOL;
    TV_BOOL(pvarRetValue) = rdk1c->InitLoger(directory, level);

    return true;
}

bool CAddInNative::InitProducer(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    if (lSizeArray != 2 || !paParams || TV_VT(paParams) != VTYPE_PWSTR)
    {
        addError(ADDIN_E_VERY_IMPORTANT, EXTENSION_NAME, L"Invalid parameters", -1);
        return false;
    }

    std::string brokers = ToString(paParams[0].pwstrVal);
    std::string topic = ToString(paParams[1].pwstrVal);

    TV_VT(pvarRetValue) = VTYPE_BOOL;
    TV_BOOL(pvarRetValue) = rdk1c->InitProducer(brokers, topic);

    return true;
}

bool CAddInNative::Produce(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    if (lSizeArray != 3 || !paParams || TV_VT(paParams) != VTYPE_PWSTR)
    {
        addError(ADDIN_E_VERY_IMPORTANT, EXTENSION_NAME, L"Invalid parameters", -1);
        return false;
    }

    std::string message = ToString(paParams[0].pwstrVal);
    std::string headers = ToString(paParams[1].pwstrVal);
    int partition = paParams[2].intVal;

    TV_VT(pvarRetValue) = VTYPE_BOOL;
    TV_BOOL(pvarRetValue) = rdk1c->Produce(message, headers, partition);

    return true;
}

bool CAddInNative::StopProducer(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    TV_VT(pvarRetValue) = VTYPE_BOOL;
    TV_BOOL(pvarRetValue) = rdk1c->StopProduser();
    return true;
}

bool CAddInNative::InitConsumer(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    if (lSizeArray != 3 || !paParams || TV_VT(paParams) != VTYPE_PWSTR)
    {
        addError(ADDIN_E_VERY_IMPORTANT, EXTENSION_NAME, L"Invalid parameter", -1);
        return false;
    }

    std::string brokers = ToString(paParams[0].pwstrVal);
    std::string topic = ToString(paParams[1].pwstrVal);
    std::string groupId = ToString(paParams[2].pwstrVal);

    TV_VT(pvarRetValue) = VTYPE_BOOL;
    TV_BOOL(pvarRetValue) = rdk1c->InitConsumer(brokers, topic, groupId);

    return true;
}

bool CAddInNative::Consume(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    TV_VT(pvarRetValue) = VTYPE_BOOL;
    TV_BOOL(pvarRetValue) = rdk1c->Consume();
    return true;
}

bool CAddInNative::GetMessageData(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    TV_VT(pvarRetValue) = VTYPE_PWSTR;
    pvarRetValue->wstrLen = ToShortWchar(&TV_WSTR(pvarRetValue), rdk1c->GetMessageData().c_str());
    return true;
}

bool CAddInNative::GetMessageMetadata(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    TV_VT(pvarRetValue) = VTYPE_PWSTR;
    pvarRetValue->wstrLen = ToShortWchar(&TV_WSTR(pvarRetValue), rdk1c->GetMessageMetadata().c_str());
    return true;
}

bool CAddInNative::StopConsumer(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    TV_VT(pvarRetValue) = VTYPE_BOOL;
    TV_BOOL(pvarRetValue) = rdk1c->StopConsumer();
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Support methods

Loger::Levels LogLevelFromString(std::string Level)
{
    Loger::Levels level;

    if (Level == "none")
        level = Loger::Levels::NONE;
    else if (Level == "debug")
        level = Loger::Levels::DEBUG;
    else if (Level == "info")
        level = Loger::Levels::INFO;
    else if (Level == "warn")
        level = Loger::Levels::WARN;
    else if (Level == "error")
        level = Loger::Levels::ERRORS;
    else
        throw "Faled to convert value '" + Level + "' to log level. Valid values: none, debug, info, warn, error";

    return level;
}

std::string LogLevelToString(Loger::Levels Level)
{
    std::string level;
    
    switch (Level)
    {
    case Loger::Levels::NONE: 
        level = "none"; 
        break;
    case Loger::Levels::DEBUG: 
        level = "debug"; 
        break;
    case Loger::Levels::INFO: 
        level = "info"; 
        break;
    case Loger::Levels::WARN: 
        level = "warn"; 
        break;
    case Loger::Levels::ERRORS: 
        level = "error"; 
        break;
    default:
        level = "undefined";
    }
    
    return level;
}

/////////////////////////////////////////////////////////////////////////////
// String conversion

// Conversion with memory allocation
// Platform 1C control memory (m_iMemory)

void CAddInNative::SetVariant(tVariant* Dest, std::string Source)
{
    TV_VT(Dest) = VTYPE_PWSTR;
    Dest->wstrLen = ToShortWchar(&TV_WSTR(Dest), Source.c_str());
}

void CAddInNative::SetVariant(tVariant* Dest, const wchar_t* Source)
{
    TV_VT(Dest) = VTYPE_PWSTR;
    Dest->wstrLen = ToShortWchar(&TV_WSTR(Dest), Source);
}

void CAddInNative::SetVariant(tVariant* Dest, const char* Source)
{
    TV_VT(Dest) = VTYPE_PWSTR;
    Dest->wstrLen = ToShortWchar(&TV_WSTR(Dest), Source);
}

void CAddInNative::SetVariant(tVariant* Dest, int Source)
{
    TV_VT(Dest) = VTYPE_INT;
    TV_INT(Dest) = Source;
}

uint32_t CAddInNative::ToShortWchar(WCHAR_T** Dest, const char* Source)
{
    wchar_t* wcstr = convToWchar(Source);
    uint32_t result = ToShortWchar(Dest, wcstr);

    delete[] wcstr;

    return result;
}

uint32_t CAddInNative::ToShortWchar(WCHAR_T** Dest, const wchar_t* Source)
{
    if (!m_iMemory)
        return 0;

    size_t size = wcslen(Source) + 1;

    if (!m_iMemory->AllocMemory((void**)Dest, (unsigned)size * sizeof(WCHAR_T)))
        return 0;

    return convToShortWchar(Dest, Source, size) - 1;
}

std::string CAddInNative::ToString(const WCHAR_T* Source)
{
    wchar_t* wcstr = convToWchar(Source);
    char* cstr = convToChar(wcstr);

    std::string result = std::string(cstr);

    delete[] wcstr;
    delete[] cstr;

    return result;
}

// Conversion without memory allocation
// Do need memory allcate before in Dest

uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, size_t len)
{
    if (!len)
        len = ::wcslen(Source) + 1;

    if (!*Dest)
        *Dest = new WCHAR_T[len];

    WCHAR_T* tmpShort = *Dest;
    wchar_t* tmpWChar = (wchar_t*) Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len * sizeof(WCHAR_T));
#ifdef __linux__
    size_t succeed = (size_t)-1;
    size_t f = len * sizeof(wchar_t), t = len * sizeof(WCHAR_T);
    const char* fromCode = sizeof(wchar_t) == 2 ? "UTF-16" : "UTF-32";
    iconv_t cd = iconv_open("UTF-16LE", fromCode);
    if (cd != (iconv_t)-1)
    {
        succeed = iconv(cd, (char**)&tmpWChar, &f, (char**)&tmpShort, &t);
        iconv_close(cd);
        if(succeed != (size_t)-1)
            return (uint32_t)succeed;
    }
#endif //__linux__
    for (; len; --len, ++res, ++tmpWChar, ++tmpShort)
    {
        *tmpShort = (WCHAR_T)*tmpWChar;
    }

    return res;
}

uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len)
{
    if (!len)
        len = getLenShortWcharStr(Source) + 1;

    if (!*Dest)
        *Dest = new wchar_t[len];

    wchar_t* tmpWChar = *Dest;
    WCHAR_T* tmpShort = (WCHAR_T*)Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len * sizeof(wchar_t));
#ifdef __linux__
    size_t succeed = (size_t)-1;
    const char* fromCode = sizeof(wchar_t) == 2 ? "UTF-16" : "UTF-32";
    size_t f = len * sizeof(WCHAR_T), t = len * sizeof(wchar_t);
    iconv_t cd = iconv_open("UTF-32LE", fromCode);
    if (cd != (iconv_t)-1)
    {
        succeed = iconv(cd, (char**)&tmpShort, &f, (char**)&tmpWChar, &t);
        iconv_close(cd);
        if(succeed != (size_t)-1)
            return (uint32_t)succeed;
    }
#endif //__linux__
    for (; len; --len, ++res, ++tmpWChar, ++tmpShort)
    {
        *tmpWChar = (wchar_t)*tmpShort;
    }

    return res;
}

uint32_t getLenShortWcharStr(const WCHAR_T* Source)
{
    uint32_t res = 0;
    WCHAR_T *tmpShort = (WCHAR_T*)Source;

    while (*tmpShort++)
        ++res;

    return res;
}

// Conversion without memory allocation
// Do need delete result array after invoke (delete [])

wchar_t* convToWchar(const char* Source)
{
    size_t converted;
    size_t len = strlen(Source) + 1;
    wchar_t* dest = new wchar_t[len];
        
    errno_t result = mbstowcs_s(&converted, dest, len, Source, len);
    if (result)
        for (int i = 0; i < len; i++)
            dest[i] = 0;

    return dest;
}

wchar_t* convToWchar(const WCHAR_T* Source)
{
    size_t len = getLenShortWcharStr(Source) + 1;
    wchar_t* dest = new wchar_t[len];

    convFromShortWchar(&dest, Source);

    return dest;
}

WCHAR_T* convToShortWchar(const char* Source)
{
    wchar_t* wcstr = convToWchar(Source);
    WCHAR_T* dest = convToShortWchar(wcstr);

    delete[] wcstr;

    return dest;
}

WCHAR_T* convToShortWchar(const wchar_t* Source)
{
    size_t len = wcslen(Source) + 1;
    WCHAR_T* dest = new WCHAR_T[len];
    
    convToShortWchar(&dest, Source);

    return dest;
}

char* convToChar(const wchar_t* Source)
{
    size_t converted;
    size_t len = wcslen(Source) + 1;
    char* dest = new char[len];
     
    errno_t result = wcstombs_s(&converted, dest, len, Source, len);
    if (result)
        for (int i = 0; i < len; i++) 
            dest[i] = 0;

    return dest;
}

