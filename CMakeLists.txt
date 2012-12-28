cmake_minimum_required(VERSION "2.6.0")
PROJECT(joystick-sensor)

SET(VEHICLE_VERSION 0.2+devel)
SET(PROJECT_NAME joystick-sensor)

if(WIN32)
  list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules/windows)
else(WIN32)
  list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules/linux)
endif (WIN32)

find_package(Ice REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}
                    ${PROJECT_BINARY_DIR}
                    ${ICE_INCLUDE_DIR})

link_directories(${ICE_LIBRARY_DIR})

set(ICE_SLICE2CPP slice2cpp)

add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/admin.cpp ${PROJECT_BINARY_DIR}/admin.h
                   COMMAND ${ICE_SLICE2CPP} -I${ICE_SLICE_DIR} --output-dir ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/remote-intrefaces/admin.ice
                   MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/remote-intrefaces/admin.ice
                   COMMENT "invoking slice2cpp on ${PROJECT_SOURCE_DIR}/remote-intrefaces/admin.ice")

add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/sensors.cpp ${PROJECT_BINARY_DIR}/sensors.h
                   COMMAND ${ICE_SLICE2CPP} -I${ICE_SLICE_DIR} --output-dir ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/remote-intrefaces/sensors.ice
                   MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/remote-intrefaces/sensors.ice
                   COMMENT "invoking slice2cpp on ${PROJECT_SOURCE_DIR}/remote-intrefaces/sensors.ice")

#                     [DEPENDS [depends...]]
#                     [IMPLICIT_DEPENDS <lang1> depend1 ...]
#                     [WORKING_DIRECTORY dir]

set(CPPFILES
  ${PROJECT_BINARY_DIR}/admin.cpp
  ${PROJECT_BINARY_DIR}/sensors.cpp
  src/SensorAdminI.cpp
  src/UpdaterThread.cpp
  src/SensorGroupI.cpp
  src/IceSvc.cpp
  src/main.cpp)

set(HEADERS
  ${PROJECT_BINARY_DIR}/admin.h
  ${PROJECT_BINARY_DIR}/sensors.h
  src/SensorAdminI.h
  src/UpdaterThread.h
  src/SensorGroupI.h
  src/IceSvc.h)
              
set(LIBRARIES 
    pthread plibjs Ice IceUtil Glacier2
   )

add_executable(joystick-sensor ${CPPFILES} ${HEADERS})

target_link_libraries(joystick-sensor
                      ${LIBRARIES})

install(TARGETS joystick-sensor
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
