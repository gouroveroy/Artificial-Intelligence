#pragma once
#include <bits/stdc++.h>
using namespace std;

enum class Heuristic
{
    Hamming,
    Manhattan,
    Euclidean,
    LinearConflict
};

string getHeuristicName(Heuristic heuristic)
{
    switch (heuristic)
    {
    case Heuristic::Hamming:
        return "Hamming";
    case Heuristic::Manhattan:
        return "Manhattan";
    case Heuristic::Euclidean:
        return "Euclidean";
    case Heuristic::LinearConflict:
        return "Linear Conflict";
    default:
        return "Unknown";
    }
}
