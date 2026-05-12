#pragma once

#include <userver/components/loggable_component_base.hpp>
#include <memory>

#include "storage.hpp"
#include "kafka/component.hpp"

namespace storage {

class StorageComponent final : public userver::components::LoggableComponentBase {
public:
    static constexpr auto kName = "storage-component";

    StorageComponent(const userver::components::ComponentConfig& config,
                     const userver::components::ComponentContext& context);

    storage::Storage& GetStorage();

private:
    std::shared_ptr<kafka::ProducerComponent> producer_;
    std::unique_ptr<storage::Storage> storage_;
};

}