#include <bits/stdc++.h>
using namespace std;

class MaximumCut
{
    int numberOfVertices;
    int numberOfEdges;
    vector<vector<vector<int>>> graph;
    unordered_set<int> vertexSet1;
    unordered_set<int> vertexSet2;

    /**
     * The function `findMaximumWeightedEdge` finds the edge with the maximum weight in a graph
     * represented as an adjacency list.
     *
     * @return A pair of integers representing the vertices of the edge with the maximum weight in the
     * graph.
     */
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

    /**
     * The function calculates the maximum weight of edges connecting vertices in two different sets.
     *
     * @return The function `getMaximumCutWeight` returns the total weight of the maximum cut in the
     * graph based on the vertices in `vertexSet1` and `vertexSet2`.
     */
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

    /**
     * The function `clearSets` clears two sets named `vertexSet1` and `vertexSet2`.
     */
    void clearSets()
    {
        vertexSet1.clear();
        vertexSet2.clear();
    }

public:
    /**
     * The function `MaximumCut` initializes the graph, number of vertices, and number of edges based
     * on the input graph data structure.
     *
     * @param vector The `MaximumCut` function you provided seems to be initializing some member
     * variables based on the input graph. The input parameter `graph` is a 3D vector, where each
     * element represents a vertex and its adjacent vertices.
     */
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

    /**
     * The `greedyAlgorithm` function implements a greedy algorithm to partition vertices into two sets
     * to maximize the total weight of edges between the two sets.
     *
     * @return The `greedyAlgorithm` function returns the weight of the maximum cut found by the
     * algorithm after processing the graph and assigning vertices to two sets based on edge weights.
     */
    int greedyAlgorithm()
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

        for (int i = 1; i <= numberOfVertices; i++)
        {
            if (assigned[i])
            {
                continue;
            }

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

            assigned[i] = true;
        }
        return getMaximumCutWeight();
    }

    /**
     * The function `randomizedAlgorithm` generates random cuts on a graph and calculates the average
     * maximum cut weight over a specified number of iterations.
     *
     * @param n The `n` parameter in the `randomizedAlgorithm` function represents the number of
     * iterations the algorithm will run. If `n` is not provided when calling the function, it defaults
     * to `-1`, in which case it will use the value of `numberOfVertices` as the number of iterations.
     *
     * @return The function `randomizedAlgorithm` returns the average maximum cut weight calculated
     * over `n` iterations.
     */
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

    /**
     * The function implements a semi-greedy algorithm to find a maximum cut in a graph with specified
     * alpha value.
     *
     * @param alpha The `alpha` parameter in the `semiGreedyAlgorithm` function represents a value
     * between 0 and 1 that determines the balance between the minimum and maximum weights when
     * calculating the threshold `mu`. It is used in the formula `mu = wMin + alpha * (wMax - wMin)
     *
     * @return The `semiGreedyAlgorithm` function returns the weight of the maximum cut found by the
     * algorithm after assigning vertices to two sets based on a semi-greedy approach with a specified
     * alpha value.
     */
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

            // The following check for an empty restrictedCandidateElements (RCL) is not needed.
            // Explanation:
            // - The RCL is constructed by including all candidates whose greedyVal >= mu.
            // - mu is calculated as wMin + alpha * (wMax - wMin), where wMin and wMax are the
            //   minimum and maximum greedy values among all candidates.
            // - By definition, at least one candidate will have greedyVal == wMax, and since
            //   mu <= wMax, there will always be at least one candidate with greedyVal >= mu.
            // - Therefore, the RCL will never be empty, and this fallback assignment is redundant.
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

    /**
     * The function implements a local search algorithm for finding the maximum cut weight in a graph
     * using different algorithms like greedy, randomized, and semi-greedy.
     *
     * @param algorithm The `algorithm` parameter in the `localSearchAlgorithm` function determines
     * which algorithm to use for the local search. The function checks the value of the `algorithm`
     * parameter and then calls the corresponding algorithm function based on the value provided.
     *
     * @return The function `localSearchAlgorithm` returns the weight of the maximum cut found by the
     * algorithm.
     */
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

    /**
     * The GRASPAlgorithm function iterates a specified number of times, performing a local search
     * algorithm and returning the best cut weight found.
     *
     * @param maxIterations The `maxIterations` parameter in the `GRASPAlgorithm` function specifies
     * the maximum number of iterations the algorithm will run for. This parameter controls how many
     * times the algorithm will attempt to find an improved solution using the local search algorithm
     * within the GRASP framework. By default, the value is set to 10.
     *
     * @return The `GRASPAlgorithm` function returns the best cut weight found after running the GRASP
     * algorithm for a specified number of iterations (default is 10).
     */
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
