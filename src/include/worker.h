#pragma once

#include <string>
#include "coordinator.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <cstdint>
#include <cstring>
#include "map_reduce_interface.h"
#include "helper.h"

namespace MapReduce
{
    class Worker
    {
    private:
        std::string job_type_;
        TaskType task_type_;
        Coordinator *coordinator_;
        MapReduceInterface *executor_;
    public:
        Worker(Coordinator *coordinator, MapReduceInterface *executor) : coordinator_(coordinator), executor_(executor) {}
        auto ReduceTaskExecute(const std::string &reduceTask, int taskNumber, int nReduce) -> void;
        auto MapTaskExecute(const std::string &filename, int taskNumber, int nReduce) -> void;
        void Work();
    };
}