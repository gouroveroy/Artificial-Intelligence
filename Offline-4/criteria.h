#ifndef _CRITERIA_H_
#define _CRITERIA_H_

#include <bits/stdc++.h>

using namespace std;

class SplitCriterion
{
public:
    virtual ~SplitCriterion() = default;
    virtual double evaluateAttribute(const vector<vector<string>> &dataset, int attrIndex) = 0;

protected:
    double entropy(const vector<vector<string>> &dataset)
    {
        if (dataset.empty())
        {
            return 0.0;
        }
        map<string, int> classCount;
        for (const auto &row : dataset)
        {
            string cls = row.back();
            classCount[cls]++;
        }
        double ent = 0.0;
        int n = dataset.size();
        for (const auto &kv : classCount)
        {
            double p = static_cast<double>(kv.second) / n;
            if (p > 0)
            {
                ent -= p * log2(p);
            }
        }
        return ent;
    }

    map<string, vector<vector<string>>> splitByAttribute(const vector<vector<string>> &dataset, int attrIndex)
    {
        map<string, vector<vector<string>>> splits;
        for (const auto &row : dataset)
        {
            string val = row[attrIndex];
            splits[val].push_back(row);
        }
        return splits;
    }
};

class IGSelector : public SplitCriterion
{
public:
    double evaluateAttribute(const vector<vector<string>> &dataset, int attrIndex) override
    {
        double baseEntropy = entropy(dataset);
        if (baseEntropy == 0.0)
        {
            return 0.0;
        }
        auto splits = splitByAttribute(dataset, attrIndex);
        int n = dataset.size();
        double weightedEntropy = 0.0;
        for (const auto &kv : splits)
        {
            const auto &subset = kv.second;
            double weight = static_cast<double>(subset.size()) / n;
            weightedEntropy += weight * entropy(subset);
        }
        return baseEntropy - weightedEntropy;
    }
};

class IGRSelector : public SplitCriterion
{
public:
    double evaluateAttribute(const vector<vector<string>> &dataset, int attrIndex) override
    {
        double baseEntropy = entropy(dataset);
        if (baseEntropy == 0.0)
        {
            return 0.0;
        }
        auto splits = splitByAttribute(dataset, attrIndex);
        int n = dataset.size();
        double weightedEntropy = 0.0;
        for (const auto &kv : splits)
        {
            const auto &subset = kv.second;
            double weight = static_cast<double>(subset.size()) / n;
            weightedEntropy += weight * entropy(subset);
        }
        double ig = baseEntropy - weightedEntropy;
        if (ig == 0.0)
        {
            return 0.0;
        }
        double iv = 0.0;
        for (const auto &kv : splits)
        {
            double p = static_cast<double>(kv.second.size()) / n;
            if (p > 0)
            {
                iv -= p * log2(p);
            }
        }
        if (iv == 0.0)
        {
            return 0.0;
        }
        return ig / iv;
    }
};

class NWIGSelector : public SplitCriterion
{
public:
    double evaluateAttribute(const vector<vector<string>> &dataset, int attrIndex) override
    {
        double baseEntropy = entropy(dataset);
        if (baseEntropy == 0.0)
        {
            return 0.0;
        }
        auto splits = splitByAttribute(dataset, attrIndex);
        int n = dataset.size();
        double weightedEntropy = 0.0;
        for (const auto &kv : splits)
        {
            const auto &subset = kv.second;
            double weight = static_cast<double>(subset.size()) / n;
            weightedEntropy += weight * entropy(subset);
        }
        double ig = baseEntropy - weightedEntropy;
        if (ig == 0.0)
        {
            return 0.0;
        }
        int k = splits.size();
        double denominator = log2(k + 1);
        if (denominator == 0.0)
        {
            return 0.0;
        }
        double factor = (1.0 - static_cast<double>(k - 1) / n);
        return (ig / denominator) * factor;
    }
};

#endif // !_CRITERIA_H_
