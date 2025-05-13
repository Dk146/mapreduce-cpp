#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <filesystem>
#include <sstream>
#include "../include/worker.h"
#include "../include/json.hpp"
#include "../include/coordinator.h"
#include "../include/helper.h"

namespace MapReduce
{
    void Worker::Work()
    {

        while (true)
        {
            AskJobReply askReply;
            this->coordinator_->AskJob(AskJobRequest{0}, askReply);
            if (askReply.task_type == MapTask)
            {
                if (askReply.file_name != "")
                {
                    MapTaskExecute(askReply.file_name, askReply.task_number, askReply.n_reduce);
                    AckJobReply ackRep;
                    coordinator_->AckJob({askReply.task_number, MapTask}, ackRep);
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
            else
            {
                if (askReply.file_name != "" && askReply.task_number >= 0)
                {
                    ReduceTaskExecute(askReply.file_name, askReply.task_number, askReply.n_reduce);
                    AckJobReply ackRep;
                    coordinator_->AckJob({askReply.task_number, ReduceTask}, ackRep);
                }
                else if (askReply.task_number == ALL_TASK_INPROGRESS_STATUS)
                {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
                else
                {
                    break;
                }
            }
        }
    }

    // mapTask function
    void Worker::MapTaskExecute(const std::string &filename, int taskNumber, int nReduce)
    {
        try
        {
            // Read file content
            std::string content = readFile(filename);
            // Apply the map function
            std::vector<KeyValue> kva = executor_->Mapf(filename, content);

            // Sort by key
            std::sort(kva.begin(), kva.end(), compareByKey);

            // Sort by hash of the key
            std::sort(kva.begin(), kva.end(), compareByHashKey);

            // Write the sorted key-value pairs to files
            writeFiles(kva, taskNumber, nReduce);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    void Worker::ReduceTaskExecute(const std::string &reduceTask, int taskNumber, int nReduce)
    {
        std::vector<KeyValue> kva;
        // Read intermediate files (mr-<mapTaskNumber>-<reduceTask>)
        for (int i = 0; i < 8; i++)
        {
            std::string filename = "file/mr-" + std::to_string(i) + "-" + reduceTask;
            // std::cout << filename << std::endl;
            if (!std::filesystem::exists(filename))
            {
                std::cout << "File does not exist: " << filename << std::endl;
            }
            std::ifstream file(filename, std::ios::in | std::ios::binary);
            if (!file.is_open())
            {
                // Skip if the file cannot be opened
                continue;
            }

            // Read KeyValue pairs from the file
            std::string line;
            while (std::getline(file, line))
            {
                // Parse each line as JSON
                nlohmann::json jsonData = nlohmann::json::parse(line);

                // Extract Key and Value
                KeyValue kv;
                kv.Key = jsonData.at("Key").get<std::string>();
                kv.Value = jsonData.at("Value").get<std::string>();

                // Append to the vector
                kva.push_back(kv);
            }
            file.close();
        }
        // Sort KeyValue pairs by key
        std::sort(kva.begin(), kva.end(), compareByKey);

        // Create a temporary output file
        std::string tempFileName = "file/mr-out-" + reduceTask + ".tmp";
        std::ofstream tempFile(tempFileName);
        if (!tempFile.is_open())
        {
            std::cerr << "ReduceTaskExecute create temp file error" << std::endl;
            std::exit(1);
        }

        // Call Reduce on each distinct key and write the result to the output file
        size_t i = 0;
        while (i < kva.size())
        {
            size_t j = i + 1;
            while (j < kva.size() && kva[j].Key == kva[i].Key)
            {
                j++;
            }

            // Collect all values for the current key
            std::vector<std::string> values;
            for (size_t k = i; k < j; k++)
            {
                values.push_back(kva[k].Value);
            }

            // Call the reduce function
            std::string output = executor_->Reducef(kva[i].Key, values);
            // Write the result to the temporary file
            tempFile << kva[i].Key << " " << output << "\n";

            i = j;
        }

        tempFile.close();

        // Rename the temporary file to the final output file
        std::string finalFileName = "file/mr-out-" + reduceTask;
        std::filesystem::rename(tempFileName, finalFileName);
    }
}