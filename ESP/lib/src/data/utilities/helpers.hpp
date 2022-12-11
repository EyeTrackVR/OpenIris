#ifndef HELPERS_HPP
#define HELPERS_HPP
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <memory>

namespace Helpers
{
    char *itoa(int value, char *result, int base);
    void split(std::string str, std::string splitBy, std::vector<std::string> &tokens);
    std::vector<std::string> split(const std::string &s, char delimiter);
    void update_progress_bar(int progress, int total);

    /// @brief
    /// @tparam ...Args
    /// @param format
    /// @param ...args
    /// @return
    template <typename... Args>
    std::string format_string(const std::string &format, Args... args)
    {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
        if (size_s <= 0)
        {
            std::cout << "Error during formatting.";
            return "";
        }
        auto size = static_cast<size_t>(size_s);
        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, format.c_str(), args...);
        return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
    }
}

#endif // HELPERS_HPP
