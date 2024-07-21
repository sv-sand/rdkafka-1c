#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "RdKafka1C.h" 

static const std::string BROKERS = "localhost";
static const std::string TOPIC = "test";
static const std::string CONSUMER_GROUP_ID = "testgroup";
static std::string DATA;

void Produce(RdKafka1C* rdk1c)
{
    srand(time(0));
    DATA = "test message " + std::to_string(rand());
    
    bool initResult = rdk1c->InitProducer(BROKERS, TOPIC);
    ASSERT_TRUE(initResult);    

    bool sendResult = rdk1c->Produce(DATA);
    ASSERT_TRUE(sendResult);

    bool stopResult = rdk1c->StopProduser();
    ASSERT_TRUE(stopResult);
}

void Consume(RdKafka1C* rdk1c)
{
    bool initResult = rdk1c->InitConsumer(BROKERS, TOPIC, CONSUMER_GROUP_ID);
    ASSERT_TRUE(initResult);    

    bool consumeResult = rdk1c->Consume();
    ASSERT_TRUE(consumeResult);

    std::string data = rdk1c->GetMessageData();
    ASSERT_STREQ(DATA.c_str(), data.c_str());

    bool stopResult = rdk1c->StopConsumer();
    ASSERT_TRUE(stopResult);
}

TEST(RdKafka1CTest, InitProducer)
{
    RdKafka1C* rdk1c = new RdKafka1C();

    bool initResult = rdk1c->InitProducer(BROKERS, TOPIC);
    ASSERT_TRUE(initResult);    

    bool stopResult = rdk1c->StopProduser();
    ASSERT_TRUE(stopResult);    

    delete rdk1c;
}

TEST(RdKafka1CTest, InitConsumer)
{
    RdKafka1C* rdk1c = new RdKafka1C();

    bool initResult = rdk1c->InitConsumer(BROKERS, TOPIC, CONSUMER_GROUP_ID);
    ASSERT_TRUE(initResult);    

    bool stopResult = rdk1c->StopConsumer();
    ASSERT_TRUE(stopResult);    

    delete rdk1c;
}

TEST(RdKafka1CTest, ProduceConsume)
{
    RdKafka1C* rdk1c = new RdKafka1C();
    
    Produce(rdk1c);
    Consume(rdk1c);

    delete rdk1c;
}

TEST(RdKafka1CTest, ProduceConsumeLogs)
{
    RdKafka1C* rdk1c = new RdKafka1C();
    std::string data = "test message";

    bool initLogs = rdk1c->InitLoger("../../logs");
    ASSERT_TRUE(initLogs);    

    Produce(rdk1c);
    Consume(rdk1c);

    delete rdk1c;
}

