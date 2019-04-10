#ifndef CONSTRUCT_SUFFIX_HPP
#define CONSTRUCT_SUFFIX_HPP

#include <algorithm>
#include <array>
#include <vector>
using namespace std;

struct suffix {
    int index;
    array<int, 2> rank; // To store ranks and next rank pair
};

// A comparison function used by sort() to compare two suffixes
// Compares two pairs, returns 1 if first pair is smaller
int cmp(struct suffix a, struct suffix b) {
    return (a.rank[0] == b.rank[0]) ? (a.rank[1] < b.rank[1] ? 1 : 0)
                                    : (a.rank[0] < b.rank[0] ? 1 : 0);
}

template <typename T>
vector<int> buildSuffixArray(T &txt) {
    auto n = txt.size();
    vector<suffix> suffixes(n);

    // Store suffixes and their indexes in an array of structures.
    // The structure is needed to sort the suffixes alphabatically
    // and maintain their old indexes while sorting
    for (int i = 0; i < n; i++) {
        suffixes[i].index = i;
        suffixes[i].rank[0] = txt[i] - 'a';
        suffixes[i].rank[1] = ((i + 1) < n) ? (txt[i + 1] - 'a') : -1;
    }

    // Sort the suffixes using the comparison function
    // defined above.
    sort(begin(suffixes), end(suffixes), cmp);

    // At this point, all suffixes are sorted according to first
    // 2 characters.  Let us sort suffixes according to first 4
    // characters, then first 8 and so on
    vector<int> ind(n);

    for (int k = 4; k < 2 * n; k = k * 2) {
        // Assigning rank and index values to first suffix
        int rank = 0;
        int prev_rank = suffixes[0].rank[0];
        suffixes[0].rank[0] = rank;
        ind[suffixes[0].index] = 0;

        // Assigning rank to suffixes
        for (int i = 1; i < n; i++) {
            if (suffixes[i].rank[0] == prev_rank &&
                suffixes[i].rank[1] == suffixes[i - 1].rank[1]) {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = rank;
            } else {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = ++rank;
            }
            ind[suffixes[i].index] = i;
        }

        // Assign next rank to every suffix
        for (int i = 0; i < n; i++) {
            int nextindex = suffixes[i].index + k / 2;
            suffixes[i].rank[1] =
                (nextindex < n) ? suffixes[ind[nextindex]].rank[0] : -1;
        }

        // Sort the suffixes according to first k characters
        sort(begin(suffixes), end(suffixes), cmp);
    }

    vector<int> suffixArr = vector<int>(n);
    for (int i = 0; i < n; i++)
        suffixArr[i] = suffixes[i].index;

    // Return the suffix array
    return suffixArr;
}

#endif
