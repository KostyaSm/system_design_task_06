#include "producer.hpp"

#include <librdkafka/rdkafkacpp.h>
#include <stdexcept>
#include <iostream>

namespace kafka {

class ProducerImpl {
public:
    ProducerImpl(const std::string& brokers) {
        std::string errstr;
        
        conf_.reset(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
        topic_conf_.reset(RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC));
        
        if (conf_->set("bootstrap.servers", brokers, errstr) != RdKafka::Conf::CONF_OK) {
            throw std::runtime_error("Failed to set bootstrap.servers: " + errstr);
        }
        
        if (conf_->set("enable.idempotence", "true", errstr) != RdKafka::Conf::CONF_OK) {
            std::cerr << "Warning: Failed to enable idempotence: " << errstr << std::endl;
        }
        
        if (conf_->set("acks", "all", errstr) != RdKafka::Conf::CONF_OK) {
            throw std::runtime_error("Failed to set acks: " + errstr);
        }
        
        if (conf_->set("retries", "3", errstr) != RdKafka::Conf::CONF_OK) {
            std::cerr << "Warning: Failed to set retries: " << errstr << std::endl;
        }
        
        producer_.reset(RdKafka::Producer::create(conf_.get(), errstr));
        if (!producer_) {
            throw std::runtime_error("Failed to create producer: " + errstr);
        }
    }
    
    ~ProducerImpl() {
        if (producer_) {
            producer_->flush(5000);
        }
    }
    
    bool Produce(const std::string& topic, const std::string& key, const std::string& payload) {
        RdKafka::ErrorCode resp = producer_->produce(
            topic,
            RdKafka::Topic::PARTITION_UA,
            RdKafka::Producer::RK_MSG_COPY,
            const_cast<char*>(payload.c_str()), payload.size(),
            key.empty() ? nullptr : const_cast<std::string*>(&key),
            nullptr
        );
        
        if (resp != RdKafka::ERR_NO_ERROR) {
            std::cerr << "Failed to produce message to topic " << topic 
                      << ": " << RdKafka::err2str(resp) << std::endl;
            return false;
        }
        
        producer_->poll(0);
        return true;
    }
    
    void Flush(int timeout_ms) {
        producer_->flush(timeout_ms);
    }

private:
    std::unique_ptr<RdKafka::Conf> conf_;
    std::unique_ptr<RdKafka::Conf> topic_conf_;
    std::unique_ptr<RdKafka::Producer> producer_;
};

Producer::Producer(const std::string& brokers) 
    : impl_(std::make_unique<ProducerImpl>(brokers)) {}

Producer::~Producer() = default;

bool Producer::Produce(const std::string& topic, const std::string& key, const std::string& payload) {
    return impl_->Produce(topic, key, payload);
}

void Producer::Flush(int timeout_ms) {
    impl_->Flush(timeout_ms);
}

}