#ifndef REPEATS_HPP
#define REPEATS_HPP

#include "suffix.hpp"

#include <algorithm>
#include <vector>
#include <map>
#include <stack>
#include <iostream>
#include <string>
#include "estl.hpp"

namespace RepeatFinding {

using namespace std;
using namespace Suffix;
using namespace estl;

using int_pair = pair<int, int>;
using repeat_map = map<int_pair, int_pair>;


class RepeatFinderResult {
  public:
    int match_length = 0;
    int matching = 0;
    vector<int> matches;
};


class RepeatFinder {
  protected:
    vector<int> sub_results;
    vector<int> match_count;

  public:
    SuffixArray sa;

    RepeatFinder(SuffixArray sa)
        : sub_results(sa.num_texts), match_count(sa.num_texts), sa(sa) {
    }

    RepeatFinder(vector<vector<int>> texts) : RepeatFinder(SuffixArray(texts)) {
    }

    virtual ~RepeatFinder(){};

    RepeatFinderResult rstr() {
        RepeatFinderResult result;
        sa.walk_maximal_substrings(2, [this, &result](
            int nb, int match_len, int start_ix, int) {

            for (int i = 0; i < sa.num_texts; ++i) {
                sub_results[i] = -1;
                match_count[i] = 0;
            }


            for (int o = start_ix; o < start_ix + nb; ++o) {
                int offset_global = sa.SA(o);
                int id_str = sa.text_at(offset_global);
                int offset = sa.text_index_at(offset_global, id_str);

                ++match_count[id_str];

                if ((sub_results[id_str] == -1) or
                    (offset < sub_results[id_str])) {
                    sub_results[id_str] = offset;
                }
            }

            for (int i = 0; i < sa.num_texts; ++i) {
                if (sub_results[i] == -1)
                    return;
            }

            int first_count = match_count[0];
            for (auto &k : match_count) {
                if (k != first_count)
                    return;
            }

            for (auto &k : sub_results) {
                if (k == -1)
                    return;
            }

            if ((match_len > result.match_length)) {
                result.matching = sa.num_texts;
                result.match_length = match_len;
                result.matches = sub_results;
            }
        });
        return result;
    }

    string LCS() {
        int best_start_ix = 0;
        int best_match_len = 0;
        sa.walk_maximal_substrings(1, [this, &best_start_ix, &best_match_len](
            int nb, int match_len, int start_ix, int) {

            for (int i = 0; i < sa.num_texts; ++i) {
                match_count[i] = 0;
            }


            for (int o = start_ix; o < start_ix + nb; ++o) {
                int offset_global = sa.SA(o);
                int id_str = sa.text_at(offset_global);
                // int offset = sa.text_index_at(offset_global, id_str);

                ++match_count[id_str];
            }

            for (int i = 0; i < sa.num_texts; ++i) {
                if (match_count[i] == 0)
                    return;
            }

            if (match_len > best_match_len) {
                best_match_len = match_len;
                best_start_ix = start_ix;
            }
        });
        string lcs = sa.s.substr(sa.SA(best_start_ix), best_match_len);
        return lcs;
    }

    vector<string> all_repeats() {
        vector<string> repeats;
        sa.walk_maximal_substrings(1, [this, &repeats](
            int nb, int match_len, int start_ix, int) {
            for (int i = 0; i < sa.num_texts; ++i) {
                match_count[i] = 0;
            }

            for (int o = start_ix; o < start_ix + nb; ++o) {
                int offset_global = sa.SA(o);
                int id_str = sa.text_at(offset_global);
                // int offset = sa.text_index_at(offset_global, id_str);

                ++match_count[id_str];
            }

            for (int i = 0; i < sa.num_texts; ++i) {
                if (match_count[i] == 0)
                    break;

                for (int i = 0; i < sa.num_texts; ++i) {
                    if (match_count[i] == 0)
                        return;
                }
            }
            repeats.push_back(sa.s.substr(sa.SA(start_ix), match_len));
        });
        return repeats;
    }

};
}

#endif
