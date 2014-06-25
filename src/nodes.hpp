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

    virtual size_t length_of_data() {
        size_t total = 0;
        for (auto &child : children) {
            total += child->length_of_data();
        }
        return total;
    }

    virtual size_t length_of_data(size_t text_ix) {
        size_t total = 0;
        for (auto &child : children) {
            total += child->length_of_data(text_ix);
        }
        return total;
    }

    virtual size_t number_of_data_elements() {
        size_t total = 0;
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
    DataNode(strings texts, ints groupings = {}) : Node(texts, groupings){};

    size_t length_of_data() override {
        return accumulate(texts.begin(),
                          texts.end(),
                          size_t(0),
                          [](size_t sum, string val) { return sum + val.size(); });
    }

    size_t length_of_data(size_t text_ix) override {
        return texts[text_ix].length();
    }
};

class BinarySplitNode : public Node {
  public:
    BinarySplitNode(strings texts, ints groupings = {}) : Node(texts, groupings){};
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

double bisect_distance(string fn0, string fn1) {
    string content0 = read_file(fn0);
    string content1 = read_file(fn1);

    // size_t size0 = content0.size();
    // size_t size1 = content1.size();

    unique_ptr<Node> root = construct({content0, content1});
    // size_t data_left0 = root->length_of_data(0);
    // size_t data_left1 = root->length_of_data(1);

    return root->length_of_data();
}
};

#endif
