#include "RdKafka1C.h"

RdKafka1C::RdKafka1C(Loger* Loger, ErrorHandler* Error) {
    loger = Loger;
    error = Error;
    
    producer = nullptr;
    consumer = nullptr;
    message = nullptr;

    config = new ConfigBuilder(loger, error);
}

RdKafka1C::~RdKafka1C() {
    loger->Info("RdKafka1C dectructor called");

    StopProduser();
    StopConsumer();

    delete_pointer(config);
}

std::string RdKafka1C::RdKafkaVersion() {
    return RdKafka::version_str();
}

void RdKafka1C::SetConfigProperty(std::string Name, std::string Value) {
    config->AddProperty(Name, Value);
}

/////////////////////////////////////////////////////////////////////////////
// Producer

bool RdKafka1C::InitProducer() {
    loger->Info("Producer initialization");
    error->Clear();

    if (producer) {
        error->Set("Failed to create producer: producer has been initialized already");
        return false;
    }

    loger->Debug("Build config");
    if (!config->BuildProducerConfig())
        return false;

    loger->Debug("Create producer");
    std::string errorDescription;
    producer = RdKafka::Producer::create(config->GetConf(), errorDescription);
	if (!producer) {
        error->Set("Failed to create producer: " + errorDescription);
        return false;
	}

    loger->Info("Producer created successful: " + producer->name());

    return true;
}

bool RdKafka1C::StopProduser() {
    if (!producer)
        return true;
        
    loger->Info("Producer stop");
    error->Clear();
    
    loger->Debug("Delete producer");
    delete_pointer(producer);

    loger->Info("Producer stopped");

    return true;
}

bool RdKafka1C::StartProduce() {
    loger->Info("Start produce message");
    error->Clear();

    if (!producer) {
        error->Set("Failed to start produce message: producer has been not initialized");
        return false;
    }

    loger->Debug("Clear message statuses");
    config->GetDeliveryReport()->ClearStatuses();

    return true;
}

