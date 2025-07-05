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
    string classLabel;

public:
    LeafNode(string cls) : classLabel(cls) {}
    /**
     * The function "predict" returns the class label based on the provided example in C++.
     *
     * @param example The `example` parameter is a vector of strings that likely contains some data or
     * features that can be used for prediction.
     *
     * @return The `classLabel` is being returned.
     */

    string predict(const vector<string> &example) override
    {
        return classLabel;
    }
    /**
     * The function isLeaf() returns true if the current node is a leaf node.
     *
     * @return a boolean value of `true`.
     */

    bool isLeaf() const override
    {
        return true;
    }
};

class DiscreteSplitNode : public Node
{
    int splitAttribute;
    string fallbackClass;
    map<string, unique_ptr<Node>> children;

public:
    DiscreteSplitNode(int attr, string fbClass) : splitAttribute(attr), fallbackClass(fbClass) {}

    /**
     * The function `addChild` adds a child node with a specified value to the current node.
     *
     * @param value The `value` parameter is a constant reference to a string.
     * @param child The `child` parameter is a `unique_ptr` that points to a `Node` object.
     */
    void addChild(const string &value, unique_ptr<Node> child)
    {
        children[value] = move(child);
    }

    /**
     * The function `predict` takes a vector of strings as input, retrieves a value based on a split
     * attribute, and recursively predicts a class using a decision tree structure.
     *
     * @param example The `example` parameter is a vector of strings that represents a single data
     * point or instance in a dataset. Each element in the vector corresponds to a feature or attribute
     * of the data point.
     *
     * @return The `predict` function is returning a string value. If the value corresponding to
     * `example[splitAttribute]` is found in the `children` map, then the function returns the result
     * of recursively calling `predict` on the child node. If the value is not found in the `children`
     * map, then the function returns the `fallbackClass` string.
     */
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

    const map<string, unique_ptr<Node>>& getChildren() const
    {
        return children;
    }

    /**
     * The function isLeaf() returns false indicating that the current node is not a leaf node.
     *
     * @return a boolean value of "false".
     */
    bool isLeaf() const override
    {
        return false;
    }
};

#endif // !_NODE_H_
