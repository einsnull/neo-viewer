CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(neo_viewer C CXX)

# Specify default build type if none provided (before project() command)
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF()

# For generators with multiple configurations (like VS), only allow Debug and Release
IF(CMAKE_CONFIGURATION_TYPES)
  SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Limited configurations" FORCE)
  MARK_AS_ADVANCED(CMAKE_CONFIGURATION_TYPES)
ENDIF()

SET(SFML_STATIC_LIBRARIES FALSE CACHE BOOL "Use static SFML libraries")

IF(SFML_STATIC_LIBRARIES)
  ADD_DEFINITIONS(-DSFML_STATIC)
ENDIF()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  # Turn on the ability to create folders to organize projects (.vcproj)
  # It creates "CMakePredefinedTargets" folder by default and adds CMake
  # defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
  SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
ELSE()
  MESSAGE(FATAL_ERROR "System not yet supported. Please open a ticket.")
ENDIF()


FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(Neo REQUIRED)
INCLUDE_DIRECTORIES(include ${LIBNEO_INCLUDE_DIR})
SET(DES_DIR "${PROJECT_SOURCE_DIR}")




SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
FIND_PACKAGE(SFML 2 COMPONENTS audio graphics window system)

IF(SFML_FOUND)
  INCLUDE_DIRECTORIES(${SFML_INCLUDE_DIR})
ELSE()
  SET(SFML_ROOT "" CACHE PATH "SFML top-level directory")
  MESSAGE(STATUS "SFML2 required for real-time viewer (libsfml-dev)")
ENDIF()

ADD_EXECUTABLE(neo_viewer src/viewer.cpp src/angle.cpp src/button.cpp)
IF(SFML_STATIC_LIBRARIES)
  SET_TARGET_PROPERTIES(neo_viewer PROPERTIES COMPILE_DEFINITIONS "SFML_STATIC")
ENDIF()
TARGET_LINK_LIBRARIES(neo_viewer ${LIBNEO_LIBRARY} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} ${CMAKE_THREAD_LIBS_INIT})

# install
INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${PROJECT_NAME})
INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/fonts
  DESTINATION ${PROJECT_NAME}
  PATTERN "CMakeLists.txt" EXCLUDE)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/images
  DESTINATION ${PROJECT_NAME}
  PATTERN "CMakeLists.txt" EXCLUDE)


