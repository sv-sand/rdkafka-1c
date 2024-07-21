#include "RdKafka1C.h"

RdKafka1C::RdKafka1C()
{
    loger = new Loger();    
    loger->Debug("RdKafka1C constructor called");

    config = new Config(loger);

    producer = nullptr;
    consumer = nullptr;
    message = nullptr;
}

RdKafka1C::~RdKafka1C()
{
    loger->Info("RdKafka1C dectructor called");

    StopProduser();
    StopConsumer();

    delete config;    
    delete loger; 
}

std::string RdKafka1C::RdKafkaVersion()
{
    return RdKafka::version_str();
}

std::string RdKafka1C::ErrorDescription()
{
    return errorDescription;
}

void RdKafka1C::SetConfigProperty(std::string Name, std::string Value)
{
    config->AddProperty(Name, Value);
}

/////////////////////////////////////////////////////////////////////////////
// Producer

bool RdKafka1C::InitProducer(std::string Brokers, std::string Topic)
{
    loger->Info("Producer initialization");
    
    if (producer)
    {
        errorDescription = "Failed to create producer: producer has been initialized already";
        loger->Error(errorDescription);
        return false;
    }

    // Create kafka config
    config->ClearProperties();
    config->AddProperty("bootstrap.servers", Brokers);

    if (!config->Build(errorDescription))
        return false;

    // Create producer using accumulated global configuration 
	producer = RdKafka::Producer::create(config->Get(), errorDescription);
	if (!producer) 
    {
        loger->Error("Failed to create producer: " + errorDescription);
        return false;
	}

    loger->Info("Producer created successful: " + producer->name());
	
    topicName = Topic;

    return true;
}

bool RdKafka1C::Produce(std::string Message)
{
    return Produce(Message, "", RdKafka::Topic::PARTITION_UA);
}

bool RdKafka1C::Produce(std::string Message, std::string Headers)
{
    return Produce(Message, Headers, RdKafka::Topic::PARTITION_UA);
}

bool RdKafka1C::Produce(std::string Message, std::string Headers, int partition)
{
    loger->Info("Produce message");

    if (!producer)
    {
        errorDescription = "Failed to produce message: producer has not been initialized";
        loger->Error(errorDescription);
        return false;
    }

    RdKafka::ErrorCode errorCode;
    RdKafka::Headers* headers = nullptr;//CreateHeaders(Headers);
    
    if (producer->outq_len() > 0)
        loger->Warn("Topic " + topicName + " has " + std::to_string(producer->outq_len()) + " not delivered message(s)");

    for (int attempt = 1; attempt < 10; attempt++)
    {
        loger->Info("Try to produce in topic");
        
        /*
         * Send/Produce message.
         * This is an asynchronous call, on success it will only
         * enqueue the message on the internal producer queue.
         * The actual delivery attempts to the broker are handled
         * by background threads.
         * The previously registered delivery report callback
         * is used to signal back to the application when the message
         * has been delivered (or failed permanently after retries).
         */
        
        errorCode = producer->produce(
            topicName, partition, RdKafka::Producer::RK_MSG_COPY,
            const_cast<char*>(Message.c_str()), Message.size(),
            NULL, 0, 0, headers, NULL);

        /*
        * If the internal queue is full, wait for
        * messages to be delivered and then retry.
        * The internal queue represents both
        * messages to be sent and messages that have
        * been sent or failed, awaiting their
        * delivery report callback to be called.
        *
        * The internal queue is limited by the
        * configuration property
        * queue.buffering.max.messages and queue.buffering.max.kbytes
        */

        producer->poll(OperationTimeout);

        if (errorCode == RdKafka::ERR__QUEUE_FULL)
            loger->Debug("Internal queue is full, " + std::to_string(producer->outq_len()) + " message(s) were not delivered");

        break;
    }

    delete headers;

    if (errorCode) 
    {
        errorDescription = "Failed to produce in topic: " + RdKafka::err2str(errorCode);
        loger->Error(errorDescription);
        return false;
    }

    loger->Info("Enqueued message for topic " + topicName);

    /* 
     * Wait for final messages to be delivered or fail.
     * flush() is an abstraction over poll() which
     * waits for all messages to be delivered. 
     */

    errorCode = producer->flush(OperationTimeout); // wait for max 1 seconds
    if (errorCode)
    {
        errorDescription = "Failed to flush producer: " + RdKafka::err2str(errorCode);
        loger->Error(errorDescription);
    }

    if (producer->outq_len() > 0)
    {
        loger->Warn("After flush topic has " + std::to_string(producer->outq_len()) + " not delivered message(s)");
        return false;
    }

    return true;
}

bool RdKafka1C::StopProduser()
{
    loger->Info("Producer stop"); 
    
    if (!producer)
        return false;

    RdKafka::ErrorCode errorCode = producer->flush(10 * OperationTimeout); // wait for max 10x
    if (errorCode)
    {
        errorDescription = "Failed to flush producer: " + RdKafka::err2str(errorCode);
        loger->Error(errorDescription);
    }
    
    delete producer;
    producer = nullptr;

    return true;
}

