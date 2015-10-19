cmake_minimum_required(VERSION 2.8)
project (TestBoost)

add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4819 /MTd" )

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    MESSAGE( "64 bits compiler detected" )
    SET( EX_PLATFORM 64 )
    SET( EX_PLATFORM_NAME "x64" )
else( ) 
    MESSAGE( "32 bits compiler detected" )
    SET( EX_PLATFORM 32 )
    SET( EX_PLATFORM_NAME "x86" )
endif(  )

IF( EX_PLATFORM EQUAL 64 )
	MESSAGE( "Outputting to lib64 and bin64" )

	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib64)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib64)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin64) 
ELSE( )
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) 
ENDIF(  )

IF( EX_PLATFORM EQUAL 64 )
	#include_directories ("N:/MATROX/Mil9_0(x64)/Mil/Include") 
	
	#link_directories ("N:/3rd_sw/boost/boost_1_54_0/stage64/lib")
	#set (EXTRA_LIBS ${EXTRA_LIBS} "winsha_vc100_64.lib" "WinEagle_vc100_64.lib")
	#link_directories ("A:/JSHe/VisLib/3rd_party/googletest-read-only/msvc/gtest/Debug64")
ELSE(  )
	#include_directories ("N:/MATROX/Mil8_0/MIL/include") 

	#link_directories ("N:/3rd_sw/boost/boost_1_54_0/stage32/lib")
	#set (EXTRA_LIBS ${EXTRA_LIBS} "winsha.lib" "wineagle.lib")
	#link_directories ("A:/JSHe/VisLib/3rd_party/googletest-read-only/msvc/gtest/Debug32")
ENDIF(  )

#set (EXTRA_LIBS ${EXTRA_LIBS} "gtestd.lib")
#link_directories ("u:/lib")

#add_subdirectory (src)


#include_directories ("${CMAKE_CURRENT_LIST_DIR}")
#include_directories ("${CMAKE_CURRENT_LIST_DIR}/test")

add_executable(testMyTaskSchedular 	
	testMyTaskSchedular.cpp
	MyTaskSchedular.cpp
	MyTaskSchedular.h
)

#target_link_libraries (test  ${EXTRA_LIBS})


add_executable(testSemaphore 	testSemaphore.cpp)
add_executable(testPPL 	testPPL.cpp)
add_executable(testPPL_then 	testPPL_then.cpp)