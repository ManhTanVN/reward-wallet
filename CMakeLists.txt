cmake_minimum_required(VERSION 3.10)

# Set project name and language
project(Reward-Wallet VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(Reward-Wallet src/main.cpp)
