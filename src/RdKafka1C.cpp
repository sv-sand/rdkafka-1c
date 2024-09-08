#include "RdKafka1C.h"

RdKafka1C::RdKafka1C()
{
    producer = nullptr;
    consumer = nullptr;
    message = nullptr;

    loger = new Loger();        
    deliveryReport = new DeliveryReport(loger);    
    rebalance = new Rebalance(loger);
    event = new Event(loger);
    
    config = new Config(loger);
    config->DeliveryReport = deliveryReport;
    config->Rebalance = rebalance;
    config->Event = event;
}

RdKafka1C::~RdKafka1C()
{
    loger->Info("RdKafka1C dectructor called");

    StopProduser();
    StopConsumer();

    delete_pointer(config);
    delete_pointer(deliveryReport);
    delete_pointer(rebalance);
    delete_pointer(event);
    delete_pointer(loger);
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

bool RdKafka1C::InitProducer(std::string Brokers)
{
    errorDescription = "";
    loger->Info("Producer initialization");
    
    if (producer)
    {
        errorDescription = "Failed to create producer: producer has been initialized already";
        loger->Error(errorDescription);
        return false;
    }

    loger->Debug("Prepare config");
    config->AddProperty("bootstrap.servers", Brokers);
    if (!config->Build(Config::Purposes::PRODUCE, errorDescription))
    {
        loger->Error("Failed to build config: " + errorDescription);
        return false;
    }

    loger->Debug("Create producer");
    producer = RdKafka::Producer::create(config->GetConf(), errorDescription);
	if (!producer) 
    {
        loger->Error("Failed to create producer: " + errorDescription);
        return false;
	}

    loger->Info("Producer created successful: " + producer->name());

    return true;
}

bool RdKafka1C::Produce(std::string Topic, std::string Message, std::string Key, std::string Headers, int partition)
{
    errorDescription = "";
    loger->Info("Produce message");

    if (!producer)
    {
        errorDescription = "Failed to produce message: producer has been not initialized";
        loger->Error(errorDescription);
        return false;
    }

    if (producer->outq_len() > 0)
        loger->Warn("Topic " + Topic + " has " + std::to_string(producer->outq_len()) + " not delivered message(s)");

    loger->Debug("Message uuid generation");
    std::string uuid = Strings::NewUuid();
    
    loger->Debug("Clear message statuses");
    deliveryReport->ClearStatuses();
    
    RdKafka::Headers* headers = RdKafka::Headers::create();
    if (!CreateHeaders(headers, Headers))
        return false;

    if (partition < 0)
        partition = RdKafka::Topic::PARTITION_UA;
    
    loger->Debug("Define data for transmit");
    const char* key = Key.c_str();
    size_t keySize = strlen(key) + 1;
    
    const char* payload = Message.c_str();
    size_t payloadSize = strlen(payload) + 1;
    loger->Debug("Message payload (" + std::to_string(payloadSize) + "): " + payload);

    RdKafka::ErrorCode errorCode;

    for (int attempt = 1; attempt < 10; attempt++)
    {
        loger->Info("Try to produce in topic " + Topic);
        
        /*
         * Send/Produce message.
         * This is an asynchronous call, on success it will only enqueue the message on the internal producer queue.
         * The actual delivery attempts to the broker are handled by background threads.
         * The previously registered delivery report callback is used to signal back to the application when the message
         * has been delivered (or failed permanently after retries).
         */
        
        errorCode = producer->produce(
            Topic, partition, RdKafka::Producer::RK_MSG_COPY,
            const_cast<char*>(payload), payloadSize,
            key, keySize,
            0, headers, (void*) &uuid);

        loger->Debug("Poll kafka for events");
        producer->poll(OperationTimeout);
        
        if (errorCode == RdKafka::ERR__QUEUE_FULL)
        {
            /*
             * If the internal queue is full, wait for messages to be delivered and then retry.
             * The internal queue represents both messages to be sent and messages that have
             * been sent or failed, awaiting their delivery report callback to be called.
             *
             * The internal queue is limited by the configuration property
             * queue.buffering.max.messages and queue.buffering.max.kbytes
             */
            
            loger->Warn("Internal queue is full, " + std::to_string(producer->outq_len()) + " message(s) were not delivered");            
            continue;
        }
        break;
    }

    if (errorCode) 
    {
        // Headers are automatically deleted only when produce was success
        delete_pointer(headers);

        errorDescription = "Failed to produce in topic: " + RdKafka::err2str(errorCode);
        loger->Error(errorDescription);
        return false;
    }

    loger->Debug("Message enqueued");

    /* 
     * Wait for final messages to be delivered or fail. 
     * flush() is an abstraction over poll() which waits for all messages to be delivered. 
     */

    loger->Debug("Flush messages");
    errorCode = producer->flush(OperationTimeout);
    if (errorCode)
    {
        errorDescription = "Failed to flush producer: " + RdKafka::err2str(errorCode);
        loger->Error(errorDescription);
    }

    loger->Debug("Producer queue len " + std::to_string(producer->outq_len()));
    if (producer->outq_len() > 0)
        loger->Warn("After flush topic has " + std::to_string(producer->outq_len()) + " not delivered message(s)");
    
    loger->Debug("Check message delivery status");
    RdKafka::Message::Status status = deliveryReport->GetStatus(uuid);
    loger->Debug("Message status = " + MessageStatusToString(status));
    if (status != RdKafka::Message::MSG_STATUS_PERSISTED)
    {
        errorDescription = "Failed to delivery message with status " + MessageStatusToString(status);
        loger->Error(errorDescription);
        return false;
    }

    loger->Info("Message persisted");

    return true;
}

bool RdKafka1C::StartProduceAsynch()
{
    errorDescription = "";
    loger->Info("Start asynch produce message");

    if (!producer)
    {
        errorDescription = "Failed to start asynch produce message: producer has been not initialized";
        loger->Error(errorDescription);
        return false;
    }

    loger->Debug("Clear message statuses");
    deliveryReport->ClearStatuses();

    return true;
}

bool RdKafka1C::ProduceAsynch(std::string Topic, std::string Message, std::string Key, std::string Headers, int partition)
{
    errorDescription = "";
    loger->Info("Produce message (asynch)");

    if (!producer)
    {
        errorDescription = "Failed to produce message: producer has been not initialized";
        loger->Error(errorDescription);
        return false;
    }

    RdKafka::Headers* headers = RdKafka::Headers::create();
    if (!CreateHeaders(headers, Headers))
        return false;

    if (partition < 0)
        partition = RdKafka::Topic::PARTITION_UA;

    loger->Debug("Define data for transmit");
    const char* key = Key.c_str();    
    size_t keySize = strlen(key) + 1;
    
    const char* payload = Message.c_str();
    size_t payloadSize = strlen(payload) + 1;    
    loger->Debug("Message payload (" + std::to_string(payloadSize) + "): " + payload);

    RdKafka::ErrorCode errorCode;

    for (int attempt = 1; attempt < 10; attempt++)
    {
        loger->Info("Try to produce in topic " + Topic);

        /*
         * Send/Produce message.
         * This is an asynchronous call, on success it will only enqueue the message on the internal producer queue.
         * The actual delivery attempts to the broker are handled by background threads.
         * The previously registered delivery report callback is used to signal back to the application when the message
         * has been delivered (or failed permanently after retries).
         */

        errorCode = producer->produce(
            Topic, partition, RdKafka::Producer::RK_MSG_COPY,
            const_cast<char*>(payload), payloadSize,
            key, keySize,
            0, headers, nullptr);

        if (errorCode == RdKafka::ERR__QUEUE_FULL)
        {
            /*
             * If the internal queue is full, wait for messages to be delivered and then retry.
             * The internal queue represents both messages to be sent and messages that have
             * been sent or failed, awaiting their delivery report callback to be called.
             *
             * The internal queue is limited by the configuration property
             * queue.buffering.max.messages and queue.buffering.max.kbytes
             */

            loger->Warn("Internal queue is full, " + std::to_string(producer->outq_len()) + " message(s) were not delivered");
            
            loger->Debug("Poll kafka for events"); 
            producer->poll(OperationTimeout);
            continue;
        }
        break;
    }

    if (errorCode)
    {
        // Headers are automatically deleted only when produce was success
        delete_pointer(headers);

        errorDescription = "Failed to produce in topic: " + RdKafka::err2str(errorCode);
        loger->Error(errorDescription);
        return false;
    }

    loger->Debug("Message enqueued");

    return true;
}

bool RdKafka1C::Flush()
{
    errorDescription = "";
    loger->Info("Flush messages");

    if (!producer)
    {
        errorDescription = "Failed to flush messages: producer has been not initialized";
        loger->Error(errorDescription);
        return false;
    }

    int lenBeforeFlush = producer->outq_len();
    loger->Debug("Out queue len before flush: " + std::to_string(lenBeforeFlush));

    if (!lenBeforeFlush)
    {
        loger->Debug("Current out queue is empty");
        return true;
    }
    
    RdKafka::ErrorCode errorCode = producer->flush(OperationTimeout);
    
    loger->Debug("Out queue len after flush: " + std::to_string(producer->outq_len()));
    int flushedMessages = producer->outq_len() - lenBeforeFlush;
    
    if (errorCode)
    {
        errorDescription = "Failed to flush messages (" + std::to_string(flushedMessages) + " messages was flushed): " + RdKafka::err2str(errorCode);
        loger->Error(errorDescription);
        return false;
    }
    
    if (producer->outq_len() > 0)
    {
        errorDescription = "After flush topic has " + std::to_string(producer->outq_len()) + " not delivered message(s)";
        loger->Error(errorDescription);
        return false;
    }

    int countUndelivered = deliveryReport->CountUndelivered();
    if (countUndelivered)
    {
        errorDescription = "Failed to delivery " + std::to_string(countUndelivered) + " message(s)";
        loger->Error(errorDescription);
        return false;
    }

    loger->Debug("Clear message statuses");
    deliveryReport->ClearStatuses();

    return true;
}

bool RdKafka1C::StopProduser()
{
    errorDescription = "";
    if (!producer)
        return true;

    loger->Info("Producer stop"); 
    
    loger->Debug("Flush messages"); 
    RdKafka::ErrorCode errorCode = producer->flush(OperationTimeout);
    if (errorCode)
    {
        errorDescription = "Failed to flush producer: " + RdKafka::err2str(errorCode);
        loger->Error(errorDescription);
    }
    
    loger->Debug("Delete producer");
    delete_pointer(producer);

    return true;
}

bool RdKafka1C::CreateHeaders(RdKafka::Headers* Headers, std::string HeadersString)
{
    if (HeadersString.empty())
        return true;

    loger->Info("Create headers");

    std::multimap map = Strings::SplitString(HeadersString, ";", ":");
    if (map.size() == 0)
    {
        errorDescription = "Can't parse string headers " + HeadersString;
        loger->Error(errorDescription);
        return false;
    }

    loger->Debug("Create headers collection");
    //headers = RdKafka::Headers::create();
    for (auto& [key, value] : map)
    {
        loger->Debug("Create header " + key + ":" + value);
        Headers->add(key, value);
    }
                
    return true;
}

int RdKafka1C::ProducerQueueLen()
{
    return producer->outq_len();
}

/////////////////////////////////////////////////////////////////////////////
// Consumer

bool RdKafka1C::InitConsumer(std::string Brokers, std::string GroupId)
{
    errorDescription = "";
    loger->Info("Consumer initialization");
    
    if (consumer)
    {
        errorDescription = "Failed to create consumer: consumer has been initialized already";
        loger->Error(errorDescription);
        return false;
    }
    
    loger->Debug("Prepare config");
    config->AddProperty("metadata.broker.list", Brokers);
    config->AddProperty("group.id", GroupId);
    
    if (!config->Build(Config::Purposes::CONSUME, errorDescription))
        return false;

    loger->Debug("Create consumer");
    consumer = RdKafka::KafkaConsumer::create(config->GetConf(), errorDescription);
    if (!consumer)
    {
        loger->Error("Failed to create consumer: " + errorDescription);
        return false;
    }

    loger->Info("Consumer created successful: " + consumer->name());

    return true;
}

bool RdKafka1C::Consume()
{
    errorDescription = "";
    loger->Info("Consume message");

    if (!consumer)
    {
        errorDescription = "Consumer has been not initialized";
        loger->Error("Failed to consume message: " + errorDescription);
        return false;
    }

    loger->Debug("Delete message readed before");
    delete_pointer(message);

    loger->Debug("Consume new message");
    message = consumer->consume(OperationTimeout);
    
    RdKafka::ErrorCode errorCode = message->err();
    switch(errorCode)
    {
        case RdKafka::ERR_NO_ERROR:
            loger->Debug("Message consumed successful");
            break;

        case RdKafka::ERR__TIMED_OUT: 
            loger->Debug("Consume message timed out: " + message->errstr());
            return false;

        case RdKafka::ERR__PARTITION_EOF: 
            loger->Debug("Consume message: " + message->errstr());
            return false;

        default:    
            errorDescription = message->errstr();
            loger->Error("Consume failed: " + errorDescription);
            return false;
    }

    return true;
}

std::string RdKafka1C::GetMessageData()
{
    errorDescription = "";
    loger->Info("Get message data");

    if (!message)
    {
        errorDescription = "There are no messages";
        loger->Error("Failed to read message: " + errorDescription);
        return "";
    }

    loger->Debug("Get payload");
    if (!message->payload())
        return "";

    loger->Debug("Convert to char");
    const char* cstr = static_cast<const char*> (message->payload());

    loger->Debug("Convert to string");
    std::string result = std::string(cstr);

    return result;
}

std::string RdKafka1C::GetMessageMetadata()
{
    errorDescription = "";
    loger->Info("Get message metadata");

    if (!message)
    {
        errorDescription = "There are no messages";
        loger->Error("Failed to read message: " + errorDescription);
        return "";
    }

    loger->Debug("Get key");
    std::string key;
    if (message->key())
        key = std::string(*message->key());

    loger->Debug("Get headers");
    boost::property_tree::ptree treeHeaders;    
    if (message->headers())
        for (auto header : message->headers()->get_all())
            treeHeaders.put(header.key(), std::string(header.value_string(), header.value_size()));

    loger->Debug("Fill JSON tree");
    boost::property_tree::ptree tree;

    loger->Debug("Get status");
    std::string status = MessageStatusToString(message->status());
    
    tree.put("broker", message->broker_id());
    tree.put("topic", message->topic_name());
    tree.put("partition", message->partition());
    tree.put("offset", message->offset());
    tree.put("key", key);
    tree.put("length", (unsigned int) message->len());
    tree.put("status", status);
    tree.put("timestamp", Strings::ToString(message->timestamp().timestamp));
    tree.put("error_code", message->err());
    tree.put("error_description", message->errstr());    
    tree.put_child("headers", treeHeaders);
    
    loger->Debug("Serialize tree to JSON");
    std::stringstream stream;
    try
    {
        boost::property_tree::write_json(stream, tree, true);
    }
    catch(boost::property_tree::json_parser_error e)
    {
        errorDescription = e.message();
        loger->Error("Failed to serialize message metadata: " + errorDescription);
        return "";
    }

    return stream.str();
}

bool RdKafka1C::CommitOffset(std::string Topic, int Partition, int64_t Offset)
{
    errorDescription = "";
    loger->Info("Commit offset");
    
    if (!consumer)
    {
        errorDescription = "Consumer has been not initialized";
        loger->Error("Failed to commit offset: " + errorDescription);
        return false;
    }

    RdKafka::TopicPartition *offset = RdKafka::TopicPartition::create(Topic, Partition, Offset);
    std::vector<RdKafka::TopicPartition *> offsets;
    offsets.push_back(offset);

    RdKafka::ErrorCode errorCode = consumer->commitSync(offsets);
    delete_pointer(offset);

    if (errorCode != RdKafka::ERR_NO_ERROR)
    {
        errorDescription = RdKafka::err2str(errorCode);
        loger->Error("Failed to commit offset: " + errorDescription);
        return false;
    }

    return true;
}

bool RdKafka1C::StopConsumer()
{
    errorDescription = "";    
    if (!consumer)
        return true;

    loger->Info("Consumer stop");

    loger->Debug("Delete last message"); 
    if (message)
        delete_pointer(message);

    loger->Debug("Close consumer"); 
    consumer->close();

    loger->Debug("Delete consumer"); 
    delete_pointer(consumer);
    
    return true;
}

int RdKafka1C::ConsumerQueueLen()
{
    return consumer->outq_len();
}

/////////////////////////////////////////////////////////////////////////////
// Subscriptions

std::string RdKafka1C::Subscription()
{
    loger->Info("Get topics subscription");

    std::vector<std::string> topics;
    RdKafka::ErrorCode errorCode = consumer->subscription(topics);
    if (errorCode)
    {
        errorDescription = RdKafka::err2str(errorCode);
        loger->Error("Failed to get subscriptions: " + errorDescription);
        return false;
    }

    loger->Debug("Print topics to string");
    std::stringstream stream;
    for (int i = 0; i < topics.size(); i++)
        stream << topics[i] << ";";

    return stream.str();
}

bool RdKafka1C::Subscribe(std::string Topic)
{
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

    return true;
}

bool RdKafka1C::Unsubscribe()
{
    loger->Info("Unsubscribe from all topics");

    RdKafka::ErrorCode errorCode = consumer->unsubscribe();
    if (errorCode)
    {
        errorDescription = RdKafka::err2str(errorCode);
        loger->Error("Failed to unsubscribe from all topics: " + errorDescription);
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Loger

bool RdKafka1C::StartLogging(std::string FileName)
{
    errorDescription = "";
    
    bool result = loger->Init(FileName, errorDescription);
    if (result)
        loger->Info("Start loging");

    return result;
}

bool RdKafka1C::StartLogging(std::string FileName, Loger::Levels Level)
{
    SetLogerLevel(Level);
    return StartLogging(FileName);
}

void RdKafka1C::StopLogging()
{
    loger->level = Loger::Levels::NONE;
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

/////////////////////////////////////////////////////////////////////////////
// Other

std::string RdKafka1C::MessageStatusToString(RdKafka::Message::Status Status)
{
    std::string status;
    switch (Status)
    {
    case RdKafka::Message::MSG_STATUS_NOT_PERSISTED:
        status = "NOT_PERSISTED";
        break;
    case RdKafka::Message::MSG_STATUS_POSSIBLY_PERSISTED:
        status = "POSSIBLY_PERSISTED";
        break;
    case RdKafka::Message::MSG_STATUS_PERSISTED:
        status = "PERSISTED";
        break;
    default:
        status = "UNKNOWN";
        break;
    }
    return status;
}