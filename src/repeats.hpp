#ifndef REPEATS_HPP
#define REPEATS_HPP

#include "suffix.hpp"

#include <algorithm>
#include <vector>
#include <map>
#include <string>
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
    };

    repeat_map all_repeats() {
        repeat_map repeats;
        sa.walk_maximal_substrings([&repeats](
            int, int match_len, int start_ix, int max_end_ix) {
            int_pair key = make_pair(start_ix, max_end_ix - start_ix + 1);
            auto &val = repeats[key];
            if (get<0>(val) < match_len) {
                val = make_tuple(match_len, start_ix);
            }
        });


        return repeats;
    }

    bool extendable(vector<int> &offsets, int delta) {
        for (unsigned int i = 0; i < offsets.size() - 2; ++i) {
            if (offsets[i] + delta < 0 or offsets[i + 1] + delta < 0 or
                offsets[i] + delta >= sa.s_len or
                offsets[i + 1] + delta >= sa.s_len) {
                return false;  // out of bounds
            } else if (sa.s[offsets[i] + delta] != sa.s[offsets[i + 1] + delta]) {
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

        int first_rest = rests[0];

        for (unsigned int i = 0; i < starts.size() - 2; ++i) {
            if (starts[i] + delta < 0 or starts[i + 1] + delta < 0 or
                starts[i] + delta >= sa.s_len or
                starts[i + 1] + delta >= sa.s_len) {
                return false;  // out of bounds
            } else if (sa.s[starts[i] + delta] == sa.s[first_rest + delta]) {
                return false;
            }
        }

        return true;
    }

    vector<Table> find_tables() {
        vector<Table> tables;

        sa.walk_maximal_substrings([this, &tables](
            int nb, int match_len, int start_ix, int) {
            if (match_len < 5)
                return;

            vector<vector<int>> positions(sa.num_texts, vector<int>());
            vector<int> all_offsets;
            all_offsets.reserve(nb);
            for (int o = start_ix; o < start_ix + nb; ++o) {
                int offset_global = sa.SA(o);
                int id_str = sa.text_at(offset_global);
                positions[id_str].push_back(offset_global);
                all_offsets.push_back(offset_global);
            }

            // int prev_offset = -1;
            // bool maximal = false;
            // for (auto &offset : all_offsets) {
            //     if (offset == 0)
            // 	return;

            //     if (prev_offset != -1 and
            // 	combined_texts[prev_offset - 1] != combined_texts[offset] and
            // 	prev_offset

            if (extendable(all_offsets, -1) or
                extendable(all_offsets, match_len)) {
                return;
            }

            vector<int> lefts;
            vector<int> left_rests;

            vector<int> rights;
            vector<int> right_rests;

            for (auto &file_positions : positions) {
                if (file_positions.empty())
                    return;

                sort(file_positions.begin(), file_positions.end());

                lefts.push_back(file_positions[0]);
                rights.push_back(file_positions[file_positions.size() - 1] +
                                 match_len - 1);

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
        });

        vector<Table> good_tables;

        for (auto &left_table : tables) {
            auto &lefts = left_table.left_extendables;
            auto lefts_size = lefts.size();
            if (not lefts_size)
                continue;
            if (left_table.left_match_length != 25 or lefts[0].size() != 8)
                continue;
            cout << lefts << " -- " << left_table.left_match_length << endl;

            for (auto &right_table : tables) {
                auto &rights = right_table.right_extendables;
                auto rights_size = rights.size();
                if (not rights_size)
                    continue;
                if (right_table.right_match_length != 16)
                    continue;

                bool match = true;

                for (size_t doc_id = 0; doc_id != lefts_size; ++doc_id) {
                    auto &doc_left = lefts[doc_id];
                    auto doc_left_size = doc_left.size();
                    auto &doc_right = rights[doc_id];
                    auto doc_right_size = doc_right.size();

                    if (doc_left_size != doc_right_size) {
                        match = false;
                        break;
                    }

                    for (size_t ix = 0; ix != doc_left_size; ++ix) {
                        auto &left_offset = doc_left[ix];
                        auto &right_offset = doc_right[ix];

                        if (left_offset >= right_offset) {
                            cout << "NOT ZIPPED " << rights << " -- "
                                 << right_table.right_match_length << endl;
                            match = false;
                            break;
                        }

                        if (ix < doc_left_size - 1) {
                            auto &next_left = doc_left[ix + 1];
                            if ((right_table.right_match_length + right_offset !=
                                 next_left)) {
                                cout << "NOT EXACT " << rights << " -- "
                                     << right_table.right_match_length << endl;
                                match = false;
                                break;
                            }
                        }
                    }
                    if (not match) {
                        break;
                    }
                }
                if (match) {
                    Table t;
                    t.left_match_length = left_table.left_match_length;
                    t.left_extendables = left_table.left_extendables;
                    t.right_match_length = right_table.right_match_length;
                    t.right_extendables = right_table.right_extendables;
                    good_tables.push_back(t);
                }
            }
        }
        return good_tables;
    }
};
}

#endif
