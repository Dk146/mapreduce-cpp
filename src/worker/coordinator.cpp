#include "../include/coordinator.h"
#include <ctime>

namespace MapReduce
{
    // response ask job, act as a server
    auto Coordinator::AskJob(AskJobRequest request, AskJobReply &reply) -> void
    {   
        std::unique_lock l(lock);
        reply.n_reduce = n_reduce_;
        if (current_job_task_ == MapTask)
        {
            auto res = GetUnstartedMapJob();
            reply.file_name = res.first;
            reply.task_number = res.second;
            reply.task_type = MapTask;
        }
        else
        {
            auto res = GetUnstartedReduceJob();
            reply.file_name = res.first;
            reply.task_number = res.second;
            reply.task_type = ReduceTask;
        }
        // std::cout << "Ask job return: " << reply.file_name << " " << reply.task_number << " " << reply.task_type << std::endl;
    }

    // response to ack job to mark job finished
    auto Coordinator::AckJob(AckJobRequest request, AckJobReply &reply) -> void
    {
        std::unique_lock l(lock);
        if (request.task_type == MapTask)
        {
            map_jobs_[request.task_number].status = StatusFinished;
            int count = map_jobs_.size();
            for (auto &job : map_jobs_)
            {
                if (job.second.status == StatusFinished)
                    --count;
            }
            if (count == 0)
                current_job_task_ = ReduceTask;
        }
        else
        {
            reduce_job_[request.task_number].status = StatusFinished;
        }
    }

    auto Coordinator::GetUnstartedMapJob() -> std::pair<std::string, int>
    {
        for (auto &job : map_jobs_)
        {
            if (job.second.status == StatusUnstarted)
            {
                job.second.status = StatusPending;
                job.second.started_time = static_cast<uint64_t>(std::time(nullptr));
                return std::make_pair(job.second.file_name, job.first);
            }
        }
        return std::make_pair("", -1);
    }

    auto Coordinator::GetUnstartedReduceJob() -> std::pair<std::string, int>
    {
        int count = 0;
        for (auto &job : reduce_job_)
        {
            if (job.second.status == StatusUnstarted)
            {
                job.second.status = StatusPending;
                job.second.started_time = static_cast<uint64_t>(std::time(nullptr));
                return std::make_pair(job.second.file_name, job.first);
            }
            else if (job.second.status == StatusPending)
            {
                ++count;
            }
        }
        if (count != 0)
        {
            return std::make_pair("", ALL_TASK_INPROGRESS_STATUS);
        }
        return std::make_pair("", -1);
    }


    auto Coordinator::Done() -> bool
    {
        bool ret = true;
        for (auto &job : map_jobs_)
        {
            if (job.second.status != StatusFinished)
                ret = false;
            if (job.second.status == StatusPending)
                if (static_cast<uint64_t>(std::time(nullptr)) - job.second.started_time >= 10)
                    job.second.status = StatusUnstarted;
        }
        if (!ret)
            return false;
        for (auto &job : reduce_job_)
        {
            if (job.second.status == StatusFinished)
                ret = false;
            if (job.second.status == StatusPending)
                if (static_cast<uint64_t>(std::time(nullptr)) - job.second.started_time >= 10)
                    job.second.status = StatusUnstarted;
        }
        return ret;
    }
}
