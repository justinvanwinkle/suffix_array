
#include <iostream>
#include "nodes.hpp"


using namespace std;
using namespace Nodes;

int main() {
    vector<string> v = vector<string>({"test", "test"});
    DataNode n(v);

    cout << n.length_of_data() << endl;

}
