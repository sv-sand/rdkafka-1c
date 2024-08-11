﻿#include "AddInNative.h"

#ifdef WIN32
#pragma setlocale("ru-RU")
#endif

static const wchar_t *g_PropNames[] = {
    L"ComponentVersion",
    L"RdKafkaVersion",
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
    L"StartLogging",
    L"StopLogging",
    L"SetConfigProperty",
    L"InitProducer",
    L"Produce",
    L"StopProducer",
    L"InitConsumer",
    L"Consume",
    L"GetMessageData",
    L"GetMessageMetadata",
    L"CommitOffset",
    L"StopConsumer",
};
static const wchar_t *g_MethodNamesRu[] = {
    L"НачатьЛогирование",
    L"ОстановитьЛогирование",
    L"УстановитьПараметр",
    L"ИнициализироватьПродюсера",
    L"Отправить",
    L"ОстановитьПродюсера",
    L"ИнициализироватьКонсюмера",
    L"Прочитать",
    L"ДанныеСообщения",
    L"МетаданныеСообщения",
    L"ЗафиксироватьОффсет",
    L"ОстановитьКонсюмера",
};

static const WCHAR_T g_kClassNames[] = u"RdKafka1C"; //"|OtherClass1|OtherClass2";
static IAddInDefBase *pAsyncEvent = NULL;

static AppCapabilities g_capabilities = eAppCapabilitiesInvalid;
static std::u16string s_names(g_kClassNames);

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
    return ToShortWchar(wsExtensionName, EXTENSION_NAME, wcslen(EXTENSION_NAME)) > 0;
}

long CAddInNative::GetNProps()
{ 
    // You may delete next lines and add your own implementation code here
    return ePropLast;
}

long CAddInNative::FindProp(const WCHAR_T* wsPropName)
{ 
    long plPropNum = -1;
    wchar_t* propName = Strings::ToWchar(wsPropName, Strings::GetLength(wsPropName));
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

    wchar_t *wsCurrentName = nullptr;
    WCHAR_T *wsPropName = nullptr;
    
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
    
    ToShortWchar(&wsPropName, wsCurrentName, wcslen(wsCurrentName));

    return wsPropName;
}

bool CAddInNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{ 
    errorDescription = "";
    
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
        SetVariant(pvarPropVal, ErrorDescription());
        return true;

    }

    return false;
}

bool CAddInNative::SetPropVal(const long lPropNum, tVariant *varPropVal)
{ 
    errorDescription = "";

    switch (lPropNum)
    {
    case ePropOperationTimeout:
        rdk1c->OperationTimeout = TV_INT(varPropVal);
        return true;
    
    case ePropLogLevel:
        return SetLogLevel(varPropVal);
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
    wchar_t* name = Strings::ToWchar(wsMethodName, Strings::GetLength(wsMethodName));

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

    WCHAR_T* wsMethodName = nullptr;
    wchar_t *wsCurrentName = nullptr;
    
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

    ToShortWchar(&wsMethodName, wsCurrentName, wcslen(wsCurrentName));

    return wsMethodName;
}

long CAddInNative::GetNParams(const long lMethodNum)
{ 
    switch(lMethodNum)
    { 
        case eMethStartLogging:
            return 1;
        case eMethSetConfigProperty:
            return 2;
        case eMethInitProducer:
            return 2;
        case eMethProduce:
            return 4;
        case eMethInitConsumer:
            return 3;
    }
    
    return 0;
}

bool CAddInNative::GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant *pvarParamDefValue)
{ 
    switch (lMethodNum)
    {
    case eMethProduce:
        switch (lParamNum)
        {
        case 1:
            SetVariant(pvarParamDefValue, "");
            return true;
        case 2:
            SetVariant(pvarParamDefValue, "");
            return true;
        case 3:
            SetVariant(pvarParamDefValue, 0);
            return true;
        }
    }   
    
    TV_VT(pvarParamDefValue) = VTYPE_EMPTY;
    return false;
} 

bool CAddInNative::HasRetVal(const long lMethodNum)
{ 
    switch(lMethodNum)
    { 
        case eMethStartLogging:
        case eMethInitProducer:
        case eMethProduce:
        case eMethStopProducer:
        case eMethInitConsumer:
        case eMethConsume:
        case eMethGetMessageData:
        case eMethGetMessageMetadata:
        case eMethCommitOffset:
        case eMethStopConsumer:
            return true;
    }

    return false;
}

