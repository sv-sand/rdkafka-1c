#include "AddInNativeTest.h"

using namespace testing;

namespace Kafka1C {

	AddInNativeTest::AddInNativeTest()
	{
		memoryManager = nullptr;
		addInNative = nullptr;
		rdk1c = nullptr;
	}

	void AddInNativeTest::SetUp()
	{
		memoryManager = new MemoryManager();
		addInNative = new MockAddInNative();
		addInNative->Init(nullptr);
		addInNative->setMemManager(memoryManager);
		rdk1c = addInNative->GetRdk1C();
	}

	void AddInNativeTest::TearDown()
	{
		delete_pointer(addInNative);
		delete_pointer(memoryManager);
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
		wsPropName = addInNative->GetPropName(CAddInNative::ePropComponentVersion, 0);
		ASSERT_STREQ("ComponentVersion", ToString(wsPropName).c_str());
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
		ASSERT_TRUE(addInNative->IsPropWritable(CAddInNative::ePropLocale));
		ASSERT_FALSE(addInNative->IsPropWritable(CAddInNative::ePropLogFile));
		ASSERT_TRUE(addInNative->IsPropWritable(CAddInNative::ePropLogLevel));
		ASSERT_TRUE(addInNative->IsPropWritable(CAddInNative::ePropOperationTimeout));
		ASSERT_FALSE(addInNative->IsPropWritable(CAddInNative::ePropError));
		ASSERT_FALSE(addInNative->IsPropWritable(CAddInNative::ePropErrorDescription));
	}

	// Geters

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

	// Setters

	TEST_F(AddInNativeTest, SetPropLocale)
	{
		tVariant* propValue = newVariant(u"en-UK");
		bool is_success = addInNative->SetPropVal(CAddInNative::ePropLocale, propValue);
		ASSERT_TRUE(is_success);
		delete_pointer(propValue);

		CheckPropLocale("en-UK");
	}

	TEST_F(AddInNativeTest, SetPropLogLevel)
	{
		tVariant* propValue = newVariant(u"debug");
		bool is_success = addInNative->SetPropVal(CAddInNative::ePropLogLevel, propValue);
		ASSERT_TRUE(is_success);
		delete_pointer(propValue);

		CheckPropLogLevel("debug");
	}

	TEST_F(AddInNativeTest, SetPropOperationTimeout)
	{
		tVariant* propValue = newVariant(3000);
		bool is_success = addInNative->SetPropVal(CAddInNative::ePropOperationTimeout, propValue);
		ASSERT_TRUE(is_success);
		delete_pointer(propValue);

		CheckPropOperationTimeout(3000);
	}

	// Checks

	void AddInNativeTest::CheckPropComponentVersion(std::string component_varsion)
	{
		tVariant* result = new tVariant();
		bool is_success = addInNative->GetPropVal(CAddInNative::ePropComponentVersion, result);

		ASSERT_TRUE(is_success);
		ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
		ASSERT_STREQ(component_varsion.c_str(), ToString(TV_WSTR(result)).c_str());

		delete_pointer(result);
	}

	void AddInNativeTest::CheckPropRdKafkaVersion(std::string rdkafka_version)
	{
		tVariant* result = new tVariant();
		bool is_success = addInNative->GetPropVal(CAddInNative::ePropRdKafkaVersion, result);

		ASSERT_TRUE(is_success);
		ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
		ASSERT_STREQ(rdkafka_version.c_str(), ToString(TV_WSTR(result)).c_str());

		delete_pointer(result);
	}

	void AddInNativeTest::CheckPropLocale(std::string locale)
	{
		tVariant* result = new tVariant();
		bool is_success = addInNative->GetPropVal(CAddInNative::ePropLocale, result);

		ASSERT_TRUE(is_success);
		ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
		ASSERT_STREQ(locale.c_str(), ToString(TV_WSTR(result)).c_str());

		delete_pointer(result);
	}

	void AddInNativeTest::CheckPropLogFile(std::string log_file)
	{
		tVariant* result = new tVariant();
		bool is_success = addInNative->GetPropVal(CAddInNative::ePropLogFile, result);

		ASSERT_TRUE(is_success);
		ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
		ASSERT_STREQ(log_file.c_str(), ToString(TV_WSTR(result)).c_str());

		delete_pointer(result);
	}

