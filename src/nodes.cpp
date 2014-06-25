
#include "nodes.hpp"
#include "estl.hpp"
#include "repeats.hpp"


using namespace RepeatFinding;
using namespace estl;


int main() {
    strings contents = read_files(glob("~/scratch/target_products_50/*", true));
    RepeatFinder rf(contents);
    // rf.print_repeats();
    // auto repeats = rf.all_repeats();
    // cout << repeats.size() << endl;
    auto result = rf.rstr();
    cout << result.match_length << endl;
}
