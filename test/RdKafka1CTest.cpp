#include "RdKafka1CTest.h"

std::string GetData();

/////////////////////////////////////////////////////////////////////////////
// Tests

TEST_F(RdKafka1CTest, Constructor)
{
    // Here method tests SetUp() & TearDown()
}

TEST_F(RdKafka1CTest, InitProducer)
{
    InitProducer();
    StopProducer();
}

TEST_F(RdKafka1CTest, InitProducer_Twice)
{
    InitProducer();
    
    bool initResult = rdk1c->InitProducer("some-broker");
    ASSERT_STREQ(rdk1c->ErrorDescription().c_str(), "Failed to create producer: producer has been initialized already"); 
    ASSERT_FALSE(initResult);
    
    StopProducer();
}

TEST_F(RdKafka1CTest, InitConsumer)
{
    InitConsumer();
    StopConsumer();
}

TEST_F(RdKafka1CTest, InitConsumer_Twice)
{
    InitConsumer();

    bool initResult = rdk1c->InitConsumer("some-broker", "some-consumer-id");
    ASSERT_STREQ(rdk1c->ErrorDescription().c_str(), "Failed to create consumer: consumer has been initialized already");
    ASSERT_FALSE(initResult);
    
    StopConsumer();
}

TEST_F(RdKafka1CTest, ProduceConsume)
{
    ConsumeAll();
    Produce();
    Consume();
}

TEST_F(RdKafka1CTest, ProduceConsumeWithKey)
{
    SetKey("Key (ключ) 1 ");    
    ConsumeAll();
    Produce();
    SetKey("");    
    
    InitConsumer();  
    ConsumeMessage();    
    std::string key = GetKeyFromMessage();    
    StopConsumer();

    ASSERT_STREQ(key.c_str(), "Key (ключ) 1 ");
}

TEST_F(RdKafka1CTest, ProduceConsumeWithHeader)
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

TEST_F(RdKafka1CTest, ConsumeFromEmptyTopic)
{
    ConsumeAll();    
    InitConsumer();    

    bool consumeResult = rdk1c->Consume();
    ASSERT_STREQ(rdk1c->ErrorDescription().c_str(), "");
    ASSERT_FALSE(consumeResult);
    
    std::string data = rdk1c->GetMessageData();
    ASSERT_STREQ(rdk1c->ErrorDescription().c_str(), "");
    ASSERT_STREQ(data.substr(0, 17).c_str(), "Fetch from broker");

    std::string metadata = rdk1c->GetMessageMetadata();
    ASSERT_STREQ(rdk1c->ErrorDescription().c_str(), "");
    
    StopConsumer();
}

TEST_F(RdKafka1CTest, StartLogging)
{
    StartLogging();
}

TEST_F(RdKafka1CTest, StartStopLogging)
{
    StartLogging();
    StopLogging();
}

TEST_F(RdKafka1CTest, StartStopLoggingInWork)
{
    StartLogging();
    ConsumeAll();
    StopLogging();
}

TEST_F(RdKafka1CTest, SetProperty)
{
    SetProperty("sasl.username", "test-user");
    
    StartLogging();
    InitProducer();
    StopProducer();
}

TEST_F(RdKafka1CTest, SASL_SSL)
{
    SetProperty("security.protocol", "sasl_ssl");
    //SetProperty("ssl.ca.location", "ca-cert");
    //SetProperty("ssl.certificate.location", "c:/");
    SetProperty("sasl.mechanism", "SCRAM-SHA-512");
    SetProperty("sasl.username", "user");
    SetProperty("sasl.password", "password");

    InitProducer();
    StopProducer();
}

/////////////////////////////////////////////////////////////////////////////
// Test class members

void RdKafka1CTest::SetUp()
{
    setlocale(LC_ALL, "ru_RU");
    rdk1c = new RdKafka1C();
    rdk1c->OperationTimeout = 10000;
}