	void AddInNativeTest::CheckPropLogLevel(std::string log_level)
	{
		tVariant* result = new tVariant();
		bool is_success = addInNative->GetPropVal(CAddInNative::ePropLogLevel, result);

		ASSERT_TRUE(is_success);
		ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
		ASSERT_STREQ(log_level.c_str(), ToString(TV_WSTR(result)).c_str());

		delete_pointer(result);
	}

	void AddInNativeTest::CheckPropOperationTimeout(long operation_timeout)
	{
		tVariant* result = new tVariant();
		bool is_success = addInNative->GetPropVal(CAddInNative::ePropOperationTimeout, result);

		ASSERT_TRUE(is_success);
		ASSERT_TRUE(TV_VT(result) = VTYPE_INT);
		ASSERT_EQ(operation_timeout, TV_INT(result));

		delete_pointer(result);
	}

	void AddInNativeTest::CheckPropError(bool error)
	{
		tVariant* result = new tVariant();
		bool is_success = addInNative->GetPropVal(CAddInNative::ePropError, result);

		ASSERT_TRUE(is_success);
		ASSERT_TRUE(TV_VT(result) = VTYPE_BOOL);
		ASSERT_FALSE(TV_BOOL(result));

		delete_pointer(result);
	}

	void AddInNativeTest::CheckPropErrorDescription(std::string error_description)
	{
		tVariant* result = new tVariant();
		bool is_success = addInNative->GetPropVal(CAddInNative::ePropErrorDescription, result);

		ASSERT_TRUE(is_success);
		ASSERT_TRUE(TV_VT(result) = VTYPE_PWSTR);
		ASSERT_STREQ(error_description.c_str(), ToString(TV_WSTR(result)).c_str());

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

	// Common methods

	TEST_F(AddInNativeTest, StartLogging)
	{
		tVariant* paParams = new tVariant[2];
		setToVariant(&paParams[0], u"file-name");
		setToVariant(&paParams[1], u"error");

		bool is_success = addInNative->CallAsProc(CAddInNative::eMethStartLogging, paParams, 2);
		ASSERT_TRUE(is_success);
		delete_array(paParams);

		CheckPropErrorDescription("");
		CheckPropError(false);
		CheckPropLogFile("file-name");
		CheckPropLogLevel("error");
	}

	TEST_F(AddInNativeTest, StopLogging)
	{
		tVariant* paParams = new tVariant();

		bool is_success = addInNative->CallAsProc(CAddInNative::eMethStopLogging, paParams, 0);
		ASSERT_TRUE(is_success);
		delete_pointer(paParams);

		CheckPropErrorDescription("");
		CheckPropError(false);
		CheckPropLogLevel("none");
	}

	TEST_F(AddInNativeTest, SetConfigProperty)
	{
		SetConfigProperty(u"bootstrap.servers", u"localhost:9092");
	}

	// Producer

	TEST_F(AddInNativeTest, InitProducer)
	{
		InitProducer();
	}

	TEST_F(AddInNativeTest, StopProducer)
	{
		InitProducer();
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
		Produce(u"topic", "message", "key", "{""header1"":1}", 0, u"");
		StopProducer();
	}

	TEST_F(AddInNativeTest, MessageStatus)
	{
		InitProducer();
		StartProduce();
		Produce(u"topic", "message", "key", "{""header1"":1}", 0, u"message-uuid");
		MessageStatus(u"message-uuid", "NOT_PERSISTED");
		StopProducer();
	}

	TEST_F(AddInNativeTest, ProduceFlush)
	{
		InitProducer();
		StartProduce();
		Produce(u"topic", "message", "key", "{""header1"":1}", 0, u"");
		Flush();
		StopProducer();
	}

	TEST_F(AddInNativeTest, ProducerQueueLen)
	{
		InitProducer();
		ProducerQueueLen();
		StopProducer();
	}

	TEST_F(AddInNativeTest, CountUndeliveredMessages)
	{
		InitProducer();
		CountUndeliveredMessages();
		StopProducer();
	}

	// Consumer

	TEST_F(AddInNativeTest, InitConsumer)
	{
		SetConfigProperty(u"bootstrap.servers", u"localhost:9092");
		SetConfigProperty(u"group.id", u"group-id");

		InitConsumer();
	}

	TEST_F(AddInNativeTest, StopConsumer)
	{
		SetConfigProperty(u"bootstrap.servers", u"localhost:9092");
		SetConfigProperty(u"group.id", u"group-id");

		InitConsumer();
		StopConsumer();
	}

	TEST_F(AddInNativeTest, Consume)
	{
		SetConfigProperty(u"bootstrap.servers", u"localhost:9092");
		SetConfigProperty(u"group.id", u"group-id");

		InitConsumer();
		
		MockMessage* message = new MockMessage();	// Will be deleted in RdKafka1C::StopConsumer()
		message->topic_name_ = "topic-name";
		message->payload_ = "payload";
		message->key_ = "key";
		message->headers_->add("header1", "value1");
		message->partition_ = 111;
		message->offset_ = 222;

		EXPECT_CALL(*rdk1c->GetConsumer(), consume(_))
			.WillOnce(Return(message));

		Consume();
		
		CheckMessageData("payload");
		CheckMessageKey("key");
		CheckMessageHeaders("header1:value1;");
		CheckMessageMetadata("topic-name", "111", "222", "PERSISTED");
		StopConsumer();
	}

	TEST_F(AddInNativeTest, AssignPartition)
	{
		SetConfigProperty(u"bootstrap.servers", u"localhost:9092");
		SetConfigProperty(u"group.id", u"group-id");

		InitConsumer();
		AssignPartition();
		StopConsumer();
	}

	TEST_F(AddInNativeTest, Subscribe)
	{
		SetConfigProperty(u"bootstrap.servers", u"localhost:9092");
		SetConfigProperty(u"group.id", u"group-id");

		InitConsumer();
		Subscribe();
		StopConsumer();
	}

	TEST_F(AddInNativeTest, Unsubscribe)
	{
		SetConfigProperty(u"bootstrap.servers", u"localhost:9092");
		SetConfigProperty(u"group.id", u"group-id");

		InitConsumer();
		Unsubscribe();
		StopConsumer();
	}
	
	TEST_F(AddInNativeTest, CommittedOffset)
	{
		SetConfigProperty(u"bootstrap.servers", u"localhost:9092");
		SetConfigProperty(u"group.id", u"group-id");

		InitConsumer();
		CommittedOffset();
		StopConsumer();
	}

	TEST_F(AddInNativeTest, CommitOffset)
	{
		SetConfigProperty(u"bootstrap.servers", u"localhost:9092");
		SetConfigProperty(u"group.id", u"group-id");

		InitConsumer();
		CommitOffset();
		StopConsumer();
	}

	// Checks

	void AddInNativeTest::SetConfigProperty(const std::u16string& name, const std::u16string& value)
	{
		tVariant* paParams = new tVariant[2];
		setToVariant(&paParams[0], name.c_str());
		setToVariant(&paParams[1], value.c_str());
		
		bool is_success = addInNative->CallAsProc(CAddInNative::eMethSetConfigProperty, paParams, 2);
		delete_array(paParams);
		
		ASSERT_TRUE(is_success);
		
		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::InitProducer()
	{
		tVariant* paParams = new tVariant();

		bool is_success = addInNative->CallAsProc(CAddInNative::eMethInitProducer, paParams, 0);
		delete_pointer(paParams);

		ASSERT_TRUE(is_success);
		
		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::StartProduce()
	{
		tVariant* paParams = new tVariant();
		bool is_success = addInNative->CallAsProc(CAddInNative::eMethStartProduce, paParams, 0);
		delete_pointer(paParams);

		ASSERT_TRUE(is_success);
		
		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::Produce(std::u16string topic, std::string message, std::string key, std::string headers,
		long partition, std::u16string id)
	{
		tVariant* paParams = new tVariant[6];
		setToVariant(&paParams[0], topic.c_str());
		setToVariant(&paParams[1], message.data());
		setToVariant(&paParams[2], key.data());
		setToVariant(&paParams[3], headers.data());
		setToVariant(&paParams[4], partition);
		setToVariant(&paParams[5], id.c_str());

		bool is_success = addInNative->CallAsProc(CAddInNative::eMethProduce, paParams, 6);
		delete_array(paParams);

		ASSERT_TRUE(is_success);

		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::Flush()
	{
		tVariant* paParams = new tVariant();

		EXPECT_CALL(*rdk1c->GetProducer(), flush(_))
			.WillOnce(Return(RdKafka::ErrorCode::ERR_NO_ERROR));

		bool is_success = addInNative->CallAsProc(CAddInNative::eMethFlush, paParams, 0);
		delete_pointer(paParams);
		
		ASSERT_TRUE(is_success);

		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::ProducerQueueLen()
	{
		tVariant* paParams = new tVariant();
		tVariant* pvarRetValue = new tVariant();	// Will be delete by MemoryManager

		bool is_success = addInNative->CallAsFunc(CAddInNative::eMethProducerQueueLen, pvarRetValue, paParams, 0);
		delete_pointer(paParams);
		ASSERT_TRUE(is_success);
		ASSERT_EQ(TV_INT(pvarRetValue), 0);
		
		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::CountUndeliveredMessages()
	{
		tVariant* paParams = new tVariant();
		tVariant* pvarRetValue = new tVariant();	// Will be delete by MemoryManager

		bool is_success = addInNative->CallAsFunc(CAddInNative::eMethCountUndeliveredMessages, pvarRetValue, paParams, 0);
		delete_pointer(paParams);
		ASSERT_TRUE(is_success);
		ASSERT_EQ(TV_INT(pvarRetValue), 0);
		
		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::StopProducer()
	{
		tVariant* paParams = new tVariant();

		bool is_success = addInNative->CallAsProc(CAddInNative::eMethStopProducer, paParams, 0);
		delete_pointer(paParams);

		ASSERT_TRUE(is_success);

		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::MessageStatus(std::u16string id, std::string status)
	{
		tVariant* pvarRetValue = new tVariant();	// Will be delete by MemoryManager
		tVariant* paParams = new tVariant[1];
		setToVariant(&paParams[0], id.c_str());
		
		bool is_success = addInNative->CallAsFunc(CAddInNative::eMethMessageStatus, pvarRetValue, paParams, 1);
		delete_array(paParams);
		
		ASSERT_TRUE(is_success);
		ASSERT_TRUE(TV_VT(pvarRetValue) = VTYPE_PWSTR);
		ASSERT_STREQ(status.c_str(), ToString(TV_WSTR(pvarRetValue)).c_str());

		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::InitConsumer()
	{
		tVariant* paParams = new tVariant();

		bool is_success = addInNative->CallAsProc(CAddInNative::eMethInitConsumer, paParams, 0);
		delete_pointer(paParams);
		
		ASSERT_TRUE(is_success);
		
		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::Consume()
	{
		tVariant* paParams = new tVariant();
		tVariant* pvarRetValue = new tVariant();	// Will be delete by MemoryManager
		
		bool is_success = addInNative->CallAsFunc(CAddInNative::eMethConsume, pvarRetValue, paParams, 0);
		delete_pointer(paParams);
		
		ASSERT_TRUE(is_success);
		ASSERT_TRUE(TV_BOOL(pvarRetValue));

		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::CheckMessageData(std::string value)
	{
		tVariant* paParams = new tVariant();
		tVariant* pvarRetValue = new tVariant();	// Will be delete by MemoryManager

		bool is_success = addInNative->CallAsFunc(CAddInNative::eMethMessageData, pvarRetValue, paParams, 0);
		delete_pointer(paParams);
		
		ASSERT_TRUE(is_success);
		ASSERT_TRUE(TV_VT(pvarRetValue) = VTYPE_PWSTR);
		ASSERT_STREQ(value.c_str(), std::string(TV_STR(pvarRetValue), pvarRetValue->strLen).c_str());

		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::CheckMessageKey(std::string value)
	{
		tVariant* paParams = new tVariant();
		tVariant* pvarRetValue = new tVariant();	// Will be delete by MemoryManager

		bool is_success = addInNative->CallAsFunc(CAddInNative::eMethMessageKey, pvarRetValue, paParams, 0);
		delete_pointer(paParams);

		ASSERT_TRUE(is_success);
		ASSERT_TRUE(TV_VT(pvarRetValue) = VTYPE_PWSTR);
		ASSERT_STREQ(value.c_str(), std::string(TV_STR(pvarRetValue), pvarRetValue->strLen).c_str());

		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::CheckMessageHeaders(std::string value)
	{
		tVariant* paParams = new tVariant();
		tVariant* pvarRetValue = new tVariant();	// Will be delete by MemoryManager

		bool is_success = addInNative->CallAsFunc(CAddInNative::eMethMessageHeaders, pvarRetValue, paParams, 0);
		delete_pointer(paParams);

		ASSERT_TRUE(is_success);
		ASSERT_TRUE(TV_VT(pvarRetValue) = VTYPE_PWSTR);
		ASSERT_STREQ(value.c_str(), std::string(TV_STR(pvarRetValue), pvarRetValue->strLen).c_str());

		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::CheckMessageMetadata(std::string topic_name, std::string partition, std::string offset, std::string status)
	{
		tVariant* paParams = new tVariant();
		tVariant* pvarRetValue = new tVariant();	// Will be delete by MemoryManager

		bool is_success = addInNative->CallAsFunc(CAddInNative::eMethMessageMetadata, pvarRetValue, paParams, 0);
		delete_pointer(paParams);

		ASSERT_TRUE(is_success);
		ASSERT_TRUE(TV_VT(pvarRetValue) = VTYPE_PWSTR);

		std::string metadata = ToString(TV_WSTR(pvarRetValue));
		ASSERT_THAT(metadata, HasSubstr(topic_name));
		ASSERT_THAT(metadata, HasSubstr(partition));
		ASSERT_THAT(metadata, HasSubstr(offset));
		ASSERT_THAT(metadata, HasSubstr(status));

		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::AssignPartition()
	{
		tVariant* paParams = new tVariant[2];
		setToVariant(&paParams[0], u"topic-name");
		setToVariant(&paParams[1], 111);

		bool is_success = addInNative->CallAsProc(CAddInNative::eMethAssignPartition, paParams, 2);
		ASSERT_TRUE(is_success);
		delete_array(paParams);

		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::Subscribe()
	{
		tVariant* paParams = new tVariant[1];
		setToVariant(&paParams[0], u"topic-name");
		
		bool is_success = addInNative->CallAsProc(CAddInNative::eMethSubscribe, paParams, 1);
		ASSERT_TRUE(is_success);
		delete_array(paParams);

		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::Unsubscribe()
	{
		tVariant* paParams = new tVariant();
		
		bool is_success = addInNative->CallAsProc(CAddInNative::eMethUnsubscribe, paParams, 0);
		ASSERT_TRUE(is_success);
		delete_pointer(paParams);

		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::CommittedOffset()
	{
		int offset = -1001; // Undefined offset value
		tVariant* pvarRetValue = new tVariant();	// Will be delete by MemoryManager
		tVariant* paParams = new tVariant[2];
		setToVariant(&paParams[0], u"topic-name");
		setToVariant(&paParams[1], 123);

		EXPECT_CALL(*rdk1c->GetConsumer(), committed(_, _))
			.WillOnce(Return(RdKafka::ErrorCode::ERR_NO_ERROR));
		
		bool is_success = addInNative->CallAsFunc(CAddInNative::eMethCommittedOffset, pvarRetValue, paParams, 2);
		delete_array(paParams);

		ASSERT_TRUE(is_success);
		ASSERT_TRUE(TV_VT(pvarRetValue) = VTYPE_INT);
		ASSERT_EQ(offset, TV_INT(pvarRetValue));

		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::CommitOffset()
	{
		tVariant* pvarRetValue = new tVariant();	// Will be delete by MemoryManager
		tVariant* paParams = new tVariant[0];
		
		EXPECT_CALL(*rdk1c->GetConsumer(), commitSync())
			.WillOnce(Return(RdKafka::ErrorCode::ERR_NO_ERROR));
		
		bool is_success = addInNative->CallAsFunc(CAddInNative::eMethCommitOffset, pvarRetValue, paParams, 0);
		delete_array(paParams);

		ASSERT_TRUE(is_success);
		ASSERT_TRUE(TV_VT(pvarRetValue) = VTYPE_BOOL);
		ASSERT_TRUE(TV_BOOL(pvarRetValue));

		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	void AddInNativeTest::StopConsumer()
	{
		tVariant* paParams = new tVariant();

		bool is_success = addInNative->CallAsProc(CAddInNative::eMethStopConsumer, paParams, 0);
		ASSERT_TRUE(is_success);
		delete_pointer(paParams);

		CheckPropErrorDescription("");
		CheckPropError(false);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Helper methods

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

	void AddInNativeTest::setToVariant(tVariant* variant, const WCHAR_T* value)
	{
		TV_VT(variant) = VTYPE_PWSTR;
		TV_WSTR(variant) = const_cast<WCHAR_T*>(value);
	}

	void AddInNativeTest::setToVariant(tVariant* variant, long value)
	{
		TV_VT(variant) = VTYPE_INT;
		TV_INT(variant) = value;
	}

	void AddInNativeTest::setToVariant(tVariant* variant, char* value)
	{
		TV_VT(variant) = VTYPE_BLOB;
		variant->pstrVal = value;
		variant->strLen = strlen(value);
	}

}