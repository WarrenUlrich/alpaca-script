cmake_minimum_required(VERSION 3.8)

project("alpaca-script")

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz)
FetchContent_MakeAvailable(json)

set(OPENSSL_ROOT_DIR "C:\\Program Files (x86)\\OpenSSL-Win32" GLOBAL)

set(OPENSSL_USE_STATIC_LIBS TRUE)

find_package(OpenSSL REQUIRED)

set(ALPACA_BOT_DIR "$ENV{USERPROFILE}\\AlpacaBot")

set(ALPACA_BOT_INCLUDE_DIR "${ALPACA_BOT_DIR}\\Include")

set(ALPACA_BOT_LIBRARY_DIR "${ALPACA_BOT_DIR}\\Library")

set(CMAKE_SHARED_LINKER_FLAGS "-Os -s -m32 -static-libgcc -static-libstdc++ -static")

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${ALPACA_BOT_INCLUDE_DIR}
)

target_link_directories(${PROJECT_NAME} INTERFACE
  ${ALPACA_BOT_LIBRARY_DIR}
)

target_link_libraries(${PROJECT_NAME} INTERFACE
  AlpacaLibrary
  nlohmann_json::nlohmann_json
  ws2_32
  OpenSSL::SSL
  OpenSSL::Crypto
  #crypt32
  crypt32
)

# set_target_properties(${PROJECT_NAME}
# PROPERTIES
# RUNTIME_OUTPUT_DIRECTORY "${ALPACA_BOT_DIR}/Scripts/Local/${PROJECT_NAME}"
# )

# set_target_properties(${PROJECT_NAME}
# PROPERTIES PREFIX ""
# SUFFIX ".dll"
# )