bool RdKafka1C::Produce(std::string Topic, std::string Message, std::string Key, std::string Headers, int Partition, std::string MessageId) {
    loger->Info("Produce message");
    error->Clear();

    if (!producer) {
        error->Set("Failed to produce message: producer has been not initialized");
        return false;
    }

    loger->Info("Create headers");
    RdKafka::Headers* headers = RdKafka::Headers::create();
    if (!SetHeaders(headers, Headers))
        return false;

    if (Partition < 0)
        Partition = RdKafka::Topic::PARTITION_UA;

    loger->Debug("Prepare data for transmit");
    const char* key = Key.c_str();    
    size_t keySize = strlen(key);
    loger->Debug("Message key (" + std::to_string(Key.length()) + "): " + Key);
    
    const char* payload = Message.c_str();
    size_t payloadSize = strlen(payload);
    loger->Debug("Message payload (" + std::to_string(Message.length()) + "): " + Message);

    const std::string* opaque = config->GetDeliveryReport()->AddEmptyStatus(MessageId);
    loger->Debug("Message id: " + MessageId);

    RdKafka::ErrorCode errorCode;

    for (int attempt = 1; attempt < 10; attempt++) {
        loger->Info("Try to produce in topic " + Topic);

        /*
         * Send/Produce message.
         * This is an asynchronous call, on success it will only enqueue the message on the internal producer queue.
         * The actual delivery attempts to the broker are handled by background threads.
         * The previously registered delivery report callback is used to signal back to the application when the message
         * has been delivered (or failed permanently after retries).
         */

        errorCode = producer->produce(
            Topic, Partition, RdKafka::Producer::RK_MSG_COPY,
            const_cast<char*>(payload), payloadSize,
            key, keySize,
            0, headers, (void*)opaque);

        if (errorCode == RdKafka::ERR__QUEUE_FULL) {
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

    if (errorCode) {
        // Headers are automatically deleted only when produce was success
        delete_pointer(headers);
        error->Set("Failed to produce in topic: " + RdKafka::err2str(errorCode));
        return false;
    }

    loger->Debug("Message enqueued");

    return true;
}

bool RdKafka1C::Flush() {
    loger->Info("Flush messages");
    error->Clear();

    if (!producer) {
        error->Set("Failed to flush messages: producer has been not initialized");
        return false;
    }

    RdKafka::ErrorCode errorCode = producer->flush(OperationTimeout);
    if (errorCode) {
        error->Set("Failed to flush messages: " + RdKafka::err2str(errorCode));
        return false;
    }
    
    return true;
}

bool RdKafka1C::SetHeaders(RdKafka::Headers* Headers, std::string HeadersString) {
    loger->Info("Fill headers");
    if (HeadersString.empty()) {
        loger->Info("Headers are empty");
        return true;
    }

    loger->Debug("Split headers string");
    std::multimap map = Strings::SplitString(HeadersString, ";", ":");
    if (map.size() == 0) {
        error->Set("Can't parse string headers " + HeadersString);
        return false;
    }

    loger->Debug("Fill headers collection");
    for (auto& [key, value] : map) {
        loger->Debug("Create header " + key + ":" + value);
        Headers->add(key, value);
    }
                
    return true;
}

int RdKafka1C::ProducerQueueLen() {
    loger->Info("Getting producer queue len");
    error->Clear();

    return producer->outq_len();
}

int RdKafka1C::CountUndeliveredMessages() {
    loger->Info("Getting count undelivered messages");
    error->Clear();

    return config->GetDeliveryReport()->CountUndelivered();
}

std::string RdKafka1C::MessageStatus(std::string MessageId) {
    loger->Info("Getting message status");
    error->Clear();

    RdKafka::Message::Status Status = config->GetDeliveryReport()->GetStatus(MessageId);

    return MessageStatusToString(Status);
}

/////////////////////////////////////////////////////////////////////////////
// Consumer

bool RdKafka1C::InitConsumer() {
    loger->Info("Consumer initialization");
    error->Clear();

    if (consumer) {
        error->Set("Failed to create consumer: consumer has been initialized already");
        return false;
    }
    
    loger->Debug("Build config");    
    if (!config->BuildConsumerConfig())
        return false;

    loger->Debug("Create consumer");
    std::string errorDescription;
    consumer = RdKafka::KafkaConsumer::create(config->GetConf(), errorDescription);
    if (!consumer) {
        error->Set("Failed to create consumer: " + errorDescription);
        return false;
    }

    loger->Info("Consumer created successful: " + consumer->name());

    return true;
}

bool RdKafka1C::StopConsumer() {
    if (!consumer)
        return true;

    loger->Info("Consumer stop");
    error->Clear();
    
    loger->Debug("Delete last message"); 
    if (message)
        delete_pointer(message);

    loger->Debug("Close consumer"); 
    consumer->close();

    loger->Debug("Delete consumer"); 
    delete_pointer(consumer);

    loger->Info("Consumer stopped");
    
    return true;
}

bool RdKafka1C::Consume() {
    loger->Info("Consume message");
    error->Clear();
    
    if (!consumer) {
        error->Set("Failed to consume message: Consumer has been not initialized");
        return false;
    }

    loger->Debug("Delete message readed before");
    delete_pointer(message);

    loger->Debug("Consume new message");
    message = consumer->consume(OperationTimeout);
    
    RdKafka::ErrorCode errorCode = message->err();
    switch(errorCode) {
        case RdKafka::ERR_NO_ERROR:
            loger->Debug("Message consumed successful");
            break;

        case RdKafka::ERR__TIMED_OUT: 
            loger->Error("Consume message timed out: " + message->errstr());
            return false;

        case RdKafka::ERR__PARTITION_EOF: 
            loger->Debug("Consume message: " + message->errstr());
            return false;

        default:    
            error->Set("Consume failed: " + message->errstr());
            return false;
    }

    return true;
}

std::string RdKafka1C::MessageData() {
    loger->Info("Get message data");
    error->Clear();

    if (!message) {
        error->Set("Failed to read message: there are no messages");
        return "";
    }

    loger->Debug("Get payload");
    if (!message->payload())
        return "";

    loger->Debug("Convert to string");
    
    const char* cstr = static_cast<const char*> (message->payload());
    std::string result = std::string(cstr, message->len());

    return result;
}

std::string RdKafka1C::MessageMetadata() {
    loger->Info("Get message metadata");
    error->Clear();

    if (!message) {
        error->Set("Failed to read message: there are no messages");
        return "";
    }

    loger->Debug("Get key");
    std::string key;
    if (message->key()) {
        std::string keyString = *message->key();
        key = std::string(keyString.c_str());
    }

    loger->Debug("Get headers");
    boost::property_tree::ptree treeHeaders;    
    if (message->headers())
        for (auto header : message->headers()->get_all())
            treeHeaders.put(header.key(), std::string(header.value_string(), header.value_size()));

    loger->Debug("Build property tree");
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
    
    loger->Debug("Serialize property tree to JSON");
    std::stringstream stream;
    try {
        boost::property_tree::write_json(stream, tree, true);
    }
    catch(boost::property_tree::json_parser_error e) {
        error->Set("Failed to serialize message metadata: " + e.message());
        return "";
    }

    return stream.str();
}

bool RdKafka1C::AssignPartition(std::string Topic, int Partition) {
    loger->Info("Assign partition");
    error->Clear();

    if (!consumer) {
        error->Set("Failed to assign partition: consumer has been not initialized");
        return false;
    }

    loger->Debug("Create topic partition pair");
    RdKafka::TopicPartition* partition = RdKafka::TopicPartition::create(Topic, Partition);
    std::vector<RdKafka::TopicPartition*> partitions;
    partitions.push_back(partition);

    RdKafka::ErrorCode errorCode = consumer->assign(partitions);
    delete_pointer(partition);

    if (errorCode != RdKafka::ERR_NO_ERROR) {
        error->Set("Failed to assign partition: " + RdKafka::err2str(errorCode));
        return false;
    }

    return true;
}

int64_t RdKafka1C::CommittedOffset(std::string Topic, int Partition) {
    loger->Info("Get commited offsets for topic " + Topic + " partition " + std::to_string(Partition));
    error->Clear();

    if (!consumer) {
        error->Set("Failed to get offset: consumer has been not initialized");
        return -1;
    }

    loger->Debug("Create topic partition pair");
    RdKafka::TopicPartition* partition = RdKafka::TopicPartition::create(Topic, Partition);
    std::vector<RdKafka::TopicPartition*> partitions;
    partitions.push_back(partition);
    
    RdKafka::ErrorCode errorCode = consumer->committed(partitions, OperationTimeout);
    
    int64_t offset = partition->offset();
    delete_pointer(partition);

    if (errorCode != RdKafka::ERR_NO_ERROR) {
        error->Set("Failed to get offset: " + RdKafka::err2str(errorCode));
        return -1;
    }

    return offset;
}

bool RdKafka1C::ChangeOffset(std::string Topic, int Partition, int64_t Offset) {
    loger->Info("Change offset");
    error->Clear();

    if (!consumer) {
        error->Set("Failed to change offset: consumer has been not initialized");
        return false;
    }

    loger->Debug("Create topic partition pair");
    RdKafka::TopicPartition *partition = RdKafka::TopicPartition::create(Topic, Partition, Offset);
    std::vector<RdKafka::TopicPartition *> partitions;
    partitions.push_back(partition);

    RdKafka::ErrorCode errorCode = consumer->commitSync(partitions);
    delete_pointer(partition);

    if (errorCode != RdKafka::ERR_NO_ERROR) {
        error->Set("Failed to change offset: " + RdKafka::err2str(errorCode));
        return false;
    }

    return true;
}

bool RdKafka1C::CommitOffset() {
    loger->Info("Commit offset");
    error->Clear();

    if (!consumer) {
        error->Set("Failed to commit offset: consumer has been not initialized");
        return false;
    }

    RdKafka::ErrorCode errorCode = consumer->commitSync();
    
    if (errorCode != RdKafka::ERR_NO_ERROR) {
        error->Set("Failed to commit offset: " + RdKafka::err2str(errorCode));
        return false;
    }

    return true;
}

int RdKafka1C::ConsumerQueueLen() {
    loger->Info("Getting consumer queue len");
    error->Clear();
    
    return consumer->outq_len();
}

/////////////////////////////////////////////////////////////////////////////
// Subscriptions

std::string RdKafka1C::Subscription() {
    loger->Info("Get topics subscription");
    error->Clear();

    std::vector<std::string> topics;
    RdKafka::ErrorCode errorCode = consumer->subscription(topics);
    if (errorCode) {
        error->Set("Failed to get subscriptions: " + RdKafka::err2str(errorCode));
        return "";
    }

    loger->Debug("Print topics to string");
    std::stringstream stream;
    for (int i = 0; i < topics.size(); i++)
        stream << topics[i] << ";";

    return stream.str();
}

bool RdKafka1C::Subscribe(std::string Topic) {
    loger->Info("Subscribe to topic " + Topic);
    error->Clear();

    std::vector<std::string> topics;
    topics.push_back(Topic);

    RdKafka::ErrorCode errorCode = consumer->subscribe(topics);
    if (errorCode) {
        error->Set("Failed to subscribe to topic " + Topic + ": " + RdKafka::err2str(errorCode));
        return false;
    }

    return true;
}

bool RdKafka1C::Unsubscribe() {
    loger->Info("Unsubscribe from all topics");
    error->Clear();

    RdKafka::ErrorCode errorCode = consumer->unsubscribe();
    if (errorCode) {
        error->Set("Failed to unsubscribe from all topics: " + RdKafka::err2str(errorCode));
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Other

std::string RdKafka1C::MessageStatusToString(RdKafka::Message::Status Status) {
    std::string status;
    switch (Status) {
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
