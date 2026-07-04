cmake_minimum_required(VERSION 3.16)

execute_process(
  COMMAND git describe --tags
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE ABLS_AGENT_PHIDGET_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)

if(NOT ABLS_AGENT_PHIDGET_VERSION)
  set(ABLS_AGENT_PHIDGET_VERSION "unknown")
endif()

if(ABLS_AGENT_PHIDGET_VERSION MATCHES "^v?([0-9]+\\.[0-9]+)")
  set(ABLS_AGENT_PHIDGET_VERSION_NUMBER "${CMAKE_MATCH_1}")
else()
  set(ABLS_AGENT_PHIDGET_VERSION_NUMBER "0.0")
endif()

project(abls-agent-phidget
  VERSION ${ABLS_AGENT_PHIDGET_VERSION_NUMBER}
  DESCRIPTION "Abls Habitat Phidget runtime"
  HOMEPAGE_URL "https://docs.abls-habitat.fr"
  LANGUAGES C
)

include(GNUInstallDirs)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(JSON_GLIB REQUIRED json-glib-1.0)
pkg_check_modules(PHIDGET REQUIRED libphidget22)
pkg_check_modules(ABLS_LIBS REQUIRED abls-libs)
pkg_check_modules(ABLS_THREAD_LIBS QUIET abls-thread-libs)

find_library(ABLS_LIBS_EXACT
  NAMES abls-libs
  PATHS /usr/local/lib64 /usr/local/lib
  NO_DEFAULT_PATH
)

if(NOT ABLS_LIBS_EXACT)
  set(ABLS_LIBS_EXACT ${ABLS_LIBS_LIBRARIES})
endif()

if(NOT ABLS_THREAD_LIBS_FOUND)
  set(ABLS_THREAD_LIBS_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/../ABLS-THREAD-LIBS/include")
  find_library(ABLS_THREAD_LIBS_LOCAL_LIBRARY
    NAMES abls-thread-libs
    PATHS "${CMAKE_SOURCE_DIR}/../ABLS-THREAD-LIBS/build"
    NO_DEFAULT_PATH
  )

  if(NOT ABLS_THREAD_LIBS_LOCAL_LIBRARY)
    message(FATAL_ERROR "abls-thread-libs not found via pkg-config and local build lib is missing. Build ABLS-THREAD-LIBS first.")
  endif()

  set(ABLS_THREAD_LIBS_LIBRARIES "${ABLS_THREAD_LIBS_LOCAL_LIBRARY}")
endif()

add_executable(abls-agent-phidget
  src/main.c
  src/phidget_config.c
  src/phidget_callbacks.c
)

target_include_directories(abls-agent-phidget PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${GLIB_INCLUDE_DIRS}
  ${JSON_GLIB_INCLUDE_DIRS}
  ${PHIDGET_INCLUDE_DIRS}
  ${ABLS_LIBS_INCLUDE_DIRS}
  ${ABLS_THREAD_LIBS_INCLUDE_DIRS}
)

target_compile_options(abls-agent-phidget PRIVATE
  -Wall -Wextra -Wno-unused-parameter -Wno-unused-result -pipe -g -ggdb
)

target_link_libraries(abls-agent-phidget PRIVATE
  ${GLIB_LIBRARIES}
  ${JSON_GLIB_LIBRARIES}
  ${PHIDGET_LIBRARIES}
  ${ABLS_LIBS_EXACT}
  ${ABLS_THREAD_LIBS_LIBRARIES}
)

install(TARGETS abls-agent-phidget
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