RdKafka::Headers* RdKafka1C::CreateHeaders(std::string Headers)
{
    loger->Info("Create headers");

    RdKafka::Headers* headers = RdKafka::Headers::create();
    std::vector<std::string> split;
    boost::algorithm::split(split, Headers, boost::is_any_of(";"));

    for (std::string& str : split)
    {
        loger->Debug("Create header: " + str);

        std::vector<std::string> keyValue;
        boost::algorithm::split(keyValue, str, boost::is_any_of(":"));

        if (keyValue.size() == 2)
        {
            std::string key = keyValue[0];
            std::string value = keyValue[1];
            boost::algorithm::trim_all(key);
            boost::algorithm::trim_all(value);
            headers->add(key, value);
        }
    }

    if (!headers->size())
    {
        delete headers;
        return NULL;        
    }
        
    return headers;
}

/////////////////////////////////////////////////////////////////////////////
// Consumer

bool RdKafka1C::InitConsumer(std::string Brokers, std::string Topic, std::string GroupId)
{
    loger->Info("Consumer initialization");
    
    if (consumer)
    {
        errorDescription = "Failed to create consumer: consumer has been initialized already";
        loger->Error(errorDescription);
        return false;
    }
    
    // Create config
    config->ClearProperties();
    config->AddProperty("group.id", GroupId);
    config->AddProperty("metadata.broker.list", Brokers);

    if (!config->Build(errorDescription))
        return false;

    // Create consumer using accumulated global configuration
    consumer = RdKafka::KafkaConsumer::create(config->Get(), errorDescription);
    if (!consumer)
    {
        loger->Error("Failed to create consumer: " + errorDescription);
        return false;
    }

    // Subscribe on topic
    loger->Info("Subscribe to topic " + Topic);

    std::vector<std::string> topics;
    topics.push_back(Topic);

    RdKafka::ErrorCode errorCode = consumer->subscribe(topics);
    if (errorCode) 
    {
        errorDescription = RdKafka::err2str(errorCode);
        loger->Error("Failed to subscribe to topic " + Topic + ": " + errorDescription);
        return false;
    }

    loger->Info("Consumer created successful: " + consumer->name());

    return true;
}

bool RdKafka1C::Consume()
{
    loger->Info("Consume message");

    if (!consumer)
    {
        errorDescription = "Consumer has not been initialized";
        loger->Error("Failed to consume message: " + errorDescription);
        return false;
    }

    // Delete message readed before
    if (message)
        delete message;

    // Get new message
    message = consumer->consume(OperationTimeout);
    
    RdKafka::ErrorCode errorCode = message->err();
    if (errorCode == RdKafka::ERR__TIMED_OUT || errorCode == RdKafka::ERR__PARTITION_EOF)
        return false;   // No one messages

    if (errorCode)
    {
        errorDescription = message->errstr();
        loger->Error("Consume failed: " + errorDescription);
        return false;
    }

    return true;
}

std::string RdKafka1C::GetMessageData()
{
    loger->Info("Get message data");

    if (!message)
    {
        errorDescription = "There are no messages";
        loger->Error("Failed to read message: " + errorDescription);
        return "";
    }

    if (!message->payload())
        return "";

    const char* cstr = static_cast<const char*> (message->payload());

    return std::string(cstr);
}

std::string RdKafka1C::GetMessageMetadata()
{
    loger->Info("Get message metadata");

    if (!message)
    {
        errorDescription = "There are no messages";
        loger->Error("Failed to read message: " + errorDescription);
        return "";
    }

    // Fill tree
    boost::property_tree::ptree tree;
    
    tree.put("broker", (int) message->broker_id());
    tree.put("topic", message->topic_name());
    tree.put("partition", (int) message->partition());
    tree.put("offset", message->offset());
    tree.put("key", message->key());
    tree.put("length", (unsigned int) message->len());
    tree.put("status", message->status());
    tree.put("timestamp", message->timestamp().timestamp);
    tree.put("error_code", (int) message->err());
    tree.put("error_description", message->errstr());
    
    // Fill headers
    boost::property_tree::ptree treeHeaders;
    
    if (message->headers())
    {
        std::vector<RdKafka::Headers::Header> headers = message->headers()->get_all();
        for (size_t i = 0; i < headers.size(); i++) 
            treeHeaders.put(headers[i].key(), headers[i].value());
    }

    tree.put_child("headers", treeHeaders);
    
    // Serialize to string
    std::stringstream stream;
    boost::property_tree::write_json(stream, tree, true);

    return stream.str();
}

bool RdKafka1C::StopConsumer()
{
    loger->Info("Consumer stop");

    if (!consumer)
        return false;

    delete message;
    message = nullptr;

    RdKafka::ErrorCode errorCode = consumer->unsubscribe();
    if (errorCode) 
    {
        errorDescription = RdKafka::err2str(errorCode);
        loger->Error("Failed to unsubscribe from all topics: " + errorDescription);
        return false;
    }

    consumer->close();

    delete consumer;
    consumer = nullptr;

    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Loger

bool RdKafka1C::InitLoger(std::string Directory)
{
    return InitLoger(Directory, Loger::Levels::DEBUG);
}

bool RdKafka1C::InitLoger(std::string Directory, Loger::Levels Level)
{
    SetLogerLevel(Level);

    bool result = loger->Init(Directory, errorDescription);
    if (result)
        loger->Info("Start loging");

    return result;
}

void RdKafka1C::SetLogerLevel(Loger::Levels Level)
{
    loger->level = Level;
}

Loger::Levels RdKafka1C::GetLogerLevel()
{
    return loger->level;
}

std::string RdKafka1C::GetCurrentLogFile()
{
    return loger->GetLogFile();
}

