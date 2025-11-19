#pragma once

#include<time.h>
#include<random>
#include<thread>


class RandomNumberGenThread
{
public:
    int intRand(const int& min, const int& max) {
        static thread_local std::mt19937* generator = nullptr;
        if (!generator) generator = new mt19937(std::hash<std::thread::id>()(std::this_thread::get_id())+clock());
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(*generator);
    }

    float floatRand(const float& min, const float& max) {
        static thread_local std::mt19937* generatorf = nullptr;
        if (!generatorf) generatorf = new mt19937(std::hash<std::thread::id>()(std::this_thread::get_id())+clock());
        std::uniform_real_distribution<float> distribution(min, max);
        return distribution(*generatorf);
    }

 

};

