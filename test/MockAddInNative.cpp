#include "MockAddInNative.h"

namespace Kafka1C {

    MockAddInNative::MockAddInNative() :
        CAddInNative() 
    {
    };

    bool MockAddInNative::Init(void* pConnection) 
    {
        bool result = CAddInNative::Init(pConnection);

		// Change to mock implementation
        delete_pointer(rdk1c);
        rdk1c = new Kafka1C::MockRdKafka1C(loger, error);

        return result;
    }

    MockRdKafka1C* MockAddInNative::GetRdk1C() 
    {
        return (MockRdKafka1C*)rdk1c;
    }

}