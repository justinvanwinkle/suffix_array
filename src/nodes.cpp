
#include "nodes.hpp"
#include "estl.hpp"
#include <future>
#include <mutex>

using namespace Nodes;
using namespace estl;


int main() {
    vector<future<void>> jobs;
    mutex io_mutex;
    strings fns = glob("~/scratch/bestbuy/*", true);
    for (auto &fn0 : fns) {
        for (auto &fn1 : fns) {
            if (fn0.compare(fn1) <= 0)
                continue;

            jobs.push_back(async(launch::async, [&]() {
                float dist = bisect_distance(fn0, fn1);
                io_mutex.lock();
                cout << dist << " " << fn0 << " " << fn1 << endl;
                io_mutex.unlock();
            }));
        }
    }

    for (auto &job : jobs) {
        job.get();
    }
}
