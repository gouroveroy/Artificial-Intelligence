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
    /**
     * The function calculates the entropy of a dataset based on the class distribution of the data.
     *
     * @param dataset A vector of vectors of strings representing a dataset. Each inner vector
     * represents a data point with its attributes, and the last element of each inner vector is the
     * class label for that data point.
     *
     * @return The function `entropy` calculates the entropy of a dataset based on the class labels in
     * the last column of each row. It returns the entropy value as a double.
     */
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

    /**
     * The function `splitByAttribute` takes a dataset and splits it into groups based on a specified
     * attribute index.
     *
     * @param dataset A vector of vectors of strings representing a dataset where each inner vector
     * represents a row of data with string values.
     * @param attrIndex The `attrIndex` parameter in the `splitByAttribute` function refers to the
     * index of the attribute based on which you want to split the dataset. It is an integer value that
     * indicates the position of the attribute within each row of the dataset.
     *
     * @return This function returns a map where the keys are strings representing unique values found
     * in the specified attribute column of the dataset, and the values are vectors of vectors of
     * strings. Each inner vector contains a row of the dataset that corresponds to the key value.
     */
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
    /**
     * The function `evaluateAttribute` calculates the information gain of a specific attribute in a
     * dataset based on entropy.
     *
     * @param dataset A vector of vectors of strings representing a dataset where each inner vector is
     * a data record with attributes stored as strings.
     * @param attrIndex The `attrIndex` parameter in the `evaluateAttribute` function refers to the
     * index of the attribute for which you want to evaluate the information gain. This function
     * calculates the information gain by splitting the dataset based on the values of the specified
     * attribute.
     *
     * @return The `evaluateAttribute` function returns a double value that represents the information
     * gain obtained by splitting the dataset based on the specified attribute index.
     */
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
    /**
     * The function `evaluateAttribute` calculates the information gain ratio for a given attribute in
     * a dataset based on entropy calculations.
     *
     * @param dataset The `dataset` parameter is a vector of vectors of strings. Each inner vector
     * represents a data point with attributes stored as strings. The outer vector contains all the
     * data points in the dataset.
     * @param attrIndex The `attrIndex` parameter in the `evaluateAttribute` function refers to the
     * index of the attribute for which you want to evaluate the information gain ratio. This function
     * calculates the information gain ratio for a specific attribute in a dataset based on the entropy
     * values.
     *
     * @return The function `evaluateAttribute` returns a double value, which is the information gain
     * ratio calculated based on the input dataset and attribute index provided.
     */
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
    /**
     * The function `evaluateAttribute` calculates the information gain ratio for a given attribute in
     * a dataset based on entropy calculations.
     *
     * @param dataset The `dataset` parameter is a vector of vectors of strings. Each inner vector
     * represents a data point with attributes stored as strings. The outer vector contains all the
     * data points in the dataset.
     * @param attrIndex `attrIndex` is the index of the attribute for which we want to evaluate the
     * information gain ratio.
     *
     * @return The function `evaluateAttribute` returns a double value that represents the evaluation
     * of a specific attribute in a dataset based on information gain and entropy calculations.
     */
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
