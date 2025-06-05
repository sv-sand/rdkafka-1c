#include "RdKafka1CIntegrtionTest.h"

std::string GetData();

/////////////////////////////////////////////////////////////////////////////
// Tests

TEST_F(RdKafka1CIntegrtionTest, Constructor)
{
    // Here method tests SetUp() & TearDown()
}

TEST_F(RdKafka1CIntegrtionTest, InitProducer)
{
    InitProducer();
    StopProducer();
}

TEST_F(RdKafka1CIntegrtionTest, InitProducer_Twice)
{
    InitProducer();
    
    rdk1c->SetConfigProperty("bootstrap.servers", "some-broker");
    
    bool initResult = rdk1c->InitProducer();
    ASSERT_STREQ(error->ErrorDescription().c_str(), "Failed to create producer: producer has been initialized already"); 
    ASSERT_FALSE(initResult);
    
    StopProducer();
}

TEST_F(RdKafka1CIntegrtionTest, InitConsumer)
{
    InitConsumer();
    StopConsumer();
}

TEST_F(RdKafka1CIntegrtionTest, InitConsumer_Twice)
{
    InitConsumer();

    rdk1c->SetConfigProperty("metadata.broker.list", "some-broker");
    rdk1c->SetConfigProperty("group.id", "some-consumer-id");

    bool initResult = rdk1c->InitConsumer();
    ASSERT_STREQ(error->ErrorDescription().c_str(), "Failed to create consumer: consumer has been initialized already");
    ASSERT_FALSE(initResult);
    
    StopConsumer();
}

TEST_F(RdKafka1CIntegrtionTest, ProduceConsume)
{
    ConsumeAll();
    Produce();
    Consume();
}

TEST_F(RdKafka1CIntegrtionTest, ProduceConsumeWithKey)
{
    SetKey("Key (ключ) 1");    
    ConsumeAll();
    Produce();
    SetKey("");    
    
    InitConsumer();  
    ConsumeMessage();    
    std::string key = GetKeyFromMessage();    
    StopConsumer();

    ASSERT_STREQ(key.c_str(), "Key (ключ) 1");
}

TEST_F(RdKafka1CIntegrtionTest, ProduceConsumeWithHeader)
{
    SetHeaders("Header 1: значение 1;  Header 2: значение 2; ");    
    ConsumeAll();
    Produce();
    SetHeaders("");
    
    InitConsumer();  
    ConsumeMessage();    
    std::string headers = GetHeadersFromMessage();    
    StopConsumer();

    ASSERT_STREQ(headers.c_str(), "Header 1:значение 1;Header 2:значение 2");
}

TEST_F(RdKafka1CIntegrtionTest, ConsumeFromEmptyTopic)
{
    ConsumeAll();    
    InitConsumer();    

    bool consumeResult = rdk1c->Consume();
    ASSERT_STREQ(error->ErrorDescription().c_str(), "");
    ASSERT_FALSE(consumeResult);
    
    std::string data = rdk1c->MessageData();
    ASSERT_STREQ(error->ErrorDescription().c_str(), "");
    ASSERT_STREQ(data.substr(0, 17).c_str(), "Fetch from broker");

    std::string metadata = rdk1c->MessageMetadata();
    ASSERT_STREQ(error->ErrorDescription().c_str(), "");
    
    StopConsumer();
}

TEST_F(RdKafka1CIntegrtionTest, StartLogging)
{
    StartLogging();
}

TEST_F(RdKafka1CIntegrtionTest, StartStopLogging)
{
    StartLogging();
    StopLogging();
}

TEST_F(RdKafka1CIntegrtionTest, StartStopLoggingInWork)
{
    StartLogging();
    ConsumeAll();
    StopLogging();
}

TEST_F(RdKafka1CIntegrtionTest, SetProperty)
{
    SetProperty("sasl.username", "test-user");
    
    StartLogging();
    InitProducer();
    StopProducer();
}

/////////////////////////////////////////////////////////////////////////////
// Test class members

void RdKafka1CIntegrtionTest::SetUp()
{
    
#ifdef WIN32
    std::setlocale(LC_ALL, "ru-RU");
#else
    std::setlocale(LC_ALL, "ru_RU");
#endif

    loger = new Loger(); 
    error = new ErrorHandler(loger);    
    rdk1c = new RdKafka1C(loger, error);
    rdk1c->OperationTimeout = 10000;
}

void RdKafka1CIntegrtionTest::TearDown()
{
    delete_pointer(rdk1c);
    delete_pointer(error);
    delete_pointer(loger);
}

void RdKafka1CIntegrtionTest::InitProducer()
{   
    rdk1c->SetConfigProperty("bootstrap.servers", BROKERS);
    bool initResult = rdk1c->InitProducer();
    ASSERT_STREQ(error->ErrorDescription().c_str(), "");   
    ASSERT_TRUE(initResult);
}

void RdKafka1CIntegrtionTest::Produce()
{   
    InitProducer();
    GenerateNewData();
    ProduceMessage();
    StopProducer();
}

