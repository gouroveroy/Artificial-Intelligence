#include <bits/stdc++.h>
#include "MAX-CUT.cpp"
using namespace std;

vector<int> known_best = {
    12078, 12084, 12077,        // G1–G3
    -1, -1, -1, -1, -1, -1, -1, // G4–G10
    627, 621, 645,              // G11–G13
    3187, 3169, 3172,           // G14–G16
    -1, -1, -1, -1, -1,         // G17–G21
    14123, 14129, 14131,        // G22–G24
    -1, -1, -1, -1, -1, -1, -1, // G25–G31
    1560, 1537, 1541,           // G32–G34
    8000, 7996, 8009,           // G35–G37
    -1, -1, -1, -1, -1,         // G38–G42
    7027, 7022, 7020,           // G43–G45
    -1, -1, -1,                 // G46–G48
    6000, 6000, 5988,           // G49–G51
    -1, -1, -1                  // G52–G54
};

int maxIterations = 50;
double alpha = 0.5;

void run_single(const string &algo, const string &graph_file)
{
    ifstream inputFile(graph_file);
    if (!inputFile.is_open())
    {
        cerr << "Error opening input file: " << graph_file << endl;
        return;
    }

    int n, m;
    inputFile >> n >> m;

    vector<vector<vector<int>>> graph(n + 1);
    for (int i = 0; i < m; ++i)
    {
        int u, v, w;
        inputFile >> u >> v >> w;
        graph[u].push_back({v, w});
        graph[v].push_back({u, w});
    }
    inputFile.close();

    MaximumCut maxCut(graph);
    int cutValue = -1;
    string algoName, algoCategory, avgOrBest;
    bool extraRow = false;

    if (algo == "randomized")
    {
        cutValue = maxCut.randomizedAlgorithm();
        algoName = "Simple Randomized or Randomized-1";
        algoCategory = "Constructive Algorithm";
    }
    else if (algo == "greedy")
    {
        cutValue = maxCut.greedyAlgorithm();
        algoName = "Simple Greedy or Greedy-1";
        algoCategory = "Constructive Algorithm";
    }
    else if (algo == "semi-greedy")
    {
        cutValue = maxCut.semiGreedyAlgorithm();
        algoName = "Semi-greedy-1";
        algoCategory = "Constructive Algorithm";
    }
    else if (algo == "local-search")
    {
        cutValue = maxCut.localSearchAlgorithm();
        algoName = "Simple local or local-1";
        algoCategory = "Local Search";
        extraRow = true;
        avgOrBest = "Average value";
    }
    else if (algo == "GRASP")
    {
        cutValue = maxCut.GRASPAlgorithm();
        algoName = "GRASP-1";
        algoCategory = "GRASP";
        extraRow = true;
        avgOrBest = "Best value";
    }
    else
    {
        cerr << "Unknown algorithm: " << algo << endl;
        return;
    }

    size_t start = graph_file.find_last_of('g') + 1;
    size_t end = graph_file.find(".rud");
    string graphNumber = graph_file.substr(start, end - start);
    string graphName = "G" + graphNumber;

    int known = known_best[stoi(graphNumber) - 1];

    ofstream outputFile("2105017.csv");
    outputFile << ",Problem,,," << algoCategory << ",Known Best Solution or Upper Bound\n";
    outputFile << "Name,|V| or n,|E| or m," << algoName << ",\n";
    if (extraRow)
    {
        outputFile << ",,,No. of iterations," << avgOrBest << ",\n";
    }
    outputFile << graphName << "," << n << "," << m << "," << cutValue << ",," << known << "\n";
    outputFile.close();
}

void run_all(int totalGraphs = 54)
{
    ofstream outputFile("2105017.csv");
    outputFile << ",Problem,,,Constructive Algorithm,,,Local Search,GRASP,,Known Best Solution or Upper Bound\n";
    outputFile << "Name,|V| or n,|E| or m,Simple Randomized or Randomized-1,Simple Greedy or Greedy-1,Semi-greedy-1(alpha = 0.5),Simple local or local-1,,GRASP-1(iterations = 50)\n";
    outputFile << ",,,,,,No. of iterations,Average value,No. of iterations, Best value,\n";

    for (int i = 1; i <= totalGraphs; ++i)
    {
        string fileName = "graph_GRASP/set1/g" + to_string(i) + ".rud";
        ifstream inputFile(fileName);
        if (!inputFile.is_open())
        {
            cerr << "Error opening input file: " << fileName << endl;
            continue;
        }

        int n, m;
        inputFile >> n >> m;
        vector<vector<vector<int>>> graph(n + 1);
        for (int j = 0; j < m; ++j)
        {
            int u, v, w;
            inputFile >> u >> v >> w;
            graph[u].push_back({v, w});
            graph[v].push_back({u, w});
        }
        inputFile.close();

        MaximumCut maxCut(graph);
        int randomizedCut = maxCut.randomizedAlgorithm();
        int greedyCut = maxCut.greedyAlgorithm();
        int semiGreedyCut = maxCut.semiGreedyAlgorithm(alpha);
        int localSearchCut = maxCut.localSearchAlgorithm();
        int graspCut = maxCut.GRASPAlgorithm(maxIterations);

        int known = known_best[i - 1];

        outputFile << "G" << i << "," << n << "," << m << "," << randomizedCut << "," << greedyCut << "," << semiGreedyCut
                   << "," << m << "," << localSearchCut << "," << maxIterations << "," << graspCut << "," << known << "\n";
    }
    outputFile.close();
}

int main(int argc, char *argv[])
{
    if (argc == 5 && string(argv[1]) == "--algo" && string(argv[3]) == "--graph")
    {
        string algo = argv[2];
        string graph_file = "graph_GRASP/set1/g" + string(argv[4]) + ".rud";
        run_single(algo, graph_file);
    }
    else
    {
        run_all(54);
    }
    return 0;
}
