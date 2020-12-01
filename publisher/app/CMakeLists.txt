set(SOURCES "MainPublisher.cpp")

add_executable(publisher ${SOURCES})
target_link_libraries(publisher PRIVATE
    Threads::Threads
    PahoMqttCpp::paho-mqttpp3
    spdlog::spdlog
    Threadsafe::Threadsafe
)
target_compile_features(publisher PUBLIC cxx_std_17)