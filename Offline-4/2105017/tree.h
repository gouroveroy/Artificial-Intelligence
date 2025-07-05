#ifndef _TREE_H_
#define _TREE_H_

#include <bits/stdc++.h>
#include "criteria.h"
#include "node.h"

using namespace std;

class DecisionTree
{
    unique_ptr<Node> root;
    vector<string> attributeNames;
    map<int, string> attrMode;
    map<int, vector<double>> discretizationThresholds;
    vector<bool> isContinuousAttr;
    unique_ptr<SplitCriterion> selector;
    int maxDepth;

    /**
     * The function `majorityClass` calculates the majority class from a dataset.
     *
     * @param data A vector of vectors of strings representing the dataset, where each inner vector
     * represents a single data point with its attributes and class label.
     *
     * @return The function `majorityClass` returns a string representing the class label that appears most
     * frequently in the dataset.
     */
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

    /**
     * The function `allSameClass` checks if all rows in the dataset belong to the same class.
     *
     * @param data A vector of vectors of strings representing the dataset, where each inner vector
     * represents a single data point with its attributes and class label.
     *
     * @return The function `allSameClass` returns a boolean value indicating whether all rows in the
     * dataset belong to the same class or not.
     */
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

    /**
     * The function `preprocessTrainingData` processes the training data by replacing missing values with
     * the mode of the attribute and discretizing continuous attributes based on predefined thresholds.
     *
     * @param data A reference to a vector of vectors of strings representing the training dataset. Each
     * inner vector represents a single data point, where the last element is the class label and the rest
     * are features.
     */
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
            if (mode == "?")
            {
                mode = "missing";
            }
            attrMode[j] = mode;
            for (auto &row : data)
            {
                if (row[j] == "?")
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

    /**
     * The function `preprocessTestData` processes a test example by replacing missing values with the
     * mode of the attribute and discretizing continuous attributes based on predefined thresholds.
     *
     * @param example A vector of strings representing a single data point or example for which we want to
     * preprocess the data.
     */
    void preprocessTestData(vector<string> &example)
    {
        int numAttrs = attributeNames.size();
        for (int j = 0; j < numAttrs; j++)
        {
            if (example[j] == "?")
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

    /**
     * The function `buildTree` recursively builds a decision tree based on the provided dataset and
     * attributes.
     *
     * @param data A vector of vectors of strings representing the dataset, where each inner vector
     * represents a single data point with its attributes and class label.
     * @param activeAttributes A vector of integers representing the indices of the active attributes
     * that can be used for splitting the dataset.
     * @param depth An integer representing the current depth of the tree being built.
     *
     * @return The function returns a unique pointer to a Node object, which can be either a LeafNode or
     * a DiscreteSplitNode, representing the root of the decision tree built from the given dataset.
     */
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

    /**
     * The function `countNodes` counts the number of nodes in a decision tree by recursively traversing
     * its nodes.
     *
     * @param node A pointer to a constant Node object, which represents the current node in the decision
     * tree.
     *
     * @return The function `countNodes` returns the total number of nodes in the decision tree.
     */
    int countNodes(const Node *node) const
    {
        if (!node)
        {
            return 0;
        }
        if (node->isLeaf())
        {
            return 1;
        }
        const DiscreteSplitNode *split = dynamic_cast<const DiscreteSplitNode *>(node);
        int sum = 1;
        if (split)
        {
            for (const auto &kv : split->getChildren())
            {
                sum += countNodes(kv.second.get());
            }
        }
        return sum;
    }

    /**
     * The function `computeDepth` calculates the depth of a decision tree by recursively traversing its
     * nodes.
     *
     * @param node A pointer to a constant Node object, which represents the current node in the decision
     * tree.
     *
     * @return The function `computeDepth` returns an integer value representing the depth of the decision
     * tree.
     */
    int computeDepth(const Node *node) const
    {
        if (!node)
        {
            return 0;
        }
        if (node->isLeaf())
        {
            return 1;
        }
        const DiscreteSplitNode *split = dynamic_cast<const DiscreteSplitNode *>(node);
        int maxChild = 0;
        if (split)
        {
            for (const auto &kv : split->getChildren())
            {
                maxChild = max(maxChild, computeDepth(kv.second.get()));
            }
        }
        return 1 + maxChild;
    }

public:
    DecisionTree(unique_ptr<SplitCriterion> sel, int maxD) : selector(move(sel)), maxDepth(maxD) {}

    /**
     * The function `train` takes a dataset as input and builds a decision tree based on the provided
     * data.
     *
     * @param dataset A vector of vectors of strings representing the training dataset. Each inner vector
     * represents a single data point, where the last element is the class label and the rest are features.
     */
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

    /**
     * The function `predict` takes a vector of strings as input and returns the predicted class label
     * using the decision tree.
     *
     * @param example A vector of strings representing a single data point or example for which we want to
     * make a prediction.
     *
     * @return The function `predict` returns a string, which is the predicted class label for the given
     * example.
     */
    string predict(vector<string> example)
    {
        if (!root)
        {
            throw runtime_error("Tree not trained");
        }
        preprocessTestData(example);
        return root->predict(example);
    }

    /**
     * The function `testAccuracy` calculates the accuracy of a decision tree by comparing predicted
     * classes with actual classes in the test dataset.
     *
     * @param testData A vector of vectors of strings representing the test dataset. Each inner vector
     * represents a single data point, where the last element is the class label and the rest are
     * features.
     *
     * @return The function `testAccuracy` returns the accuracy of the decision tree on the given test
     * dataset as a double value between 0.0 and 1.0.
     */
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

    /**
     * The function `getNodeCount` counts the number of nodes in a decision tree by recursively traversing
     * the tree.
     *
     * @return The function `getNodeCount()` returns the total number of nodes in the decision tree.
     */
    int getNodeCount() const
    {
        return countNodes(root.get());
    }

    /**
     * The function `getTreeDepth` calculates the depth of a decision tree by recursively computing the
     * depth of its nodes.
     *
     * @return The function `getTreeDepth()` returns the depth of the tree, which is an integer value.
     */
    int getTreeDepth() const
    {
        return computeDepth(root.get());
    }
};

#endif // !_TREE_H_
