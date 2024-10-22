#pragma once

#include <iostream>
#include <filesystem>
#include <tuple>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "RdKafka1C.h"

#ifdef WIN32
#pragma setlocale("ru-RU")
#endif

class RdKafka1CTest : public testing::Test 
{
    public:

        void SetUp();
        void TearDown();

    protected:

        RdKafka1C* rdk1c;

        void CreateRdKafka1C();
        void DeleteRdKafka1C();

        void InitProducer();
        void Produce();
        void ProduceMessage();
        void StopProducer();
        
        void InitConsumer();
        void Consume();
        void ConsumeAll();  
        void ConsumeMessage();
        void StopConsumer();

        void GenerateNewData();
        void StartLogging();
        void StopLogging();
        std::string GetKeyFromMessage();
        std::string GetHeadersFromMessage();
        void SetKey(std::string key);
        void SetHeaders(std::string headers);
        void SetProperty(std::string name, std::string value);
        
    private:

        const std::string BROKERS = "localhost";
        const std::string TOPIC = "test";
        const std::string CONSUMER_GROUP_ID = "testgroup";
        const std::string LOG_FILE = "testing.log";

        std::string DATA;       
        std::string KEY;
        std::string HEADERS;

        int NewRand();        
};