void RdKafka1CIntegrtionTest::ProduceMessage()
{   
    bool sendResult = rdk1c->Produce(TOPIC, DATA, KEY, HEADERS, 0, "");
    ASSERT_STREQ(error->ErrorDescription().c_str(), "");
    ASSERT_TRUE(sendResult);

    bool flushResult = rdk1c->Flush();
    ASSERT_STREQ(error->ErrorDescription().c_str(), "");
    ASSERT_TRUE(flushResult);
}

void RdKafka1CIntegrtionTest::StopProducer()
{   
    bool stopResult = rdk1c->StopProduser();
    ASSERT_STREQ(error->ErrorDescription().c_str(), "");
    ASSERT_TRUE(stopResult);
}

void RdKafka1CIntegrtionTest::InitConsumer()
{
    SetProperty("auto.offset.reset", "smallest");

    rdk1c->SetConfigProperty("metadata.broker.list", BROKERS);
    rdk1c->SetConfigProperty("group.id", CONSUMER_GROUP_ID);

    bool initResult = rdk1c->InitConsumer();
    ASSERT_STREQ(error->ErrorDescription().c_str(), "");
    ASSERT_TRUE(initResult);

    bool subscribeResult = rdk1c->Subscribe(TOPIC);
    ASSERT_STREQ(error->ErrorDescription().c_str(), "");
    ASSERT_TRUE(subscribeResult);
}

void RdKafka1CIntegrtionTest::Consume()
{
    InitConsumer();  
    ConsumeMessage();
    StopConsumer();
}

void RdKafka1CIntegrtionTest::ConsumeMessage()
{
    bool consumeResult = rdk1c->Consume();
    ASSERT_STREQ(error->ErrorDescription().c_str(), "");
    ASSERT_TRUE(consumeResult);
    
    std::string data = rdk1c->MessageData();
    ASSERT_STREQ(error->ErrorDescription().c_str(), "");
    ASSERT_STREQ(DATA.c_str(), data.c_str());
}

void RdKafka1CIntegrtionTest::ConsumeAll()
{
    InitConsumer();

    while (rdk1c->Consume()) 
    {
        // Read all messages from topic       
    }    
    ASSERT_STREQ(error->ErrorDescription().c_str(), "");

    StopConsumer();
}

void RdKafka1CIntegrtionTest::StopConsumer()
{
    bool stopResult = rdk1c->StopConsumer();
    ASSERT_STREQ(error->ErrorDescription().c_str(), "");
    ASSERT_TRUE(stopResult);
}

void RdKafka1CIntegrtionTest::StartLogging()
{
    loger->level = Loger::Levels::DEBUG;
    std::string errorDescription;    
    bool result = loger->Init(LOG_FILE, errorDescription);

    ASSERT_STREQ(errorDescription.c_str(), ""); 
    ASSERT_TRUE(result);     
}

void RdKafka1CIntegrtionTest::StopLogging()
{
    loger->level = Loger::Levels::NONE;

    std::string logfile = loger->GetLogFile();
    ASSERT_STRNE(logfile.c_str(), "");
    ASSERT_TRUE(std::filesystem::exists(logfile));
}

std::string RdKafka1CIntegrtionTest::GetKeyFromMessage()
{
    std::string metadata = rdk1c->MessageMetadata();
    std::stringstream stream(metadata);
    boost::property_tree::ptree tree;
    std::string key;

    try
    {
        boost::property_tree::read_json(stream, tree);
        key = tree.get<std::string>("key", "");
    }
    catch (boost::property_tree::json_parser_error e)
    {
        throw "Failed to parse JSON: " + e.message();
    }

    return key;
}

std::string RdKafka1CIntegrtionTest::GetHeadersFromMessage()
{
    std::string metadata = rdk1c->MessageMetadata();
    std::stringstream stream(metadata);
    boost::property_tree::ptree tree;

    try
    {
        boost::property_tree::read_json(stream, tree);

        stream.str("");
        for (auto & keyValue : tree.get_child("headers"))
        {
            stream << keyValue.first << ":"
                   << keyValue.second.get<std::string>("") + ";";      
        }
    }
    catch (boost::property_tree::json_parser_error e)
    {
        throw "Failed to parse JSON: " + e.message();
    }

    std::string result = stream.str();

    return result.substr(0, result.length() - 1);
}

/////////////////////////////////////////////////////////////////////////////
// Support methods

int RdKafka1CIntegrtionTest::NewRand()
{
    srand(time(0));
    return rand();
}

void RdKafka1CIntegrtionTest::GenerateNewData()
{
    std::stringstream stream;
    stream << "test message (тестовое сообщение) " << NewRand();
    DATA = stream.str();
}

void RdKafka1CIntegrtionTest::SetKey(std::string key)
{
    KEY = key;
}

void RdKafka1CIntegrtionTest::SetHeaders(std::string headers)
{
    HEADERS = headers;
}

void RdKafka1CIntegrtionTest::SetProperty(std::string name, std::string value)
{
    rdk1c->SetConfigProperty(name, value);
}




