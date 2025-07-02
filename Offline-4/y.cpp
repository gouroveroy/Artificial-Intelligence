#include <bits/stdc++.h>
using namespace std;

class Node
{
public:
    bool isLeaf;
    string classLabel;
    int attribute;
    bool isContinuous;
    double threshold;
    map<string, Node *> categoricalChildren;
    Node *left;
    Node *right;

    Node(bool _isLeaf, const string &_classLabel, int _attribute = -1, bool _isContinuous = false, double _threshold = 0.0)
        : isLeaf(_isLeaf), classLabel(_classLabel), attribute(_attribute), isContinuous(_isContinuous), threshold(_threshold), left(nullptr), right(nullptr) {}
};

using Dataset = vector<vector<string>>;

bool isNumber(const string &s)
{
    try
    {
        stod(s);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

vector<bool> determineContinuous(const Dataset &data)
{
    int numAttributes = data[0].size() - 1;
    vector<bool> isContinuous(numAttributes, true);
    for (const auto &row : data)
    {
        for (int i = 0; i < numAttributes; ++i)
        {
            if (!isNumber(row[i]))
            {
                isContinuous[i] = false;
            }
        }
    }
    return isContinuous;
}

map<string, int> getClassDistribution(const Dataset &S)
{
    map<string, int> dist;
    for (const auto &row : S)
    {
        dist[row.back()]++;
    }
    return dist;
}

double entropy(const Dataset &S)
{
    auto dist = getClassDistribution(S);
    double total = S.size();
    double ent = 0.0;
    for (const auto &pair : dist)
    {
        double p = pair.second / total;
        if (p > 0)
            ent -= p * log2(p);
    }
    return ent;
}

double computeCategoricalIG(const Dataset &S, int attribute)
{
    map<string, Dataset> subsets;
    for (const auto &row : S)
    {
        subsets[row[attribute]].push_back(row);
    }
    double weightedEntropy = 0.0;
    for (const auto &pair : subsets)
    {
        double p = static_cast<double>(pair.second.size()) / S.size();
        weightedEntropy += p * entropy(pair.second);
    }
    return entropy(S) - weightedEntropy;
}

double computeCategoricalIV(const Dataset &S, int attribute)
{
    map<string, int> valueCounts;
    for (const auto &row : S)
    {
        valueCounts[row[attribute]]++;
    }
    double iv = 0.0;
    double total = S.size();
    for (const auto &pair : valueCounts)
    {
        double p = pair.second / total;
        if (p > 0)
            iv -= p * log2(p);
    }
    return iv;
}

double computeCategoricalScore(const Dataset &S, int attribute, const string &criterion)
{
    double ig = computeCategoricalIG(S, attribute);
    if (criterion == "IG")
        return ig;
    map<string, int> valueCounts;
    for (const auto &row : S)
    {
        valueCounts[row[attribute]]++;
    }
    int k = valueCounts.size();
    if (criterion == "IGR")
    {
        double iv = computeCategoricalIV(S, attribute);
        return (iv > 0) ? ig / iv : 0;
    }
    else if (criterion == "NWIG")
    {
        return (ig / log2(k + 1)) * (1 - static_cast<double>(k - 1) / S.size());
    }
    return 0;
}

class ContinuousSplit
{
public:
    double threshold;
    double score;
};

ContinuousSplit computeContinuousScore(const Dataset &S, int attribute, const string &criterion)
{
    vector<pair<double, string>> valueClass;
    for (const auto &row : S)
    {
        double value = stod(row[attribute]);
        valueClass.emplace_back(value, row.back());
    }
    sort(valueClass.begin(), valueClass.end());
    map<string, int> leftDist, rightDist;
    for (const auto &pair : valueClass)
        rightDist[pair.second]++;
    double total = S.size();
    double bestScore = -1;
    double bestThreshold = 0;
    int leftCount = 0;
    for (size_t i = 0; i < valueClass.size() - 1; ++i)
    {
        leftDist[valueClass[i].second]++;
        rightDist[valueClass[i].second]--;
        leftCount++;
        if (valueClass[i].first < valueClass[i + 1].first)
        {
            double leftEntropy = 0.0;
            for (const auto &pair : leftDist)
            {
                double p = static_cast<double>(pair.second) / leftCount;
                if (p > 0)
                    leftEntropy -= p * log2(p);
            }
            double rightEntropy = 0.0;
            int rightCount = S.size() - leftCount;
            for (const auto &pair : rightDist)
            {
                double p = static_cast<double>(pair.second) / rightCount;
                if (p > 0)
                    rightEntropy -= p * log2(p);
            }
            double weightedEntropy = (leftCount / total) * leftEntropy + (rightCount / total) * rightEntropy;
            double ig = entropy(S) - weightedEntropy;
            double score;
            if (criterion == "IG")
            {
                score = ig;
            }
            else if (criterion == "IGR")
            {
                double p1 = leftCount / total;
                double p2 = rightCount / total;
                double iv = 0;
                if (p1 > 0)
                    iv -= p1 * log2(p1);
                if (p2 > 0)
                    iv -= p2 * log2(p2);
                score = (iv > 0) ? ig / iv : 0;
            }
            else if (criterion == "NWIG")
            {
                double k = 2;
                score = (ig / log2(k + 1)) * (1 - (k - 1) / total);
            }
            if (score > bestScore)
            {
                bestScore = score;
                bestThreshold = (valueClass[i].first + valueClass[i + 1].first) / 2;
            }
        }
    }
    return {bestThreshold, bestScore};
}

class Split
{
public:
    int attribute;
    double threshold;
    double score;
};

Split findBestSplit(const Dataset &S, const string &criterion, const vector<bool> &isContinuous)
{
    int numAttributes = isContinuous.size();
    double bestScore = -1;
    int bestAttribute = -1;
    double bestThreshold = 0;
    for (int attr = 0; attr < numAttributes; ++attr)
    {
        if (isContinuous[attr])
        {
            ContinuousSplit splitResult = computeContinuousScore(S, attr, criterion);
            if (splitResult.score > bestScore)
            {
                bestScore = splitResult.score;
                bestAttribute = attr;
                bestThreshold = splitResult.threshold;
            }
        }
        else
        {
            double score = computeCategoricalScore(S, attr, criterion);
            if (score > bestScore)
            {
                bestScore = score;
                bestAttribute = attr;
                bestThreshold = 0;
            }
        }
    }
    return {bestAttribute, bestThreshold, bestScore};
}

Node *buildTree(const Dataset &S, int currentDepth, int maxDepth, const string &criterion, const vector<bool> &isContinuous)
{
    if (S.empty())
        return nullptr;
    string firstLabel = S[0].back();
    bool allSame = true;
    for (const auto &row : S)
    {
        if (row.back() != firstLabel)
        {
            allSame = false;
            break;
        }
    }
    if (allSame || (maxDepth > 0 && currentDepth >= maxDepth))
    {
        auto dist = getClassDistribution(S);
        string majorityClass = max_element(dist.begin(), dist.end(), [](const auto &a, const auto &b)
                                           { return a.second < b.second; })
                                   ->first;
        return new Node(true, majorityClass);
    }
    Split splitResult = findBestSplit(S, criterion, isContinuous);
    int attribute = splitResult.attribute;
    double threshold = splitResult.threshold;
    double score = splitResult.score;
    if (score <= 0)
    {
        auto dist = getClassDistribution(S);
        string majorityClass = max_element(dist.begin(), dist.end(), [](const auto &a, const auto &b)
                                           { return a.second < b.second; })
                                   ->first;
        return new Node(true, majorityClass);
    }
    Node *node = new Node(false, "", attribute, isContinuous[attribute], threshold);
    if (isContinuous[attribute])
    {
        Dataset left, right;
        for (const auto &row : S)
        {
            double value = stod(row[attribute]);
            if (value <= threshold)
                left.push_back(row);
            else
                right.push_back(row);
        }
        node->left = buildTree(left, currentDepth + 1, maxDepth, criterion, isContinuous);
        node->right = buildTree(right, currentDepth + 1, maxDepth, criterion, isContinuous);
    }
    else
    {
        map<string, Dataset> subsets;
        for (const auto &row : S)
        {
            subsets[row[attribute]].push_back(row);
        }
        for (const auto &pair : subsets)
        {
            node->categoricalChildren[pair.first] = buildTree(pair.second, currentDepth + 1, maxDepth, criterion, isContinuous);
        }
    }
    return node;
}

string predict(Node *node, const vector<string> &instance)
{
    if (node->isLeaf)
        return node->classLabel;
    int attr = node->attribute;
    if (node->isContinuous)
    {
        double value = stod(instance[attr]);
        if (value <= node->threshold)
            return predict(node->left, instance);
        else
            return predict(node->right, instance);
    }
    else
    {
        const string &value = instance[attr];
        if (node->categoricalChildren.count(value))
        {
            return predict(node->categoricalChildren[value], instance);
        }
        else
        {
            return "";
        }
    }
}

double evaluate(const Dataset &test, Node *tree)
{
    int correct = 0;
    for (const auto &row : test)
    {
        string pred = predict(tree, row);
        if (pred == row.back())
            correct++;
    }
    return static_cast<double>(correct) / test.size();
}

int getDepth(Node *node)
{
    if (node->isLeaf)
        return 0;
    int maxChildDepth = 0;
    if (node->isContinuous)
    {
        maxChildDepth = max(getDepth(node->left), getDepth(node->right));
    }
    else
    {
        for (const auto &pair : node->categoricalChildren)
        {
            int childDepth = getDepth(pair.second);
            if (childDepth > maxChildDepth)
                maxChildDepth = childDepth;
        }
    }
    return 1 + maxChildDepth;
}

int getNumNodes(Node *node)
{
    if (node->isLeaf)
        return 1;
    int num = 1;
    if (node->isContinuous)
    {
        num += getNumNodes(node->left) + getNumNodes(node->right);
    }
    else
    {
        for (const auto &pair : node->categoricalChildren)
        {
            num += getNumNodes(pair.second);
        }
    }
    return num;
}

void deleteTree(Node *node)
{
    if (node == nullptr)
        return;
    if (node->isContinuous)
    {
        deleteTree(node->left);
        deleteTree(node->right);
    }
    else
    {
        for (auto &pair : node->categoricalChildren)
        {
            deleteTree(pair.second);
        }
    }
    delete node;
}

// Helper to trim whitespace from both ends
string trim(const string &s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

Dataset readDataset(const string &filename, bool hasHeader)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        throw runtime_error("Cannot open file");
    }
    Dataset data;
    string line;
    if (hasHeader)
    {
        getline(file, line); // skip header
    }
    while (getline(file, line))
    {
        vector<string> row;
        stringstream ss(line);
        string cell;
        while (getline(ss, cell, ','))
        {
            row.push_back(trim(cell));
        }
        data.push_back(row);
    }
    return data;
}

void splitDataset(const Dataset &data, Dataset &train, Dataset &test, double trainRatio)
{
    vector<int> indices(data.size());
    iota(indices.begin(), indices.end(), 0);
    static mt19937 rng(12345); // Fixed seed for reproducibility
    shuffle(indices.begin(), indices.end(), rng);
    int trainSize = static_cast<int>(trainRatio * data.size());
    for (int i = 0; i < trainSize; ++i)
    {
        train.push_back(data[indices[i]]);
    }
    for (int i = trainSize; i < data.size(); ++i)
    {
        test.push_back(data[indices[i]]);
    }
}

// Compute the most common (mode) value for each attribute (excluding label)
vector<string> computeAttributeModes(const Dataset &data)
{
    int numAttributes = data[0].size() - 1;
    vector<string> modes(numAttributes);
    for (int i = 0; i < numAttributes; ++i)
    {
        map<string, int> freq;
        for (const auto &row : data)
        {
            if (row[i] != "" && row[i] != "?")
                freq[row[i]]++;
        }
        if (!freq.empty())
        {
            modes[i] = max_element(freq.begin(), freq.end(), [](const auto &a, const auto &b)
                                   { return a.second < b.second; })
                           ->first;
        }
        else
        {
            modes[i] = "";
        }
    }
    return modes;
}

// Fill missing values in the dataset using the provided modes
void fillMissingWithModes(Dataset &data, const vector<string> &modes)
{
    int numAttributes = modes.size();
    for (auto &row : data)
    {
        for (int i = 0; i < numAttributes; ++i)
        {
            if (row[i] == "" || row[i] == "?")
            {
                row[i] = modes[i];
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cerr << "Usage: decision_tree.exe <criterion> <maxDepth> <dataset>" << endl;
        return 1;
    }
    string criterion = argv[1];
    int maxDepth = atoi(argv[2]);
    string datasetFile = argv[3];
    bool hasHeader = false;
    if (datasetFile == "Datasets/Iris.csv")
        hasHeader = true;
    if (datasetFile == "Datasets/adult.data")
        hasHeader = false;
    Dataset data = readDataset(datasetFile, hasHeader);
    if (data.empty())
    {
        cerr << "Error: No data loaded from file. Check file path and format." << endl;
        return 1;
    }
    size_t expectedCols = data[0].size();
    // Remove rows with wrong column count
    data.erase(remove_if(data.begin(), data.end(), [expectedCols](const vector<string> &row)
                         { return row.size() != expectedCols; }),
               data.end());
    if (hasHeader && datasetFile == "Datasets/Iris.csv")
    {
        for (auto &row : data)
        {
            row.erase(row.begin()); // remove Id column
        }
    }
    // Fill missing values in the whole dataset before splitting
    vector<string> attrModes = computeAttributeModes(data);
    fillMissingWithModes(data, attrModes);
    vector<bool> isContinuous = determineContinuous(data);
    const int numRuns = 20;
    double totalAccuracy = 0.0;
    double totalNumNodes = 0.0;
    double totalDepth = 0.0;
    for (int run = 0; run < numRuns; ++run)
    {
        Dataset train, test;
        splitDataset(data, train, test, 0.8);
        // Recompute modes on train, fill missing in train and test using train's modes
        vector<string> trainModes = computeAttributeModes(train);
        fillMissingWithModes(train, trainModes);
        fillMissingWithModes(test, trainModes);
        Node *tree = buildTree(train, 0, maxDepth, criterion, isContinuous);
        double accuracy = evaluate(test, tree);
        totalAccuracy += accuracy;
        int numNodes = getNumNodes(tree);
        totalNumNodes += numNodes;
        if (maxDepth == 0)
        {
            int depth = getDepth(tree);
            totalDepth += depth;
        }
        deleteTree(tree);
    }
    double avgAccuracy = totalAccuracy / numRuns;
    double avgNumNodes = totalNumNodes / numRuns;
    cout << "Average accuracy: " << avgAccuracy << endl;
    cout << "Average number of nodes: " << avgNumNodes << endl;
    if (maxDepth == 0)
    {
        double avgDepth = totalDepth / numRuns;
        cout << "Average depth: " << avgDepth << endl;
    }
    return 0;
}