bool CAddInNative::CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray)
{ 
    errorDescription = "";

    switch (lMethodNum)
    {
    case eMethStopLogging:        
        return StopLogging();

    case eMethSetConfigProperty:        
        return SetConfigProperty(paParams, lSizeArray);
    }

    return false;
}

bool CAddInNative::CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{ 
    errorDescription = "";

    switch(lMethodNum)
    {
    case eMethStartLogging:        
        return StartLogging(pvarRetValue, paParams, lSizeArray);

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

    case eMethCommitOffset:
        return CommitOffset(pvarRetValue, paParams, lSizeArray);

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
// General action

std::string CAddInNative::ErrorDescription()
{
    if (!errorDescription.empty())
        return errorDescription;

    return rdk1c->ErrorDescription();
}

bool CAddInNative::StartLogging(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    if (lSizeArray != 1 || !paParams || TV_VT(paParams) != VTYPE_PWSTR)
    {
        errorDescription = "Invalid parameters";
        return false;
    }

    std::string directory = ToString(&paParams[0]);
    
    auto result = rdk1c->StartLogging(directory);
    SetVariant(pvarRetValue, result);

    return true;
}

bool CAddInNative::StopLogging()
{
    rdk1c->StopLogging();
    return true;
}

bool CAddInNative::SetLogLevel(tVariant* varPropVal)
{
    std::string level = ToString(varPropVal);
    
    if (level == "none")
        rdk1c->SetLogerLevel(Loger::Levels::NONE);
    else if (level == "debug")
        rdk1c->SetLogerLevel(Loger::Levels::DEBUG);
    else if (level == "info")
        rdk1c->SetLogerLevel(Loger::Levels::INFO);
    else if (level == "warn")
        rdk1c->SetLogerLevel(Loger::Levels::WARN);
    else if (level == "error")
        rdk1c->SetLogerLevel(Loger::Levels::ERRORS);
    else
    {
        errorDescription = "Faled to convert value '" + level + "' to log level. Valid values: none, debug, info, warn, error";
        return false;
    }

    return true;    
}

std::string CAddInNative::GetLogLevel()
{
    switch (rdk1c->GetLogerLevel())
    {
    case Loger::Levels::NONE:
        return "none";
    case Loger::Levels::DEBUG:
        return "debug";
    case Loger::Levels::INFO:
        return "info";
    case Loger::Levels::WARN:
        return "warn";
    case Loger::Levels::ERRORS:
        return "error";
    default:
        return "undefined";
    }
}

bool CAddInNative::SetConfigProperty(tVariant* paParams, const long lSizeArray)
{
    if (lSizeArray != 2 || !paParams || TV_VT(paParams) != VTYPE_PWSTR)
    {
        errorDescription = "Invalid parameters";
        return false;
    }
    
    std::string paramName = ToString(&paParams[0]);
    std::string paramValue = ToString(&paParams[1]);

    rdk1c->SetConfigProperty(paramName, paramValue);

    return true;
}

bool CAddInNative::InitProducer(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    if (lSizeArray != 2 || !paParams || TV_VT(paParams) != VTYPE_PWSTR)
    {
        errorDescription = "Invalid parameters";
        return false;
    }

    std::string brokers = ToString(&paParams[0]);
    std::string topic = ToString(&paParams[1]);
        
    auto result = rdk1c->InitProducer(brokers, topic);
    SetVariant(pvarRetValue, result);

    return true;
}

bool CAddInNative::Produce(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    if (lSizeArray != 4 || !paParams || TV_VT(paParams) != VTYPE_PWSTR)
    {
        errorDescription = "Invalid parameters";
        return false;
    }

    std::string message = ToString(&paParams[0]);
    std::string key = ToString(&paParams[1]);
    std::string headers = ToString(&paParams[2]);
    int partition = paParams[3].intVal;

    auto result = rdk1c->Produce(message, key, headers, partition);
    SetVariant(pvarRetValue, result);

    return true;
}

bool CAddInNative::StopProducer(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    auto result = rdk1c->StopProduser();
    SetVariant(pvarRetValue, result);

    return true;
}

bool CAddInNative::InitConsumer(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    if (lSizeArray != 3 || !paParams || TV_VT(paParams) != VTYPE_PWSTR)
    {
        errorDescription = "Invalid parameters";
        return false;
    }

    std::string brokers = ToString(&paParams[0]);
    std::string topic = ToString(&paParams[1]);
    std::string groupId = ToString(&paParams[2]);

    bool initResult = rdk1c->InitConsumer(brokers, groupId);
    if (!initResult)
    {
        SetVariant(pvarRetValue, false);
        return false;
    }

    bool subscribeResult = rdk1c->Subscribe(topic);
    if (!subscribeResult)
    {
        SetVariant(pvarRetValue, false);
        return false;
    }

    // Set success
    SetVariant(pvarRetValue, true);

    return true;
}

bool CAddInNative::Consume(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    auto result = rdk1c->Consume();
    SetVariant(pvarRetValue, result);
    return true;
}

bool CAddInNative::GetMessageData(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    auto result = rdk1c->GetMessageData();
    SetVariant(pvarRetValue, result);
    return true;
}

bool CAddInNative::GetMessageMetadata(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    auto result = rdk1c->GetMessageMetadata();
    SetVariant(pvarRetValue, result);
    return true;
}

bool CAddInNative::CommitOffset(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    if (lSizeArray != 3 || !paParams || TV_VT(paParams) != VTYPE_PWSTR)
    {
        errorDescription = "Invalid parameters";
        return false;
    }

    std::string topic = ToString(&paParams[0]);
    int offcet = paParams[1].intVal;
    int partition = paParams[2].intVal;

    auto result = rdk1c->CommitOffset(topic, offcet, partition);
    SetVariant(pvarRetValue, result);

    return true;
}

bool CAddInNative::StopConsumer(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    auto result = rdk1c->StopConsumer();
    SetVariant(pvarRetValue, result);
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Support methods


/////////////////////////////////////////////////////////////////////////////
// String conversion

std::string CAddInNative::ToString(tVariant* Source)
{
    wchar_t* wcstr = Strings::ToWchar(Source->pwstrVal, Source->wstrLen);
    char* cstr = Strings::ToChar(wcstr, Source->wstrLen);

    std::string result = std::string(cstr);

    delete[] wcstr;
    delete[] cstr;

    return result;
}

void CAddInNative::SetVariant(tVariant* Dest, std::string Source)
{
    TV_VT(Dest) = VTYPE_PWSTR;
    Dest->wstrLen = ToShortWchar(&TV_WSTR(Dest), Source.c_str(), Source.length());
}

void CAddInNative::SetVariant(tVariant* Dest, const wchar_t* Source)
{
    TV_VT(Dest) = VTYPE_PWSTR;
    Dest->wstrLen = ToShortWchar(&TV_WSTR(Dest), Source, wcslen(Source));
}

void CAddInNative::SetVariant(tVariant* Dest, const char* Source)
{
    TV_VT(Dest) = VTYPE_PWSTR;
    Dest->wstrLen = ToShortWchar(&TV_WSTR(Dest), Source, strlen(Source));
}

void CAddInNative::SetVariant(tVariant* Dest, int Source)
{
    TV_VT(Dest) = VTYPE_INT;
    TV_INT(Dest) = Source;
}

void CAddInNative::SetVariant(tVariant* Dest, bool Source)
{
    TV_VT(Dest) = VTYPE_BOOL;
    TV_BOOL(Dest) = Source;
}

// Conversion with memory allocation
// Platform 1C control memory via garbage collector m_iMemory

uint32_t CAddInNative::ToShortWchar(WCHAR_T** Dest, const char* Source, uint32_t Length)
{
    wchar_t* wcstr = Strings::ToWchar(Source, Length);
    uint32_t result = ToShortWchar(Dest, wcstr, Length);

    delete[] wcstr;

    return result;
}

uint32_t CAddInNative::ToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t Length)
{
    if (!m_iMemory)
        return 0;

    size_t size = Length + 1;

    if (!m_iMemory->AllocMemory((void**)Dest, size * sizeof(WCHAR_T)))
        return 0;

    return Strings::ConvertToShortWchar(Dest, Source, Length);
}

