#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <AddInNative.h>
#include <codecvt>
#include "utils.h"
#include "MemoryManager.h"

class AddInNativeTest : public testing::Test
{
protected:
	MemoryManager* memoryManager;
	CAddInNative* addInNative;
	
	AddInNativeTest();
	void SetUp() override;
	void TearDown() override;

	std::string ToString(const char16_t* source);
	tVariant* newVariant(const WCHAR_T* value);
	tVariant* newVariant(long value);
	void setToVariant(tVariant* variant, int index, const WCHAR_T* value);
	void setToVariant(tVariant* variant, int index, long value);

	void CheckPropComponentVersion(std::string value);
	void CheckPropRdKafkaVersion(std::string value);
	void CheckPropLocale(std::string value);
	void CheckPropLogFile(std::string value);
	void CheckPropLogLevel(std::string value);
	void CheckPropOperationTimeout(long value);
	void CheckPropError(bool value);
	void CheckPropErrorDescription(std::string value);

	void InitProducer();
	void StopProducer();
	void StartProduce();
};

