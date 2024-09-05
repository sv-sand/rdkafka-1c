#pragma once

#include <wchar.h>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <map>
#include <boost/uuid/uuid.hpp>           
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include "types.h"

namespace Strings
{
	uint32_t GetLength(const WCHAR_T* Source);
	
	// String conversion without memory allocation
	// You must allcate memory before for Dest before invoke this methods	
	uint32_t ConvertToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t Length);
	uint32_t ConvertToWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t Length);
	
	// String conversion with memory allocation
	// You must delete result array after invoke this methods (delete [])
	wchar_t* ToWchar(const char* Source, uint32_t Length);
	wchar_t* ToWchar(const WCHAR_T* Source, uint32_t Length);
	WCHAR_T* ToShortWchar(const char* Source, uint32_t Length);
	WCHAR_T* ToShortWchar(const wchar_t* Source, uint32_t Length);
	char* ToChar(const wchar_t* source, uint32_t Length);

	// Date & Time
	const std::string TimeStamp();
	std::string ToString(int64_t timestamp);

	// Other methods
	std::string NewUuid();
	std::multimap<std::string, std::string> SplitString(std::string String, std::string PairSeporator, std::string KeyValueSeporator);
}