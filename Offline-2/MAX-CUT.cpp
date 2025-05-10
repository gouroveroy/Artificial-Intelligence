#include <bits/stdc++.h>
using namespace std;

class MaximumCut
{
    int numberOfVertices;
    int numberOfEdges;
    vector<vector<vector<int>>> graph;
    unordered_set<int> vertexSet1;
    unordered_set<int> vertexSet2;

    pair<int, int> findMaximumWeightedEdge()
    {
        int u_max = -1, v_max = -1, maxWeight = INT_MIN;

        for (int u = 1; u <= numberOfVertices; ++u)
        {
            for (auto adj : graph[u])
            {
                int v = adj[0];
                int w = adj[1];
                if (u < v && w > maxWeight)
                {
                    maxWeight = w;
                    u_max = u;
                    v_max = v;
                }
            }
        }
        return {u_max, v_max};
    }

    int getMaximumCutWeight()
    {
        int maximumCutWeight = 0;
        for (int i = 1; i <= numberOfVertices; i++)
        {
            for (auto neighbor : graph[i])
            {
                if (i < neighbor[0])
                {
                    if (vertexSet1.find(i) != vertexSet1.end() && vertexSet2.find(neighbor[0]) != vertexSet2.end())
                    {
                        maximumCutWeight += neighbor[1];
                    }
                    else if (vertexSet2.find(i) != vertexSet2.end() && vertexSet1.find(neighbor[0]) != vertexSet1.end())
                    {
                        maximumCutWeight += neighbor[1];
                    }
                }
            }
        }
        return maximumCutWeight;
    }

    void clearSets()
    {
        vertexSet1.clear();
        vertexSet2.clear();
    }

public:
    MaximumCut(vector<vector<vector<int>>> &graph)
    {
        srand(time(0));
        this->graph = graph;
        numberOfVertices = graph.size() - 1;
        numberOfEdges = 0;

        for (int i = 1; i <= numberOfVertices; i++)
        {
            numberOfEdges += graph[i].size();
        }

        numberOfEdges /= 2;
    }

    int greedyAlgorithm()
    {
        clearSets();

        for (int i = 1; i <= numberOfVertices; i++)
        {
            int edgeWeightSet1 = 0;
            int edgeWeightSet2 = 0;

            for (auto neighbor : graph[i])
            {
                if (vertexSet1.find(neighbor[0]) != vertexSet1.end())
                {
                    edgeWeightSet1 += neighbor[1];
                }

                else if (vertexSet2.find(neighbor[0]) != vertexSet2.end())
                {
                    edgeWeightSet2 += neighbor[1];
                }
            }

            if (edgeWeightSet1 < edgeWeightSet2)
            {
                vertexSet1.insert(i);
            }

            else
            {
                vertexSet2.insert(i);
            }
        }
        return getMaximumCutWeight();
    }

    int randomizedAlgorithm(int n = -1)
    {
        if (n == -1)
        {
            n = numberOfVertices;
        }

        int maximumCutWeight = 0;

        for (int i = 0; i < n; i++)
        {
            clearSets();

            for (int i = 1; i <= numberOfVertices; i++)
            {
                if (rand() % 2 == 0)
                {
                    vertexSet1.insert(i);
                }

                else
                {
                    vertexSet2.insert(i);
                }
            }
            maximumCutWeight += getMaximumCutWeight();
        }
        maximumCutWeight /= n;
        return maximumCutWeight;
    }

