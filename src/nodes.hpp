#ifndef NODES_HPP
#define NODES_HPP

#include <vector>
#include <string>
#include <sstream>
#include "repeats.hpp"


namespace Nodes {

using namespace std;
using namespace RepeatFinding;

string html_list(vector<string> ss, string tag = "td") {
    ostringstream os;
    for (auto &s : ss) {
        os << "\n <" << tag << ">"
           << "\n   <div class=\"dataXX\">\n" << s // needs to be escaped!
           << "\n    </div>\n  </" << tag << ">";
    }

    return os.str();
}

class Node {
  public:
    static const bool leaf = false;
    Node(){};

    Node(vector<string> texts, vector<int> grouping = {}) {
        if (grouping.empty())
            grouping.resize(texts.size(), 1);
    }

    virtual int length_of_data() {
        int total = 0;
        for (auto &child : children) {
            total += child.length_of_data();
        }
        return total;
    }

    virtual int number_of_data_elements() {
        int total = 0;
        for (auto &child : children) {
            total += child.number_of_data_elements();
        }
        return total;
    }

    virtual Node construct() {}
    virtual string to_html() {return "Not Implemented";}

  protected:
    vector<Node> children;
    vector<string> texts;
    vector<int> grouping;
};

class DataNode : public Node {

  public:
    static const bool leaf = true;
    DataNode(vector<string> texts, vector<int> grouping = {}) : Node(texts, grouping) {}

    string to_html() {
        ostringstream os;
        os << "<tr>" << html_list(texts) << "</tr>";
        return os.str();
    }
};

class BinarySplitNode : public Node {
public:
    virtual Node construct() {
        if (texts.size() < 2)
            return DataNode(texts);
        RepeatFinder rpf(texts);
        RepeatFinderResult rstr = rpf.rstr();
        if (rstr.match_length < 2)
            return DataNode(texts);


    }

};
};

#endif
