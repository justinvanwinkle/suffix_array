/**
 * Local Variables:
 * flycheck-clang-language-standard: "c++11"
 * flycheck-clang-standard-library: "libc++"
 * flycheck-clang-include-path: ("../include")
 * flycheck-clang-warnings: ("all" "extra")
 * End:
 */

#include "repeats.hpp"

using namespace std;

SuffixArray::SuffixArray(string &s, vector<int> text_positions) {
    int len = s.size();

    suffix_array.resize(s.size(), 0);
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

    if(not text_positions.empty()) {
	// Fix lcp for multi strings
	for(unsigned int i=0; i < text_positions.size() - 1; ++i) {
	    int doc_end = text_positions[i + 1] - 1;
	    int doc_length = doc_end - text_positions[i] + 1;
	    for(int k=0; k < doc_length; ++k) {
		int lcp_value = lcp[rank[doc_end - k]];
		if(lcp_value > k) {
		    lcp[rank[doc_end - k]] = k;
		}
	    }
	}
    }

    for(unsigned int ix=0; ix < 10; ix++)
	cout << lcp[ix] << ", ";
    cout << endl;
}

SuffixArray::~SuffixArray() {

}


RepeatFinder::RepeatFinder(std::vector<std::string> texts) {
    num_texts = texts.size();
    text_positions = std::vector<int>();
    sub_results = std::vector<int>(num_texts);

    string combined_texts;

    for(auto s : texts) {
	text_positions.push_back(combined_texts.length());
	combined_texts.append(s);
	combined_texts.append("\2");
    }
    text_positions.push_back(combined_texts.length());
    sa = new SuffixArray(combined_texts, text_positions);
}

RepeatFinder::~RepeatFinder() {

}

int RepeatFinder::text_index_at(int o, int text_num) {
    return o - text_positions[text_num];
}

int RepeatFinder::text_at(int o) {
    auto begin = text_positions.begin();
    auto upper = std::upper_bound(
	begin, text_positions.end(), o);
    return upper - begin - 1;
}


RepeatFinderResult* RepeatFinder::rstr() {
    unsigned int i = 0,
	top = 0,
	previous_lcp_len = 0;

    auto result = new RepeatFinderResult();
    vector<int> &lcp = sa->lcp;

    std::stack<int_trip> stack;

    int pos1 = sa->suffix_array[0];

    for(i=0; i < lcp.size() - 1; ++i) {
	int current_lcp_len = lcp[i + 1];
	int pos2 = sa->suffix_array[i];
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

    return result;
}


int RepeatFinder::remove_many(std::stack<int_trip> &stack,
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


void RepeatFinder::evaluate_match(int nb, int match_len, int start_ix,
				  RepeatFinderResult* result) {
    if(match_len < 1)
	return;
    for(int i=0; i < num_texts; ++i)
	sub_results[i] = -1;

    int hit_docs = 0;
    for(int o=start_ix; o < start_ix + nb; ++o) {
	int offset_global = sa->suffix_array[o];
	if(o == sa->lcp.size()) {
	    cout << o << ", " << start_ix << ", " << nb << ", " << sa->lcp.size() << endl;
	    continue;
	}
	int id_str = text_at(offset_global);
	int offset = text_index_at(offset_global, id_str);

	if(sub_results[id_str] == -1) {
	    sub_results[id_str] = offset;
	    ++hit_docs;
	} else {
	    return;
	}
    }

    for(int i=0; i < num_texts; ++i) {
	if(sub_results[i] == -1)
	    return;
    }


    if(hit_docs > result->matching or
       (match_len > result->match_length and
	hit_docs >= result->matching)) {
	result->matching = hit_docs;
	result->match_length = match_len;
	result->matches = std::vector<int>(sub_results);
    }

}


void CommonRepeatFinder::evaluate_match(int nb, int match_len, int start_ix,
					RepeatFinderResult* result) {
    if(match_len < 1)
	return;

    vector<vector<int>> positions(num_texts, vector<int>(0));

    for(int o=start_ix; o < start_ix + nb; ++o) {
	int offset_global = sa->suffix_array[o];
	int id_str = text_at(offset_global);
	positions[id_str].push_back(offset_global);
    }

    vector<int> starts(num_texts);
    vector<int> rests;

    for(auto file_positions : positions) {
	if(file_positions.empty())
	    return;

	sort(file_positions.begin(), file_positions.end());

	starts.push_back(file_positions[0]);

	if(file_positions.size() == 1)
	    continue;

	for(unsigned int ix=1; ix < file_positions.size(); ++ix) {
	    rests.push_back(file_positions[ix]);
	}



    }


}



// flott_result get_entropy(char *bytes, size_t length) {
//     flott_object *op = flott_create_instance(1);
//     flott_source *source = &(op->input.source[0]);
//     source->storage_type = FLOTT_DEV_MEM;
//     source->length = length;
//     source->data.bytes = bytes;
//     flott_initialize(op);
//     flott_t_transform(op);
//     flott_result result = (op->result);
//     flott_destroy(op);
//     return result;
// }

// double nti_distance(char *bytes1, size_t length1,
// 		    char *bytes2, size_t length2) {
//     flott_object *op = flott_create_instance(2);
//     flott_source *source = &(op->input.source[0]);
//     source->storage_type = FLOTT_DEV_MEM;
//     source->length = length1;
//     source->data.bytes = bytes1;

//     source = &(op->input.source[1]);
//     source->storage_type = FLOTT_DEV_MEM;
//     source->length = length2;
//     source->data.bytes = bytes2;

//     double nti_dist = 0.0;

//     flott_nti_dist(op, &nti_dist);
//     flott_destroy(op);
//     return nti_dist;
// }

// double ntc_distance(char *bytes1, size_t length1,
// 		    char *bytes2, size_t length2) {
//     flott_object *op = flott_create_instance(3);
//     flott_source *source = &(op->input.source[0]);
//     source->storage_type = FLOTT_DEV_MEM;
//     source->length = length1;
//     source->data.bytes = bytes1;

//     source = &(op->input.source[1]);
//     source->storage_type = FLOTT_DEV_MEM;
//     source->length = length2;
//     source->data.bytes = bytes2;

//     double ntc_dist = 0.0;

//     flott_ntc_dist(op, &ntc_dist);
//     flott_destroy(op);
//     return ntc_dist;
// }
