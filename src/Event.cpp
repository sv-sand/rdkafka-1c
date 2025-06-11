#include "Event.h"

namespace RdKafka1C {

    Event::Event(Loger* Loger) {
        loger = Loger;
    }

    void Event::event_cb(RdKafka::Event& event) {
        std::stringstream stream;

        switch (event.type()) {
        case RdKafka::Event::EVENT_ERROR:
            stream << "[KAFKA] ";

            if (event.fatal())
                stream << "FATAL ";

            stream << "ERROR: " << RdKafka::err2str(event.err()) << ": " << event.str();
            loger->Error(stream.str());
            break;

        case RdKafka::Event::EVENT_STATS:
            stream << "[KAFKA] STATS: " << event.str();
            loger->Debug(stream.str());
            break;

        case RdKafka::Event::EVENT_LOG:
            stream << "[KAFKA] LOG-" << event.severity() << "-" << event.fac() << ": " << event.str();
            loger->Debug(stream.str());
            break;

        default:
            stream << "[KAFKA] EVENT " << event.type() << ": " << RdKafka::err2str(event.err()) << ": " << event.str();
            loger->Info(stream.str());
            break;
        }
    }

}