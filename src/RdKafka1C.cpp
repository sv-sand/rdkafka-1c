#include "RdKafka1C.h"

RdKafka1C::RdKafka1C()
{
    error = false;
    errorDescription = "";
    
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

bool RdKafka1C::Error()
{
    return error;
}

std::string RdKafka1C::ErrorDescription()
{
    return errorDescription;
}

void RdKafka1C::SetError(std::string Description)
{
    error = true;
    errorDescription = Description;

    loger->Error(errorDescription);
}

void RdKafka1C::ClearErrors()
{
    error = false;
    errorDescription = "";
}

void RdKafka1C::SetConfigProperty(std::string Name, std::string Value)
{
    config->AddProperty(Name, Value);
}

/////////////////////////////////////////////////////////////////////////////
// Producer

bool RdKafka1C::InitProducer(std::string Brokers)
{
    loger->Info("Producer initialization");
    ClearErrors();

    if (producer)
    {
        SetError("Failed to create producer: producer has been initialized already");
        return false;
    }

    loger->Debug("Prepare config");
    config->AddProperty("bootstrap.servers", Brokers);
    if (!config->Build(Config::Purposes::PRODUCE, errorDescription))
    {
        SetError("Failed to build config: " + errorDescription);
        return false;
    }

    loger->Debug("Create producer");
    producer = RdKafka::Producer::create(config->GetConf(), errorDescription);
	if (!producer) 
    {
        SetError("Failed to create producer: " + errorDescription);
        return false;
	}

    loger->Info("Producer created successful: " + producer->name());

    return true;
}

bool RdKafka1C::StopProduser()
{
    loger->Info("Producer stop");
    ClearErrors();
    
    if (!producer)
        return true;
        
    loger->Debug("Flush messages"); 
    RdKafka::ErrorCode errorCode = producer->flush(OperationTimeout);
    if (errorCode)
        SetError("Failed to flush producer: " + RdKafka::err2str(errorCode));
    
    loger->Debug("Delete producer");
    delete_pointer(producer);

    return true;
}

bool RdKafka1C::Produce(std::string Topic, std::string Message, std::string Key, std::string Headers, int partition)
{
    loger->Info("Produce message");
    ClearErrors();

    if (!producer)
    {
        SetError("Failed to produce message: producer has been not initialized");
        return false;
    }

    if (producer->outq_len() > 0)
        loger->Warn("Topic " + Topic + " has " + std::to_string(producer->outq_len()) + " not delivered message(s)");

    loger->Debug("Message uuid generation");
    std::string uuid = Strings::NewUuid();
    
    loger->Debug("Clear message statuses");
    deliveryReport->ClearStatuses();
    
    loger->Info("Create headers");
    RdKafka::Headers* headers = RdKafka::Headers::create();
    if (!FillHeaders(headers, Headers))
        return false;

    if (partition < 0)
        partition = RdKafka::Topic::PARTITION_UA;
    
    loger->Debug("Define data for transmit");
    const char* key = Key.c_str();
    size_t keySize = strlen(key) + 1;
    
    const char* payload = Message.c_str();
    size_t payloadSize = strlen(payload) + 1;
    loger->Debug("Message payload (" + std::to_string(Message.length()) + "): " + Message);

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

        SetError("Failed to produce in topic: " + RdKafka::err2str(errorCode));
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
        SetError("Failed to flush producer: " + RdKafka::err2str(errorCode));

    loger->Debug("Producer queue len " + std::to_string(producer->outq_len()));
    if (producer->outq_len() > 0)
        loger->Warn("After flush topic has " + std::to_string(producer->outq_len()) + " not delivered message(s)");
    
    loger->Debug("Check message delivery status");
    RdKafka::Message::Status status = deliveryReport->GetStatus(uuid);
    loger->Debug("Message status = " + MessageStatusToString(status));
    if (status != RdKafka::Message::MSG_STATUS_PERSISTED)
    {
        SetError("Failed to delivery message with status " + MessageStatusToString(status));
        return false;
    }

    loger->Info("Message persisted");

    return true;
}

bool RdKafka1C::StartProduceAsynch()
{
    loger->Info("Start asynch produce message");
    ClearErrors();

    if (!producer)
    {
        SetError("Failed to start asynch produce message: producer has been not initialized");
        return false;
    }

    loger->Debug("Clear message statuses");
    deliveryReport->ClearStatuses();

    return true;
}

bool RdKafka1C::ProduceAsynch(std::string Topic, std::string Message, std::string Key, std::string Headers, int partition)
{
    loger->Info("Produce message (asynch)");
    ClearErrors();

    if (!producer)
    {
        SetError("Failed to produce message: producer has been not initialized");
        return false;
    }

    loger->Info("Create headers");
    RdKafka::Headers* headers = RdKafka::Headers::create();
    if (!FillHeaders(headers, Headers))
        return false;

    if (partition < 0)
        partition = RdKafka::Topic::PARTITION_UA;

    loger->Debug("Define data for transmit");
    const char* key = Key.c_str();    
    size_t keySize = strlen(key) + 1;
    
    const char* payload = Message.c_str();
    size_t payloadSize = strlen(payload) + 1;
    loger->Debug("Message payload (" + std::to_string(Message.length()) + "): " + Message);

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

        SetError("Failed to produce in topic: " + RdKafka::err2str(errorCode));
        return false;
    }

    loger->Debug("Message enqueued");

    return true;
}

