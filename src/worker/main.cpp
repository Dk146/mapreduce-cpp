#include <iostream>
#include <vector>
#include <thread>
#include <filesystem>
#include "../include/coordinator.h"
#include "../include/worker.h"
#include "../include/wc.h"

int main(int argc, char *argv[])
{
    std::vector<std::string> files;
    if (argc < 2)
    {
        return -1;
    }
    for (int i = 1; i < argc; ++i)
    {
        files.emplace_back(argv[i]);
    }
    if (!std::filesystem::exists("file")) {
        std::filesystem::create_directory("file");
        std::cout << "Directory created: " << "file" << std::endl;
    }
    MapReduce::Coordinator coor(files, 10);
    std::vector<std::thread> threads;
    int num_of_worker = 8;
    for (int i = 0; i < num_of_worker; ++i)
    {
        MapReduce::WordCount wc;
        MapReduce::Worker woker(&coor, &wc);
        auto t = std::thread(&MapReduce::Worker::Work, &woker);
        threads.emplace_back(std::move(t));
    }
    for (auto &thread : threads)
    {
        thread.join();
    }

    return 0;
}