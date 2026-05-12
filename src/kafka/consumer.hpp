#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>

namespace kafka {

class ConsumerImpl;

struct Message {
    std::string topic;
    std::string key;
    std::string payload;
    int partition;
    int64_t offset;
};

class Consumer {
public:
    Consumer(const std::string& brokers, const std::string& group_id, 
             const std::vector<std::string>& topics);
    ~Consumer();

    std::optional<Message> Poll(int timeout_ms = 1000);

    void Commit();

    void Close();

private:
    std::unique_ptr<ConsumerImpl> impl_;
};

}