#pragma once

#include <vector>
#include <string>
#include "worker.h"
#include "helper.h"

namespace MapReduce
{
    class MapReduceInterface
    {
    public:
        virtual ~MapReduceInterface() = default;

        virtual auto Mapf(const std::string &filename, const std::string &contents) -> std::vector<KeyValue> = 0;

        virtual auto Reducef(const std::string &key, const std::vector<std::string> &values) -> std::string = 0;
    };
}