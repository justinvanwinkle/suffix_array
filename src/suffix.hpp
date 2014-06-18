#ifndef SUFFIX_HPP
#define SUFFIX_HPP

#include "sais.hpp"

#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace Suffix {

using namespace std;

template <typename T> ostream &operator<<(ostream &s, pair<T, T> t) {
    s << "(" << t.first << ", " << t.second << ")";
    return s;
}

template <typename T> ostream &operator<<(ostream &s, vector<T> t) {
    s << "[";
    for (unsigned int i = 0; i < t.size(); i++)
        s << t[i] << (i == t.size() - 1 ? "" : ", ");
    return s << "]";
}

class SuffixArray {

  public:
    vector<int> suffix_array;
    vector<int> lcp;
    vector<int> rank;
    string s;
    size_t s_len;
    size_t num_texts;
    vector<size_t> length_before_docs;

    SuffixArray(vector<string> &texts) {
        num_texts = texts.size();
        for (auto &text : texts) {
            length_before_docs.push_back(s.length());
            s.append(text);
            s.append("\x02");
        }
        length_before_docs.push_back(s.length());

        s_len = s.length();

        suffix_array.resize(s_len, 0);

        // divsufsort((const unsigned char *)s.data(), suffix_array.data(), len);
        saisxx((const unsigned char *)s.data(), suffix_array.data(), (int)s_len);

        rank.resize(s_len, 0);

        for (unsigned int i = 0; i < s_len; ++i) {
            rank[suffix_array[i]] = i;
        }

        setup_lcp();
    }

    ~SuffixArray(){};

//     SuffixArray(SuffixArray sa, vector<pair<int, int>> windows) {
//         num_texts = 0;
//         vector<string> test_v;
//         vector<pair<int, int>> skips;
//         int last_end = 0;

//         for (auto &key_val : windows) {
//             auto &window_start = key_val.first;
//             auto &window_end = key_val.second;
//             skips.push_back(make_pair(last_end, max(0, window_start - last_end - 1)));
//             last_end = window_end;

//             length_before_docs.push_back(s.length());
//             s.append(sa.s.substr(window_start, window_end - window_start));
//             test_v.push_back(sa.s.substr(window_start, window_end - window_start));
//             s.append("\x02");
//             ++num_texts;
//         }
//         length_before_docs.push_back(s.length());

//         cout << test_v << endl;
//         cout << "windows " << windows << endl << endl;
//         cout << "skips: " << skips << endl;

//         s_len = s.length();

//         suffix_array.resize(s_len, -1);
//         rank.resize(s_len);
//         int new_pos = 0;
//         for (auto &key_val : windows) {
//             auto &window_start = key_val.first;
//             auto &window_end = key_val.second;
//             for (int ix = window_start; ix < window_end; ++ix) {
//                 auto &sa_ix = sa.rank[ix];
//                 auto &sa_val = sa.suffix_array[sa_ix];

//                 auto fixed_sa_ix = get_delta(skips, sa_ix);
//                 auto fixed_sa_val = get_delta(skips, sa_val);
//                 cout << sa_ix << " -ix-> " << fixed_sa_ix << endl;
//                 cout << sa_val << " -> " << fixed_sa_val << endl;
//                 suffix_array[fixed_sa_ix] = fixed_sa_val;
//                 rank[new_pos] = fixed_sa_ix;
//                 ++new_pos;
//             }
//         }

//         vector<int> goofy_sa;

//         for (unsigned int ix=0; ix < num_texts; ++ix) {
//             int index = sa.suffix_array[ix];
//             int new_index = index;
//             for (auto &key_val : skips) {
//                 auto &skip_ix = key_val.first;
//                 auto &delta = key_val.second;
//                 if(index > skip_ix) {
//                     cout << "XXX " << index << ", " << new_index << "  --" << skip_ix << " " << delta << endl;
//                     if (index > skip_ix + delta) {
//                         new_index -= delta;
//                     } else {
//                         new_index -= (index - skip_ix);
//                     }
//                 }
//             }
//             suffix_array[ix] = new_index;
//             goofy_sa.push_back(new_index);
//         }

//         for(size_t ix=0; ix < sa.suffix_array.size(); ++ix) {
//             auto sa_val = sa.suffix_array[ix];

//             for (auto &key_val : windows) {
//                 auto window_start = key_val.first;
//                 auto window_end = key_val.second;
//                 if (sa_val >= window_start and sa_val < window_end) {
//                     goofy_sa.push_back(get_delta(skips, sa_val));
//                 }
//             }
//         }
//         SuffixArray test_sa = SuffixArray(test_v);
//         cout << "old  " << sa.s << endl;
//         cout << "test " << test_sa.s << endl;
//         cout << "new  " << s << endl;

//         cout << "old  " << sa.suffix_array << endl;
//         cout << "test " << test_sa.suffix_array << endl;
//         cout << "goof " << goofy_sa << endl;
//         cout << "new  " << suffix_array << endl << endl;
//         setup_lcp();

//     }

    int text_index_at(size_t o, size_t text_num) {
        return o - length_before_docs[text_num];
    }

    int text_at(size_t o) {
        size_t ix = 0;
        while (length_before_docs[++ix] <= o)
            ;
        return ix - 1;
    }

  protected:
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
        lcp.resize(s_len, 0);

        size_t l {0};

        for (size_t j = 0; j < s_len; ++j) {
            if (l != 0)
                --l;

            size_t i = rank[j];
            if (i != 0) {
                int j2 = suffix_array[i - 1];
                while (j + l < s_len and j2 + l < s_len and s[j + l] == s[j2 + l]) {
                    ++l;
                }

                lcp[i] = l;
            } else {
                l = 0;
            }
        }

        if (not length_before_docs.empty()) {
            // Fix lcp for multi strings
            for (unsigned int doc_idx = 0; doc_idx < length_before_docs.size() - 1;
                 ++doc_idx) {
                int doc_end_pos = length_before_docs[doc_idx + 1] - 1;
                int doc_start_pos = length_before_docs[doc_idx];
                for (int needle_pos = doc_end_pos; needle_pos >= doc_start_pos;
                     --needle_pos) {
                    int lcp_value = lcp[rank[needle_pos]];
                    if (lcp_value > doc_end_pos - needle_pos) {
                        lcp[rank[needle_pos]] = doc_end_pos - needle_pos;
                    }
                }
            }
        }
    }
};

};

#endif
