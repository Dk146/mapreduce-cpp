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

#include "../include/wc.h"
#include "../include/worker.h"

namespace MapReduce
{
    std::vector<KeyValue> WordCount::Mapf(const std::string &filename, const std::string &contents)
    {
        // Function to check if a character is a letter.
        auto isLetter = [](char c)
        { return std::isalpha(c); };

        // Split contents into words based on non-letter characters.
        std::vector<KeyValue> kva;
        std::string word;
        for (size_t i = 0; i < contents.size(); ++i)
        {
            if (isLetter(contents[i]))
            {
                word += contents[i];
            }
            else if (!word.empty())
            {
                kva.push_back({word, "1"});
                word.clear();
            }
        }
        // Add the last word if it exists.
        if (!word.empty())
        {
            kva.push_back({word, "1"});
        }

        return kva;
    }

    std::string WordCount::Reducef(const std::string &key, const std::vector<std::string> &values)
    {
        // Return the number of occurrences of this word.
        return std::to_string(values.size());
    }
}