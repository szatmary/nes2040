#pragma once
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <thread>
#include <vector>


// https://www.nesdev.org/wiki/Cycle_reference_chart
class Clock {
private:
    using freq = std::ratio<1, 236'250'000>;
    using duration = std::chrono::duration<int64_t, freq>;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;

    std::vector<std::pair<int, std::function<void()>>> divizors;

public:
    Clock()
    {
    }

    void run()
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        for (int64_t ticks = 0;; ++ticks) {
            for (auto& div : divizors) {
                if (ticks % div.first == 0) {
                    div.second();
                }
            }

            auto nextTick = startTime + duration(ticks + 11);
            std::this_thread::sleep_until(nextTick);
            // std::fprintf(stderr, "Tick %ld\n", nextTick.time_since_epoch().count());
        }
    }

    void addDivizor(int div, std::function<void()> callback)
    {
        divizors.push_back({ div, callback });
    }
};