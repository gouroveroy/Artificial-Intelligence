#include <bits/stdc++.h>

using namespace std;

// Attribute selection criteria types
enum class CriterionType
{
    IG,
    IGR,
    NWIG
};

// Node class: represents either a leaf or internal split
class Node
{
public:
    bool is_leaf = false;
    string label;           // leaf label
    int attr_index = -1;    // index of attribute to split
    double threshold = 0.0; // for numeric splits
    bool is_numeric_split = false;
    map<string, Node *> children; // for categorical splits
    Node *left = nullptr;         // <= threshold
    Node *right = nullptr;        // > threshold
    ~Node()
    {
        for (auto &p : children)
            delete p.second;
        delete left;
        delete right;
    }
};

// Criterion class: handles IG, IGR, NWIG for both numeric & categorical
class Criterion
{
public:
    CriterionType type;
    Criterion(CriterionType t) : type(t) {}

    // Score: for numeric attr, returns best IG and sets threshold via ref
    double score(const vector<vector<string>> &X,
                 const vector<string> &y,
                 int attr,
                 bool is_numeric,
                 double &best_threshold)
    {
        if (!is_numeric)
        {
            return score_categorical(X, y, attr);
        }
        else
        {
            return score_numeric(X, y, attr, best_threshold);
        }
    }

private:
    static double entropy(const vector<string> &y)
    {
        unordered_map<string, int> cnt;
        for (auto &v : y)
            cnt[v]++;
        double H = 0;
        int n = y.size();
        for (auto &p : cnt)
        {
            double p_i = double(p.second) / n;
            if (p_i > 0)
                H -= p_i * log2(p_i);
        }
        return H;
    }

    double score_categorical(const vector<vector<string>> &X,
                             const vector<string> &y,
                             int attr)
    {
        double ig = information_gain_cat(X, y, attr);
        if (type == CriterionType::IG)
            return ig;
        int k = unique_count_cat(X, attr);
        if (type == CriterionType::IGR)
        {
            double iv = intrinsic_value_cat(X, attr);
            return iv > 0 ? ig / iv : 0.0;
        }
        double denom = log2(k + 1);
        double factor = 1.0 - double(k - 1) / X.size();
        return (ig / denom) * factor;
    }

    double score_numeric(const vector<vector<string>> &X,
                         const vector<string> &y,
                         int attr,
                         double &best_threshold)
    {
        // gather (value,label)
        int n = X.size();
        vector<pair<double, string>> arr;
        arr.reserve(n);
        for (int i = 0; i < n; ++i)
            arr.emplace_back(stod(X[i][attr]), y[i]);
        sort(arr.begin(), arr.end(),
             [](auto &a, auto &b)
             { return a.first < b.first; });
        double best_ig = -1.0;
        // try midpoints
        for (int i = 1; i < n; ++i)
        {
            if (arr[i - 1].first == arr[i].first)
                continue;
            double thr = (arr[i - 1].first + arr[i].first) / 2;
            vector<string> left_y, right_y;
            for (auto &p : arr)
            {
                if (p.first <= thr)
                    left_y.push_back(p.second);
                else
                    right_y.push_back(p.second);
            }
            double Hs = entropy(y);
            double H_cond = double(left_y.size()) / n * entropy(left_y) + double(right_y.size()) / n * entropy(right_y);
            double ig = Hs - H_cond;
            if (ig > best_ig)
            {
                best_ig = ig;
                best_threshold = thr;
            }
        }
        if (best_ig < 0)
            best_ig = 0;
        if (type == CriterionType::IG)
            return best_ig;
        // for IGR & NWIG, approximate k=2
        if (type == CriterionType::IGR)
        {
            double iv = 1.0; // for binary split, iv = 1
            return best_ig;
        }
        double denom = log2(3.0);
        double factor = 1.0 - 1.0 / X.size();
        return (best_ig / denom) * factor;
    }

    static double information_gain_cat(const vector<vector<string>> &X,
                                       const vector<string> &y,
                                       int attr)
    {
        double Hs = entropy(y);
        int n = X.size();
        unordered_map<string, vector<string>> groups;
        for (int i = 0; i < n; ++i)
            groups[X[i][attr]].push_back(y[i]);
        double H_cond = 0;
        for (auto &g : groups)
        {
            H_cond += double(g.second.size()) / n * entropy(g.second);
        }
        return Hs - H_cond;
    }
    static double intrinsic_value_cat(const vector<vector<string>> &X,
                                      int attr)
    {
        int n = X.size();
        unordered_map<string, int> cnt;
        for (auto &row : X)
            cnt[row[attr]]++;
        double iv = 0;
        for (auto &p : cnt)
        {
            double frac = double(p.second) / n;
            iv -= frac * log2(frac);
        }
        return iv;
    }
    static int unique_count_cat(const vector<vector<string>> &X,
                                int attr)
    {
        unordered_set<string> s;
        for (auto &row : X)
            s.insert(row[attr]);
        return s.size();
    }
};