bool RdKafka1C::Flush()
{
    loger->Info("Flush messages");
    ClearErrors();

    if (!producer)
    {
        SetError("Failed to flush messages: producer has been not initialized");
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
        SetError("Failed to flush messages (" + std::to_string(flushedMessages) + " messages was flushed): " + RdKafka::err2str(errorCode));
        return false;
    }
    
    if (producer->outq_len() > 0)
    {
        SetError("After flush topic has " + std::to_string(producer->outq_len()) + " not delivered message(s)");
        return false;
    }

    int countUndelivered = deliveryReport->CountUndelivered();
    if (countUndelivered)
    {
        SetError("Failed to delivery " + std::to_string(countUndelivered) + " message(s)");
        return false;
    }

    loger->Debug("Clear message statuses");
    deliveryReport->ClearStatuses();

    return true;
}

bool RdKafka1C::FillHeaders(RdKafka::Headers* Headers, std::string HeadersString)
{
    loger->Info("Fill headers");
    if (HeadersString.empty())
    {
        loger->Info("Headers are empty");
        return true;
    }

    loger->Debug("Split headers string");
    std::multimap map = Strings::SplitString(HeadersString, ";", ":");
    if (map.size() == 0)
    {
        SetError("Can't parse string headers " + HeadersString);
        return false;
    }

    loger->Debug("Fill headers collection");
    for (auto& [key, value] : map)
    {
        loger->Debug("Create header " + key + ":" + value);
        Headers->add(key, value);
    }
                
    return true;
}

int RdKafka1C::ProducerQueueLen()
{
    loger->Info("Getting producer queue len");
    ClearErrors();

    return producer->outq_len();
}

/////////////////////////////////////////////////////////////////////////////
// Consumer

bool RdKafka1C::InitConsumer(std::string Brokers, std::string GroupId)
{
    loger->Info("Consumer initialization");
    ClearErrors();

    if (consumer)
    {
        SetError("Failed to create consumer: consumer has been initialized already");
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
        SetError("Failed to create consumer: " + errorDescription);
        return false;
    }

    loger->Info("Consumer created successful: " + consumer->name());

    return true;
}

bool RdKafka1C::StopConsumer()
{
    loger->Info("Consumer stop");
    ClearErrors();
    
    if (!consumer)
        return true;

    loger->Debug("Delete last message"); 
    if (message)
        delete_pointer(message);

    loger->Debug("Close consumer"); 
    consumer->close();

    loger->Debug("Delete consumer"); 
    delete_pointer(consumer);
    
    return true;
}

bool RdKafka1C::Consume()
{
    loger->Info("Consume message");
    ClearErrors();
    
    if (!consumer)
    {
        SetError("Failed to consume message: Consumer has been not initialized");
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
            SetError("Consume failed: " + message->errstr());
            return false;
    }

    return true;
}

std::string RdKafka1C::GetMessageData()
{
    loger->Info("Get message data");
    ClearErrors();

    if (!message)
    {
        SetError("Failed to read message: there are no messages");
        return "";
    }

    loger->Debug("Get payload");
    if (!message->payload())
        return "";

    loger->Debug("Convert to char");
    const char* cstr = static_cast<const char*> (message->payload());

    loger->Debug("Convert to string");
    std::string result = std::string(cstr, message->len());

    return result;
}

std::string RdKafka1C::GetMessageMetadata()
{
    loger->Info("Get message metadata");
    ClearErrors();

    if (!message)
    {
        SetError("Failed to read message: there are no messages");
        return "";
    }

    loger->Debug("Get key");
    std::string key;
    if (message->key())
    {
        std::string keyString = *message->key();
        key = std::string(keyString.c_str());
    }

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
        SetError("Failed to serialize message metadata: " + e.message());
        return "";
    }

    return stream.str();
}

