#pragma once

#include <sstream>
#include <librdkafka/rdkafkacpp.h>
#include "Loger.h"

class Event : public RdKafka::EventCb  {
    
    public:
        Event(Loger* Loger);
        void event_cb(RdKafka::Event& event);

    private:
        Loger* loger;
};
    