// DecisionTree class
class DecisionTree
{
public:
    DecisionTree(CriterionType ct, int md)
        : criterion(ct), max_depth(md) {}
    ~DecisionTree() { delete root; }

    void train(const vector<vector<string>> &X,
               const vector<string> &y)
    {
        n_features = X[0].size();
        is_numeric = detect_numeric(X);
        root = build(X, y, 0, is_numeric);
    }

    string predict(const vector<string> &instance) const
    {
        Node *node = root;
        while (!node->is_leaf)
        {
            if (node->is_numeric_split)
            {
                double val = stod(instance[node->attr_index]);
                node = (val <= node->threshold) ? node->left : node->right;
            }
            else
            {
                auto it = node->children.find(instance[node->attr_index]);
                if (it != node->children.end())
                    node = it->second;
                else
                    break;
            }
        }
        return node->label;
    }

private:
    Criterion criterion;
    int max_depth;
    Node *root = nullptr;
    int n_features;
    vector<bool> is_numeric;

    vector<bool> detect_numeric(const vector<vector<string>> &X)
    {
        vector<bool> is_num(X[0].size(), true);
        regex num_re("^-?\\d+(\\.\\d+)?");
        for (int j = 0; j < X[0].size(); ++j)
        {
            for (auto &row : X)
            {
                if (!regex_match(row[j], num_re))
                {
                    is_num[j] = false;
                    break;
                }
            }
        }
        return is_num;
    }

    Node *build(const vector<vector<string>> &X,
                const vector<string> &y,
                int depth,
                const vector<bool> &is_num)
    {
        Node *node = new Node();
        if (all_same(y) || depth == max_depth || X.empty())
        {
            node->is_leaf = true;
            node->label = majority(y);
            return node;
        }
        double best_score = -1;
        int best_attr = -1;
        double best_thr = 0;
        for (int j = 0; j < X[0].size(); ++j)
        {
            double thr = 0;
            double sc = criterion.score(X, y, j, is_num[j], thr);
            if (sc > best_score)
            {
                best_score = sc;
                best_attr = j;
                best_thr = thr;
            }
        }
        if (best_attr < 0)
        {
            node->is_leaf = true;
            node->label = majority(y);
            return node;
        }
        node->attr_index = best_attr;
        if (is_num[best_attr])
        {
            node->is_numeric_split = true;
            node->threshold = best_thr;
            vector<vector<string>> Xl, Xr;
            vector<string> yl, yr;
            for (int i = 0; i < X.size(); ++i)
            {
                double v = stod(X[i][best_attr]);
                if (v <= best_thr)
                {
                    Xl.push_back(X[i]);
                    yl.push_back(y[i]);
                }
                else
                {
                    Xr.push_back(X[i]);
                    yr.push_back(y[i]);
                }
            }
            node->left = build(Xl, yl, depth + 1, is_num);
            node->right = build(Xr, yr, depth + 1, is_num);
        }
        else
        {
            map<string, vector<vector<string>>> groupsX;
            map<string, vector<string>> groupsY;
            for (int i = 0; i < X.size(); ++i)
            {
                groupsX[X[i][best_attr]].push_back(X[i]);
                groupsY[X[i][best_attr]].push_back(y[i]);
            }
            for (auto &g : groupsX)
            {
                vector<vector<string>> X_child = remove_attr(g.second, best_attr);
                vector<bool> is_num_child = is_num;
                is_num_child.erase(is_num_child.begin() + best_attr);
                node->children[g.first] = build(X_child, groupsY[g.first], depth + 1, is_num_child);
            }
        }
        return node;
    }

    static bool all_same(const vector<string> &y)
    {
        return all_of(y.begin(), y.end(), [&](const string &v)
                      { return v == y[0]; });
    }
    static string majority(const vector<string> &y)
    {
        unordered_map<string, int> cnt;
        for (auto &v : y)
            cnt[v]++;
        return max_element(cnt.begin(), cnt.end(),
                           [](auto &a, auto &b)
                           { return a.second < b.second; })
            ->first;
    }
    static vector<vector<string>> remove_attr(
        const vector<vector<string>> &X, int idx)
    {
        vector<vector<string>> R;
        for (auto &row : X)
        {
            vector<string> r = row;
            r.erase(r.begin() + idx);
            R.push_back(r);
        }
        return R;
    }
};

