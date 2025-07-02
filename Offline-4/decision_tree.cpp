#include <bits/stdc++.h>
#include "tree.h"

using namespace std;

/**
 * The function `readData` reads data from a CSV file and stores it in a 2D vector of strings.
 *
 * @param filename The `filename` parameter is a `const string&` type, which means it is a constant
 * reference to a string. This function is designed to read data from a file specified by the filename
 * provided as an argument.
 *
 * @return The function `readData` returns a vector of vectors of strings, which represents the data
 * read from the specified file. Each inner vector corresponds to a row of data, and each string within
 * the inner vector represents a cell value in that row.
 */
vector<vector<string>> readData(const string &filename)
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
            // row.erase(row.begin());
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
    vector<vector<string>> data = readData(filename);
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
    unsigned int timeSeed = static_cast<unsigned int>(chrono::system_clock::now().time_since_epoch().count());
    default_random_engine generator(timeSeed);
    vector<double> runAccuracies;
    for (int run = 0; run < numRuns; run++)
    {
        unsigned int runSeed = timeSeed + run;
        mt19937 g(runSeed);
        shuffle(data.begin(), data.end(), g);
        int trainSize = static_cast<int>(data.size() * 0.8);
        vector<vector<string>> trainData(data.begin(), data.begin() + trainSize);
        vector<vector<string>> testData(data.begin() + trainSize, data.end());
        unique_ptr<SplitCriterion> selector;
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
    double variance = 0.0;
    for (double acc : runAccuracies)
    {
        variance += pow(acc - averageAccuracy, 2);
    }
    double stdDev = sqrt(variance / numRuns);
    cout << "Standard Deviation: " << (stdDev * 100.0) << "%" << endl;
    return 0;
}
