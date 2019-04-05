#ifndef SUFFIX_HPP
#define SUFFIX_HPP

//#include "sais.hpp"
#include "sais.hpp"
#include "estl.hpp"
#include "mystack.hpp"
#include <iostream>
#include <map>
#include <stack>
#include <vector>
#include <limits>
#include <vector>
#include<iterator>

namespace Suffix {

using namespace std;

using stack_entry = tuple<int, int, int>;
using sarray_entry = unsigned int;


class SuffixArray {
  public:
    vector<int32_t> s;
    int s_len;
    int num_texts;
    vector<int> max_index_of_text;

    SuffixArray(vector<vector<int32_t>> &texts) {
        num_texts = texts.size();
        for (int text_id = 0; text_id < num_texts; text_id++) {
            auto &text = texts[text_id];
            int text_len = text.size();
            for (int doc_offset = 0; doc_offset < text_len; doc_offset++) {
                text_ids.push_back(text_id);
            }
            text_ids.push_back(text_id);
            for (auto i: text) {
                s.push_back(i);
            }
            //copy(begin(text), end(text), back_inserter(s));
            s.push_back(0);
            max_index_of_text.push_back(s.size() - 1);
        }

        s_len = s.size();
        // cout << s << endl;
        suffix_array.resize(s_len, 0);

        // divsufsort(
        //     (const unsigned char *)s.data(), suffix_array.data(), (int)s_len);
        saisxx(s.data(), suffix_array.data(), (int)s_len);

        //cout << suffix_array << endl;

        setup_lcp();
    }

    int SA(size_t ix) {
        return suffix_array[ix];
    }

    int global_text_start_ix(int text_num) {
        int text_start_ix = 0;
        if (text_num != 0) {
            text_start_ix = max_index_of_text[text_num - 1] + 1;
        };
        return text_start_ix;
    }

    int global_text_end_ix(int text_num) {
        return max_index_of_text[text_num];
    }

    int text_length(int text_num) {
        return global_text_end_ix(text_num) - global_text_start_ix(text_num) +
               1;
    }

    int global_index(int text_num, int index) {
        return global_text_start_ix(text_num) + index;
    }

    int text_index_at(int o, int text_num) {
        int new_ix = o - global_text_start_ix(text_num);
        // cout << new_ix << " " << old_ix << endl;
        return new_ix;
    }

    int text_at(int o) {
        return text_ids[o];
    }

    bool same_text(int o1, int o2) {
        return text_at(o1) == text_at(o2);
    }

    template <class Function>
    void walk_maximal_substrings(int min_length, Function fn) {
        int top = 0;

        my_stack<stack_entry> stack;

        int last_lcp_len = lcp[0];
        int last_string_ix = suffix_array[0];

        for (int low_ix = 1; low_ix < s_len; low_ix++) {
            auto lcp_len = lcp[low_ix];
            auto string_ix = suffix_array[low_ix];
            auto end_ix = max(last_string_ix, string_ix) + lcp_len;
            int lcp_diff = last_lcp_len - lcp_len;

            if (lcp_diff < 0) {
                //cout << "push" << endl;
                stack.emplace(-lcp_diff, low_ix - 1, end_ix);
                top -= lcp_diff;
            } else if (lcp_diff > 0) {
                //cout << "remove_many" << endl;
                top = remove_many(
                    stack, top, lcp_diff, low_ix - 1, min_length, fn);
            } else if (top > 0 and end_ix > get<2>(stack.top())) {
                //cout << "change_top:" << end_ix << endl;
                get<2>(stack.top()) = end_ix;
            }

            // cout << "lcp_len:" << lcp_len
            //      << " string_ix:" << string_ix
            //      << " end_ix:" << end_ix
            //      << " lcp_diff:" << lcp_diff
            //      << " top:" << top << endl;
            // cout << "Stack:" << stack << endl;

            last_lcp_len = lcp_len;
            last_string_ix = string_ix;
        }

        if (top > 0) {
            remove_many(stack, top, top, s_len - 1, min_length, fn);
        }
    }

    ~SuffixArray(){};

    vector<int> suffix_array;
    vector<int> lcp;
    vector<int> text_ids;

    template <class Function>
    int remove_many(stack<stack_entry> &stack,
                    int top,
                    int current_lcp_diff,
                    int end_ix,
                    int min_length,
                    Function fn) {
        int last_start_ix = -1, max_end_ix, lcp_diff, start_ix, nb;

        while (current_lcp_diff > 0) {
            tie(lcp_diff, start_ix, max_end_ix) = stack.top();
            stack.pop();
            if (last_start_ix != start_ix) {
                nb = end_ix - start_ix + 1;
                if (top >= min_length)
                    fn(nb, top, start_ix, max_end_ix);
                last_start_ix = start_ix;
            }
            current_lcp_diff -= lcp_diff;
            top -= lcp_diff;
        }
        if (current_lcp_diff < 0) {
            stack.emplace(-current_lcp_diff,
                          start_ix,
                          max_end_ix - lcp_diff - current_lcp_diff);
            top -= current_lcp_diff;
        }
        return top;
    }

    void setup_lcp() {
        vector<int> rank(s_len, 0);
        for (int i = 0; i < s_len; i++) {
            rank[suffix_array[i]] = i;
        }

        lcp.resize(s_len, 0);

        int l = 0;

        for (int j = 0; j < s_len; j++) {
            if (l != 0)
                --l;

            int i = rank[j];
            if (i != 0) {
                int j2 = suffix_array[i - 1];
                while (j + l < s_len and j2 + l < s_len and
                       s[j + l] == s[j2 + l]) {
                    ++l;
                }

                lcp[i] = l;
            } else {
                l = 0;
            }
        }
        //cout << "num_texts:" << num_texts << endl;

        // int last_left_to_end = 0;
        // for (int suffix_ix = 0; suffix_ix < s_len; suffix_ix++) {
        //     int global_ix = suffix_array[suffix_ix];
        //     int doc_num = text_at(global_ix);
        //     int left_to_end = max_index_of_text[doc_num] - global_ix + 1;
        //     // cout << "ix:" << suffix_ix << " lcp:" << lcp[suffix_ix]
        //     //      << " let_to_end:" << left_to_end
        //     //      << " last_left_to_end:" << last_left_to_end << endl;
        //     last_left_to_end = left_to_end;
        // }

        // for (int ix = 0; ix < s_len; ix++) {
        //     cout << suffix_array[ix] << " ";
        // }
        // cout << endl;
        // for (int ix = 0; ix < s_len; ix++) {
        //     cout << lcp[ix] << " ";
        // }
        // cout << endl;
        // for (int ix = 0; ix < s_len; ix++) {
        //     cout << s[ix] << " ";
        // }
        // cout << endl;
        // cout << "LCP:" << lcp << endl;
    }
};
}; // namespace Suffix

#endif
