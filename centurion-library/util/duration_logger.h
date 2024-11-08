/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <string>
#include <chrono>
#include <deque>

class DurationLogger
{
public:
    explicit DurationLogger(const std::string&name);
    explicit DurationLogger(const std::string& name, const DurationLogger* other);
    ~DurationLogger(void);
    void Initialize();
    void EnableClearing();
    std::string Name;
    uint8_t Depth = 0;
    std::deque<double> Durations;
private:
    DurationLogger(void) = delete;
    DurationLogger& operator=(DurationLogger source) = delete;
    void SaveData();
    void Clear();

    std::chrono::steady_clock::time_point Start;
    std::string File;
    bool Ready = false;
    bool clear = false;
    uint8_t Id;
};
