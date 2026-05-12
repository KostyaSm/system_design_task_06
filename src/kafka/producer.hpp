#pragma once

#include <string>
#include <memory>

namespace kafka {

class ProducerImpl;

class Producer {
public:
    explicit Producer(const std::string& brokers);
    ~Producer();

    bool Produce(const std::string& topic, const std::string& key, const std::string& payload);

    void Flush(int timeout_ms = 5000);

private:
    std::unique_ptr<ProducerImpl> impl_;
};

}