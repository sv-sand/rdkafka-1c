#include "MockAddInNative.h"

namespace RdKafka1C {

    bool MockAddInNative::Init(void* pConnection) {
        loger = new Loger();
        error = new ErrorHandler(loger);
        rdk1c = new MockRdKafka1C(loger, error);
        m_iConnect = (IAddInDefBase*)pConnection;
        return m_iConnect != nullptr;
    }

    MockRdKafka1C* MockAddInNative::GetRdk1C() {
        return (MockRdKafka1C*)rdk1c;
    }

}