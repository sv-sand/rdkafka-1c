#include "DeliveryReport.h"

namespace RdKafka1C {

    DeliveryReport::DeliveryReport(Loger* Loger) {
        loger = Loger;
    }

    void DeliveryReport::dr_cb(RdKafka::Message& Message) {
        LogMessageStatus(Message);

        std::string messageId = ExtractMessageId(Message);
        SetStatus(messageId, Message.status());
    }

    void DeliveryReport::LogMessageStatus(RdKafka::Message& Message) {
        std::stringstream stream;

        stream << "Message delivery report: "
            << "broker: " << Message.broker_id() << ", "
            << "topic: " << Message.topic_name() << ", "
            << "offset: " << Message.offset() << ", "
            << "partition: " << Message.partition() << ", ";

        switch (Message.status()) {
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

    std::string DeliveryReport::ExtractMessageId(RdKafka::Message& Message) {
        loger->Debug("Extract message status");

        if (!Message.msg_opaque())
            return "";

        std::string* opaque = (std::string*)Message.msg_opaque();
        return *opaque;
    }

    void DeliveryReport::ClearStatuses() {
        loger->Debug("Clear message statuses");
        statuses.clear();
    }

    const std::string* DeliveryReport::AddEmptyStatus(std::string MessageId) {
        loger->Debug("Add message status");

        if (MessageId.empty())
            return nullptr;

        statuses[MessageId] = RdKafka::Message::Status::MSG_STATUS_NOT_PERSISTED;

        return &statuses.find(MessageId)->first;
    }

    void DeliveryReport::SetStatus(std::string MessageId, RdKafka::Message::Status Status) {
        loger->Debug("Set message status");

        if (MessageId.empty())
            return;

        statuses[MessageId] = Status;
    }

    RdKafka::Message::Status DeliveryReport::GetStatus(std::string MessageId) {
        RdKafka::Message::Status status;

        try {
            status = statuses.at(MessageId);
        }
        catch (const std::out_of_range& e) {
            return RdKafka::Message::MSG_STATUS_NOT_PERSISTED;
        }

        return status;
    }

    int DeliveryReport::CountUndelivered()
    {
        int count = 0;

        for (const auto& [key, value] : statuses)
            if (value != RdKafka::Message::MSG_STATUS_PERSISTED)
                count++;

        return count;
    }
}