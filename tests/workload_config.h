#pragma once

#include <cmath>
#include <cstddef>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

struct workload_config
{
    double lookup_ratio = 0.8;
    double insert_ratio = 0.2;
    std::size_t operations = 0;
    std::size_t num_buckets = 1024;
};

inline std::string trim(const std::string& text)
{
    const auto start = text.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
    {
        return {};
    }
    const auto end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

inline workload_config load_workload_config(const std::string& path)
{
    std::ifstream file(path);
    if (!file)
    {
        throw std::runtime_error("Failed to open workload config: " + path);
    }

    workload_config config;
    std::string line;
    while (std::getline(file, line))
    {
        line = trim(line);
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        const auto separator = line.find('=');
        if (separator == std::string::npos)
        {
            continue;
        }

        const std::string key = trim(line.substr(0, separator));
        const std::string value = trim(line.substr(separator + 1));

        if (key == "lookup_ratio")
        {
            config.lookup_ratio = std::stod(value);
        }
        else if (key == "insert_ratio")
        {
            config.insert_ratio = std::stod(value);
        }
        else if (key == "operations")
        {
            config.operations = static_cast<std::size_t>(std::stoull(value));
        }
        else if (key == "num_buckets")
        {
            config.num_buckets = static_cast<std::size_t>(std::stoull(value));
        }
    }

    if (config.operations == 0)
    {
        throw std::runtime_error("workload config must specify operations > 0");
    }

    const double ratio_sum = config.lookup_ratio + config.insert_ratio;
    if (std::abs(ratio_sum - 1.0) > 1e-6)
    {
        throw std::runtime_error("lookup_ratio and insert_ratio must sum to 1.0");
    }

    return config;
}
