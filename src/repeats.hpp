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

public:
    int len;
    const unsigned char* s;
    int* suffix_array;

    SuffixArray(const unsigned char* _s, int _s_length) {
	len = _s_length;
	s = _s;
	suffix_array = (int *)calloc(len, sizeof(int));
	divsufsort(s, suffix_array, len);

    }

    ~SuffixArray() {
	free(suffix_array);
    }

    std::vector<int>* lcp() {
	auto lcp_p = new std::vector<int>;
	auto &lcp = *lcp_p;
	lcp.resize(len, 0);
	std::vector<int> rank;
	rank.resize(len, 0);

	for(int i=0; i < len; ++i){
	    rank[suffix_array[i]] = i;
	}

	int l = 0;

	for(int j=0; j < len; ++j) {
	    if(l != 0)
		--l;

	    int i = rank[j];
	    int j2 = suffix_array[i - 1];

	    if(i != 0) {
		while(j + l < len and j2 + l < len and s[j + l] == s[j2 + l]) {
		    ++l;
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

typedef std::unordered_map<int_pair, int_pair> int_tuple_map;
typedef std::tuple<int, int, int> int_trip;


class RepeatFinderResult {
public:
    int match_length = 0;
    int matching = 0;
    int confusion = 999999999;
    std::vector<int> matches;
};


class RepeatFinder {

private:

    int num_texts;
    std::vector<int> text_positions;
    std::string* combined_texts;
    int length;
    SuffixArray *sa;
    int min_matching;
    //int_tuple_map results;

public:

    RepeatFinder(std::vector<std::string> &texts, int _min_matching) {
	min_matching = _min_matching;
	num_texts = texts.size();
	text_positions = std::vector<int>();

	combined_texts = new std::string;
	for(auto s : texts) {
	    text_positions.push_back(combined_texts->length());
	    combined_texts->append(s);
	    combined_texts->append("\2");
	}
	text_positions.push_back(combined_texts->length());
	length = combined_texts->length();
	const unsigned char* c_str = (const unsigned char*) combined_texts->c_str();
	sa = new SuffixArray(c_str, length);
    }

    ~RepeatFinder() {
	delete sa;
	delete combined_texts;
    }

    int text_index_at(int o) {
	return o - text_positions[text_at(o)];
    }

    int text_at(int o) {
	auto begin = text_positions.begin();
	auto upper = std::upper_bound(begin,
				      text_positions.end(),
				      o);
	return upper - begin - 1;
    }


    RepeatFinderResult* rstr() {
	int i = 0,
	    top = 0,
	    previous_lcp_len = 0;

	auto result = new RepeatFinderResult();
	auto lcp = sa->lcp();

	std::vector<int> rank;
	rank.resize(length, 0);

	for(int i=0; i < length; ++i){
	    rank[sa->suffix_array[i]] = i;
	}

	// Fix lcp for multi strings
	for(unsigned int i=0; i < text_positions.size() - 1; ++i) {
	    int doc_end = text_positions[i + 1] - 1;
	    int doc_length = doc_end - text_positions[i] + 1;
	    for(int k=0; k < doc_length; ++k) {
		int lcp_value = lcp->at(rank[doc_end - k]);
		if(lcp_value > k) {
		    lcp->at(rank[doc_end - k]) = std::min(k, lcp_value);
		}
	    }
	}
	std::stack<int_trip> stack;

	int pos1 = sa->suffix_array[0];

	for(i=0; i < length - 1; ++i) {
	    int current_lcp_len = lcp->at(i + 1);
	    int pos2 = sa->suffix_array[i + 1];
	    int end_ix = std::max(pos1, pos2) + current_lcp_len;
	    int n = previous_lcp_len - current_lcp_len;
	    if(n < 0) {
		stack.push(int_trip(-n, i, end_ix));
		top -= n;
	    } else if(n > 0) {
		top = remove_many(stack, top, n, i, result);
	    } else if(top > 0 and end_ix > std::get<2>(stack.top())) {
		std::get<2>(stack.top()) = end_ix;
	    }
	    previous_lcp_len = current_lcp_len;
	    pos1 = pos2;
	}

	if(top > 0) {
	    top = remove_many(stack, top, top, i + 1, result);
	}

	delete lcp;
	return result;
    }



    int remove_many(std::stack<int_trip> &stack,
		    int top,
		    int m,
		    int end_ix,
		    RepeatFinderResult* result) {
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
		evaluate_match(nb, top, start_ix, result);
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


    void evaluate_match(int nb, int match_len, int start_ix,
			RepeatFinderResult* result) {
	// if(match_len < 2 or nb < result->matching or
	//    (nb == result->matching and match_len <= result->match_length))
	//     return;
	std::vector<int> sub_results;
	for(int i=0; i < num_texts; ++i)
	    sub_results.push_back(-1);

	int confusion = 0;
	for(int o=start_ix; o < start_ix + nb; ++o) {
	    int offset_global = sa->suffix_array[o];
	    if(o == length) {
		break;
	    }
	    int offset = text_index_at(offset_global);
	    int id_str = text_at(offset_global);

	    if(sub_results[id_str] == -1) {
		sub_results[id_str] = offset;
	    } else if (sub_results[id_str] > offset) {
		sub_results[id_str] = offset;
		++confusion;
	    } else {
		++confusion;
	    }
	}

	int hit_docs = num_texts;
	for(int match_start : sub_results) {
	    if(match_start == -1)
		--hit_docs;
	}

	if(hit_docs >= min_matching) {
	    if(hit_docs > result->matching or
	       (hit_docs >= result->matching and
		match_len > result->match_length and
		confusion <= result->confusion) ) {
		result->confusion = confusion;
		result->matching = hit_docs;
		result->match_length = match_len;
		result->matches = sub_results;
	    }
	}

    }

};


#endif