    int semiGreedyAlgorithm(double alpha = 0.5)
    {
        clearSets();

        vector<bool> assigned(numberOfVertices + 1, false);

        auto edge = findMaximumWeightedEdge();
        int u_max = edge.first;
        int v_max = edge.second;

        vertexSet1.insert(u_max);
        vertexSet2.insert(v_max);
        assigned[u_max] = true;
        assigned[v_max] = true;

        int assignedVertexCount = 2;

        while (assignedVertexCount < numberOfVertices)
        {
            vector<int> candidateElements;
            vector<int> sigmaX(numberOfVertices + 1), sigmaY(numberOfVertices + 1), greedyVal(numberOfVertices + 1);
            int wMin = INT_MAX, wMax = INT_MIN;

            for (int i = 1; i <= numberOfVertices; i++)
            {
                if (assigned[i])
                {
                    continue;
                }
                candidateElements.push_back(i);

                int sumX = 0, sumY = 0;

                for (auto adj : graph[i])
                {
                    int v = adj[0];
                    int w = adj[1];

                    if (!assigned[v])
                    {
                        continue;
                    }

                    if (vertexSet1.find(v) != vertexSet1.end())
                    {
                        sumX += w;
                    }

                    else if (vertexSet2.find(v) != vertexSet2.end())
                    {
                        sumY += w;
                    }
                }

                sigmaX[i] = sumX;
                sigmaY[i] = sumY;
                greedyVal[i] = max(sigmaX[i], sigmaY[i]);

                wMin = min(wMin, min(sigmaX[i], sigmaY[i]));
                wMax = max(wMax, max(sigmaX[i], sigmaY[i]));
            }

            double mu = wMin + alpha * (wMax - wMin);

            vector<int> restrictedCandidateElements;

            for (int i = 0; i < candidateElements.size(); i++)
            {
                if (greedyVal[candidateElements[i]] >= mu)
                {
                    restrictedCandidateElements.push_back(candidateElements[i]);
                }
            }

            if (restrictedCandidateElements.empty())
            {
                restrictedCandidateElements = candidateElements;
            }

            int chosenVertex = restrictedCandidateElements[rand() % restrictedCandidateElements.size()];

            if (sigmaX[chosenVertex] < sigmaY[chosenVertex])
            {
                vertexSet1.insert(chosenVertex);
            }

            else
            {
                vertexSet2.insert(chosenVertex);
            }

            assigned[chosenVertex] = true;
            assignedVertexCount++;
        }

        return getMaximumCutWeight();
    }

    int localSearchAlgorithm(string algorithm = "randomized")
    {
        if (algorithm == "greedy")
        {
            greedyAlgorithm();
        }

        else if (algorithm == "randomized")
        {
            randomizedAlgorithm();
        }

        else if (algorithm == "semi-greedy")
        {
            semiGreedyAlgorithm();
        }

        bool improved = true;

        while (improved)
        {
            improved = false;
            int bestVertex = -1;
            int bestDelta = 0;

            for (int i = 1; i <= numberOfVertices; i++)
            {
                int sigmaX = 0, sigmaY = 0;
                bool inSet1 = vertexSet1.count(i);

                for (auto adj : graph[i])
                {
                    int v = adj[0];
                    int w = adj[1];

                    if ((vertexSet1.count(v) && inSet1) || (vertexSet2.count(v) && !inSet1))
                    {
                        sigmaX += w; // to same set
                    }
                    else
                    {
                        sigmaY += w; // to opposite set
                    }
                }

                int delta = sigmaX - sigmaY;

                if (delta > bestDelta)
                {
                    bestDelta = delta;
                    bestVertex = i;
                    improved = true;
                }
            }

            if (improved and bestVertex != -1)
            {
                if (vertexSet1.find(bestVertex) != vertexSet1.end())
                {
                    vertexSet1.erase(bestVertex);
                    vertexSet2.insert(bestVertex);
                }
                else if (vertexSet2.find(bestVertex) != vertexSet2.end())
                {
                    vertexSet2.erase(bestVertex);
                    vertexSet1.insert(bestVertex);
                }
            }
        }

        return getMaximumCutWeight();
    }

    int GRASPAlgorithm(int maxIterations = 10)
    {
        int bestCutWeight = INT_MIN;

        for (int i = 0; i < maxIterations; i++)
        {
            clearSets();
            int cutWeight = localSearchAlgorithm("semi-greedy");
            if (cutWeight > bestCutWeight)
            {
                bestCutWeight = cutWeight;
            }
        }

        return bestCutWeight;
    }
};
