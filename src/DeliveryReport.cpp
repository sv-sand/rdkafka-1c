#include "DeliveryReport.h"

void DeliveryReport::SetLoger(Loger* Loger)
{
    loger = Loger;
}

void DeliveryReport::dr_cb(RdKafka::Message& Message) 
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
