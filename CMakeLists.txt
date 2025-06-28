cmake_minimum_required(VERSION 3.10)

project(Reward-Wallet VERSION 1.0 LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output folder for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

# Source and header files
set(SOURCES
    src/main.cpp
    src/cli.cpp
    src/data_manager.cpp
    src/user_account.cpp
    src/sha256.cpp
    src/auth.cpp
    src/wallet.cpp
    src/otp.cpp
)

set(HEADERS
    include/main.h
    include/cli.h
    include/data_manager.h
    include/user_account.h
    include/sha256.h
    include/auth.h
    include/wallet.h
    include/otp.h
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE include)

# -----------------------------
# Platform-specific options
# -----------------------------
if(WIN32)
    message(STATUS "Compiling on Windows")
elseif(APPLE)
    message(STATUS "Compiling on macOS")
endif()
