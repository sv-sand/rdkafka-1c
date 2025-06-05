#include "AddInNativeTest.h"

std::string ToString(const char16_t* source)
{
	std::u16string sourceStr(source);
	std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> converter;
	return converter.to_bytes(source);
}

/////////////////////////////////////////////////////////////////////////////
// Tests

TEST_F(AddInNativeTest, GetInfo)
{
	ASSERT_EQ(2000, addInNative->GetInfo());
}

TEST_F(AddInNativeTest, RegisterExtensionAs)
{
	WCHAR_T** wsExtensionName = new WCHAR_T*; // Will be delete by Memory manager
	addInNative->RegisterExtensionAs(wsExtensionName);
	ASSERT_STREQ("RdKafka1C", ToString(*wsExtensionName).c_str());
}

//
// Props
//

TEST_F(AddInNativeTest, GetNProps)
{
	ASSERT_EQ(8, addInNative->GetNProps());
}

TEST_F(AddInNativeTest, FindProp)
{
	ASSERT_EQ(0, addInNative->FindProp(u"ComponentVersion"));
}

TEST_F(AddInNativeTest, GetPropName)
{
	const WCHAR_T* wsPropName; // Will be delete by Memory manager
	wsPropName = addInNative->GetPropName(0, 0);
	ASSERT_STREQ("ComponentVersion", ToString(wsPropName).c_str());
}

TEST_F(AddInNativeTest, GetPropVal0)
{
	tVariant* result = new tVariant();
	bool isSuccess = addInNative->GetPropVal(0, result);
	
	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
	ASSERT_STREQ("1.3.1", ToString(TV_WSTR(result)).c_str());

	delete_pointer(result);
}

TEST_F(AddInNativeTest, GetPropVal1)
{
	tVariant* result = new tVariant();
	bool isSuccess = addInNative->GetPropVal(1, result);
	
	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
	ASSERT_STREQ("2.10.0-devel", ToString(TV_WSTR(result)).c_str());

	delete_pointer(result);
}

TEST_F(AddInNativeTest, GetPropVal2)
{
	tVariant* result = new tVariant();
	bool isSuccess = addInNative->GetPropVal(2, result);
	
	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
	ASSERT_STREQ("ru-RU", ToString(TV_WSTR(result)).c_str());

	delete_pointer(result);
}

TEST_F(AddInNativeTest, GetPropVal3)
{
	tVariant* result = new tVariant();
	bool isSuccess = addInNative->GetPropVal(3, result);
	
	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
	ASSERT_STREQ("", ToString(TV_WSTR(result)).c_str());

	delete_pointer(result);
}

TEST_F(AddInNativeTest, GetPropVal4)
{
	tVariant* result = new tVariant();
	bool isSuccess = addInNative->GetPropVal(4, result);
	
	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
	ASSERT_STREQ("none", ToString(TV_WSTR(result)).c_str());

	delete_pointer(result);
}

TEST_F(AddInNativeTest, GetPropVal5)
{
	tVariant* result = new tVariant();
	bool isSuccess = addInNative->GetPropVal(5, result);
	
	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_INT);
	ASSERT_EQ(1000, TV_INT(result));

	delete_pointer(result);
}

TEST_F(AddInNativeTest, GetPropVal6)
{
	tVariant* result = new tVariant();
	bool isSuccess = addInNative->GetPropVal(6, result);
	
	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_BOOL);
	ASSERT_FALSE(TV_BOOL(result));

	delete_pointer(result);
}

TEST_F(AddInNativeTest, GetPropVal7)
{
	tVariant* result = new tVariant();
	bool isSuccess = addInNative->GetPropVal(7, result);
	
	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
	ASSERT_STREQ("", ToString(TV_WSTR(result)).c_str());

	delete_pointer(result);
}

