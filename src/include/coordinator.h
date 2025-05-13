#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <mutex>
#include "helper.h"

namespace MapReduce
{
    class Coordinator
    {
    private:
        std::map<int, TaskStatus> map_jobs_;
        std::map<int, TaskStatus> reduce_job_;
        int n_reduce_;
        TaskType current_job_task_;
        std::mutex lock;
    public:
        Coordinator(std::vector<std::string> files, int n_reduce) : n_reduce_(n_reduce)
        {
            int pos = 0;
            for (const auto &f : files)
            {
                std::cout << f << std::endl;
                map_jobs_[pos] = TaskStatus{f, StatusUnstarted, 0};
                ++pos;
            }
            for (int i = 0; i < 10; ++i)
            {
                reduce_job_[i] = TaskStatus{std::to_string(i), StatusUnstarted, 0};
            }
            current_job_task_ = MapTask;
        }

        auto AskJob(AskJobRequest request, AskJobReply &reply) -> void;
        auto AckJob(AckJobRequest request, AckJobReply &reply) -> void;
        auto Done() -> bool;
        auto GetUnstartedMapJob() -> std::pair<std::string, int>;
        auto GetUnstartedReduceJob() -> std::pair<std::string, int>;
    };
}