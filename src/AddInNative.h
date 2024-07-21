#pragma once

#include <stdio.h>
#include <wchar.h>
#include <string>
#include <clocale>
#include "stdafx.h"
#include "ComponentBase.h"
#include "AddInDefBase.h"
#include "IMemoryManager.h"
#include "RdKafka1C.h"

#if defined( __linux__ ) || defined(__APPLE__)
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <iconv.h>
#include <locale.h>
#include <sys/time.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// class CAddInNative
class CAddInNative : public IComponentBase
{
public:
    enum Props
    {
        ePropComponentVersion = 0,
        ePropRdKafkaVersion,
        ePropLogFile,
        ePropLogLevel,
        ePropOperationTimeout,
        ePropErrorDescription,
        ePropLast      // Always last
    };

    enum Methods
    {
        eMethEnableLogging = 0,
        eMethSetConfigProperty,
        eMethInitProducer,
        eMethProduce,
        eMethStopProducer,
        eMethInitConsumer,
        eMethConsume,
        eMethGetMessageData,
        eMethGetMessageMetadata,
        eMethStopConsumer,
        eMethLast      // Always last
    };

    CAddInNative(void);
    virtual ~CAddInNative();

    // IInitDoneBase
    virtual bool ADDIN_API Init(void*) override;
    virtual bool ADDIN_API setMemManager(void* mem) override;
    virtual long ADDIN_API GetInfo() override;
    virtual void ADDIN_API Done() override;

    // ILanguageExtenderBase
    virtual bool ADDIN_API RegisterExtensionAs(WCHAR_T**) override;

    virtual long ADDIN_API GetNProps() override;
    virtual long ADDIN_API FindProp(const WCHAR_T* wsPropName) override;
    virtual const WCHAR_T* ADDIN_API GetPropName(long lPropNum, long lPropAlias) override;
    virtual bool ADDIN_API GetPropVal(const long lPropNum, tVariant* pvarPropVal) override;
    virtual bool ADDIN_API SetPropVal(const long lPropNum, tVariant* varPropVal) override;
    virtual bool ADDIN_API IsPropReadable(const long lPropNum) override;
    virtual bool ADDIN_API IsPropWritable(const long lPropNum) override;

    virtual long ADDIN_API GetNMethods() override;
    virtual long ADDIN_API FindMethod(const WCHAR_T* wsMethodName) override;
    virtual const WCHAR_T* ADDIN_API GetMethodName(const long lMethodNum, const long lMethodAlias) override;
    virtual long ADDIN_API GetNParams(const long lMethodNum) override;
    virtual bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant *pvarParamDefValue) override;   
    virtual bool ADDIN_API HasRetVal(const long lMethodNum) override;
    virtual bool ADDIN_API CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray) override;
    virtual bool ADDIN_API CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) override;
    
    // LocaleBase
    virtual void ADDIN_API SetLocale(const WCHAR_T* loc) override;
    
    // UserLanguageBase
    virtual void ADDIN_API SetUserInterfaceLanguageCode(const WCHAR_T* lang) override;
    
private:
    const wchar_t* EXTENSION_NAME = L"RdKafka1C";
    const wchar_t* COMPONENT_VERSION = L"0.1b";

    IAddInDefBase* m_iConnect;
    IMemoryManager* m_iMemory;
    std::u16string m_userLang;
    RdKafka1C* rdk1c;

    bool SetConfigProperty(tVariant* paParams, const long lSizeArray);
    bool EnableLogging(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
    bool InitProducer(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
    bool Produce(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
    bool StopProducer(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
    bool InitConsumer(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
    bool Consume(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
    bool GetMessageData(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
    bool GetMessageMetadata(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
    bool StopConsumer(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);

    long findName(const wchar_t* names[], const wchar_t* name, const uint32_t size) const;
    void addError(uint32_t wcode, const wchar_t* source, const wchar_t* descriptor, long code);
    void addError(uint32_t wcode, const char16_t* source, const char16_t* descriptor, long code);

    void SetVariant(tVariant* Dest, std::string Source);
    void SetVariant(tVariant* Dest, const wchar_t* Source);
    void SetVariant(tVariant* Dest, const char* Source);
    void SetVariant(tVariant* Dest, int Source);

    uint32_t ToShortWchar(WCHAR_T** Dest, const wchar_t* Source);
    uint32_t ToShortWchar(WCHAR_T** Dest, const char* Source);
    std::string ToString(const WCHAR_T* Source);
    
    bool SetLogLevel(WCHAR_T* pwstrLogLevel);
    std::string GetLogLevel();
    
};