// Impute missing and load
void load_data(const string &file,
               vector<vector<string>> &X,
               vector<string> &y)
{
    ifstream in(file);
    string line;
    // read header
    if (!getline(in, line))
    {
        cerr << "[ERROR] Could not read header from file: " << file << endl;
        return;
    }
    int header_cols = 0;
    {
        stringstream ss(line);
        string tok;
        while (getline(ss, tok, ','))
            header_cols++;
    }
    vector<vector<string>> raw;
    int expected_cols = header_cols;
    int line_num = 1;
    while (getline(in, line))
    {
        line_num++;
        if (line.empty())
            continue;
        stringstream ss(line);
        string tok;
        vector<string> row;
        while (getline(ss, tok, ','))
        {
            row.push_back(tok == "?" ? "" : tok);
        }
        if (row.size() != expected_cols)
        {
            cerr << "[WARN] Skipping malformed line " << line_num << ": expected " << expected_cols << ", got " << row.size() << endl;
            continue; // skip malformed lines
        }
        raw.push_back(row);
    }
    if (raw.empty())
    {
        cerr << "[ERROR] No valid data rows loaded from file: " << file << endl;
        return;
    }
    // Detect if file is Iris (skip first column) or not
    bool is_iris = file.find("Iris") != string::npos || file.find("iris") != string::npos;
    int cols = is_iris ? (raw[0].size() - 2) : (raw[0].size() - 1);
    if (cols < 1)
    {
        cerr << "[ERROR] Not enough columns in data." << endl;
        return;
    }
    // compute most common per column
    vector<string> common(cols);
    if (is_iris)
    {
        for (int j = 1; j <= cols; ++j)
        {
            unordered_map<string, int> cnt;
            for (auto &r : raw)
                if (j < r.size() && !r[j].empty())
                    cnt[r[j]]++;
            if (!cnt.empty())
                common[j - 1] = max_element(cnt.begin(), cnt.end(),
                                            [](auto &a, auto &b)
                                            { return a.second < b.second; })
                                    ->first;
            else
                common[j - 1] = "";
        }
        for (auto &r : raw)
        {
            if (r.size() < cols + 2)
                continue;
            vector<string> ins;
            for (int j = 1; j <= cols; ++j)
                ins.push_back((j < r.size() && !r[j].empty()) ? r[j] : common[j - 1]);
            X.push_back(ins);
            y.push_back(r.back());
        }
    }
    else
    {
        for (int j = 0; j < cols; ++j)
        {
            unordered_map<string, int> cnt;
            for (auto &r : raw)
                if (j < r.size() && !r[j].empty())
                    cnt[r[j]]++;
            if (!cnt.empty())
                common[j] = max_element(cnt.begin(), cnt.end(),
                                        [](auto &a, auto &b)
                                        { return a.second < b.second; })
                                ->first;
            else
                common[j] = "";
        }
        for (auto &r : raw)
        {
            if (r.size() < cols + 1)
                continue;
            vector<string> ins;
            for (int j = 0; j < cols; ++j)
                ins.push_back((j < r.size() && !r[j].empty()) ? r[j] : common[j]);
            X.push_back(ins);
            y.push_back(r.back());
        }
    }
}

int main(int argc, char *argv[])
{
    int t1 = clock();
    if (argc != 4)
    {
        cerr << "Usage: " << argv[0] << " <IG|IGR|NWIG> <maxDepth> <data.csv>\n";
        return 1;
    }
    string c(argv[1]);
    CriterionType ct = CriterionType::IG;
    if (c == "IGR")
        ct = CriterionType::IGR;
    else if (c == "NWIG")
        ct = CriterionType::NWIG;
    int md = stoi(argv[2]);
    vector<vector<string>> X_all;
    vector<string> y_all;
    load_data(argv[3], X_all, y_all);
    int N = X_all.size();
    int n_train = N * 0.8;
    int n_test = N - n_train;
    double sum_acc = 0;
    std::random_device rd;
    std::mt19937 g(rd());
    for (int trial = 0; trial < 20; ++trial)
    {
        // Shuffle
        vector<int> idx(N);
        iota(idx.begin(), idx.end(), 0);
        shuffle(idx.begin(), idx.end(), g);
        vector<vector<string>> X_train, X_test;
        vector<string> y_train, y_test;
        for (int i = 0; i < n_train; ++i)
        {
            X_train.push_back(X_all[idx[i]]);
            y_train.push_back(y_all[idx[i]]);
        }
        for (int i = n_train; i < N; ++i)
        {
            X_test.push_back(X_all[idx[i]]);
            y_test.push_back(y_all[idx[i]]);
        }
        DecisionTree dt(ct, md);
        dt.train(X_train, y_train);
        int correct_test = 0;
        for (int i = 0; i < X_test.size(); ++i)
            if (dt.predict(X_test[i]) == y_test[i])
                correct_test++;
        double acc_test = 100.0 * correct_test / X_test.size();
        int correct_train = 0;
        for (int i = 0; i < X_train.size(); ++i)
            if (dt.predict(X_train[i]) == y_train[i])
                correct_train++;
        double acc_train = 100.0 * correct_train / X_train.size();
        sum_acc += acc_test;
        cout << "Trial " << (trial + 1) << ": Train Accuracy = " << acc_train << "% | Test Accuracy = " << acc_test << "%\n";
    }
    cout << "Average test accuracy over 20 runs: " << sum_acc / 20 << "%\n";
    int t2 = clock();
    cout << "time: " << double(t2 - t1) / CLOCKS_PER_SEC << " seconds\n";
    return 0;
}
