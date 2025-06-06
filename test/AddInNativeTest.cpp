#include "AddInNativeTest.h"

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
	wsPropName = addInNative->GetPropName(CAddInNative::ePropComponentVersion, 0);
	ASSERT_STREQ("ComponentVersion", ToString(wsPropName).c_str());
}

TEST_F(AddInNativeTest, PropComponentVersion)
{
	CheckPropComponentVersion("1.3.1");
}

TEST_F(AddInNativeTest, PropRdKafkaVersion)
{
	CheckPropRdKafkaVersion("2.10.0-devel");
}

TEST_F(AddInNativeTest, PropLocale)
{
	CheckPropLocale("ru-RU");
}

TEST_F(AddInNativeTest, PropLogFile)
{
	CheckPropLogFile("");
}

TEST_F(AddInNativeTest, PropLogLevel)
{
	CheckPropLogLevel("none");
}

TEST_F(AddInNativeTest, PropOperationTimeout)
{
	CheckPropOperationTimeout(1000);
}

TEST_F(AddInNativeTest, PropError)
{
	CheckPropError(false);
}

TEST_F(AddInNativeTest, PropErrorDescription)
{
	CheckPropErrorDescription("");
}

TEST_F(AddInNativeTest, SetPropVal2)
{
	tVariant* propValue = newVariant(u"en-UK");
	bool isSuccess = addInNative->SetPropVal(CAddInNative::ePropLocale, propValue);
	ASSERT_TRUE(isSuccess);
	delete_pointer(propValue);

	CheckPropLocale("en-UK");
}

TEST_F(AddInNativeTest, SetPropVal4)
{
	tVariant* propValue = newVariant(u"debug");
	bool isSuccess = addInNative->SetPropVal(CAddInNative::ePropLogLevel, propValue);
	ASSERT_TRUE(isSuccess);
	delete_pointer(propValue);

	CheckPropLogLevel("debug");
}

TEST_F(AddInNativeTest, SetPropVal5)
{
	tVariant* propValue = newVariant(3000);
	bool isSuccess = addInNative->SetPropVal(CAddInNative::ePropOperationTimeout, propValue);
	ASSERT_TRUE(isSuccess);
	delete_pointer(propValue);

	CheckPropOperationTimeout(3000);
}

TEST_F(AddInNativeTest, IsPropReadable)
{
	ASSERT_TRUE(addInNative->IsPropReadable(CAddInNative::ePropComponentVersion));
	ASSERT_TRUE(addInNative->IsPropReadable(CAddInNative::ePropRdKafkaVersion));
	ASSERT_TRUE(addInNative->IsPropReadable(CAddInNative::ePropLocale));
	ASSERT_TRUE(addInNative->IsPropReadable(CAddInNative::ePropLogFile));
	ASSERT_TRUE(addInNative->IsPropReadable(CAddInNative::ePropLogLevel));
	ASSERT_TRUE(addInNative->IsPropReadable(CAddInNative::ePropOperationTimeout));
	ASSERT_TRUE(addInNative->IsPropReadable(CAddInNative::ePropError));
	ASSERT_TRUE(addInNative->IsPropReadable(CAddInNative::ePropErrorDescription));
}

TEST_F(AddInNativeTest, IsPropWritable)
{
	ASSERT_FALSE(addInNative->IsPropWritable(CAddInNative::ePropComponentVersion));
	ASSERT_FALSE(addInNative->IsPropWritable(CAddInNative::ePropRdKafkaVersion));
	ASSERT_TRUE (addInNative->IsPropWritable(CAddInNative::ePropLocale));
	ASSERT_FALSE(addInNative->IsPropWritable(CAddInNative::ePropLogFile));
	ASSERT_TRUE (addInNative->IsPropWritable(CAddInNative::ePropLogLevel));
	ASSERT_TRUE (addInNative->IsPropWritable(CAddInNative::ePropOperationTimeout));
	ASSERT_FALSE(addInNative->IsPropWritable(CAddInNative::ePropError));
	ASSERT_FALSE(addInNative->IsPropWritable(CAddInNative::ePropErrorDescription));
}

// Checks

