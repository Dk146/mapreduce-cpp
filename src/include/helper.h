#pragma once

#include <iostream>
namespace MapReduce
{
    struct KeyValue
    {
        std::string Key;
        std::string Value;
    };

    enum TaskType
    {
        MapTask,
        ReduceTask
    };

    constexpr int ALL_TASK_INPROGRESS_STATUS = -2;

    enum Status
    {
        StatusUnstarted,
        StatusPending,
        StatusFinished
    };

    struct TaskStatus
    {
        std::string file_name;
        Status status;
        uint64_t started_time;
    };

    struct AskJobRequest
    {
        int x;
    };

    struct AskJobReply
    {
        TaskType task_type;
        int n_reduce;
        int task_number;
        std::string file_name;
    };

    struct AckJobRequest
    {
        int task_number;
        TaskType task_type;
    };

    struct AckJobReply
    {
    };
    
    auto ihash(const std::string &key) -> int;
    auto compareByKey(const KeyValue &a, const KeyValue &b) -> bool;
    auto compareByHashKey(const KeyValue &a, const KeyValue &b) -> bool;
    auto readFile(const std::string &filename) -> std::string;
    auto writeFiles(const std::vector<KeyValue> &intermediate, int workerNumber, int nReduce) -> void;
}