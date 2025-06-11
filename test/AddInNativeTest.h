#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <codecvt>
#include "MockAddInNative.h"
#include "utils.h"
#include "MemoryManager.h"

namespace RdKafka1C {

	class AddInNativeTest
		: public testing::Test
	{
	protected:
		MemoryManager* memoryManager;
		MockAddInNative* addInNative;

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
		void InitProducer();
		void StopProducer();
		void StartProduce();
		void Produce(std::u16string topic, std::string message, std::string key, std::string headers, long partition, std::u16string id);
		void Flush();

		// Helper methods
		std::string ToString(const char16_t* source);
		tVariant* newVariant(const WCHAR_T* value);
		tVariant* newVariant(long value);
		void setToVariant(tVariant* variant, const WCHAR_T* value);
		void setToVariant(tVariant* variant, long value);
		void setToVariant(tVariant* variant, char* value);
	};

}