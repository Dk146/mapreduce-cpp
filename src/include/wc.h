#pragma once
#include <iostream>
#include "worker.h"
#include "map_reduce_interface.h"

namespace MapReduce
{
    class WordCount : public MapReduceInterface
    {
    public:
        ~WordCount() override {}

        virtual auto Mapf(const std::string &filename, const std::string &contents) -> std::vector<KeyValue> override;

        virtual auto Reducef(const std::string &key, const std::vector<std::string> &values) -> std::string override;
    };
}