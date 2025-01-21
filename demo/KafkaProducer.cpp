#include "KafkaProducer.h"
#include <iostream>
#include <chrono>

KafkaProducer::KafkaProducer(const std::string& brokers, const std::string& topic_name) {
    std::string errstr;

    conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    conf->set("bootstrap.servers", brokers, errstr);

    producer = RdKafka::Producer::create(conf, errstr);
    if (!producer) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        throw std::runtime_error("Failed to create producer");
    }

    topic = RdKafka::Topic::create(producer, topic_name, nullptr, errstr);
    if (!topic) {
        std::cerr << "Failed to create topic: " << errstr << std::endl;
        throw std::runtime_error("Failed to create topic");
    }
}

KafkaProducer::~KafkaProducer() {
    delete topic;
    delete producer;
    delete conf;
}

void KafkaProducer::produce(const void* payload, size_t len) {
    RdKafka::ErrorCode resp = producer->produce(topic, RdKafka::Topic::PARTITION_UA, RdKafka::Producer::RK_MSG_COPY,
                                                const_cast<void*>(payload), len, nullptr, nullptr);
    if (resp != RdKafka::ERR_NO_ERROR) {
        std::cerr << "Failed to produce message: " << RdKafka::err2str(resp) << std::endl;
    } else {
        std::cout << "Produced message to topic" << std::endl;
    }

    producer->flush(10000);
}

void KafkaProducer::produce(const void* payload, size_t len, uint32_t node_id) {
    RdKafka::Headers *produce_headers = RdKafka::Headers::create();
    produce_headers->add("node_id", std::to_string(node_id));
    // Get the current time point from the system clock
    auto now = std::chrono::system_clock::now();

    // Convert the time point to milliseconds since epoch
    int64_t milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();
    RdKafka::ErrorCode resp = producer->produce(
        topic_name,
        RdKafka::Topic::PARTITION_UA,
        RdKafka::Producer::RK_MSG_COPY,
        const_cast<void*>(payload),
        len,
        nullptr, 0, milliseconds_since_epoch, produce_headers, nullptr);

    if (resp != RdKafka::ERR_NO_ERROR) {
        std::cerr << "Failed to produce message: " << RdKafka::err2str(resp) << std::endl;
    } else {
        std::cout << "Produced message to topic" << std::endl;
    }

    producer->flush(10000);
}
