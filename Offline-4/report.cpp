#include <bits/stdc++.h>

using namespace std;

int main()
{
    // Compile the decision tree executable
    system("g++ decision_tree.cpp -o decision_tree.exe");

    vector<string> datasets = {"Datasets/Iris.csv", "Datasets/adult.data"};
    vector<string> criteria = {"IG", "IGR", "NWIG"};
    vector<int> depths = {0, 1, 2, 3, 4, 5}; // 0 means no pruning
    string reportFile = "report.txt";

    // Clear the report file at the start
    ofstream report(reportFile);
    report.close();

    for (const auto &dataset : datasets)
    {
        for (const auto &criterion : criteria)
        {
            for (int depth : depths)
            {
                stringstream cmd;
                cmd << "decision_tree.exe " << criterion << " " << depth << " " << dataset << " >> " << reportFile;
                system(cmd.str().c_str());
                ofstream report(reportFile, ios::app);
                report << "\n";
                report.close();
            }
        }
    }
    cout << "All experiments completed. Results written to " << reportFile << endl;
    return 0;
}
