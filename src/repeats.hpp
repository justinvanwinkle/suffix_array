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
//#include "divsufsort.h"
#include <math.h>
#include <sstream>

namespace RepeatFinding {

using namespace std;
using namespace Suffix;


typedef pair<int, int> int_pair;
typedef pair<int_pair, int_pair> int_pair_pair;
using stack_entry = tuple<int, int, size_t>;

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

    RepeatFinder(vector<string> texts) : RepeatFinder(SuffixArray(texts)) {
    }

    virtual ~RepeatFinder(){
    };

    RepeatFinderResult rstr() {
        unsigned int i = 0, top = 0, previous_lcp_len = 0;

        auto result = RepeatFinderResult();
        auto &lcp = sa.lcp;

        stack<stack_entry> stack;

        size_t pos1 = sa.suffix_array[0];

        for (i = 0; i < sa.s_len - 2; ++i) {
            size_t current_lcp_len = lcp[i + 1];
            size_t pos2 = sa.suffix_array[i + 1];
            size_t end_ix = max(pos1, pos2) + current_lcp_len;
            int n = previous_lcp_len - current_lcp_len;
            if (n < 0) {
                stack.push(stack_entry(-n, i, end_ix));
                top -= n;
            } else if (n > 0) {
                top = remove_many(stack, top, n, i, result);
            } else if (top > 0 and end_ix > get<2>(stack.top())) {
                get<2>(stack.top()) = end_ix;
            }

            previous_lcp_len = current_lcp_len;
            pos1 = pos2;
        }

        if (top > 0) {
            remove_many(stack, top, top, i + 1, result);
        }

        return result;
    }

    int remove_many(stack<stack_entry> &stack,
                    int top,
                    int m,
                    int end_ix,
                    RepeatFinderResult &result) {
        int last_start_ix = -1, max_end_ix, n, start_ix, nb;

        assert(m >= 0);
        while (m > 0) {
            tie(n, start_ix, max_end_ix) = stack.top();
            stack.pop();
            if (last_start_ix != start_ix) {
                nb = end_ix - start_ix + 1;
                evaluate_match(nb, top, start_ix, result);
                last_start_ix = start_ix;
            }
            m -= n;
            top -= n;
        }
        if (m < 0) {
            stack.push(make_tuple(-m, start_ix, max_end_ix - n - m));
            top -= m;
        }
        return top;
    }

    virtual void
    evaluate_match(int nb, int match_len, int start_ix, RepeatFinderResult &result) {
        if (match_len < 2) {
            return;
        }

        for (unsigned int i = 0; i < sa.num_texts; ++i) {
            sub_results[i] = -1;
            match_count[i] = 0;
        }

        for (int o = start_ix; o < start_ix + nb; ++o) {
            int offset_global = sa.suffix_array[o];
            int id_str = sa.text_at(offset_global);
            int offset = sa.text_index_at(offset_global, id_str);

            ++match_count[id_str];

            if ((sub_results[id_str] == -1) or (offset < sub_results[id_str])) {
                sub_results[id_str] = offset;
            }
        }

        for (unsigned int i = 0; i < sa.num_texts; ++i) {
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
    }
};

class Table {
  public:
    int left_match_length;
    int right_match_length;
    vector<vector<int>> left_extendables;
    vector<vector<int>> right_extendables;
};

class CommonRepeatFinder : public RepeatFinder {
  public:
    vector<Table> tables;
    CommonRepeatFinder(SuffixArray sa) : RepeatFinder(sa) {
    }
    CommonRepeatFinder(vector<string> texts) : RepeatFinder(texts) {
    }

    virtual ~CommonRepeatFinder() {
    }

    virtual void
    evaluate_match(int nb, int match_len, int start_ix, RepeatFinderResult &) {
        if (match_len < 2)
            return;

        vector<int> all_offsets;
        all_offsets.reserve(nb);
        for (int o = start_ix; o < start_ix + nb; ++o) {
            int offset_global = sa.suffix_array[o];
            all_offsets.push_back(offset_global);
        }

        if (extendable(all_offsets, -1) or extendable(all_offsets, match_len)) {
            return;  // Not Maximal
        }
        sort(all_offsets.begin(), all_offsets.end());

        vector<vector<int>> positions(sa.num_texts, vector<int>());
        for (auto &offset_global : all_offsets) {
            int id_str = sa.text_at(offset_global);
            positions[id_str].push_back(offset_global);
        }

        vector<int> lefts;
        vector<int> left_rests;

        vector<int> rights;
        vector<int> right_rests;

        for (auto &file_positions : positions) {
            if (file_positions.empty())
                return;

            lefts.push_back(file_positions[0]);
            rights.push_back(file_positions[file_positions.size() - 1] + match_len - 1);

            if (file_positions.size() < 2)
                continue;

            for (unsigned int ix = 1; ix != file_positions.size(); ++ix) {
                left_rests.push_back(file_positions[ix]);
            }
            for (unsigned int ix = 0; ix != file_positions.size() - 1; ++ix) {
                right_rests.push_back(file_positions[ix] + match_len - 1);
            }
        }

        Table table;
        bool good_table = false;
        if (has_extension(lefts, left_rests, -1)) {
            table.left_extendables = positions;
            table.left_match_length = match_len;
            good_table = true;
        }

        if (has_extension(rights, right_rests, 1)) {
            table.right_extendables = positions;
            table.right_match_length = match_len;
            good_table = true;
        }

        if (good_table)
            tables.push_back(table);
    }

    bool match_tables() {
        vector<Table> good_tables;

        for (auto &left_table : tables) {
            auto &lefts = left_table.left_extendables;
            if (lefts.empty())
                continue;
            for (auto &right_table : tables) {
                auto &rights = right_table.right_extendables;
                if (rights.empty())
                    continue;
                bool match = true;
                bool long_group_seen = false;

                for (size_t doc_id = 0; doc_id != lefts.size(); ++doc_id) {
                    auto &doc_left = lefts[doc_id];
                    auto &doc_right = rights[doc_id];
                    if (doc_left.size() != doc_right.size()) {
                        match = false;
                        break;
                    }

                    if (doc_left.size() > 2) {
                        long_group_seen = true;
                    }

                    for (size_t ix = 0; ix != doc_left.size(); ++ix) {
                        auto &left_offset = doc_left[ix];
                        auto &right_offset = doc_right[ix];

                        if (left_offset >= right_offset) {
                            match = false;
                            break;
                        }

                        if (ix < doc_left.size() - 1) {
                            auto &next_left = doc_left[ix + 1];
                            if ((next_left <= right_offset) or
                                (right_table.right_match_length + right_offset <
                                 next_left)) {
                                match = false;
                                break;
                            }
                        }
                    }
                    if (not match) {
                        break;
                    }
                }
                if (match and long_group_seen) {
                    Table t;
                    t.left_match_length = left_table.left_match_length;
                    t.left_extendables = left_table.left_extendables;
                    t.right_match_length = right_table.right_match_length;
                    t.right_extendables = right_table.right_extendables;
                    good_tables.push_back(t);
                }
            }
        }
        tables = good_tables;

        return true;
    }

  private:
    bool extendable(vector<int> &offsets, int delta) {
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
    }

    bool has_extension(vector<int> &starts, vector<int> &rests, int delta) {
        // There is extension for rests
        if (rests.size() < 3) {
            return false;
        }

        if (not extendable(rests, delta)) {
            return false;
        }

        auto first_extension = sa.s[rests[0] + delta];

        // for (unsigned int i = 0, starts_size=starts.size(); i < starts_size - 2; ++i)
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
    }
};
}

#endif
