

#include <algorithm>
#include <vector>
#include <string>
#include <tuple>
#include <unordered_map>
#include "lcp.hpp"
#include "divsufsort.h"
#include <Python.h>

using namespace std;

class SuffixArray {

public:
    int length;
    const unsigned char* s;
    int* suffix_array;

    SuffixArray(const unsigned char* _s, int _s_length) {
	length = _s_length;
	s = _s;
	suffix_array = (int *)PyMem_Malloc(length * sizeof(int));

    }

    ~SuffixArray() {
	PyMem_Free(suffix_array);
    }
};


typedef pair<int, int> int_pair;

namespace std {
    template <>
        struct hash<int_pair> {
        public :
            size_t operator()(const int_pair &t ) const
            {
                return hash<int>()(t.first) ^ hash<int>()(t.second);
            }
    };
};

class RepeatFinder {

private:
    int num_texts;
    vector<int> text_positions;
    const unsigned char* combined_texts;
    SuffixArray *sa;
    int min_matching;
public:
    RepeatFinder(vector<string> &texts, int _min_matching) {
	min_matching = _min_matching;
	text_positions = vector<int>();
	num_texts = texts.size();

	string accum = "";
	for(string s : texts) {
	    accum += s;
	    accum += "\2";
	    text_positions.push_back(accum.length());

	}
	combined_texts = (const unsigned char*) accum.c_str();
	sa = new SuffixArray(combined_texts, accum.length());
    }

    int text_index_at(int q) {
	int index_at = text_at(q);
	int start = 0;
	if(index_at != 0)
	    start = text_positions[index_at - 1];
	return q = start;
    }

    int text_at(int q) {
	int ix = *lower_bound(text_positions.begin(),
			      text_positions.end(),
			      q);
	if(text_positions[ix] == q)
	    ++ix;

	return ix;
    }

    unordered_map<int_pair, int_pair> rstr() {
	int Xi,
	    Xn,
	    _,
	    end_ix,
	    i,
	    n,
	    pos1,
	    pos2,
	    current_lcp_len = 0,
	    previous_lcp_len = 0,
	    len_lcp = sa->length - 1;;
	unordered_map<int_pair, int_pair> d;
	if(sa->length == 0)
	    return d;

	const int* suffix_array = sa->suffix_array;
	vector<int> lcp;
	make_lcp(combined_texts,
		 suffix_array,
		 sa->length,
		 lcp);

	pos1 = suffix_array[0];

	for(i=0; i < len_lcp; ++i) {
	    current_lcp_len = lcp[i + 1];
	    pos2 = suffix_array[i + 1];
	    end_ix = max(pos1, pos2) + current_lcp_len;
	    n = previous_lcp_len - current_lcp_len;
	    if(n < 0) {
	    } else if(n > 0) {
	    } else if(stack.top > 0 && end_ix) {
	    }


	}



    }

};
