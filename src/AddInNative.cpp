#include "AddInNative.h"

static const wchar_t *g_PropNames[] = {
    L"ComponentVersion",
    L"RdKafkaVersion",
    L"Locale",
    L"LogFile",
    L"LogLevel",
    L"OperationTimeout",
    L"Error",
    L"ErrorDescription",
};
static const wchar_t *g_PropNamesRu[] = {
    L"ВерсияКомпоненты",
    L"ВерсияRdKafka",
    L"Локаль",
    L"ЛогФайл",
    L"УровеньЛогирования",
    L"ТаймаутОпераций",
    L"Отказ",
    L"ОписаниеОшибки",
};

static const wchar_t *g_MethodNames[] = {

    // Logging
    L"StartLogging",
    L"StopLogging",
    L"SetConfigProperty",

    // Producer
    L"InitProducer",
    L"StartProduce",
    L"Produce",
    L"Flush",
    L"StopProducer",
    L"ProducerQueueLen",
    L"CountUndeliveredMessages",
    L"MessageStatus",
    
    // Consumer
    L"InitConsumer",
    L"Consume",
    L"MessageData",
    L"MessageKey",
    L"MessageHeaders",
    L"MessageMetadata",
    L"StopConsumer",
    L"AssignPartition",
    L"CommitOffset",
    L"ChangeOffset",
    L"CommittedOffset",
    L"ConsumerQueueLen",

    // Subscriptions
    L"Subscribe",
    L"Unsubscribe",
    L"Subscription",
};
static const wchar_t *g_MethodNamesRu[] = {
    
    // Логирование
    L"НачатьЛогирование",
    L"ОстановитьЛогирование",
    L"УстановитьПараметр",
    
    // Продюсер
    L"ИнициализироватьПродюсера",
    L"НачатьОтправку",
    L"Отправить",
    L"Слить",
    L"ОстановитьПродюсера",
    L"ОчередьСообщенийПродюсера",
    L"КоличествоНеДоставленныхСообщений",
    L"СтатусСообщения",
    
    // Консюмер
    L"ИнициализироватьКонсюмера",
    L"Прочитать",
    L"ДанныеСообщения",
    L"КлючСообщения",
    L"ЗаголовкиСообщения",
    L"МетаданныеСообщения",
    L"ОстановитьКонсюмера",
    L"УстановитьПартицию",
    L"ЗафиксироватьОффсет",
    L"ИзменитьОффсет",
    L"ПолучитьОффсет",
    L"ОчередьСообщенийКонсюмера",

    // Подписки
    L"Подписаться",
    L"Отписаться",
    L"Подписки",
};

static const WCHAR_T g_kClassNames[] = u"RdKafka1C"; //"|OtherClass1|OtherClass2";
static IAddInDefBase *pAsyncEvent = NULL;

static AppCapabilities g_capabilities = eAppCapabilitiesInvalid;
static std::u16string s_names(g_kClassNames);

/////////////////////////////////////////////////////////////////////////////
// IComponentBase

long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface) {
    if(!*pInterface)
    {
        *pInterface= new CAddInNative;
        return (long)*pInterface;
    }
    return 0;
}

AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities) {
    g_capabilities = capabilities;
    return eAppCapabilitiesLast;
}

AttachType GetAttachType() {
    return eCanAttachAny;
}

long DestroyObject(IComponentBase** pIntf) {
    if(!*pIntf)
        return -1;

    delete *pIntf;
    *pIntf = 0;
    return 0;
}

const WCHAR_T* GetClassNames() {
    return s_names.c_str();
} 

/////////////////////////////////////////////////////////////////////////////
// CAddInNative

CAddInNative::CAddInNative() {

#ifdef WIN32
    SetLocale("ru-RU");
#else
    SetLocale("ru_RU");
#endif
    
    m_iMemory = nullptr; 
    m_iConnect = nullptr;
    loger = nullptr;
    error = nullptr;
    rdk1c = nullptr;
}

