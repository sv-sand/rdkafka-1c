#pragma once

#include <sstream>
#include <librdkafka/rdkafkacpp.h>
#include "Loger.h"

class DeliveryReport : public RdKafka::DeliveryReportCb 
{

public:
	void SetLoger(Loger* Loger);
	void dr_cb(RdKafka::Message& Message);

private:
	Loger* loger;
};