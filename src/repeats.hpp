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


#include "flott/flott.h"
#include "flott/flott_nid.h"

namespace RepeatFinder {

using namespace std;

typedef pair<int, int> int_pair;
typedef pair<int_pair, int_pair> int_pair_pair;
typedef tuple<int, int, int> int_trip;


class SuffixArray {

public:
    vector<int> suffix_array;
    vector<int> lcp;
    vector<int> rank;
    SuffixArray();
    SuffixArray(string &s, vector<int> length_before_docs={});
    ~SuffixArray();

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
    RepeatFinder(vector<string> texts);
    virtual ~RepeatFinder();

    RepeatFinderResult rstr();
    int text_index_at(int o, int text_num);
    int text_at(int o);
    int remove_many(stack<int_trip> &stack,
		    int top,
		    int m,
		    int end_ix,
		    RepeatFinderResult& result);
    virtual void evaluate_match(int nb, int match_len, int start_ix,
				RepeatFinderResult& result);

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
