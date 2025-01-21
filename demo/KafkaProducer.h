#ifndef KAFKA_PRODUCER_H
#define KAFKA_PRODUCER_H

#include "librdkafka/rdkafkacpp.h"
#include <string>

class KafkaProducer {
public:
    KafkaProducer(const std::string& brokers, const std::string& topic_name);
    ~KafkaProducer();
    void produce(const void* payload, size_t len);
    void produce(const void* payload, size_t len, uint32_t node_id);

private:
    RdKafka::Conf* conf;
    RdKafka::Producer* producer;
    RdKafka::Topic* topic;
};

#endif // KAFKA_PRODUCER_H
