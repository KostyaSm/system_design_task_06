#include "component.hpp"

namespace kafka {

ProducerComponent::ProducerComponent(const std::string& brokers)
    : producer_(std::make_unique<Producer>(brokers)) {}

ProducerComponent::~ProducerComponent() = default;

Producer& ProducerComponent::GetProducer() {
    return *producer_;
}

}