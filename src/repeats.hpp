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


typedef std::pair<int, int> int_pair;
typedef std::pair<int_pair, int_pair> int_pair_pair;
typedef std::tuple<int, int, int> int_trip;


class SuffixArray {

public:
    std::vector<int> suffix_array;
    std::vector<int> lcp;
    std::vector<int> rank;

    SuffixArray(std::string s);
    ~SuffixArray();

};

class RepeatFinderResult {
public:
    int match_length = 0;
    int matching = 0;
    std::vector<int> matches;
};


class RepeatFinder {

protected:
    int num_texts;
    std::vector<int> text_positions;
    std::vector<int> text_lengths;
    SuffixArray *sa;
    std::vector<int> sub_results;
    //int_tuple_map results;
public:
    RepeatFinder(std::vector<std::string> texts);
    ~RepeatFinder();

    RepeatFinderResult* rstr();
    int text_index_at(int o, int text_num);
    int text_at(int o);
    int remove_many(std::stack<int_trip> &stack,
		    int top,
		    int m,
		    int end_ix,
		    RepeatFinderResult* result);
    void evaluate_match(int nb, int match_len, int start_ix,
			RepeatFinderResult* result);

};

class CommonRepeatFinder: public RepeatFinder {
    void evaluate_match(int nb, int match_len, int start_ix,
			RepeatFinderResult* result);
};


#endif
