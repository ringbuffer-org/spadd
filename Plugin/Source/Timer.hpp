/**
 * \class Window
 *
 *
 * \brief The Timer class is used for benchmarking.
 *
 *
 * \author Hilko Tondock
 *
 * \version  0.1
 *
 * \date   2023/03/31
 *
 * Contact: h.tondock@campus.tu-berlin.de
 *
 */

#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <cmath>

const int functionCount = 2000;

class Timer
{
public:
    Timer();
    
    ~Timer();

    void stop(); 
    
    static void initializeTimeData();

    static std::vector<double> getTimeData();

    static void writeTimeData(int channels, int partials, bool domain, int bufferSize, float sampleRate);

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimepoint;

    static std::vector<double> timeData;
};