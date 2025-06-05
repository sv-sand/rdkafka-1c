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
};

