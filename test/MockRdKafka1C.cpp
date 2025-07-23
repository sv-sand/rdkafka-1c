#include "MockRdKafka1C.h"

namespace Kafka1C 
{
	MockRdKafka1C::MockRdKafka1C(Loger* Loger, ErrorHandler* Error) : 
		RdKafka1C(Loger, Error)
	{
		OperationTimeout = 1000;
	}

	bool MockRdKafka1C::InitProducer()
	{
		bool result = RdKafka1C::InitProducer();

		// Change to mock implementation
		delete_pointer(producer);
		producer = new MockProducer();

		return result;
	}
	
	bool MockRdKafka1C::InitConsumer()
	{
		bool result = RdKafka1C::InitConsumer();

		// Change to mock implementation
		delete_pointer(consumer);
		consumer = new MockKafkaConsumer();

		return result;
	}
	
	MockProducer* MockRdKafka1C::GetProducer()
	{
		return (MockProducer*) producer;
	}
	MockKafkaConsumer* MockRdKafka1C::GetConsumer()
	{
		return (MockKafkaConsumer*) consumer;
	}
}
