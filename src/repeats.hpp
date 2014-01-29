#ifndef REPEATS_HPP
#define REPEATS_HPP
/**
 * Local Variables:
 * flycheck-clang-language-standard: "c++11"
 * flycheck-clang-include-path: ("../include")
 * flycheck-clang-warnings: ("all" "extra")
 * End:
 */

#include <algorithm>
#include <vector>
#include <string>
#include <tuple>
#include <stack>
#include <unordered_map>
#include <map>
#include <iostream>
#include "divsufsort.h"


class SuffixArray {

 private:

  int numdocs;


 public:
  int len;
  const unsigned char* s;
  int* suffix_array;

  SuffixArray(const unsigned char* _s, int _s_length, int _numdocs=0) {
    len = _s_length;
    s = _s;
    numdocs = _numdocs;
    suffix_array = (int *)calloc(len, sizeof(int));
    divsufsort(s, suffix_array, len);

  }

  ~SuffixArray() {
    free(suffix_array);
  }

  std::vector<int>* lcp () {
    auto lcp_p = new std::vector<int>;
    auto &lcp = *lcp_p;
    lcp.resize(len, 0);
    std::vector<int> rank;
    rank.resize(len, 0);

    for(int i=numdocs; i < len; ++i){
      rank[suffix_array[i]] = i;
    }
    int l = 0;

    lcp[0] = -1;
    for(int j=0; j < len; ++j) {
      if(l != 0)
        --l;

      int i = rank[j];
      int j2 = suffix_array[i - 1];

      if(i != 0) {
        while(j + l < len and j2 + l < len and s[j + l] == s[j2 + l]) {
          l += 1;
        }

        lcp[i] = l;
      } else {
        l = 0;
      }
    }
    return lcp_p;
  }
};


typedef std::pair<int, int> int_pair;
typedef std::pair<int_pair, int_pair> int_pair_pair;

namespace std {
template <>
struct hash<int_pair> // denotes a specialization of hash<...>
{
  size_t operator() (const int_pair& t) const
  {
    return hash<int>()(t.first) ^ hash<int>()(t.second);
  }
};
}

typedef std::map<int_pair, int_pair> int_tuple_map;
typedef std::tuple<int, int, int> int_trip;

class RepeatFinder {

 private:

  int num_texts;
  std::vector<int> text_positions;
  std::string* combined_texts;
  SuffixArray *sa;
  int min_matching;

 public:

  int match_length = 0;
  std::vector<int> matches;

  RepeatFinder(std::vector<std::string> &texts, int _min_matching) {
    min_matching = _min_matching;
    num_texts = texts.size();

    combined_texts = new std::string;
    for(auto s : texts) {
      combined_texts->append(s);
      combined_texts->append("\2");
      text_positions.push_back(combined_texts->length());
    }
    const unsigned char* c_str = (const unsigned char*) combined_texts->c_str();
    sa = new SuffixArray(c_str, combined_texts->length(), num_texts);
  }

  ~RepeatFinder() {
    delete sa;
    delete combined_texts;
  }

  int text_index_at(int q) {
    int index_at = text_at(q);
    int start = 0;
    if(index_at != 0)
      start = text_positions[index_at - 1];
    return q - start;
  }

  int text_at(int q) {
    auto low = lower_bound(text_positions.begin(),
                           text_positions.end(),
                           q);
    int ix = low - text_positions.begin();
    if(text_positions[ix] == q)
      ix--;
    return ix;
  }


  int_tuple_map* rstr() {
    int i = 0,
        top = 0,
        previous_lcp_len = 0,
        len_lcp = sa->len - 1;
    auto results = new int_tuple_map;
    if(sa->len == 0)
      return results;

    auto &lcp = *sa->lcp();

    std::stack<int_trip> stack;

    int pos1 = sa->suffix_array[0];

    for(i=0; i < len_lcp; ++i) {
      int current_lcp_len = lcp[i + 1];
      int pos2 = sa->suffix_array[i + 1];
      int end_ix = std::max(pos1, pos2) + current_lcp_len;
      int n = previous_lcp_len - current_lcp_len;
      if(n < 0) {
        stack.push(int_trip(-n, i, end_ix));
        top -= n;
      } else if(n > 0) {
        top = remove_many(stack, results, top, n, i);
      } else if(top > 0 and end_ix > std::get<2>(stack.top())) {
        std::get<2>(stack.top()) = end_ix;
      }
      previous_lcp_len = current_lcp_len;
      pos1 = pos2;
    }

    if(top > 0) {
      top = remove_many(stack, results, top, top, i + 1);
    }

    return results;
  }



  int remove_many(std::stack<int_trip> &stack,
                  int_tuple_map *results,
                  int top,
                  int m,
                  int end_ix) {
    int last_start_ix = -1,
        max_end_ix,
        n,
        start_ix,
        nb;

    while(m > 0) {
      std::tie(n, start_ix, max_end_ix) = stack.top();
      stack.pop();
      if(last_start_ix != start_ix) {
        nb = end_ix - start_ix + 1;
        if(nb >= min_matching and top > 1) {
          int_pair id(max_end_ix, nb);
          auto entry = results->find(id);
          if(entry == results->end()) {
            results->insert(int_pair_pair(id, int_pair(top, start_ix)));
          } else if (entry->second.first < top) {
            entry->second.first = top;
            entry->second.second = start_ix;
          }
        }
        last_start_ix = start_ix;
      }
      m -= n;
      top -= n;
    }
    if(m < 0) {
      stack.push(std::make_tuple(-m, start_ix, max_end_ix - n - m));
      top -= m;
    }
    return top;
  }


  void go_rstr() {
    int offset_end = 0,
        nb = 0,
        match_len = 0,
        start_ix = 0,
        most_docs = 0,
        largest = 0;

    if(num_texts < min_matching)
      return;

    auto results = rstr();
    std::vector<int> best_results;
    for (auto it = results->begin(); it != results->end(); ++it ) {
      std::tie(offset_end, nb) = it->first;
      std::tie(match_len, start_ix) = it->second;

      if(match_len < 2 or nb < min_matching)
        continue;

      std::vector<int> sub_results;
      for(int i=0; i < num_texts; ++i)
        sub_results.push_back(-1);

      for(int o=start_ix; o < start_ix + nb; ++o) {
        int offset_global = sa->suffix_array[o];
        int offset = text_index_at(offset_global);
        int id_str = text_at(offset_global);
        int id_str_end = text_at(offset_global + match_len);
        if(id_str == id_str_end and sub_results[id_str] == -1)
          sub_results[id_str] = offset;
      }

      int hit_docs = num_texts;
      for(int match_start : sub_results) {
        if(match_start == -1)
          --hit_docs;
      }

      if(hit_docs >= min_matching) {
        if(hit_docs > most_docs or
           (hit_docs >= most_docs and match_len > largest)) {
          most_docs = hit_docs;
          largest = match_len;
          best_results = sub_results;
        }
      }
    }

    matches = best_results;
    match_length = largest;
  }

};


#endif
