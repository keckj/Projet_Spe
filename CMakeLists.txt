
cmake_minimum_required(VERSION 3.0)

project(GPGPU)
set(GPGPU_VERSION_MAJOR 1)
set(GPGPU_VERSION_MINOR 2)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}")

#EXTERNAL LIBRARIES
find_package(OpenCL REQUIRED )
if(OPENCL_HAS_CPP_BINDINGS )
	message("-- OpenCL has CPP bindings. Full include is: " ${OPENCL_INCLUDE_DIRS})
else(OPENCL_HAS_CPP_BINDINGS )
	message("-- No OpenCL CPP bindings found")
endif(OPENCL_HAS_CPP_BINDINGS)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(CMAKE_THREAD_PREFER_PTHREAD ON)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED system thread)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(Qt4 REQUIRED QtCore QtGui QtXml QtOpenGL)
find_package(Log4cpp REQUIRED)
find_package(PythonLibs REQUIRED)

#CXX COMPILER
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    #-Wshadow -Wstrict-aliasing -Weffc++ -Werror
    set(CMAKE_CXX_FLAGS "-W -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-comment -pedantic -std=c++11 -m64")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-Weverything")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "/Wall")
endif()

#LINKER
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

#Find source files
file(GLOB_RECURSE header_files src/*.hpp)
file(GLOB_RECURSE source_files src/*.cpp)
file(GLOB_RECURSE test_files test/*.cpp)
file(GLOB_RECURSE template_files src/*.tpp)
file(GLOB_RECURSE kernel_files src/*.cu)

#Local includes
foreach(header ${header_files})
    get_filename_component(dir ${header} DIRECTORY)
    list(APPEND LOCAL_INCLUDE_DIRS ${dir})
endforeach()
list(REMOVE_DUPLICATES LOCAL_INCLUDE_DIRS)

#Header directories
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS} 
    ${GLUT_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${LOG4CPP_INCLUDE_DIRS} 
    ${PYTHON_INCLUDE_DIR}
    ${LOCAL_INCLUDE_DIRS} 
    ${OPENCL_INCLUDE_DIRS}
)

#Library directories
link_directories(
    ${Boost_LIBRARY_DIRS}
    ${LOG4CPP_LIBRARY_DIRS}
)

#Executables
add_executable(main ${header_files} ${source_files} ${template_files})
add_executable(main2 ${header_files} ${source_files} ${template_files})
add_executable(main3 ${header_files} ${source_files} ${template_files})
add_executable(main4 ${header_files} ${source_files} ${template_files})

#Link executable

set(LINK
    ${Boost_LIBRARIES}
    ${OPENCL_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES} 
    ${GLUT_glut_LIBRARY}
    Qt4::QtGui 
    Qt4::QtCore 
    Qt4::QtXml 
    Qt4::QtOpenGL
    ${LOG4CPP_LIBRARIES}
    ${PYTHON_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

target_link_libraries(main ${LINK})
target_link_libraries(main2 ${LINK})
target_link_libraries(main3 ${LINK})
target_link_libraries(main4 ${LINK})

#Properties
set_target_properties(main PROPERTIES COMPILE_DEFINITIONS _N_MAIN=0)
set_target_properties(main2 PROPERTIES COMPILE_DEFINITIONS _N_MAIN=1)
set_target_properties(main3 PROPERTIES COMPILE_DEFINITIONS _N_MAIN=2)
set_target_properties(main4 PROPERTIES COMPILE_DEFINITIONS _N_MAIN=3)

enable_testing()