bool RdKafka1C::AssignPartition(std::string Topic, int Partition)
{
    loger->Info("Assign partition");
    ClearErrors();

    if (!consumer)
    {
        SetError("Failed to assign partition: consumer has been not initialized");
        return false;
    }

    loger->Debug("Create topic partition pair");
    RdKafka::TopicPartition* partition = RdKafka::TopicPartition::create(Topic, Partition);
    std::vector<RdKafka::TopicPartition*> partitions;
    partitions.push_back(partition);

    RdKafka::ErrorCode errorCode = consumer->assign(partitions);
    delete_pointer(partition);

    if (errorCode != RdKafka::ERR_NO_ERROR)
    {
        SetError("Failed to assign partition: " + RdKafka::err2str(errorCode));
        return false;
    }

    return true;
}

int64_t RdKafka1C::CommittedOffset(std::string Topic, int Partition)
{
    loger->Info("Get commited offsets for topic " + Topic + " partition " + std::to_string(Partition));
    ClearErrors();

    if (!consumer)
    {
        SetError("Failed to get offset: consumer has been not initialized");
        return -1;
    }

    loger->Debug("Create topic partition pair");
    RdKafka::TopicPartition* partition = RdKafka::TopicPartition::create(Topic, Partition);
    std::vector<RdKafka::TopicPartition*> partitions;
    partitions.push_back(partition);
    
    RdKafka::ErrorCode errorCode = consumer->committed(partitions, OperationTimeout);
    
    int64_t offset = partition->offset();
    delete_pointer(partition);

    if (errorCode != RdKafka::ERR_NO_ERROR)
    {
        SetError("Failed to get offset: " + RdKafka::err2str(errorCode));
        return -1;
    }

    return offset;
}

bool RdKafka1C::CommitOffset(std::string Topic, int Partition, int64_t Offset)
{
    loger->Info("Commit offset");
    ClearErrors();

    if (!consumer)
    {
        SetError("Failed to commit offset: consumer has been not initialized");
        return false;
    }

    loger->Debug("Create topic partition pair");
    RdKafka::TopicPartition *partition = RdKafka::TopicPartition::create(Topic, Partition, Offset);
    std::vector<RdKafka::TopicPartition *> partitions;
    partitions.push_back(partition);

    RdKafka::ErrorCode errorCode = consumer->commitSync(partitions);
    delete_pointer(partition);

    if (errorCode != RdKafka::ERR_NO_ERROR)
    {
        SetError("Failed to commit offset: " + RdKafka::err2str(errorCode));
        return false;
    }

    return true;
}

int RdKafka1C::ConsumerQueueLen()
{
    loger->Info("Getting consumer queue len");
    ClearErrors();
    
    return consumer->outq_len();
}

/////////////////////////////////////////////////////////////////////////////
// Subscriptions

std::string RdKafka1C::Subscription()
{
    loger->Info("Get topics subscription");
    ClearErrors();

    std::vector<std::string> topics;
    RdKafka::ErrorCode errorCode = consumer->subscription(topics);
    if (errorCode)
    {
        SetError("Failed to get subscriptions: " + RdKafka::err2str(errorCode));
        return "";
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
    ClearErrors();

    std::vector<std::string> topics;
    topics.push_back(Topic);

    RdKafka::ErrorCode errorCode = consumer->subscribe(topics);
    if (errorCode)
    {
        SetError("Failed to subscribe to topic " + Topic + ": " + RdKafka::err2str(errorCode));
        return false;
    }

    return true;
}

bool RdKafka1C::Unsubscribe()
{
    loger->Info("Unsubscribe from all topics");
    ClearErrors();

    RdKafka::ErrorCode errorCode = consumer->unsubscribe();
    if (errorCode)
    {
        SetError("Failed to unsubscribe from all topics: " + RdKafka::err2str(errorCode));
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Loger

bool RdKafka1C::StartLogging(std::string FileName, Loger::Levels Level)
{
    ClearErrors(); 
    SetLogerLevel(Level);
    
    bool result = loger->Init(FileName, errorDescription);
    if (result)
        loger->Info("Start loging");

    return result;
}

void RdKafka1C::StopLogging()
{
    loger->Info("Stop logging");
    ClearErrors();
    
    loger->level = Loger::Levels::NONE;
}

void RdKafka1C::SetLogerLevel(Loger::Levels Level)
{
    loger->Info("Set loger level");
    ClearErrors();

    loger->level = Level;
}

Loger::Levels RdKafka1C::GetLogerLevel()
{
    loger->Info("Get loger level");
    ClearErrors();

    return loger->level;
}

std::string RdKafka1C::GetCurrentLogFile()
{
    loger->Info("Get current log file");
    ClearErrors();

    return loger->GetLogFile();
}

/////////////////////////////////////////////////////////////////////////////
// Other

std::string RdKafka1C::MessageStatusToString(RdKafka::Message::Status Status)
{
    loger->Info("Message status " + std::to_string(Status) + " to string");
    ClearErrors();
    
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