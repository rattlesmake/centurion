/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <list>
#include <string>
#include <vector>
#include <unordered_set>

namespace std 
{
    template <typename T>
    [[nodiscard]] std::list<T> unorderedSet_to_list(const std::unordered_set<T>& us)
    {
        std::list<T> newList;
        for (auto const& it : us)
            newList.push_back(it);
        return newList;
    }

    [[nodiscard]] std::vector<uint8_t> stringToVector(const std::string& str);
    [[nodiscard]] std::string vectorToString(const std::vector<uint8_t>& vec);

    // trim from start (in place)
    void ltrim(std::string& s);

    // trim from end (in place)
    void rtrim(std::string& s);

    // trim from both ends (in place)
    void trim(std::string& s);

    // trim from start (copying)
    std::string ltrim_copy(std::string s);

    // trim from end (copying)
    std::string rtrim_copy(std::string s);

    // trim from both ends (copying)
    std::string trim_copy(std::string s);

    // Returns a raw value from an input string
    std::string uniform_string(std::string s);

    //Remove the extension of a file. The function accepts a file name, not a path!
    std::string remove_extension(const std::string& filename);

    bool is_number(std::string str);

    bool split_string(const std::string& str, std::string& firstHalf, std::string& secondHalf, const char delimiter);

    std::vector<std::string> split_string(std::string input, char del);

    template <typename T>
    void concat_vectors(std::vector<T>& v1, std::vector<T>&& v2)
    {
        v1.insert(
            v1.end(),
            std::make_move_iterator(v2.begin()),
            std::make_move_iterator(v2.end())
        );
    }
};
