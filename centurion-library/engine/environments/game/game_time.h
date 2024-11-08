/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once
#include <header.h>

using namespace std::chrono;

class Time
{
public:
    Time(void);
    explicit Time(std::vector<uint8_t>&& timeBytes);
    ~Time(void);

    [[nodiscard]] uint32_t GetCurrentFrame(void) const;

    [[nodiscard]] double GetElapsedSecs(void) const;

    /// <summary>
    /// This function resets all the time values to 0.
    /// </summary>
    void Reset(void);

    /// <summary>
    /// This function performs an update of all the time values.
    /// </summary>
    /// <param name="currentTime"></param>
    void Update(void);

    /// <summary>
    /// This function returns the full time (hh:mm:ss) as string.
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] std::string GetFullTimeString(void) const;

    // total values (e.g. 10 minutes and 10 seconds = 10 total minutes and 610 total seconds

    /// <summary>
    /// This function returns the amount of minutes of the session as a unsigned int.
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] uint64_t GetTotalMinutesInt(void) const;

    /// <summary>
    /// This function returns the amount of seconds of the session as a unsigned int.
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] uint64_t GetTotalSecondsInt(void) const;

    /// <summary>
    /// This function returns the amount of minutes of the session as a float.
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] double GetTotalMinutes(void) const;

    /// <summary>
    /// This function returns the amount of seconds of the session as a float.
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] double GetTotalSeconds(void) const;

    /// <summary>
    /// This function returns the amount of minutes of the session as a string.
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] std::string GetTotalMinutesString(void) const;

    /// <summary>
    /// This function returns the amount of seconds of the session as a string.
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] std::string GetTotalSecondsString(void) const;

    // actual values (e.g. 10 minutes and 10 seconds)

    /// <summary>
    /// This function returns the hours as uint32_t.
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] uint32_t GetHours(void) const;

    /// <summary>
    /// This function returns the minutes as uint8_t.
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] uint8_t GetMinutes(void) const;

    /// <summary>
    /// This function returns the seconds as uint8_t.
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] uint8_t GetSeconds(void) const;

    /// <summary>
    /// This function returns the hours as float.
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] float GetHoursFloat(void) const;

    /// <summary>
    /// This function returns the minutes as float.
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] float GetMinutesFloat(void) const;

    /// <summary>
    /// This function returns the seconds as float.
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] float GetSecondsFloat(void) const;

    /// <summary>
    /// This function returns the hours as string.
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] std::string GetHoursString(void) const;

    /// <summary>
    /// This function returns the minutes as string.
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] std::string GetMinutesString(void) const;

    /// <summary>
    /// This function returns the seconds as string.
    /// </summary>
    /// <returns></returns>
    [[nodiscard]] std::string GetSecondsString(void) const;

    [[nodiscard]] bool IsFrameMultipleOf(const uint32_t value) const;

    void GetBinRepresentation(std::vector<uint8_t>& data);
private:
    uint32_t framesCounter = 0;
    double totalMs = 0.0;
    double lastTime = 0.0;
    double totalSeconds = 0.0;
    uint32_t hours = 0;
    uint8_t minutes = 0;
    uint8_t seconds = 0;

    double oldElapsedSeconds = 0.0;
    //Indicates when the timer has been started to count.
    system_clock::time_point start;
    
    static double frameDurationMs;
};
