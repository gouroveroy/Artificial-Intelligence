#include <bits/stdc++.h>
using namespace std;

enum class Criterion
{
    IG,
    IGR,
    NWIG
};

class Row
{
public:
    vector<string> cat; // categorical values
    vector<double> num; // numeric values
    string label;
};

class Dataset
{
public:
    vector<string> header;
    vector<bool> isNumeric;
    vector<Row> rows;

    void readDataset(const string &path)
    {
        ifstream fin(path);
        string line;
        getline(fin, line);
        {
            istringstream ss(line);
            string c;
            while (getline(ss, c, ','))
            {
                header.push_back(c);
            }
        }
        int M = header.size();
        isNumeric.assign(M - 1, false);
        vector<vector<string>> raw;
        while (getline(fin, line))
        {
            if (line.empty())
            {
                continue;
            }
            istringstream ss(line);
            vector<string> rec;
            string cell;
            while (getline(ss, cell, ','))
            {
                rec.push_back(cell);
            }
            if (rec.size() == M)
            {
                raw.push_back(rec);
            }
        }
        // detect numeric columns
        for (int j = 0; j < M - 1; ++j)
        {
            bool allnum = true;
            for (auto &r : raw)
            {
                try
                {
                    stod(r[j]);
                }
                catch (...)
                {
                    allnum = false;
                    break;
                }
            }
            isNumeric[j] = allnum;
        }
        // parse rows
        for (auto &r : raw)
        {
            Row row;
            for (int j = 0; j < M - 1; ++j)
            {
                if (isNumeric[j])
                {
                    row.num.push_back(stod(r[j]));
                }
                else
                {
                    row.cat.push_back(r[j]);
                }
            }
            row.label = r[M - 1];
            rows.push_back(move(row));
        }
    }
};

class Node
{
public:
    bool isLeaf = false;
    string label;
    int featIdx = -1;
    double threshold = 0.0;
    unordered_map<string, Node *> children;
    Node *left = nullptr, *right = nullptr;
};

double entropy(const vector<Row *> &S)
{
    unordered_map<string, int> cnt;
    for (auto *r : S)
    {
        cnt[r->label]++;
    }
    double E = 0.0, N = S.size();
    for (auto &p : cnt)
    {
        double pc = p.second / N;
        E -= pc * log2(pc);
    }
    return E;
}

double infoGain(const vector<Row *> &S, const vector<Row *> &subset,
                Criterion crit, int k = 1, const unordered_map<string, vector<Row *>> *parts = nullptr)
{
    double Hs = entropy(S);
    double w = double(subset.size()) / S.size();
    double Hsv = w * entropy(subset);
    double IGv = Hs - Hsv;
    if (crit == Criterion::IG)
    {
        return IGv;
    }
    double IV = 0.0;
    if (parts)
    {
        for (auto &p : *parts)
        {
            double w = double(p.second.size()) / S.size();
            if (w > 1e-12)
                IV -= w * log2(w);
        }
    }
    else
    {
        IV = -w * log2(max(w, 1e-12)) - (1.0 - w) * log2(max(1.0 - w, 1e-12));
    }
    if (crit == Criterion::IGR)
    {
        return IV < 1e-12 ? 0.0 : IGv / IV;
    }
    return (IGv / log2(k + 1)) * (1.0 - double(k - 1) / S.size());
}

class Split
{
public:
    bool isNum;
    int idx;
    double thresh, score;
    Split(bool isNum = false, int idx = -1, double thresh = 0.0, double score = -1e9)
        : isNum(isNum), idx(idx), thresh(thresh), score(score) {}
};

Split bestSplitOn(const vector<Row *> &S, const Dataset &ds, Criterion crit)
{
    Split best{false, -1, 0.0, -1e9};
    int M = ds.header.size() - 1;
    for (int j = 0; j < M; ++j)
    {
        if (!ds.isNumeric[j])
        {
            unordered_map<string, vector<Row *>> parts;
            for (auto *r : S)
            {
                parts[r->cat[j]].push_back(r);
            }
            double weightedEntropy = 0.0;
            for (auto &p : parts)
            {
                double w = double(p.second.size()) / S.size();
                weightedEntropy += w * entropy(p.second);
            }
            double sc = entropy(S) - weightedEntropy;
            if (crit == Criterion::IGR)
            {
                double IV = 0.0;
                for (auto &p : parts)
                {
                    double w = double(p.second.size()) / S.size();
                    if (w > 1e-12)
                        IV -= w * log2(w);
                }
                sc = IV < 1e-12 ? 0.0 : sc / IV;
            }
            else if (crit == Criterion::NWIG)
            {
                int k = parts.size();
                sc = (sc / log2(k + 1)) * (1.0 - double(k - 1) / S.size());
            }
            if (sc > best.score)
            {
                best = {false, j, 0.0, sc};
            }
        }
        else
        {
            vector<double> vals;
            for (auto *r : S)
            {
                vals.push_back(r->num[j]);
            }
            sort(vals.begin(), vals.end());
            for (int i = 1; i < (int)vals.size(); ++i)
            {
                if (vals[i] == vals[i - 1])
                {
                    continue;
                }
                double t = 0.5 * (vals[i] + vals[i - 1]);
                vector<Row *> L, R;
                for (auto *r : S)
                {
                    if (r->num[j] <= t)
                    {
                        L.push_back(r);
                    }
                    else
                    {
                        R.push_back(r);
                    }
                }
                double scL = infoGain(S, L, crit, 2);
                double scR = infoGain(S, R, crit, 2);
                double sc = scL + scR;
                if (sc > best.score)
                    best = {true, j, t, sc};
            }
        }
    }
    return best;
}

