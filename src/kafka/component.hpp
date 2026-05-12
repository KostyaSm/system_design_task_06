#pragma once

#include <memory>
#include <string>
#include "producer.hpp"

namespace kafka {

class ProducerComponent {
public:
    explicit ProducerComponent(const std::string& brokers);
    ~ProducerComponent();

    Producer& GetProducer();

private:
    std::unique_ptr<Producer> producer_;
};

}