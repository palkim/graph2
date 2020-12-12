#pragma once

#include <string>

namespace fst
{
    namespace math
    {
        static float min(float a, float b)
        {
            return a < b ? a : b;
        }

        static float max(float a, float b)
        {
            return a > b ? a : b;
        }

        static int string2Index(std::string s, int i){
            int nextSpaceIndex = s.find(' ', i);
            int index = std::stoi(s.substr(i+1, nextSpaceIndex-i-1))-1;
            return index;
        }
    }
}