CAddInNative::~CAddInNative() {
    delete_pointer(rdk1c);
    delete_pointer(error);
    delete_pointer(loger);
}

bool CAddInNative::Init(void* pConnection) {
    loger = new Loger(); 
    error = new ErrorHandler(loger); 
    rdk1c = new RdKafka1C(loger, error);
    m_iConnect = (IAddInDefBase*)pConnection;
    return m_iConnect != nullptr;
}

long CAddInNative::GetInfo() {
    // Component should put supported component technology version 
    // This component supports 2.0 version
    return 2000; 
}

void CAddInNative::Done() {
}

bool CAddInNative::setMemManager(void* mem) {
    m_iMemory = (IMemoryManager*)mem;
    return m_iMemory != 0;
}

/////////////////////////////////////////////////////////////////////////////
// ILanguageExtenderBase

bool CAddInNative::RegisterExtensionAs(WCHAR_T** wsExtensionName) {
    ToShortWchar(wsExtensionName, EXTENSION_NAME);
    return true;
}

long CAddInNative::GetNProps() {
    // You may delete next lines and add your own implementation code here
    return ePropLast;
}

long CAddInNative::FindProp(const WCHAR_T* wsPropName) {
    long plPropNum = -1;
    wchar_t* propName = Strings::ToWchar(wsPropName);
    plPropNum = findName(g_PropNames, propName, ePropLast);

    if (plPropNum == -1)
        plPropNum = findName(g_PropNamesRu, propName, ePropLast);

    delete[] propName;

    return plPropNum;
}

