#pragma once

#define delete_pointer(ptr) {\
    delete ptr;\
    ptr = nullptr;\
}