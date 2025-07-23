#pragma once

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <clocale>
#include "stdafx.h"
#include "ComponentBase.h"
#include "AddInDefBase.h"
#include "IMemoryManager.h"
#include "Loger.h"
#include "ErrorHandler.h"
#include "RdKafka1C.h"
#include "Strings.h"
#include "utils.h"

#if defined( __linux__ )
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
class CAddInNative : public IComponentBase {
    
    public:
        enum Props {
            ePropComponentVersion = 0,
            ePropRdKafkaVersion,
            ePropLocale,
            ePropLogFile,
            ePropLogLevel,
            ePropOperationTimeout,
            ePropError,
            ePropErrorDescription,
            ePropLast      // Always last
        };

        enum Methods {
            // Logging
            eMethStartLogging = 0,
            eMethStopLogging,

            // Config
            eMethSetConfigProperty,

            // Producer
            eMethInitProducer,
            eMethStartProduce,
            eMethProduce,
            eMethFlush,
            eMethStopProducer,
            eMethProducerQueueLen,
            eMethCountUndeliveredMessages,
            eMethMessageStatus,
            
            // Consumer
            eMethInitConsumer,
            eMethConsume,
            eMethMessageData,
            eMethMessageKey,
            eMethMessageHeaders,
            eMethMessageMetadata,
            eMethStopConsumer,
            eMethAssignPartition,
            eMethCommitOffset,
            eMethChangeOffset,
            eMethCommittedOffset,
            eMethConsumerQueueLen,

            // Subscriptions
            eMethSubscribe,
            eMethUnsubscribe,
            eMethSubscription,

            eMethLast      // Always last
        };

        
        CAddInNative(void);
        ~CAddInNative();

        // IInitDoneBase
        bool ADDIN_API Init(void*) override;
        bool ADDIN_API setMemManager(void* mem) override;
        long ADDIN_API GetInfo() override;
        void ADDIN_API Done() override;

        // ILanguageExtenderBase
        bool ADDIN_API RegisterExtensionAs(WCHAR_T**) override;

        long ADDIN_API GetNProps() override;
        long ADDIN_API FindProp(const WCHAR_T* wsPropName) override;
        const WCHAR_T* ADDIN_API GetPropName(long lPropNum, long lPropAlias) override;
        bool ADDIN_API GetPropVal(const long lPropNum, tVariant* pvarPropVal) override;
        bool ADDIN_API SetPropVal(const long lPropNum, tVariant* varPropVal) override;
        bool ADDIN_API IsPropReadable(const long lPropNum) override;
        bool ADDIN_API IsPropWritable(const long lPropNum) override;

        long ADDIN_API GetNMethods() override;
        long ADDIN_API FindMethod(const WCHAR_T* wsMethodName) override;
        const WCHAR_T* ADDIN_API GetMethodName(const long lMethodNum, const long lMethodAlias) override;
        long ADDIN_API GetNParams(const long lMethodNum) override;
        bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant *pvarParamDefValue) override;   
        bool ADDIN_API HasRetVal(const long lMethodNum) override;
        bool ADDIN_API CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray) override;
        bool ADDIN_API CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) override;
        
        // LocaleBase
        void ADDIN_API SetLocale(const WCHAR_T* loc) override;
        
        // UserLanguageBase
        void ADDIN_API SetUserInterfaceLanguageCode(const WCHAR_T* lang) override;

    protected:
        Kafka1C::Loger* loger;
        Kafka1C::ErrorHandler* error;
        Kafka1C::IRdKafka1C* rdk1c;

    private:

        const wchar_t* EXTENSION_NAME = L"RdKafka1C";
        const wchar_t* COMPONENT_VERSION = L"1.3.1";
        
        std::string currentLocale;
        IAddInDefBase* m_iConnect;
        IMemoryManager* m_iMemory;    
        
        bool SetLocale(tVariant* varPropVal);
        bool SetLocale(std::string LocaleName);
        
        // Common methods
        Kafka1C::IRdKafka1C* CreateRdKafka1C(Kafka1C::Loger* loger, Kafka1C::ErrorHandler* error);

        bool StartLogging(tVariant* paParams, const long lSizeArray);
        bool StopLogging(tVariant* paParams, const long lSizeArray);
        bool SetLogLevel(tVariant* varPropVal);
        std::string GetLogLevel();
        std::string GetLogFile();
        Kafka1C::Loger::Levels StringToLogLevel(std::string String);
        
        bool SetConfigProperty(tVariant* paParams, const long lSizeArray);
        
        // Producer
        bool InitProducer(tVariant* paParams, const long lSizeArray);
        bool StopProducer(tVariant* paParams, const long lSizeArray);
        bool StartProduce(tVariant* paParams, const long lSizeArray);
        bool Produce(tVariant* paParams, const long lSizeArray);
        bool Flush(tVariant* paParams, const long lSizeArray);
        bool ProducerQueueLen(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
        bool CountUndeliveredMessages(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
        bool MessageStatus(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);

        // Consumer
        bool InitConsumer(tVariant* paParams, const long lSizeArray);
        bool StopConsumer(tVariant* paParams, const long lSizeArray);
        bool Consume(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
        bool MessageData(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
        bool MessageKey(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
        bool MessageHeaders(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
        bool MessageMetadata(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
        bool ConsumerQueueLen(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
        bool AssignPartition(tVariant* paParams, const long lSizeArray);
        bool CommittedOffset(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
        bool CommitOffset(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
        bool ChangeOffset(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);

        bool Subscription(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
        bool Subscribe(tVariant* paParams, const long lSizeArray);
        bool Unsubscribe(tVariant* paParams, const long lSizeArray);
        
        // Variant conversion
        int ToInt(tVariant* Source);
        int ToInt(tVariant* Source, int defaultValue);
        int64_t ToLongInt(tVariant* Source);
        std::string ToString(tVariant* Source);
        std::string ToString(tVariant* Source, std::string defaultValue);
        std::string ToBinaryString(tVariant* Source);

        void SetVariant(tVariant* Dest, std::string Source);
        void SetVariant(tVariant* Dest, const wchar_t* Source);
        void SetVariant(tVariant* Dest, const char* Source);
        void SetVariant(tVariant* Dest, int Source);
        void SetVariant(tVariant* Dest, int64_t Source);
        void SetVariant(tVariant* Dest, bool Source);
        void SetVariantBlob(tVariant* Dest, std::string Source);
        void ToShortWchar(WCHAR_T** Dest, const wchar_t* Source);
        void ToShortWchar(WCHAR_T** Dest, const char* Source);
        
        // Other
        long findName(const wchar_t* names[], const wchar_t* name, const uint32_t size) const;
};
