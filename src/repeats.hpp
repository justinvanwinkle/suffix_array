#ifndef REPEATS_HPP
#define REPEATS_HPP


#include <algorithm>
#include <vector>
#include <memory>
#include <string>
#include <math.h>
#include <tuple>
#include <stack>
#include <iostream>
#include <string.h>
#include "divsufsort.h"
#include <math.h>
#include <sstream>


namespace RepeatFinder {

    using namespace std;

    template<typename T> ostream &operator<<(ostream &s,vector<T> t) {
	s << "[";
	for(unsigned int i = 0; i < t.size(); i++)
	    s << t[i] << (i==t.size()-1 ? "" : ", ");
	return s << "]";
    }

typedef pair<int, int> int_pair;
typedef pair<int_pair, int_pair> int_pair_pair;
typedef tuple<int, int, int> int_trip;


class SuffixArray {

public:
    vector<int> suffix_array;
    vector<int> lcp;
    vector<int> rank;
    SuffixArray() {}
    SuffixArray(string &s, vector<int> length_before_docs=vector<int>()) {
    int len = s.size();

    suffix_array.resize(len, 0);

    divsufsort((const unsigned char *) s.data(), suffix_array.data(), len);


    lcp.resize(len, 0);
    rank.resize(len, 0);

    for(int i=0; i < len; ++i){
	rank[suffix_array[i]] = i;
    }

    int l = 0;

    for(int j=0; j < len; ++j) {
	if(l != 0)
	    --l;

	int i = rank[j];
	if(i != 0) {
	    int j2 = suffix_array[i - 1];
	    while(j + l < len and j2 + l < len and s[j + l] == s[j2 + l]) {
		++l;
	    }

	    lcp[i] = l;
	} else {
	    l = 0;
	}
    }

    if(not length_before_docs.empty()) {
	// Fix lcp for multi strings
	for(unsigned int doc_idx=0; doc_idx < length_before_docs.size() - 1; ++doc_idx) {
	    int doc_end_pos = length_before_docs[doc_idx + 1] - 1;
	    int doc_start_pos = length_before_docs[doc_idx];
	    for(int needle_pos=doc_end_pos; needle_pos >= doc_start_pos; --needle_pos) {
		int lcp_value = lcp[rank[needle_pos]];
		if(lcp_value > doc_end_pos - needle_pos) {
		    lcp[rank[needle_pos]] = doc_end_pos - needle_pos;
		}
	    }
	}
    }

    }
    ~SuffixArray() {};

};


class RepeatFinderResult {
public:
    int match_length = 0;
    int matching = 0;
    vector<int> matches;
};


class RepeatFinder {
protected:
    int num_texts;
    string combined_texts;
    vector<int> length_before_docs;
    SuffixArray sa;
    vector<int> sub_results;
    vector<int> match_count;

public:
    RepeatFinder(vector<string> texts) {
    num_texts = texts.size();
    sub_results.resize(num_texts, -1);
    match_count.resize(num_texts, 0);

    for(auto &s : texts) {
	length_before_docs.push_back(combined_texts.length());
	combined_texts.append(s);
	combined_texts.append("\x02");
    }
    length_before_docs.push_back(combined_texts.length());
    sa = SuffixArray(combined_texts, length_before_docs);
}

    virtual ~RepeatFinder() {};

    RepeatFinderResult rstr() {
    unsigned int i = 0,
	top = 0,
	previous_lcp_len = 0;

    auto result = RepeatFinderResult();
    auto &lcp = sa.lcp;

    stack<int_trip> stack;

    int pos1 = sa.suffix_array[0];

    for(i=0; i < lcp.size() - 2; ++i) {
	int current_lcp_len = lcp[i + 1];
	int pos2 = sa.suffix_array[i + 1];
	int end_ix = max(pos1, pos2) + current_lcp_len;
	int n = previous_lcp_len - current_lcp_len;
	if(n < 0) {
	    stack.push(int_trip(-n, i, end_ix));
	    top -= n;
	} else if(n > 0) {
	    top = remove_many(stack, top, n, i, result);
	} else if(top > 0 and end_ix > get<2>(stack.top())) {
	    get<2>(stack.top()) = end_ix;
	}

	previous_lcp_len = current_lcp_len;
	pos1 = pos2;
    }

    if(top > 0) {
	remove_many(stack, top, top, i + 1, result);
    }

    return result;
}
    int text_index_at(int o, int text_num){
    return o - length_before_docs[text_num];
}
    int text_at(int o) {
    for(unsigned int ix=0; ix < length_before_docs.size() - 1; ++ix) {
	if(length_before_docs[ix + 1] > o){
	    return ix;
	}
    }

    throw "FUCK";
}
    int remove_many(stack<int_trip> &stack,
		    int top,
		    int m,
		    int end_ix,
		    RepeatFinderResult& result) {
    int last_start_ix = -1,
	max_end_ix,
	n,
	start_ix,
	nb;

    if(m < 0)
	throw "FUCK";

    while(m > 0) {
	tie(n, start_ix, max_end_ix) = stack.top();
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
	stack.push(make_tuple(-m, start_ix, max_end_ix - n - m));
	top -= m;
    }
    return top;
}
    virtual void evaluate_match(int nb, int match_len, int start_ix,
				RepeatFinderResult& result)  {
    if (match_len < 2) { return; }

    for (int i=0; i < num_texts; ++i) {
    	sub_results[i] = -1;
	match_count[i] = 0;
    }

    for (int o=start_ix; o < start_ix + nb; ++o) {
	int offset_global = sa.suffix_array[o];

	int id_str = text_at(offset_global);

	int offset = text_index_at(offset_global, id_str);

	++match_count[id_str];

	if ((sub_results[id_str] == -1) or
	    (offset < sub_results[id_str])) {
	    sub_results[id_str] = offset;
	}
    }

    for (int i=0; i < num_texts; ++i) {
	if(sub_results[i] == -1)
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
	result.matching = num_texts;
	result.match_length = match_len;
	result.matches = vector<int>(sub_results);
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


class CommonRepeatFinder: public RepeatFinder {
private:
    bool extendable(vector<int> &offsets, int delta);
public:
    CommonRepeatFinder(vector<string> texts):
	RepeatFinder(texts) {
	vector<Table> tables;
    };
    virtual ~CommonRepeatFinder();
    vector<Table> tables;
    virtual void evaluate_match(int nb, int match_len, int start_ix,
			RepeatFinderResult& result);
    int has_extension(vector<int> &starts,
		      vector<int> &rests,
		      int delta);
    bool match_tables(int);
};

}

#endif
