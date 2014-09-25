cmake_minimum_required(VERSION 2.8)
project(test)

file( GLOB  APP_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp )
foreach( testsourcefile ${APP_SOURCES} )
    string( REPLACE ".cpp" "" testname ${testsourcefile} )
	add_executable( ${testname}  ${testsourcefile} )
endforeach( testsourcefile ${APP_SOURCES} )