Node *buildTree(const vector<Row *> &S, const Dataset &ds, Criterion crit, int depth, int maxDepth)
{
    Node *node = new Node();
    double H = entropy(S);
    if (H < 1e-9 || depth == maxDepth)
    {
        unordered_map<string, int> cnt;
        for (auto *r : S)
        {
            cnt[r->label]++;
        }
        node->isLeaf = true;
        node->label = max_element(cnt.begin(), cnt.end(), [](auto &a, auto &b)
                                  { return a.second < b.second; })
                          ->first;
        return node;
    }
    Split best = bestSplitOn(S, ds, crit);
    if (best.idx < 0)
    {
        node->isLeaf = true;
        node->label = S.front()->label;
        return node;
    }
    node->featIdx = best.idx;
    if (!best.isNum)
    {
        unordered_map<string, vector<Row *>> parts;
        for (auto *r : S)
        {
            parts[r->cat[best.idx]].push_back(r);
        }
        for (auto &p : parts)
        {
            node->children[p.first] = buildTree(p.second, ds, crit, depth + 1, maxDepth);
        }
    }
    else
    {
        vector<Row *> L, R;
        for (auto *r : S)
        {
            if (r->num[best.idx] <= best.thresh)
            {
                L.push_back(r);
            }
            else
            {
                R.push_back(r);
            }
        }
        node->threshold = best.thresh;
        node->left = buildTree(L, ds, crit, depth + 1, maxDepth);
        node->right = buildTree(R, ds, crit, depth + 1, maxDepth);
    }
    return node;
}

string predict(Node *node, const Row &r)
{
    while (!node->isLeaf)
    {
        int j = node->featIdx;
        if (node->children.empty())
        {
            node = (r.num[j] <= node->threshold ? node->left : node->right);
        }
        else
        {
            auto it = node->children.find(r.cat[j]);
            if (it == node->children.end())
            {
                return node->label; // Fallback to node's majority label
            }
            node = it->second;
        }
    }
    return node->label;
}

void treeStats(Node *n, int depth, int &maxD, int &nodes)
{
    if (!n)
        return;
    nodes++;
    maxD = max(maxD, depth);
    if (!n->children.empty())
    {
        for (auto &p : n->children)
        {
            treeStats(p.second, depth + 1, maxD, nodes);
        }
    }
    else
    {
        treeStats(n->left, depth + 1, maxD, nodes);
        treeStats(n->right, depth + 1, maxD, nodes);
    }
}

void freeTree(Node *n)
{
    if (!n)
    {
        return;
    }
    for (auto &p : n->children)
    {
        freeTree(p.second);
    }
    freeTree(n->left);
    freeTree(n->right);
    delete n;
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        cerr << "Usage: " << argv[0] << " <IG|IGR|NWIG> <maxDepth> <dataset>\n";
        return 1;
    }
    string sc = argv[1];
    Criterion criterion = Criterion::IG;
    if (sc == "IG")
    {
        criterion = Criterion::IG;
    }
    else if (sc == "IGR")
    {
        criterion = Criterion::IGR;
    }
    else if (sc == "NWIG")
    {
        criterion = Criterion::NWIG;
    }
    else
    {
        cerr << "Unknown criterion\n";
        return 1;
    }
    int maxDepth = atoi(argv[2]);

    Dataset ds;
    ds.readDataset(argv[3]);

    vector<Row *> ptrs;
    for (auto &r : ds.rows)
    {
        ptrs.push_back(&r);
    }

    // Use a fixed seed for reproducibility
    mt19937_64 rng(12345);
    double sumAcc = 0;
    int sumNodes = 0, sumDepth = 0;
    for (int run = 0; run < 20; ++run)
    {
        shuffle(ptrs.begin(), ptrs.end(), rng);
        int n = ptrs.size();
        vector<Row *> train(ptrs.begin(), ptrs.begin() + n * 0.8);
        vector<Row *> test(ptrs.begin() + n * 0.8, ptrs.end());

        Node *root = buildTree(train, ds, criterion, 0, maxDepth);
        int correct = 0;
        for (auto *r : test)
        {
            if (predict(root, *r) == r->label)
            {
                correct++;
            }
        }
        sumAcc += double(correct) / test.size();

        int nodes = 0, depth = 0;
        treeStats(root, 0, depth, nodes);
        sumNodes += nodes;
        sumDepth += depth;
        freeTree(root); // Free memory after each run
    }
    cout << fixed << setprecision(2);
    cout << "================ Decision Tree ================\n";
    cout << left << setw(20) << "Criterion:" << sc << '\n';
    cout << left << setw(20) << "Max Depth:" << maxDepth << '\n';
    cout << left << setw(20) << "Avg Accuracy:" << (sumAcc / 20.0) * 100 << "%\n";
    cout << left << setw(20) << "Avg # Nodes:" << int(sumNodes / 20) << '\n';
    cout << left << setw(20) << "Avg Tree Depth:" << int(sumDepth / 20) << '\n';
    cout << "========================================================\n";
    return 0;
}