void AddInNativeTest::CheckPropComponentVersion(std::string value)
{
	tVariant* result = new tVariant();
	bool isSuccess = addInNative->GetPropVal(CAddInNative::ePropComponentVersion, result);

	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
	ASSERT_STREQ(value.c_str(), ToString(TV_WSTR(result)).c_str());

	delete_pointer(result);
}

void AddInNativeTest::CheckPropRdKafkaVersion(std::string value)
{
	tVariant* result = new tVariant();
	bool isSuccess = addInNative->GetPropVal(CAddInNative::ePropRdKafkaVersion, result);

	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
	ASSERT_STREQ(value.c_str(), ToString(TV_WSTR(result)).c_str());

	delete_pointer(result);
}

void AddInNativeTest::CheckPropLocale(std::string value)
{
	tVariant* result = new tVariant();
	bool isSuccess = addInNative->GetPropVal(CAddInNative::ePropLocale, result);

	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
	ASSERT_STREQ(value.c_str(), ToString(TV_WSTR(result)).c_str());

	delete_pointer(result);
}

void AddInNativeTest::CheckPropLogFile(std::string value)
{
	tVariant* result = new tVariant();
	bool isSuccess = addInNative->GetPropVal(CAddInNative::ePropLogFile, result);

	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
	ASSERT_STREQ(value.c_str(), ToString(TV_WSTR(result)).c_str());

	delete_pointer(result);
}

void AddInNativeTest::CheckPropLogLevel(std::string value)
{
	tVariant* result = new tVariant();
	bool isSuccess = addInNative->GetPropVal(CAddInNative::ePropLogLevel, result);

	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
	ASSERT_STREQ(value.c_str(), ToString(TV_WSTR(result)).c_str());

	delete_pointer(result);
}

void AddInNativeTest::CheckPropOperationTimeout(long value)
{
	tVariant* result = new tVariant();
	bool isSuccess = addInNative->GetPropVal(CAddInNative::ePropOperationTimeout, result);

	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_INT);
	ASSERT_EQ(value, TV_INT(result));

	delete_pointer(result);
}

void AddInNativeTest::CheckPropError(bool value)
{
	tVariant* result = new tVariant();
	bool isSuccess = addInNative->GetPropVal(CAddInNative::ePropError, result);

	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_BOOL);
	ASSERT_FALSE(TV_BOOL(result));

	delete_pointer(result);
}

void AddInNativeTest::CheckPropErrorDescription(std::string value)
{
	tVariant* result = new tVariant();
	bool isSuccess = addInNative->GetPropVal(CAddInNative::ePropErrorDescription, result);

	ASSERT_TRUE(isSuccess);
	ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
	ASSERT_STREQ(value.c_str(), ToString(TV_WSTR(result)).c_str());

	delete_pointer(result);
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
	wsPropName = addInNative->GetMethodName(CAddInNative::eMethStartLogging, 0);
	ASSERT_STREQ("StartLogging", ToString(wsPropName).c_str());
}

TEST_F(AddInNativeTest, GetNParams)
{
	// Logging
	ASSERT_EQ(2, addInNative->GetNParams(CAddInNative::eMethStartLogging));
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethStopLogging));
	
	// Config
	ASSERT_EQ(2, addInNative->GetNParams(CAddInNative::eMethSetConfigProperty));

	// Producer
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethInitProducer));
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethStartProduce));
	ASSERT_EQ(6, addInNative->GetNParams(CAddInNative::eMethProduce));
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethFlush));
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethStopProducer));
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethProducerQueueLen));
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethCountUndeliveredMessages));
	ASSERT_EQ(1, addInNative->GetNParams(CAddInNative::eMethMessageStatus));

	// Consumer
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethInitConsumer));
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethConsume));
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethMessageData));
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethMessageKey));
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethMessageHeaders));
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethMessageMetadata));
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethStopConsumer));
	ASSERT_EQ(2, addInNative->GetNParams(CAddInNative::eMethAssignPartition));
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethCommitOffset));
	ASSERT_EQ(3, addInNative->GetNParams(CAddInNative::eMethChangeOffset));
	ASSERT_EQ(2, addInNative->GetNParams(CAddInNative::eMethCommittedOffset));
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethConsumerQueueLen));

	// Subscriptions
	ASSERT_EQ(1, addInNative->GetNParams(CAddInNative::eMethSubscribe));
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethUnsubscribe));
	ASSERT_EQ(0, addInNative->GetNParams(CAddInNative::eMethSubscription));
}

