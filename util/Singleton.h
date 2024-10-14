#pragma once
#include <map>
template <typename T>
T& Singleton(){
    static T inner;
    return inner;
}