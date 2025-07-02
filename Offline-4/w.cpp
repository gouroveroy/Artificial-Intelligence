#include <bits/stdc++.h>

using namespace std;

class AttributeSelector
{
public:
    virtual ~AttributeSelector() = default;
    virtual double evaluateAttribute(const vector<vector<string>> &dataset, int attrIndex) = 0;
};

class IGSelector : public AttributeSelector
{
public:
    double entropy(const vector<vector<string>> &dataset)
    {
        if (dataset.empty())
            return 0.0;
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

    double evaluateAttribute(const vector<vector<string>> &dataset, int attrIndex) override
    {
        double baseEntropy = entropy(dataset);
        if (baseEntropy == 0.0)
            return 0.0;

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

class IGRSelector : public IGSelector
{
public:
    double evaluateAttribute(const vector<vector<string>> &dataset, int attrIndex) override
    {
        double ig = IGSelector::evaluateAttribute(dataset, attrIndex);
        if (ig == 0.0)
            return 0.0;

        auto splits = splitByAttribute(dataset, attrIndex);
        int n = dataset.size();
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
            return 0.0;
        return ig / iv;
    }
};

class NWIGSelector : public IGSelector
{
public:
    double evaluateAttribute(const vector<vector<string>> &dataset, int attrIndex) override
    {
        double ig = IGSelector::evaluateAttribute(dataset, attrIndex);
        if (ig == 0.0)
            return 0.0;

        auto splits = splitByAttribute(dataset, attrIndex);
        int k = splits.size();
        int n = dataset.size();
        double denominator = log2(k + 1);
        if (denominator == 0.0)
            return 0.0;
        double factor = (1.0 - static_cast<double>(k - 1) / n);
        return (ig / denominator) * factor;
    }
};

class Node
{
public:
    virtual ~Node() = default;
    virtual string predict(const vector<string> &example) = 0;
    virtual bool isLeaf() const = 0;
};

class LeafNode : public Node
{
private:
    string classLabel;

public:
    LeafNode(string cls) : classLabel(cls) {}
    string predict(const vector<string> &example) override
    {
        return classLabel;
    }
    bool isLeaf() const override { return true; }
};

class SplitNode : public Node
{
protected:
    int splitAttribute;
    string fallbackClass;

public:
    SplitNode(int attr, string fbClass) : splitAttribute(attr), fallbackClass(fbClass) {}
    bool isLeaf() const override { return false; }
};

class DiscreteSplitNode : public SplitNode
{
private:
    map<string, unique_ptr<Node>> children;

public:
    DiscreteSplitNode(int attr, string fbClass) : SplitNode(attr, fbClass) {}
    void addChild(const string &value, unique_ptr<Node> child)
    {
        children[value] = move(child);
    }
    string predict(const vector<string> &example) override
    {
        string value = example[splitAttribute];
        auto it = children.find(value);
        if (it != children.end())
        {
            return it->second->predict(example);
        }
        return fallbackClass;
    }
};

class DecisionTree
{
private:
    unique_ptr<Node> root;
    vector<string> attributeNames;
    map<int, string> attrMode;
    map<int, vector<double>> discretizationThresholds;
    vector<bool> isContinuousAttr;
    unique_ptr<AttributeSelector> selector;
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
            return true;
        string firstClass = data[0].back();
        for (const auto &row : data)
        {
            if (row.back() != firstClass)
                return false;
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
                            allNumeric = false;
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
    DecisionTree(unique_ptr<AttributeSelector> sel, int maxD) : selector(move(sel)), maxDepth(maxD) {}

    void train(vector<vector<string>> dataset)
    {
        if (dataset.empty())
            return;
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
            return 0.0;
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

vector<vector<string>> readCSV(const string &filename)
{
    ifstream file(filename);
    vector<vector<string>> data;
    string line;
    if (file.is_open())
    {
        getline(file, line);
        while (getline(file, line))
        {
            stringstream ss(line);
            string cell;
            vector<string> row;
            while (getline(ss, cell, ','))
            {
                row.push_back(cell);
            }
            data.push_back(row);
        }
    }
    return data;
}

void removeIdColumn(vector<vector<string>> &data)
{
    for (auto &row : data)
    {
        if (!row.empty())
        {
            row.erase(row.begin());
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cerr << "Usage: " << argv[0] << " <criterion> <maxDepth> <dataset>" << endl;
        cerr << "  criterion: IG, IGR, or NWIG" << endl;
        cerr << "  maxDepth: maximum depth of the decision tree" << endl;
        cerr << "  dataset: path to the CSV file" << endl;
        return 1;
    }
    string criterion = argv[1];
    int maxDepth = stoi(argv[2]);
    string filename = argv[3];

    vector<vector<string>> data = readCSV(filename);
    // cout << "Rows read: " << data.size() << endl;
    // if (!data.empty())
    // {
    //     cout << "Columns per row: " << data[0].size() << endl;
    //     cout << "First row: ";
    //     for (const auto &val : data[0])
    //         cout << '[' << val << "] ";
    //     cout << endl;
    // }
    if (data.empty())
    {
        cerr << "No data read from file: " << filename << endl;
        return 1;
    }
    if (data[0][0] == "Id")
    {
        removeIdColumn(data);
    }

    double totalAccuracy = 0.0;
    int numRuns = 20;

    // Use time-based seed for true randomness
    unsigned int timeSeed = static_cast<unsigned int>(chrono::system_clock::now().time_since_epoch().count());
    default_random_engine generator(timeSeed);
    vector<double> runAccuracies;

    for (int run = 0; run < numRuns; run++)
    {
        // Create a new seed for each run using the time seed plus the run number
        unsigned int runSeed = timeSeed + run;
        mt19937 g(runSeed);

        shuffle(data.begin(), data.end(), g);
        int trainSize = static_cast<int>(data.size() * 0.8);
        vector<vector<string>> trainData(data.begin(), data.begin() + trainSize);
        vector<vector<string>> testData(data.begin() + trainSize, data.end());

        unique_ptr<AttributeSelector> selector;
        if (criterion == "IG")
        {
            selector = make_unique<IGSelector>();
        }
        else if (criterion == "IGR")
        {
            selector = make_unique<IGRSelector>();
        }
        else if (criterion == "NWIG")
        {
            selector = make_unique<NWIGSelector>();
        }
        DecisionTree tree(move(selector), maxDepth);
        tree.train(trainData);
        double accuracy = tree.testAccuracy(testData);
        runAccuracies.push_back(accuracy);
        totalAccuracy += accuracy;

        cout << "Run " << (run + 1) << " accuracy: " << (accuracy * 100.0) << "% (seed: " << runSeed << ")" << endl;
    }

    double averageAccuracy = totalAccuracy / numRuns;
    cout << "\nAverage Accuracy (" << criterion << "): " << (averageAccuracy * 100.0) << "%" << endl;

    // Calculate standard deviation to show variability
    double variance = 0.0;
    for (double acc : runAccuracies)
    {
        variance += pow(acc - averageAccuracy, 2);
    }
    double stdDev = sqrt(variance / numRuns);
    cout << "Standard Deviation: " << (stdDev * 100.0) << "%" << endl;

    return 0;
}
