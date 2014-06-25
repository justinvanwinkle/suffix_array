#ifndef REPEATS_HPP
#define REPEATS_HPP

#include "suffix.hpp"

#include <algorithm>
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <math.h>
#include <tuple>
#include <stack>
#include <iostream>
#include <string>
#include "estl.hpp"
//#include "divsufsort.h"
#include <math.h>
#include <sstream>

namespace RepeatFinding {

using namespace std;
using namespace Suffix;


using int_pair = pair<int, int>;
using int_triple = tuple<int, int, int>;
using repeat_map = map<int_pair, int_pair>;


class RepeatFinderResult {
  public:
    int match_length = 0;
    int matching = 0;
    vector<int> matches;
};


class Table {
  public:
    int left_match_length;
    int right_match_length;
    vector<vector<int>> left_extendables;
    vector<vector<int>> right_extendables;
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

    RepeatFinder(vector<string> texts) : RepeatFinder(SuffixArray(texts)) {
    }

    virtual ~RepeatFinder(){};

    RepeatFinderResult rstr() {
        RepeatFinderResult result;
        sa.walk_maximal_substrings([this, &result](
            int nb, int match_len, int start_ix, int) {

            for (int i = 0; i < sa.num_texts; ++i) {
                sub_results[i] = -1;
                match_count[i] = 0;
            }


            for (int o = start_ix; o < start_ix + nb; ++o) {
                int offset_global = sa.suffix_array[o];
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
    };

    repeat_map all_repeats() {
        repeat_map repeats;
        sa.walk_maximal_substrings([&repeats](
            int nb, int match_len, int start_ix, int max_end_ix) {
            int_pair key = make_pair(start_ix, max_end_ix - start_ix + 1);
            auto &val = repeats[key];
            if (get<0>(val) < match_len) {
                val = make_tuple(match_len, start_ix);
            }
        });


        return repeats;
    }

    void print_repeats() {
        repeat_map repeats = all_repeats();

        for (auto &entry : repeats) {
            cout << entry.first << " -> " << entry.second << endl;
        }
    }

    vector<Table> find_tables() {
        auto extendable = [this](vector<int> &offsets, int delta) {
            for (auto &k : offsets) {
                if (k + delta < 0 or k + delta >= (int)sa.s_len) {
                    return false;  // out of bounds
                }
            }

            auto first_extension = sa.s[offsets[0] + delta];
            for (auto &k : offsets) {
                if (sa.s[k + delta] != first_extension) {
                    return false;  // No extension
                }
            }

            return true;
        };

        auto has_extension = [this, extendable](vector<int> &starts,
                                                vector<int> &rests,
                                                int delta) -> bool {
            // There is extension for rests
            if (rests.size() < 3) {
                return false;
            }

            if (not extendable(rests, delta)) {
                return false;
            }

            auto first_extension = sa.s[rests[0] + delta];

            // for (unsigned int i = 0, starts_size=starts.size(); i <
            // starts_size - 2;
            // ++i)
            // {
            for (auto &k : starts) {
                if (k + delta < 0 or k + delta >= (int)sa.s_len) {
                    return false;  // out of bounds
                }
                if (sa.s[k + delta] == first_extension) {
                    return false;
                }
            }

            return true;
        };
        vector<Table> tables;


        return tables;
    }
};
}

#endif
