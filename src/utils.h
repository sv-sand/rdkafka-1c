#pragma once

#define delete_pointer(ptr) { delete ptr; ptr = nullptr; }

#define delete_array(ptr) { delete [] ptr; ptr = nullptr; }

#define delete_pointer_to_array(ptr) { delete_array(*ptr); delete_pointer(ptr); }
