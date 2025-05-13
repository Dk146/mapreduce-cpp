
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
#include "../include/helper.h"
#include "../include/json.hpp"

namespace MapReduce
{
    auto ihash(const std::string &key) -> int
    {
        const uint32_t fnv_prime = 16777619u;
        const uint32_t offset_basis = 2166136261u;

        uint32_t hash = offset_basis;
        for (char c : key)
        {
            hash ^= static_cast<uint32_t>(c);
            hash *= fnv_prime;
        }

        return static_cast<int>(hash & 0x7fffffff); // Ensure the hash is non-negative
    }

    auto compareByKey(const KeyValue &a, const KeyValue &b) -> bool
    {
        return a.Key < b.Key;
    }

    auto compareByHashKey(const KeyValue &a, const KeyValue &b) -> bool
    {
        return ihash(a.Key) % 10 < ihash(b.Key) % 10;
    }

    // Read the content of a file
    auto readFile(const std::string &filename) -> std::string
    {
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }

    auto writeFiles(const std::vector<KeyValue> &intermediate, int workerNumber, int nReduce) -> void
    {
        size_t i = 0;
        while (i < intermediate.size())
        {
            // partition key
            size_t j = i + 1;
            while (j < intermediate.size() && ihash(intermediate[j].Key) % nReduce == ihash(intermediate[i].Key) % nReduce)
            {
                j++;
            }
            auto a = intermediate[i];
            
            std::vector<KeyValue> values(intermediate.begin() + i, intermediate.begin() + j);
            
            //generate file name
            std::string file_name = "file/mr-" + std::to_string(workerNumber) + "-" + std::to_string(ihash(intermediate[i].Key) % nReduce);

            // create a temp file
            // std::cout << filename << std::endl;
            auto temp_file_name = file_name + "temp";
            std::ofstream tempFile(temp_file_name, std::ios::out | std::ios::trunc);
            if (!tempFile.is_open())
            {
                std::cerr << "writeFiles create temp file error" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            // write to temp file
            for (const auto &kv : values)
            {
                nlohmann::json jsonObj = {{"Key", kv.Key}, {"Value", kv.Value}};
                tempFile << jsonObj.dump() << std::endl;
            }
            tempFile.close();

            // rename the tem file to the final filename
            if (std::rename(temp_file_name.c_str(), file_name.c_str()) != 0)
            {
                std::cerr << "writeFiles rename temp file eroor" << std::endl;
                std::exit(EXIT_FAILURE);
            }

            i = j;
        }
    }
}