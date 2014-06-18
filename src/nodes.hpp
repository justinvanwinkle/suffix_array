#ifndef NODES_HPP
#define NODES_HPP

#include <vector>
#include <string>
#include <sstream>
#include <numeric>
#include "repeats.hpp"
#include "estl.hpp"

namespace Nodes {

using namespace std;
using namespace RepeatFinding;
using namespace estl;

class Node {
  public:
    static const bool leaf = false;
    strings texts;
    vector<unique_ptr<Node>> children;

    Node(strings texts, ints grouping = {}) : texts(texts) {
        if (grouping.empty())
            grouping.resize(texts.size(), 1);
    }

    virtual int length_of_data() {
        int total = 0;
        for (auto &child : children) {
            total += child->length_of_data();
        }
        return total;
    }

    virtual int number_of_data_elements() {
        int total = 0;
        for (auto &child : children) {
            total += child->number_of_data_elements();
        }
        return total;
    }

  protected:
    ints grouping;
};

class DataNode : public Node {
  public:
    static const bool leaf = true;
    using Node::Node;

    int length_of_data() override {
        return accumulate(texts.begin(), texts.end(), int(0), [](int sum, string val) {
            return sum + val.size();
        });
    }
};

class BinarySplitNode : public Node {
  public:
    using Node::Node;
};

unique_ptr<Node> construct(strings texts, ints grouping = {}) {
    if (texts.size() < 2)
        return unique_ptr<Node>(new DataNode(texts));
    RepeatFinder rpf(texts);
    RepeatFinderResult result = rpf.rstr();
    if (result.match_length < 2)
        return unique_ptr<Node>(new DataNode(texts));

    vector<string> lefts;
    vector<string> rights;

    enumerate(texts, [&](size_t ix, string s) {
        auto start_offset = result.matches[ix];
        lefts.push_back(s.substr(0, start_offset));
        rights.push_back(s.substr(start_offset + result.match_length));
    });
    unique_ptr<Node> node(new BinarySplitNode(texts, grouping));
    node->children.emplace_back(construct(lefts, grouping));
    node->children.emplace_back(construct(rights, grouping));
    return node;
}
};

#endif
