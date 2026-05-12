#include "storage_component.hpp"

#include <userver/storages/mongo/component.hpp>
#include <userver/utils/assert.hpp>

namespace storage {

StorageComponent::StorageComponent(const userver::components::ComponentConfig& config,
                                   const userver::components::ComponentContext& context)
    : userver::components::LoggableComponentBase(config, context) {
    
    auto mongo_component = context.FindComponent<userver::storages::mongo::Client>();
    auto mongo_client_ptr = mongo_component.GetClient();

    std::string brokers = config["kafka-brokers"].As<std::string>("localhost:9092");

    producer_ = std::make_shared<kafka::ProducerComponent>(brokers);

    storage_ = std::make_unique<storage::Storage>(mongo_client_ptr, producer_);
}

storage::Storage& StorageComponent::GetStorage() {
    UASSERT(storage_);
    return *storage_;
}

}