#ifndef SUFFIX_HPP
#define SUFFIX_HPP

#include "sais.hpp"

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <iostream>

namespace Suffix {

using namespace std;

using stack_entry = tuple<int, int, int>;


class SuffixArray {
  public:
    string s;
    int s_len;
    int num_texts;
    vector<int> length_before_docs;

    SuffixArray(vector<string> &texts) {
        num_texts = texts.size();
        for (int text_id = 0; text_id < num_texts; ++text_id) {
            auto &text = texts[text_id];
            int text_len = text.length();
            for (int doc_offset = 0; doc_offset < text_len; ++doc_offset) {
                text_ids.push_back(text_id);
            }
            text_ids.push_back(text_id);

            length_before_docs.push_back(s.length());
            s.append(text);
            s.append("\x02");
        }
        length_before_docs.push_back(s.length());

        s_len = s.length();

        suffix_array.resize(s_len, 0);

        // divsufsort((const unsigned char *)s.data(), suffix_array.data(),
        // len);
        saisxx((const unsigned char *)s.data(), suffix_array.data(), (int)s_len);

        setup_lcp();
    }

    int SA(size_t ix) {
        return suffix_array[ix];
    }

    int text_index_at(int o, int text_num) {
        return o - length_before_docs[text_num];
    }

    int text_at(int o) {
        return text_ids[o];
    }

    template <class Function>
    void walk_maximal_substrings(Function fn) {
        int top = 0;

        stack<stack_entry> stack;

        int last_lcp_len = lcp[0];
        int last_string_ix = suffix_array[0];

        for (int low_ix = 1; low_ix < s_len - 1; ++low_ix) {
            auto &lcp_len = lcp[low_ix];
            auto &string_ix = suffix_array[low_ix];
            auto end_ix = max(last_string_ix, string_ix) + lcp_len;
            int lcp_diff = last_lcp_len - lcp_len;
            if (lcp_diff < 0) {
                stack.emplace(-lcp_diff, low_ix - 1, end_ix);
                top -= lcp_diff;
            } else if (lcp_diff > 0) {
                top = remove_many(stack, top, lcp_diff, low_ix - 1, fn);
            } else if (top > 0 and end_ix > get<2>(stack.top())) {
                get<2>(stack.top()) = end_ix;
            }

            last_lcp_len = lcp_len;
            last_string_ix = string_ix;
        }

        if (top > 0) {
            remove_many(stack, top, top, s_len - 1, fn);
        }
    }

    ~SuffixArray(){};

  protected:
    vector<int> suffix_array;
    vector<int> lcp;
    vector<short> text_ids;

    template <class Function>
    int remove_many(stack<stack_entry> &stack,
                    int top,
                    int m,
                    int end_ix,
                    Function fn) {
        int last_start_ix = -1, max_end_ix, lcp_diff, start_ix, nb;

        while (m > 0) {
            tie(lcp_diff, start_ix, max_end_ix) = stack.top();
            stack.pop();
            if (last_start_ix != start_ix) {
                nb = end_ix - start_ix + 1;
                if (top > 1)
                    fn(nb, top, start_ix, max_end_ix);
                last_start_ix = start_ix;
            }
            m -= lcp_diff;
            top -= lcp_diff;
        }
        if (m < 0) {
            stack.push(make_tuple(-m, start_ix, max_end_ix - lcp_diff - m));
            top -= m;
        }
        return top;
    }

    int get_delta(vector<pair<int, int>> &skips, int index) {
        int new_index = index;
        for (auto &key_val : skips) {
            auto &skip_ix = key_val.first;
            auto &delta = key_val.second;
            if (index >= skip_ix) {
                new_index -= delta;
            }
        }
        return new_index;
    }

    void setup_lcp() {
        vector<int> rank(s_len, 0);

        for (int i = 0; i < s_len; ++i) {
            rank[suffix_array[i]] = i;
        }

        lcp.resize(s_len, 0);

        int l = 0;

        for (int j = 0; j < s_len; ++j) {
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

        if (not length_before_docs.empty()) {
            // Fix lcp for multi strings
            for (int doc_idx = 0, max = length_before_docs.size() - 1;
                 doc_idx < max;
                 ++doc_idx) {
                int doc_end_pos = length_before_docs[doc_idx + 1] - 1;
                int doc_start_pos = length_before_docs[doc_idx];
                for (int needle_pos = doc_end_pos; needle_pos >= doc_start_pos;
                     --needle_pos) {
                    int &lcp_value = lcp[rank[needle_pos]];
                    lcp_value = min(doc_end_pos - needle_pos, lcp_value);
                }
            }
        }
    }
};
};

#endif
