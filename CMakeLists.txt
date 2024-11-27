cmake_minimum_required (VERSION 3.30)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

if(WIN32)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# DLL 
project(RdKafka1C) 

find_package(Boost 1.85 CONFIG REQUIRED COMPONENTS filesystem iostreams program_options regex system json)
find_package(RdKafka CONFIG REQUIRED)

add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
add_definitions("-DLIBRDKAFKA_STATICLIB")

add_library(RdKafka1C SHARED    
	include/dllmain.cpp
	include/stdafx.cpp
	include/md5.cpp
	src/RdKafka1C.cpp
	src/AddInNative.cpp
	src/ErrorHandler.cpp
	src/Loger.cpp
	src/ConfigBuilder.cpp
	src/DeliveryReport.cpp
	src/Event.cpp
	src/Rebalance.cpp
	src/strings.cpp) 

if(WIN32)
	if(NOT MSVC)
		message(FATAL_ERROR "Must be compiled with MSVC on Windows")
	endif()

	target_include_directories(RdKafka1C PRIVATE include 
		${Boost_INCLUDE_DIRS} 
		${CMAKE_CURRENT_SOURCE_DIR}/include 
		${RdKafka_INCLUDE_DIRS})

	target_link_libraries(RdKafka1C 
		${Boost_LIBRARIES}
		RdKafka::rdkafka
		RdKafka::rdkafka++
		) 
	
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
 
	target_compile_definitions(RdKafka1C PRIVATE
		_WINDOWS
		_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING) 

else()
	target_include_directories(RdKafka1C PRIVATE include 
		${Boost_INCLUDE_DIRS} 
		${CMAKE_CURRENT_SOURCE_DIR}/include 
		${RdKafka_INCLUDE_DIRS})

	target_link_libraries(RdKafka1C 
		${Boost_LIBRARIES}
		RdKafka::rdkafka
		RdKafka::rdkafka++
		)
endif()

# Testing
project(testing)
find_package(GTest CONFIG REQUIRED)
enable_testing()

add_executable(testing 
	test/main.cpp
	test/RdKafka1CTest.cpp
	src/RdKafka1C.cpp
	src/ErrorHandler.cpp
	src/Loger.cpp
	src/ConfigBuilder.cpp
	src/DeliveryReport.cpp
	src/Event.cpp
	src/Rebalance.cpp)

target_include_directories(testing PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/include)	

target_link_libraries(testing PRIVATE
	RdKafka1C
	GTest::gtest  
	GTest::gtest_main 
	GTest::gmock 
	GTest::gmock_main)
