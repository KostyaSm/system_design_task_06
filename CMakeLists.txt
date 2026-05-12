cmake_minimum_required(VERSION 3.22)
project(fitness_api CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Werror -Wpedantic)

find_package(PkgConfig REQUIRED)
pkg_check_modules(USERVER REQUIRED userver)

pkg_check_modules(RDKAFKA REQUIRED rdkafka++)

find_package(nlohmann_json 3.2.0 REQUIRED)

find_package(fmt REQUIRED)

set(KAFKA_STORAGE_SOURCES
    src/kafka/producer.cpp
    src/kafka/consumer.cpp
    src/storage/storage.cpp
    src/storage/storage_component.cpp
)

set(API_SOURCES
    ${KAFKA_STORAGE_SOURCES}
    src/main.cpp
    src/handlers/auth_register.cpp
    src/handlers/auth_login.cpp
    src/handlers/get_user_by_login.cpp
    src/handlers/search_users.cpp
    src/handlers/create_exercise.cpp
    src/handlers/get_exercises.cpp
    src/handlers/create_workout.cpp
    src/handlers/add_exercise_to_workout.cpp
    src/handlers/get_workout_history.cpp
    src/handlers/get_workout_stats.cpp
)

set(CONSUMER_SOURCES
    src/kafka/consumer.cpp
    src/kafka/statistics_consumer.cpp
)

add_executable(fitness_api ${API_SOURCES})
target_include_directories(fitness_api PRIVATE 
    ${USERVER_INCLUDE_DIRS} 
    ${RDKAFKA_INCLUDE_DIRS}
)
target_link_libraries(fitness_api PRIVATE
    ${USERVER_LIBRARIES}
    ${RDKAFKA_LDFLAGS}
    nlohmann_json::nlohmann_json
    fmt::fmt
    pthread
)

add_executable(statistics_consumer ${CONSUMER_SOURCES})
target_include_directories(statistics_consumer PRIVATE ${RDKAFKA_INCLUDE_DIRS})
target_link_libraries(statistics_consumer PRIVATE
    ${RDKAFKA_LDFLAGS}
    fmt::fmt
    pthread
)

configure_file(configs/config.yaml ${CMAKE_BINARY_DIR}/config.yaml COPYONLY)