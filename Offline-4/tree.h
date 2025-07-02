#ifndef _TREE_H_
#define _TREE_H_

#include <bits/stdc++.h>
#include "criteria.h"
#include "node.h"

using namespace std;

class DecisionTree
{
private:
    unique_ptr<Node> root;
    vector<string> attributeNames;
    map<int, string> attrMode;
    map<int, vector<double>> discretizationThresholds;
    vector<bool> isContinuousAttr;
    unique_ptr<SplitCriterion> selector;
    int maxDepth;

    string majorityClass(const vector<vector<string>> &data)
    {
        map<string, int> classCount;
        for (const auto &row : data)
        {
            string cls = row.back();
            classCount[cls]++;
        }
        string majorityClass;
        int maxCount = 0;
        for (const auto &kv : classCount)
        {
            if (kv.second > maxCount)
            {
                maxCount = kv.second;
                majorityClass = kv.first;
            }
        }
        return majorityClass;
    }

    bool allSameClass(const vector<vector<string>> &data)
    {
        if (data.empty())
        {
            return true;
        }
        string firstClass = data[0].back();
        for (const auto &row : data)
        {
            if (row.back() != firstClass)
            {
                return false;
            }
        }
        return true;
    }

    void preprocessTrainingData(vector<vector<string>> &data)
    {
        int numAttrs = attributeNames.size();
        for (int j = 0; j < numAttrs; j++)
        {
            map<string, int> frequency;
            for (const auto &row : data)
            {
                string val = row[j];
                if (!val.empty())
                {
                    frequency[val]++;
                }
            }
            string mode = "";
            int maxFreq = 0;
            for (const auto &kv : frequency)
            {
                if (kv.second > maxFreq)
                {
                    maxFreq = kv.second;
                    mode = kv.first;
                }
            }
            if (mode == "")
            {
                mode = "missing";
            }
            attrMode[j] = mode;
            for (auto &row : data)
            {
                if (row[j].empty())
                {
                    row[j] = mode;
                }
            }
        }
        isContinuousAttr.resize(numAttrs, false);
        for (int j = 0; j < numAttrs; j++)
        {
            set<string> distinct;
            bool allNumeric = true;
            for (const auto &row : data)
            {
                string val = row[j];
                distinct.insert(val);
                if (allNumeric)
                {
                    try
                    {
                        size_t pos;
                        stod(val, &pos);
                        if (pos != val.size())
                        {
                            allNumeric = false;
                        }
                    }
                    catch (...)
                    {
                        allNumeric = false;
                    }
                }
            }
            if (distinct.size() > 10 && allNumeric)
            {
                isContinuousAttr[j] = true;
                vector<double> values;
                for (auto &row : data)
                {
                    try
                    {
                        double dval = stod(row[j]);
                        values.push_back(dval);
                    }
                    catch (...)
                    {
                        values.push_back(0.0);
                    }
                }
                sort(values.begin(), values.end());
                int n = values.size();
                double p33 = values[n / 3];
                double p66 = values[2 * n / 3];
                discretizationThresholds[j] = {p33, p66};
                for (auto &row : data)
                {
                    double dval = stod(row[j]);
                    if (dval <= p33)
                    {
                        row[j] = "low";
                    }
                    else if (dval <= p66)
                    {
                        row[j] = "medium";
                    }
                    else
                    {
                        row[j] = "high";
                    }
                }
            }
        }
    }

    void preprocessTestExample(vector<string> &example)
    {
        int numAttrs = attributeNames.size();
        for (int j = 0; j < numAttrs; j++)
        {
            if (example[j].empty())
            {
                if (attrMode.find(j) != attrMode.end())
                {
                    example[j] = attrMode[j];
                }
            }
            if (isContinuousAttr[j] && discretizationThresholds.find(j) != discretizationThresholds.end())
            {
                double dval;
                try
                {
                    dval = stod(example[j]);
                }
                catch (...)
                {
                    dval = stod(attrMode[j]);
                }
                double p33 = discretizationThresholds[j][0];
                double p66 = discretizationThresholds[j][1];
                if (dval <= p33)
                {
                    example[j] = "low";
                }
                else if (dval <= p66)
                {
                    example[j] = "medium";
                }
                else
                {
                    example[j] = "high";
                }
            }
        }
    }

    unique_ptr<Node> buildTree(vector<vector<string>> data, vector<int> activeAttributes, int depth)
    {
        if (data.empty())
        {
            return make_unique<LeafNode>("unknown");
        }
        if (allSameClass(data))
        {
            return make_unique<LeafNode>(data[0].back());
        }
        if (activeAttributes.empty() || (maxDepth > 0 && depth >= maxDepth))
        {
            return make_unique<LeafNode>(majorityClass(data));
        }
        double bestScore = -1.0;
        int bestAttr = -1;
        for (int index : activeAttributes)
        {
            double score = selector->evaluateAttribute(data, index);
            if (score > bestScore)
            {
                bestScore = score;
                bestAttr = index;
            }
        }
        if (bestAttr == -1)
        {
            return make_unique<LeafNode>(majorityClass(data));
        }
        map<string, vector<vector<string>>> splits;
        for (const auto &row : data)
        {
            string value = row[bestAttr];
            splits[value].push_back(row);
        }
        auto newNode = make_unique<DiscreteSplitNode>(bestAttr, majorityClass(data));
        vector<int> newActiveAttributes;
        for (int attr : activeAttributes)
        {
            if (attr != bestAttr)
            {
                newActiveAttributes.push_back(attr);
            }
        }
        for (auto &kv : splits)
        {
            unique_ptr<Node> child = buildTree(kv.second, newActiveAttributes, depth + 1);
            dynamic_cast<DiscreteSplitNode *>(newNode.get())->addChild(kv.first, move(child));
        }
        return newNode;
    }

public:
    DecisionTree(unique_ptr<SplitCriterion> sel, int maxD) : selector(move(sel)), maxDepth(maxD) {}

    void train(vector<vector<string>> dataset)
    {
        if (dataset.empty())
        {
            return;
        }
        int numAttrs = dataset[0].size() - 1;
        attributeNames.resize(numAttrs);
        for (int i = 0; i < numAttrs; i++)
        {
            attributeNames[i] = "attr" + to_string(i);
        }
        preprocessTrainingData(dataset);
        vector<int> activeAttributes;
        for (int i = 0; i < numAttrs; i++)
        {
            activeAttributes.push_back(i);
        }
        root = buildTree(dataset, activeAttributes, 0);
    }

    string predict(vector<string> example)
    {
        if (!root)
        {
            throw runtime_error("Tree not trained");
        }
        preprocessTestExample(example);
        return root->predict(example);
    }

    double testAccuracy(const vector<vector<string>> &testData)
    {
        if (!root)
        {
            return 0.0;
        }
        int correct = 0;
        for (const auto &example : testData)
        {
            vector<string> features(example.begin(), example.end() - 1);
            string actualClass = example.back();
            string predicted = predict(features);
            if (predicted == actualClass)
            {
                correct++;
            }
        }
        return static_cast<double>(correct) / testData.size();
    }
};

#endif // !_TREE_H_