const WCHAR_T* CAddInNative::GetPropName(long lPropNum, long lPropAlias) {
    if (lPropNum >= ePropLast)
        return NULL;

    wchar_t *wsCurrentName = nullptr;
    WCHAR_T *wsPropName = nullptr;
    
    switch(lPropAlias) {
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

bool CAddInNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal) {
        
    switch(lPropNum) {
        case ePropComponentVersion:
            SetVariant(pvarPropVal, COMPONENT_VERSION);
            return true;

        case ePropRdKafkaVersion:
            SetVariant(pvarPropVal, rdk1c->RdKafkaVersion());
            return true;

        case ePropLocale:
            SetVariant(pvarPropVal, currentLocale);
            return true;

        case ePropLogFile:
            SetVariant(pvarPropVal, GetLogFile());
            return true;

        case ePropLogLevel:
            SetVariant(pvarPropVal, GetLogLevel());
            return true;

        case ePropOperationTimeout:
            SetVariant(pvarPropVal, rdk1c->OperationTimeout);
            return true;

        case ePropError:
            SetVariant(pvarPropVal, error->Error());
            return true;

        case ePropErrorDescription:
            SetVariant(pvarPropVal, error->ErrorDescription());
            return true;
    }

    return false;
}

bool CAddInNative::SetPropVal(const long lPropNum, tVariant *varPropVal) {

    switch (lPropNum) {
        case ePropLocale:
            return SetLocale(varPropVal);

        case ePropLogLevel:
            return SetLogLevel(varPropVal);
        
        case ePropOperationTimeout:
            rdk1c->OperationTimeout = TV_INT(varPropVal);
            return true;
    }
    
    return false;
}

bool CAddInNative::IsPropReadable(const long lPropNum) {
    
    switch(lPropNum){ 
        case ePropComponentVersion:
        case ePropRdKafkaVersion:
        case ePropLocale:
        case ePropLogFile:
        case ePropLogLevel:
        case ePropOperationTimeout:
        case ePropError:
        case ePropErrorDescription:
            return true;
    }

    return false;
}

bool CAddInNative::IsPropWritable(const long lPropNum) {
    
    switch (lPropNum) {
        case ePropLocale:
        case ePropLogLevel:
        case ePropOperationTimeout:
            return true;
    }

    return false;
}

long CAddInNative::GetNMethods() {
    return eMethLast;
}

long CAddInNative::FindMethod(const WCHAR_T* wsMethodName) {
    long plMethodNum = -1;
    wchar_t* name = Strings::ToWchar(wsMethodName);

    plMethodNum = findName(g_MethodNames, name, eMethLast);

    if (plMethodNum == -1)
        plMethodNum = findName(g_MethodNamesRu, name, eMethLast);

    delete[] name;

    return plMethodNum;
}

const WCHAR_T* CAddInNative::GetMethodName(const long lMethodNum, const long lMethodAlias) {
    if (lMethodNum >= eMethLast)
        return NULL;

    WCHAR_T* wsMethodName = nullptr;
    wchar_t *wsCurrentName = nullptr;
    
    switch(lMethodAlias) {
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

long CAddInNative::GetNParams(const long lMethodNum) {
    switch(lMethodNum)
    { 
        case eMethStartLogging:
            return 2;
        case eMethSetConfigProperty:
            return 2;
        case eMethProduce:
            return 6;
        case eMethSubscribe:
            return 1;
        case eMethAssignPartition:
            return 2;
        case eMethCommittedOffset:
            return 2;
        case eMethChangeOffset:
            return 3;
        case eMethMessageStatus:
            return 1;        
    }    
    return 0;
}

bool CAddInNative::GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant *pvarParamDefValue) {
    
    switch (lMethodNum) {
        case eMethProduce:
            switch (lParamNum) {
                case 2:
                    SetVariant(pvarParamDefValue, "");
                    return true;
                case 3:
                    SetVariant(pvarParamDefValue, "");
                    return true;
                case 4:
                    SetVariant(pvarParamDefValue, -1);
                    return true;
                case 5:
                    SetVariant(pvarParamDefValue, "");
                    return true;
            }
            break;
    }   
    
    TV_VT(pvarParamDefValue) = VTYPE_EMPTY;
    return false;
} 

bool CAddInNative::HasRetVal(const long lMethodNum) {
    switch(lMethodNum)
    { 
        // Producer
        case eMethCountUndeliveredMessages:
        case eMethMessageStatus:
        case eMethProducerQueueLen:

        // Consumer
        case eMethConsume:
        case eMethMessageData:
        case eMethMessageKey:
        case eMethMessageHeaders:
        case eMethMessageMetadata:
        case eMethCommittedOffset:
        case eMethCommitOffset:
        case eMethChangeOffset:
        case eMethConsumerQueueLen:
        
        // Subscriptions
        case eMethSubscription:
            return true;
    }
    return false;
}

bool CAddInNative::CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray) {
    error->Clear();

    switch (lMethodNum) {

        // Logging
        case eMethStartLogging:
            return StartLogging(paParams, lSizeArray);

        case eMethStopLogging:        
            return StopLogging(paParams, lSizeArray);

        case eMethSetConfigProperty:        
            return SetConfigProperty(paParams, lSizeArray);

        // Producer
        case eMethInitProducer:
            return InitProducer(paParams, lSizeArray);

        case eMethStopProducer:
            return StopProducer(paParams, lSizeArray);

        case eMethStartProduce:
            return StartProduce(paParams, lSizeArray);
        
        case eMethProduce:
            return Produce(paParams, lSizeArray);

        case eMethFlush:
            return Flush(paParams, lSizeArray);

        // Consumer
        case eMethInitConsumer:
            return InitConsumer(paParams, lSizeArray);

        case eMethStopConsumer:
            return StopConsumer(paParams, lSizeArray);

        case eMethAssignPartition:
            return AssignPartition(paParams, lSizeArray);

        // Subscription
        case eMethSubscribe:
            return Subscribe(paParams, lSizeArray);

        case eMethUnsubscribe:
            return Unsubscribe(paParams, lSizeArray);
    }

    return false;
}

bool CAddInNative::CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) {
    error->Clear();

    switch(lMethodNum) {

        // Producer
        case eMethProducerQueueLen:
            return ProducerQueueLen(pvarRetValue, paParams, lSizeArray);
        
        case eMethCountUndeliveredMessages:
            return CountUndeliveredMessages(pvarRetValue, paParams, lSizeArray);

        case eMethMessageStatus:
            return MessageStatus(pvarRetValue, paParams, lSizeArray);

        // Consumer
        case eMethConsume:
            return Consume(pvarRetValue, paParams, lSizeArray);

        case eMethMessageData:
            return MessageData(pvarRetValue, paParams, lSizeArray);

        case eMethMessageKey:
            return MessageKey(pvarRetValue, paParams, lSizeArray);

        case eMethMessageHeaders:
            return MessageHeaders(pvarRetValue, paParams, lSizeArray);

        case eMethMessageMetadata:
            return MessageMetadata(pvarRetValue, paParams, lSizeArray);

        case eMethCommittedOffset:
            return CommittedOffset(pvarRetValue, paParams, lSizeArray);

        case eMethCommitOffset:
            return CommitOffset(pvarRetValue, paParams, lSizeArray);

        case eMethChangeOffset:
            return ChangeOffset(pvarRetValue, paParams, lSizeArray);

        case eMethConsumerQueueLen:
            return ConsumerQueueLen(pvarRetValue, paParams, lSizeArray);

        // Subscriptions
        case eMethSubscription:
            return Subscription(pvarRetValue, paParams, lSizeArray);
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////
// UserLanguageBase

void ADDIN_API CAddInNative::SetUserInterfaceLanguageCode(const WCHAR_T * lang) {
    wchar_t* wstr = Strings::ToWchar(lang);
    char* cstr = Strings::ToChar(wstr);

    std::string language(cstr);
    std::string country(cstr);
    std::transform(country.begin(), country.end(), country.begin(), ::toupper);

    SetLocale(language + "_" + country);

    delete[] wstr;
    delete[] cstr;
}

/////////////////////////////////////////////////////////////////////////////
// LocaleBase

void ADDIN_API CAddInNative::SetLocale(const WCHAR_T* locale) {
    // Deprecated its.1c.ru/db/metod8dev#content:3221:hdoc 
}

bool CAddInNative::SetLocale(tVariant* varPropVal) {
    std::string localeName = ToString(varPropVal);
    
    if (error->Error())
        return true;    

    return SetLocale(localeName);
}

bool CAddInNative::SetLocale(std::string LocaleName) {
    char * cstr = std::setlocale(LC_ALL, LocaleName.c_str());

    if (cstr)
        currentLocale = std::string(cstr);

    return true;
}

long CAddInNative::findName(const wchar_t* names[], const wchar_t* name, const uint32_t size) const {
    long ret = -1;
    for (uint32_t i = 0; i < size; i++) {
        if (!wcscmp(names[i], name)) {
            ret = i;
            break;
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////
// Logging

bool CAddInNative::StartLogging(tVariant* paParams, const long lSizeArray) {
    error->Clear(); 
    
    if (lSizeArray != 2 || !paParams) {
        error->Set("Invalid parameters");
        return false;
    }

    std::string fileName = ToString(&paParams[0]);
    std::string levelName = ToString(&paParams[1]);
    Loger::Levels level = StringToLogLevel(levelName);

    if (error->Error()) 
        return true;
    
    loger->level = level;
    std::string errorDescription;    
    bool result = loger->Init(fileName, errorDescription);
    if (result)
        loger->Info("Start loging");
    else
        error->Set("Failed to init loger: " + errorDescription);

    return true;
}

bool CAddInNative::StopLogging(tVariant* paParams, const long lSizeArray) {
    loger->Info("Stop logging");
    error->Clear();
    
    loger->level = Loger::Levels::NONE;
    return true;
}

bool CAddInNative::SetLogLevel(tVariant* varPropVal) {
    loger->Info("Set loger level");
    error->Clear();

    std::string level = ToString(varPropVal);
    
    if (error->Error())
        return true;    

    Loger::Levels logLevel = StringToLogLevel(level);
    if (error->Error())
        return true;

    loger->level = logLevel;    

    return true;
}

std::string CAddInNative::GetLogFile() {
    loger->Info("Get current log file");
    error->Clear();

    return loger->GetLogFile();
}

Loger::Levels CAddInNative::StringToLogLevel(std::string String) {
    Loger::Levels Level = Loger::Levels::NONE;
    
    if (String == "none")
        Level = Loger::Levels::NONE;
    else if (String == "debug")
        Level = Loger::Levels::DEBUG;
    else if (String == "info")
        Level = Loger::Levels::INFO;
    else if (String == "warn")
        Level = Loger::Levels::WARN;
    else if (String == "error")
        Level = Loger::Levels::ERRORS;
    else
    {
        error->Set("Faled to convert value '" + String + "' to log level. Valid values: none, debug, info, warn, error");
        return Level;
    }

    return Level;
}

std::string CAddInNative::GetLogLevel() {
    loger->Info("Get loger level");
    error->Clear();
    
    switch (loger->level)
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

/////////////////////////////////////////////////////////////////////////////
// General action

bool CAddInNative::SetConfigProperty(tVariant* paParams, const long lSizeArray) {
    if (lSizeArray != 2 || !paParams ) {
        error->Set("Invalid parameters");
        return false;
    }
    
    std::string paramName = ToString(&paParams[0]);
    std::string paramValue = ToString(&paParams[1]);

    if (error->Error())
        return true;

    rdk1c->SetConfigProperty(paramName, paramValue);

    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Producer

bool CAddInNative::InitProducer(tVariant* paParams, const long lSizeArray) {
    rdk1c->InitProducer();
    return true;
}

bool CAddInNative::StartProduce(tVariant* paParams, const long lSizeArray) {
    rdk1c->StartProduce();
    return true;
}

bool CAddInNative::Produce(tVariant* paParams, const long lSizeArray) {
    if (lSizeArray != 6 || !paParams) {
        error->Set("Invalid parameters");
        return false;
    }

    std::string topic = ToString(&paParams[0]);
    std::string message = ToBinaryString(&paParams[1]);
    std::string key = ToBinaryString(&paParams[2]);
    std::string headers = ToBinaryString(&paParams[3]);
    int partition = ToInt(&paParams[4], -1);
    std::string messageId = ToString(&paParams[5], "");
    
    if (error->Error())
        return true;

    rdk1c->Produce(topic, message, key, headers, partition, messageId);
    
    return true;
}

bool CAddInNative::Flush(tVariant* paParams, const long lSizeArray) {
    rdk1c->Flush();
    return true;
}

bool CAddInNative::StopProducer(tVariant* paParams, const long lSizeArray) {
    rdk1c->StopProduser();
    return true;
}

bool CAddInNative::ProducerQueueLen(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) {
    auto result = rdk1c->ProducerQueueLen();
    SetVariant(pvarRetValue, result);
    return true;
}

bool CAddInNative::CountUndeliveredMessages(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) {
    auto result = rdk1c->CountUndeliveredMessages();
    SetVariant(pvarRetValue, result);
    return true;
}

bool CAddInNative::MessageStatus(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) {
    if (lSizeArray != 1 || !paParams ) {
        error->Set("Invalid parameters");
        return false;
    }
    
    std::string messageId = ToString(&paParams[0]);
    
    if (error->Error())
        return true;

    auto result = rdk1c->MessageStatus(messageId);
    SetVariant(pvarRetValue, result);

    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Consumer

bool CAddInNative::InitConsumer(tVariant* paParams, const long lSizeArray) {
    rdk1c->InitConsumer();
    return true;
}

bool CAddInNative::Consume(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) {
    auto result = rdk1c->Consume();
    SetVariant(pvarRetValue, result);
    return true;
}

bool CAddInNative::MessageData(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) {
    auto result = rdk1c->MessageData();
    SetVariantBlob(pvarRetValue, result);
    return true;
}

bool CAddInNative::MessageKey(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) {
    auto result = rdk1c->MessageKey();
    SetVariantBlob(pvarRetValue, result);
    return true;
}

bool CAddInNative::MessageHeaders(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) {
    auto result = rdk1c->MessageHeaders();
    SetVariantBlob(pvarRetValue, result);
    return true;
}

bool CAddInNative::MessageMetadata(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) {
    auto result = rdk1c->MessageMetadata();
    SetVariant(pvarRetValue, result);
    return true;
}

bool CAddInNative::StopConsumer(tVariant* paParams, const long lSizeArray) {
    rdk1c->StopConsumer();
    return true;
}

bool CAddInNative::ConsumerQueueLen(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) {
    auto result = rdk1c->ConsumerQueueLen();
    SetVariant(pvarRetValue, result);
    return true;
}

bool CAddInNative::AssignPartition(tVariant* paParams, const long lSizeArray) {
    if (lSizeArray != 2 || !paParams) {
        error->Set("Invalid parameters");
        return false;
    }

    std::string topic = ToString(&paParams[0]);
    int partition = ToInt(&paParams[1], 0);

    if (error->Error()) 
        return true;

    rdk1c->AssignPartition(topic, partition);

    return true;
}

bool CAddInNative::CommitOffset(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) {
    auto result = rdk1c->CommitOffset();
    SetVariant(pvarRetValue, result);
    return true;
}

bool CAddInNative::ChangeOffset(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) {
    if (lSizeArray != 3 || !paParams) {
        error->Set("Invalid parameters");
        return false;
    }

    std::string topic = ToString(&paParams[0]);
    int partition = ToInt(&paParams[1]);
    int64_t offset = ToLongInt(&paParams[2]);

    if (error->Error()) {
        SetVariant(pvarRetValue, false);
        return true;
    }

    auto result = rdk1c->ChangeOffset(topic, partition, offset);
    SetVariant(pvarRetValue, result);

    return true;
}

bool CAddInNative::CommittedOffset(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) {
    if (lSizeArray != 2 || !paParams) {
        error->Set("Invalid parameters");
        return false;
    }

    std::string topic = ToString(&paParams[0]);
    int partition = ToInt(&paParams[1]);

    if (error->Error()) {
        SetVariant(pvarRetValue, false);
        return true;
    }

    auto result = rdk1c->CommittedOffset(topic, partition);
    SetVariant(pvarRetValue, result);

    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Subscription

bool CAddInNative::Subscription(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) {
    auto result = rdk1c->Subscription();
    SetVariant(pvarRetValue, result);
    return true;
}

bool CAddInNative::Subscribe(tVariant* paParams, const long lSizeArray) {
    if (lSizeArray != 1 || !paParams) {
        error->Set("Invalid parameters");
        return false;
    }

    std::string topic = ToString(&paParams[0]);

    if (error->Error()) 
        return true;

    rdk1c->Subscribe(topic);
    
    return true;
}

bool CAddInNative::Unsubscribe(tVariant* paParams, const long lSizeArray) {
    rdk1c->Unsubscribe();
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Variant conversion

int CAddInNative::ToInt(tVariant* Source, int defaultValue) {
    if (TV_VT(Source) == VTYPE_EMPTY)
        return defaultValue;

    return ToInt(Source);
}

int CAddInNative::ToInt(tVariant* Source) {
    if (TV_VT(Source) != VTYPE_I2
        && TV_VT(Source) != VTYPE_I4
        && TV_VT(Source) != VTYPE_UI1
        && TV_VT(Source) != VTYPE_UI2
        && TV_VT(Source) != VTYPE_UI4
        && TV_VT(Source) != VTYPE_I8
        && TV_VT(Source) != VTYPE_UI8
        && TV_VT(Source) != VTYPE_INT
        && TV_VT(Source) != VTYPE_UINT) {
        error->Set("Value isn't integer");
        return 0;
    }

    return TV_INT(Source);
}

int64_t CAddInNative::ToLongInt(tVariant* Source) {
    if (TV_VT(Source) != VTYPE_I2
        && TV_VT(Source) != VTYPE_I4
        && TV_VT(Source) != VTYPE_UI1
        && TV_VT(Source) != VTYPE_UI2
        && TV_VT(Source) != VTYPE_UI4
        && TV_VT(Source) != VTYPE_I8
        && TV_VT(Source) != VTYPE_UI8
        && TV_VT(Source) != VTYPE_INT
        && TV_VT(Source) != VTYPE_UINT) {
        error->Set("Value isn't integer");
        return 0;
    }

    return TV_INT(Source);
}

std::string CAddInNative::ToString(tVariant* Source, std::string defaultValue) {
    if (TV_VT(Source) == VTYPE_EMPTY)
        return defaultValue;

    return ToString(Source);
}

std::string CAddInNative::ToString(tVariant* Source) {
    if (TV_VT(Source) != VTYPE_PWSTR) {
        error->Set("Value isn't string");
        return "";
    }

    wchar_t* wcstr = Strings::ToWchar(Source->pwstrVal);
    char* cstr = Strings::ToChar(wcstr);

    std::string result = std::string(cstr);

    delete[] wcstr;
    delete[] cstr;

    return result;
}

std::string CAddInNative::ToBinaryString(tVariant* Source) {
    if (TV_VT(Source) != VTYPE_BLOB) {
        error->Set("Value isn't binary string");
        return "";
    }
    
    return std::string(Source->pstrVal, Source->strLen);
}

void CAddInNative::SetVariant(tVariant* Dest, std::string Source) {
    TV_VT(Dest) = VTYPE_PWSTR;

    ToShortWchar(&TV_WSTR(Dest), Source.c_str());
    Dest->wstrLen = Strings::GetLength(TV_WSTR(Dest));
}

void CAddInNative::SetVariant(tVariant* Dest, const wchar_t* Source) {
    TV_VT(Dest) = VTYPE_PWSTR;
    ToShortWchar(&TV_WSTR(Dest), Source);
    Dest->wstrLen = Strings::GetLength(TV_WSTR(Dest));
}

void CAddInNative::SetVariant(tVariant* Dest, const char* Source) {
    TV_VT(Dest) = VTYPE_PWSTR;
    ToShortWchar(&TV_WSTR(Dest), Source);
    Dest->wstrLen = Strings::GetLength(TV_WSTR(Dest));
}

void CAddInNative::SetVariant(tVariant* Dest, int Source) {
    TV_VT(Dest) = VTYPE_INT;
    TV_INT(Dest) = Source;
}

void CAddInNative::SetVariant(tVariant* Dest, int64_t Source) {
    TV_VT(Dest) = VTYPE_INT;
    TV_INT(Dest) = Source;
}

void CAddInNative::SetVariant(tVariant* Dest, bool Source) {
    TV_VT(Dest) = VTYPE_BOOL;
    TV_BOOL(Dest) = Source;
}

void CAddInNative::SetVariantBlob(tVariant* Dest, std::string Source) {
    if (!m_iMemory)
        return;

    TV_VT(Dest) = VTYPE_BLOB;

    if (!m_iMemory->AllocMemory((void**)(&Dest->pstrVal), Source.length()))
        return;
    
    memcpy(Dest->pstrVal, Source.c_str(), Source.length());
    Dest->wstrLen = Source.length();
}

// Conversion with memory allocation
// Platform 1C control memory via garbage collector m_iMemory

void CAddInNative::ToShortWchar(WCHAR_T** Dest, const char* Source) {
    wchar_t* wcstr = Strings::ToWchar(Source);
    ToShortWchar(Dest, wcstr);
    delete[] wcstr;
}

void CAddInNative::ToShortWchar(WCHAR_T** Dest, const wchar_t* Source) {
    if (!m_iMemory)
        return;
    
    uint32_t length = wcslen(Source);
    size_t size = sizeof(WCHAR_T) * (length + 1);

    if (!m_iMemory->AllocMemory((void**)Dest, size))
        return;

    Strings::ConvertToShortWchar(Dest, Source);
}
