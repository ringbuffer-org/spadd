#include "Timer.hpp"

std::vector<double> Timer::timeData;

Timer::Timer()
{
    m_startTimepoint = std::chrono::high_resolution_clock::now();
}

Timer::~Timer()
{
    stop();
}

void Timer::stop() 
{
    auto endTimepoint = std::chrono::high_resolution_clock::now();
    
    auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch().count();
    auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
    
    auto duration = end - start;

    if (timeData.size() < functionCount)
    {
        timeData.push_back(duration * 0.001);
    }
}

void Timer::initializeTimeData()
{
    timeData.clear();
    timeData.reserve(functionCount);
}

std::vector<double> Timer::getTimeData()
{
    return timeData;
}

void Timer::writeTimeData(int channels, int partials, bool domain, int bufferSize, float sampleRate)
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream filePath;
    filePath << "./" << "SpectroSpatialSynth_Benchmark_" << std::put_time(&tm, "%d-%m-%Y_%H-%M-%OS") << ".csv";
    
    std::ofstream timeDataFile;
    timeDataFile.open(filePath.str());

    timeDataFile << "Benchmark Results" << "\n";

    timeDataFile << "Channels:" << "," << channels << "\n";
    timeDataFile << "Partials:" << "," << partials << "\n";

    std::string d;

    if (domain)
        d = "Frequency";
    else 
        d = "Time";
        
    timeDataFile << "Domain:" << "," << d << "\n";
    timeDataFile << "Buffer size:" << "," << bufferSize << "\n";
    timeDataFile << "Sample Rate:" << "," << static_cast<int>(sampleRate) << "\n";

    timeDataFile << "Time data:" << "\n";
    
    double sum = 0;
    
    for (auto& time: timeData)
    {
        timeDataFile << time << "," << "ms" << "\n";
        sum += time;
    }
    
    double empiricalMean = sum / static_cast<double>(timeData.size());
    
    double empiricalStandardDeviation = 0;

    for (auto& time: timeData)
    {
        empiricalStandardDeviation += (time - empiricalMean) * (time - empiricalMean);
    }

    empiricalStandardDeviation = sqrt(empiricalStandardDeviation / (static_cast<double>(timeData.size()) - 1));
    
    const auto [min, max] = std::minmax_element(begin(timeData), end(timeData));

    timeDataFile << "Emprircal Mean" << "," << "Minimum" << "," << "Maximum" << "," << "Empirical Standard Deviation" << "\n";
    timeDataFile << empiricalMean << "," << *min << "," << *max << "," << empiricalStandardDeviation << "\n";
    
    timeDataFile.close();
}

 