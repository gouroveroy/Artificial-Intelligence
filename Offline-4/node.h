#ifndef _NODE_H_
#define _NODE_H_

#include <bits/stdc++.h>

using namespace std;

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

class DiscreteSplitNode : public Node
{
private:
    int splitAttribute;
    string fallbackClass;
    map<string, unique_ptr<Node>> children;

public:
    DiscreteSplitNode(int attr, string fbClass) : splitAttribute(attr), fallbackClass(fbClass) {}
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
    bool isLeaf() const override { return false; }
};

#endif // !_NODE_H_
