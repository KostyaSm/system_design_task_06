#include "consumer.hpp"
#include <iostream>
#include <thread>
#include <csignal>
#include <string>

volatile bool running = true;

void signal_handler(int) {
    running = false;
}

void process_event(const kafka::Message& msg) {
    std::cout << "\n[CONSUMER] Received message:" << std::endl;
    std::cout << "  Topic:    " << msg.topic << std::endl;
    std::cout << "  Partition:" << msg.partition << std::endl;
    std::cout << "  Offset:   " << msg.offset << std::endl;
    std::cout << "  Key:      " << msg.key << std::endl;
    std::cout << "  Payload:  " << msg.payload << std::endl;

    // Простая эмуляция обработки событий для CQRS
    if (msg.payload.find("\"UserRegistered\"") != std::string::npos) {
        std::cout << "  -> [ACTION] Updating user_activity_timeline (CQRS Read Model)" << std::endl;
        // Здесь будет вызов Storage::UpdateUserTimeline(msg.key, msg.payload);
    } 
    else if (msg.payload.find("\"WorkoutCreated\"") != std::string::npos) {
        std::cout << "  -> [ACTION] Recalculating workout_statistics (CQRS Read Model)" << std::endl;
        // Здесь будет вызов Storage::RecalculateStats(msg.key, msg.payload);
    }
}

int main(int argc, char* argv[]) {
    std::string brokers = "localhost:9092";
    std::string group_id = "fitness-cqrs-consumer";
    std::vector<std::string> topics = {"user-events", "workout-events"};

    if (argc > 1) {
        brokers = argv[1];
    }

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::cout << "[CONSUMER] Initializing consumer..." << std::endl;
    std::cout << "  Brokers:  " << brokers << std::endl;
    std::cout << "  Group ID: " << group_id << std::endl;
    std::cout << "  Topics:   " << topics.size() << std::endl;

    try {
        kafka::Consumer consumer(brokers, group_id, topics);
        std::cout << "[CONSUMER] Successfully connected. Waiting for events...\n" << std::endl;

        while (running) {
            auto msg = consumer.Poll(1000);
            
            if (msg.has_value()) {
                process_event(msg.value());
                
                consumer.Commit();
            }
        }

        std::cout << "\n[CONSUMER] Graceful shutdown initiated..." << std::endl;
        consumer.Close();
        
    } catch (const std::exception& ex) {
        std::cerr << "[CONSUMER] Fatal error: " << ex.what() << std::endl;
        return 1;
    }

    std::cout << "[CONSUMER] Exited successfully." << std::endl;
    return 0;
}