TEST_F(AddInNativeTest, HasRetVal)
{
	// Logging
	ASSERT_FALSE(addInNative->HasRetVal(CAddInNative::eMethStartLogging));
	ASSERT_FALSE(addInNative->HasRetVal(CAddInNative::eMethStopLogging));

	// Config
	ASSERT_FALSE(addInNative->HasRetVal(CAddInNative::eMethSetConfigProperty));

	// Producer
	ASSERT_FALSE(addInNative->HasRetVal(CAddInNative::eMethInitProducer));
	ASSERT_FALSE(addInNative->HasRetVal(CAddInNative::eMethStartProduce));
	ASSERT_FALSE(addInNative->HasRetVal(CAddInNative::eMethProduce));
	ASSERT_FALSE(addInNative->HasRetVal(CAddInNative::eMethFlush));
	ASSERT_FALSE(addInNative->HasRetVal(CAddInNative::eMethStopProducer));
	ASSERT_TRUE(addInNative->HasRetVal(CAddInNative::eMethProducerQueueLen));
	ASSERT_TRUE(addInNative->HasRetVal(CAddInNative::eMethCountUndeliveredMessages));
	ASSERT_TRUE(addInNative->HasRetVal(CAddInNative::eMethMessageStatus));

	// Consumer
	ASSERT_FALSE(addInNative->HasRetVal(CAddInNative::eMethInitConsumer));
	ASSERT_TRUE(addInNative->HasRetVal(CAddInNative::eMethConsume));
	ASSERT_TRUE(addInNative->HasRetVal(CAddInNative::eMethMessageData));
	ASSERT_TRUE(addInNative->HasRetVal(CAddInNative::eMethMessageKey));
	ASSERT_TRUE(addInNative->HasRetVal(CAddInNative::eMethMessageHeaders));
	ASSERT_TRUE(addInNative->HasRetVal(CAddInNative::eMethMessageMetadata));
	ASSERT_FALSE(addInNative->HasRetVal(CAddInNative::eMethStopConsumer));
	ASSERT_FALSE(addInNative->HasRetVal(CAddInNative::eMethAssignPartition));
	ASSERT_TRUE(addInNative->HasRetVal(CAddInNative::eMethCommitOffset));
	ASSERT_TRUE(addInNative->HasRetVal(CAddInNative::eMethChangeOffset));
	ASSERT_TRUE(addInNative->HasRetVal(CAddInNative::eMethCommittedOffset));
	ASSERT_TRUE(addInNative->HasRetVal(CAddInNative::eMethConsumerQueueLen));

	// Subscriptions
	ASSERT_FALSE(addInNative->HasRetVal(CAddInNative::eMethSubscribe));
	ASSERT_FALSE(addInNative->HasRetVal(CAddInNative::eMethUnsubscribe));
	ASSERT_TRUE(addInNative->HasRetVal(CAddInNative::eMethSubscription));
}

// CallAsProc

TEST_F(AddInNativeTest, StartLogging)
{
	tVariant* paParams = new tVariant[2];
	setToVariant(paParams, 0, u"file-name");
	setToVariant(paParams, 1, u"error");

	bool isSuccess = addInNative->CallAsProc(CAddInNative::eMethStartLogging, paParams, 2);
	ASSERT_TRUE(isSuccess);
	delete_array(paParams);

	CheckPropErrorDescription("");
	CheckPropError(false);
	CheckPropLogFile("file-name");
	CheckPropLogLevel("error");
}

TEST_F(AddInNativeTest, StopLogging)
{
	tVariant* paParams = new tVariant();

	bool isSuccess = addInNative->CallAsProc(CAddInNative::eMethStopLogging, paParams, 0);
	ASSERT_TRUE(isSuccess);
	delete_pointer(paParams);

	CheckPropErrorDescription("");
	CheckPropError(false);
	CheckPropLogLevel("none");
}