TEST_F(AddInNativeTest, SetPropVal2)
{
	const WCHAR_T* value = u"en-UK";
	tVariant* propValue = new tVariant();
	TV_VT(propValue) = VTYPE_PWSTR;
	TV_WSTR(propValue) = const_cast<WCHAR_T*>(value);

	bool isSuccess = addInNative->SetPropVal(2, propValue);
	ASSERT_TRUE(isSuccess);
	
	delete_pointer(propValue);

	// Check propertie
	tVariant* result = new tVariant();
	isSuccess = addInNative->GetPropVal(2, result);

	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
	ASSERT_STREQ("en-UK", ToString(TV_WSTR(result)).c_str());

	delete_pointer(result);
}

TEST_F(AddInNativeTest, SetPropVal4)
{
	const WCHAR_T* value = u"debug";
	tVariant* propValue = new tVariant();
	TV_VT(propValue) = VTYPE_PWSTR;
	TV_WSTR(propValue) = const_cast<WCHAR_T*>(value);

	bool isSuccess = addInNative->SetPropVal(4, propValue);
	ASSERT_TRUE(isSuccess);

	delete_pointer(propValue);

	// Check propertie
	tVariant* result = new tVariant();
	isSuccess = addInNative->GetPropVal(4, result);

	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
	ASSERT_STREQ("debug", ToString(TV_WSTR(result)).c_str());

	delete_pointer(result);
}

TEST_F(AddInNativeTest, SetPropVal5)
{
	tVariant* propValue = new tVariant();
	TV_VT(propValue) = VTYPE_INT;
	TV_INT(propValue) = 3000;

	bool isSuccess = addInNative->SetPropVal(5, propValue);
	ASSERT_TRUE(isSuccess);

	delete_pointer(propValue);

	// Check propertie
	tVariant* result = new tVariant();
	isSuccess = addInNative->GetPropVal(5, result);

	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_INT);
	ASSERT_EQ(3000, TV_INT(result));

	delete_pointer(result);
}

TEST_F(AddInNativeTest, IsPropReadable)
{
	ASSERT_TRUE(addInNative->IsPropReadable(0));
	ASSERT_TRUE(addInNative->IsPropReadable(1));
	ASSERT_TRUE(addInNative->IsPropReadable(2));
	ASSERT_TRUE(addInNative->IsPropReadable(3));
	ASSERT_TRUE(addInNative->IsPropReadable(4));
	ASSERT_TRUE(addInNative->IsPropReadable(5));
	ASSERT_TRUE(addInNative->IsPropReadable(6));
	ASSERT_TRUE(addInNative->IsPropReadable(7));
}

TEST_F(AddInNativeTest, IsPropWritable)
{
	ASSERT_FALSE(addInNative->IsPropWritable(0));
	ASSERT_FALSE(addInNative->IsPropWritable(1));
	ASSERT_TRUE (addInNative->IsPropWritable(2));
	ASSERT_FALSE(addInNative->IsPropWritable(3));
	ASSERT_TRUE (addInNative->IsPropWritable(4));
	ASSERT_TRUE (addInNative->IsPropWritable(5));
	ASSERT_FALSE(addInNative->IsPropWritable(6));
	ASSERT_FALSE(addInNative->IsPropWritable(7));
}

//
// Methods
//

TEST_F(AddInNativeTest, GetNMethods)
{
	ASSERT_EQ(26, addInNative->GetNMethods());
}

TEST_F(AddInNativeTest, FindMethod)
{
	ASSERT_EQ(0, addInNative->FindMethod(u"StartLogging"));
}

TEST_F(AddInNativeTest, GetMethodName)
{
	const WCHAR_T* wsPropName; // Will be delete by Memory manager
	wsPropName = addInNative->GetMethodName(0, 0);
	ASSERT_STREQ("StartLogging", ToString(wsPropName).c_str());
}


/////////////////////////////////////////////////////////////////////////////
// Test class members

AddInNativeTest::AddInNativeTest() 
{
	memoryManager = nullptr;
	addInNative = nullptr;
}

void AddInNativeTest::SetUp()
{
	memoryManager = new MemoryManager;
	addInNative = new CAddInNative();
	addInNative->Init(nullptr);
	addInNative->setMemManager(memoryManager);
}

void AddInNativeTest::TearDown()
{
	delete_pointer(addInNative);
	delete_pointer(memoryManager);
}
