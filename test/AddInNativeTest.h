#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <codecvt>
#include "MockAddInNative.h"
#include "MockRdKafka1C.h"
#include "MockProducer.h"
#include "utils.h"
#include "MemoryManager.h"
#include "MockMessage.h"

namespace Kafka1C {

	class AddInNativeTest
		: public testing::Test
	{
	protected:
		MemoryManager* memoryManager;
		MockAddInNative* addInNative;
		MockRdKafka1C* rdk1c;
		
		AddInNativeTest();
		void SetUp() override;
		void TearDown() override;

		// Props
		void CheckPropComponentVersion(std::string value);
		void CheckPropRdKafkaVersion(std::string value);
		void CheckPropLocale(std::string value);
		void CheckPropLogFile(std::string value);
		void CheckPropLogLevel(std::string value);
		void CheckPropOperationTimeout(long value);
		void CheckPropError(bool value);
		void CheckPropErrorDescription(std::string value);

		// Methods
		void SetConfigProperty(const std::u16string& name, const std::u16string& value);

		void InitProducer();
		void StartProduce();
		void Produce(std::u16string topic, std::string message, std::string key, std::string headers, long partition, std::u16string id);
		void Flush();
		void ProducerQueueLen();
		void CountUndeliveredMessages();
		void MessageStatus(std::u16string id, std::string status);
		void StopProducer();

		void InitConsumer();
		void Consume();
		void CheckMessageData(std::string value);
		void CheckMessageKey(std::string value);
		void CheckMessageHeaders(std::string value);
		void CheckMessageMetadata(std::string topic_name, std::string partition, std::string offset, std::string status);
		void AssignPartition();
		void Subscribe();
		void Unsubscribe();
		void CommittedOffset();
		void CommitOffset();
		void ChangeOffset();
		void ConsumerQueueLen();
		void Subscription();
		void StopConsumer();

		// Helper methods
		std::string ToString(const char16_t* source);
		tVariant* newVariant(const WCHAR_T* value);
		tVariant* newVariant(long value);
		void setToVariant(tVariant* variant, const WCHAR_T* value);
		void setToVariant(tVariant* variant, long value);
		void setToVariant(tVariant* variant, char* value);
	};

}