TEST_F(AddInNativeTest, SetConfigProperty)
{
	tVariant* paParams = new tVariant[2];
	setToVariant(paParams, 0, u"group.id");
	setToVariant(paParams, 1, u"id");

	bool isSuccess = addInNative->CallAsProc(CAddInNative::eMethSetConfigProperty, paParams, 2);
	ASSERT_TRUE(isSuccess);
	delete_pointer(paParams);

	CheckPropErrorDescription("");
	CheckPropError(false);
}

TEST_F(AddInNativeTest, InitProducer)
{
	InitProducer();
}

TEST_F(AddInNativeTest, StopProducer)
{
	StopProducer();
}

TEST_F(AddInNativeTest, StartProduce)
{
	InitProducer();
	StartProduce();
}

TEST_F(AddInNativeTest, Produce)
{
	InitProducer();
	StartProduce();

	tVariant* paParams = new tVariant[6];
	setToVariant(paParams, 0, u"topic");
	setToVariant(paParams, 1, u"message");
	setToVariant(paParams, 2, u"key");
	setToVariant(paParams, 3, u"headers");
	setToVariant(paParams, 4, (long) 0);
	setToVariant(paParams, 5, u"id");

	bool isSuccess = addInNative->CallAsProc(CAddInNative::eMethProduce, paParams, 6);
	ASSERT_TRUE(isSuccess);
	delete_pointer(paParams);

	CheckPropErrorDescription("");
	CheckPropError(false);

	StopProducer();
}

void AddInNativeTest::InitProducer()
{
	tVariant* paParams = new tVariant();

	bool isSuccess = addInNative->CallAsProc(CAddInNative::eMethInitProducer, paParams, 0);
	ASSERT_TRUE(isSuccess);
	delete_pointer(paParams);

	CheckPropErrorDescription("");
	CheckPropError(false);
}

void AddInNativeTest::StopProducer()
{
	tVariant* paParams = new tVariant();

	bool isSuccess = addInNative->CallAsProc(CAddInNative::eMethStopProducer, paParams, 0);
	ASSERT_TRUE(isSuccess);
	delete_pointer(paParams);

	CheckPropErrorDescription("");
	CheckPropError(false);
}

void AddInNativeTest::StartProduce()
{
	tVariant* paParams = new tVariant();
	bool isSuccess = addInNative->CallAsProc(CAddInNative::eMethStartProduce, paParams, 0);
	ASSERT_TRUE(isSuccess);
	delete_pointer(paParams);

	CheckPropErrorDescription("");
	CheckPropError(false);
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
	memoryManager = new MemoryManager();
	addInNative = new CAddInNative();
	addInNative->Init(nullptr);
	addInNative->setMemManager(memoryManager);
}

void AddInNativeTest::TearDown()
{
	delete_pointer(addInNative);
	delete_pointer(memoryManager);
}

std::string AddInNativeTest::ToString(const char16_t* source)
{
	std::u16string sourceStr(source);
	std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> converter;
	return converter.to_bytes(source);
}

tVariant* AddInNativeTest::newVariant(long value)
{
	tVariant* variant = new tVariant();
	TV_VT(variant) = VTYPE_INT;
	TV_INT(variant) = value;
	return variant;
}

tVariant* AddInNativeTest::newVariant(const WCHAR_T* value)
{
	tVariant* variant = new tVariant();
	TV_VT(variant) = VTYPE_PWSTR;
	TV_WSTR(variant) = const_cast<WCHAR_T*>(value);
	return variant;
}

void AddInNativeTest::setToVariant(tVariant* variant, int index, const WCHAR_T* value)
{
	TV_VT(&variant[index]) = VTYPE_PWSTR;
	TV_WSTR(&variant[index]) = const_cast<WCHAR_T*>(value);
}

void AddInNativeTest::setToVariant(tVariant* variant, int index, long value)
{
	TV_VT(&variant[index]) = VTYPE_INT;
	TV_INT(&variant[index]) = value;
}
