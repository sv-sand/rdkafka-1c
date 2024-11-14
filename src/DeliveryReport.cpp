#include "DeliveryReport.h"

DeliveryReport::DeliveryReport(Loger* Loger, MessageStatusCollector* MessageStatusCollector)
{
    loger = Loger;
    messageStatusCollector = MessageStatusCollector;
}

void DeliveryReport::dr_cb(RdKafka::Message& Message) 
{
    LogMessageStatus(Message);
    AddMessageStatus(Message);       
}

void DeliveryReport::LogMessageStatus(RdKafka::Message& Message)
{
    std::stringstream stream;

    stream << "Message delivery report: "
        << "broker: " << Message.broker_id() << ", "
        << "topic: " << Message.topic_name() << ", "
        << "offset: " << Message.offset() << ", "
        << "partition: " << Message.partition() << ", ";

    switch (Message.status())
    {
    case RdKafka::Message::MSG_STATUS_NOT_PERSISTED:
        stream << "status: NOT_PERSISTED, Details: " << Message.errstr();
        break;
    case RdKafka::Message::MSG_STATUS_POSSIBLY_PERSISTED:
        stream << "status: POSSIBLY_PERSISTED, Details: " << Message.errstr();
        break;
    case RdKafka::Message::MSG_STATUS_PERSISTED:
        stream << "status: PERSISTED";
        break;
    default:
        stream << "status: UNKNOWN, Details: " << Message.errstr();
        break;
    }

    loger->Debug(stream.str());
}

void DeliveryReport::AddMessageStatus(RdKafka::Message& Message)
{
    if (!Message.msg_opaque())
        return;
    
    std::string* opaque = (std::string*)Message.msg_opaque();
    std::string uuid = std::string(*opaque);

    if (uuid.empty())
        return;

    messageStatusCollector->Add(uuid, Message.status());
}
