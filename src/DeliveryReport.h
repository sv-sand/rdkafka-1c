#pragma once

#include <librdkafka/rdkafkacpp.h>
#include <sstream>
#include "Loger.h"
#include "MessageStatusCollector.h"
#include "utils.h"

class DeliveryReport : public RdKafka::DeliveryReportCb 
{

public:
	DeliveryReport(Loger* Loger, MessageStatusCollector* MessageStatusCollector);

	void dr_cb(RdKafka::Message& Message);

private:
	Loger* loger;
	MessageStatusCollector* messageStatusCollector;

	void LogMessageStatus(RdKafka::Message& Message);
	void AddMessageStatus(RdKafka::Message& Message);
};