cmake_minimum_required(VERSION 3.14...3.18)

project(
    MQTTExamplePublisher
    VERSION 1.0
    DESCRIPTION "A simple MQTT publisher"
    LANGUAGES CXX
)

find_package(Threads REQUIRED)
find_package(PahoMqttCpp REQUIRED)

include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.8.1
)
FetchContent_Declare(
    threadsafe
    GIT_REPOSITORY https://github.com/pettinz/threadsafe
    GIT_TAG        v1.0
)
FetchContent_MakeAvailable(spdlog)
FetchContent_MakeAvailable(threadsafe)


add_subdirectory(app)