void RdKafka1CTest::TearDown()
{
    delete rdk1c;
}

void RdKafka1CTest::InitProducer()
{   
    bool initResult = rdk1c->InitProducer(BROKERS);
    ASSERT_STREQ(rdk1c->ErrorDescription().c_str(), "");   
    ASSERT_TRUE(initResult);
}

void RdKafka1CTest::Produce()
{   
    InitProducer();    
    GenerateNewData();
    ProduceMessage();
    StopProducer();
}

void RdKafka1CTest::ProduceMessage()
{   
    bool sendResult = rdk1c->Produce(TOPIC, DATA, KEY, HEADERS, 0);
    ASSERT_STREQ(rdk1c->ErrorDescription().c_str(), "");
    ASSERT_TRUE(sendResult);
}

void RdKafka1CTest::StopProducer()
{   
    bool stopResult = rdk1c->StopProduser();
    ASSERT_STREQ(rdk1c->ErrorDescription().c_str(), "");
    ASSERT_TRUE(stopResult);
}

void RdKafka1CTest::InitConsumer()
{
    bool initResult = rdk1c->InitConsumer(BROKERS, CONSUMER_GROUP_ID);
    ASSERT_STREQ(rdk1c->ErrorDescription().c_str(), "");
    ASSERT_TRUE(initResult);

    bool subscribeResult = rdk1c->Subscribe(TOPIC);
    ASSERT_STREQ(rdk1c->ErrorDescription().c_str(), "");
    ASSERT_TRUE(subscribeResult);
}

void RdKafka1CTest::Consume()
{
    InitConsumer();  
    ConsumeMessage();
    StopConsumer();
}

void RdKafka1CTest::ConsumeMessage()
{
    bool consumeResult = rdk1c->Consume();
    ASSERT_STREQ(rdk1c->ErrorDescription().c_str(), "");
    ASSERT_TRUE(consumeResult);
    
    std::string data = rdk1c->GetMessageData();
    ASSERT_STREQ(rdk1c->ErrorDescription().c_str(), "");
    ASSERT_STREQ(DATA.c_str(), data.c_str());
}

void RdKafka1CTest::ConsumeAll()
{
    InitConsumer();

    while (rdk1c->Consume()) 
    {
        // Read all messages from topic       
    }    
    ASSERT_STREQ(rdk1c->ErrorDescription().c_str(), "");

    StopConsumer();
}

void RdKafka1CTest::StopConsumer()
{
    bool stopResult = rdk1c->StopConsumer();
    ASSERT_STREQ(rdk1c->ErrorDescription().c_str(), "");
    ASSERT_TRUE(stopResult);
}

void RdKafka1CTest::StartLogging()
{
    bool initLogs = rdk1c->StartLogging(LOG_FILE);
    ASSERT_TRUE(initLogs);     
}

void RdKafka1CTest::StopLogging()
{
    rdk1c->StopLogging();

    std::string logfile = rdk1c->GetCurrentLogFile();
    ASSERT_STRNE(logfile.c_str(), "");
    ASSERT_TRUE(std::filesystem::exists(logfile));
}

std::string RdKafka1CTest::GetKeyFromMessage()
{
    std::string metadata = rdk1c->GetMessageMetadata();
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

std::string RdKafka1CTest::GetHeadersFromMessage()
{
    std::string metadata = rdk1c->GetMessageMetadata();
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

int RdKafka1CTest::NewRand()
{
    srand(time(0));
    return rand();
}

void RdKafka1CTest::GenerateNewData()
{
    std::stringstream stream;
    stream << "test message (тестовое сообщение) " << NewRand();
    DATA = stream.str();
}

void RdKafka1CTest::SetKey(std::string key)
{
    KEY = key;
}

void RdKafka1CTest::SetHeaders(std::string headers)
{
    HEADERS = headers;
}

void RdKafka1CTest::SetProperty(std::string name, std::string value)
{
    rdk1c->SetConfigProperty(name, value);
}




