#include "consumer.hpp"

#include <librdkafka/rdkafkacpp.h>
#include <iostream>
#include <stdexcept>

namespace kafka {

class ConsumerImpl {
public:
    ConsumerImpl(const std::string& brokers, const std::string& group_id,
                 const std::vector<std::string>& topics) {
        std::string errstr;

        conf_.reset(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
        
        if (conf_->set("bootstrap.servers", brokers, errstr) != RdKafka::Conf::CONF_OK) {
            throw std::runtime_error("Failed to set bootstrap.servers: " + errstr);
        }

        if (conf_->set("group.id", group_id, errstr) != RdKafka::Conf::CONF_OK) {
            throw std::runtime_error("Failed to set group.id: " + errstr);
        }

        if (conf_->set("auto.offset.reset", "earliest", errstr) != RdKafka::Conf::CONF_OK) {
            std::cerr << "Warning: Failed to set auto.offset.reset: " << errstr << std::endl;
        }

        if (conf_->set("enable.auto.commit", "false", errstr) != RdKafka::Conf::CONF_OK) {
            throw std::runtime_error("Failed to set enable.auto.commit: " + errstr);
        }

        consumer_.reset(RdKafka::Consumer::create(conf_.get(), errstr));
        if (!consumer_) {
            throw std::runtime_error("Failed to create consumer: " + errstr);
        }

        std::vector<std::string> topics_vec(topics.begin(), topics.end());
        if (consumer_->subscribe(topics_vec) != RdKafka::ERR_NO_ERROR) {
            throw std::runtime_error("Failed to subscribe to topics");
        }
    }

    ~ConsumerImpl() {
        if (consumer_) {
            consumer_->close();
            RdKafka::wait_destroyed(5000);
        }
    }

    std::optional<Message> Poll(int timeout_ms) {
        RdKafka::Message* msg = consumer_->poll(timeout_ms);
        if (!msg) {
            return std::nullopt;
        }

        if (msg->err()) {
            if (msg->err() == RdKafka::ERR__TIMED_OUT || msg->err() == RdKafka::ERR__PARTITION_EOF) {
                delete msg;
                return std::nullopt;
            }
            
            std::cerr << "Consumer error: " << msg->errstr() << std::endl;
            delete msg;
            return std::nullopt;
        }

        Message result;
        result.topic = msg->topic_name();
        result.partition = msg->partition();
        result.offset = msg->offset();
        
        if (msg->key()) {
            result.key = std::string(static_cast<const char*>(msg->key()->c_str()), msg->key()->len());
        }

        if (msg->payload()) {
            result.payload = std::string(static_cast<const char*>(msg->payload()), msg->len());
        }

        delete msg;
        return result;
    }

    void Commit() {
        RdKafka::ErrorCode err = consumer_->commitSync();
        if (err != RdKafka::ERR_NO_ERROR) {
            std::cerr << "Failed to commit offset: " << RdKafka::err2str(err) << std::endl;
        }
    }

    void Close() {
        if (consumer_) {
            consumer_->close();
        }
    }

private:
    std::unique_ptr<RdKafka::Conf> conf_;
    std::unique_ptr<RdKafka::Consumer> consumer_;
};

Consumer::Consumer(const std::string& brokers, const std::string& group_id,
                   const std::vector<std::string>& topics)
    : impl_(std::make_unique<ConsumerImpl>(brokers, group_id, topics)) {}

Consumer::~Consumer() = default;

std::optional<Message> Consumer::Poll(int timeout_ms) {
    return impl_->Poll(timeout_ms);
}

void Consumer::Commit() {
    impl_->Commit();
}

void Consumer::Close() {
    impl_->Close();
}

}