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

template<typename T> ostream &operator<<(ostream &s,vector<T> t) {
  s<<"[";
  for(unsigned int i = 0; i < t.size(); i++)
    s << t[i] << (i==t.size()-1 ? "" : ", ");
  return s << "]";
}

SuffixArray::SuffixArray(string &s, vector<int> length_before_docs) {
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

SuffixArray::~SuffixArray() {

}


RepeatFinder::RepeatFinder(std::vector<std::string> texts) {
    num_texts = texts.size();
    length_before_docs = std::vector<int>();
    sub_results = std::vector<int>(num_texts);

    //string combined_texts;

    for(auto &s : texts) {
	length_before_docs.push_back(combined_texts.length());
	combined_texts.append(s);
	combined_texts.append("\x02");
    }
    length_before_docs.push_back(combined_texts.length());
    sa = new SuffixArray(combined_texts, length_before_docs);
}

RepeatFinder::~RepeatFinder() {

}

int RepeatFinder::text_index_at(int o, int text_num) {
    return o - length_before_docs[text_num];
}

int RepeatFinder::text_at(int o) {
    for(unsigned int ix=0; ix < length_before_docs.size() - 1; ++ix) {
	if(length_before_docs[ix + 1] > o){
	    return ix;
	}
    }

    throw "FUCK";
}


RepeatFinderResult* RepeatFinder::rstr() {
    unsigned int i = 0,
	top = 0,
	previous_lcp_len = 0;

    auto result = new RepeatFinderResult();
    vector<int> &lcp = sa->lcp;

    std::stack<int_trip> stack;

    int pos1 = sa->suffix_array[0];

    for(i=0; i < lcp.size() - 2; ++i) {
	int current_lcp_len = lcp[i + 1];
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
	remove_many(stack, top, top, i + 1, result);
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

    if(m < 0)
	throw "FUCK";
    //cout << "WTF" << m << ", " << n << endl;

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


CommonRepeatFinder::~CommonRepeatFinder() {};

void CommonRepeatFinder::evaluate_match(int nb, int match_len, int start_ix,
					RepeatFinderResult* result) {
    if(match_len < 4)
	return;

    vector<vector<int>> positions(num_texts, vector<int>());
    vector<int> all_offsets;
    for(int o=start_ix; o < start_ix + nb; ++o) {
	int offset_global = sa->suffix_array[o];
	int id_str = text_at(offset_global);
	positions[id_str].push_back(offset_global);
	all_offsets.push_back(offset_global);
    }

    if(extendable(all_offsets, -1) or
       extendable(all_offsets, match_len)) {
	return;
    }

    for(auto &single_page_positions : positions) {
	if(single_page_positions.size() == 0) {
	    return;
	}
    }
    vector<int> lefts;
    vector<int> left_rests;

    vector<int> rights;
    vector<int> right_rests;

    for(auto &file_positions : positions) {
	if(file_positions.empty())
	    continue;

	sort(file_positions.begin(), file_positions.end());

	lefts.push_back(file_positions[0]);
	rights.push_back(file_positions[file_positions.size() - 1] + match_len - 1);

	if(file_positions.size() < 2)
	    continue;

	for(unsigned int ix=1; ix < file_positions.size(); ++ix) {
	    left_rests.push_back(file_positions[ix]);
	}
	for(unsigned int ix=0; ix < file_positions.size() - 1; ++ix) {
	    right_rests.push_back(file_positions[ix] + match_len - 1);
	}
    }

    Table table;
    bool good_table = false;
    if(has_extension(lefts, left_rests, -1)) {
	table.left_extendables = positions;
	table.left_match_length = match_len;

	//cout << "left extend" << match_len << " " << positions << endl;
	good_table = true;
    }

    if(has_extension(rights, right_rests, 1)) {
	table.right_extendables = positions;
	table.right_match_length = match_len;

	//cout << "right extend" << match_len << " " << positions << endl;
	good_table = true;
    }

    if(good_table)
	tables.push_back(table);
}


bool CommonRepeatFinder::match_tables(int max_tables=1) {
    vector<Table> good_tables;

    for(auto &left_table : tables) {
	auto &lefts = left_table.left_extendables;
	if(left_table.left_extendables.empty())
	    continue;
	for(auto &right_table : tables) {
	    if(right_table.right_extendables.empty())
		continue;
	    auto &rights = right_table.right_extendables;
	    bool match = true;
	    bool long_group_seen = false;
	    int offset_delta = -1;
	    for(size_t doc_id=0; doc_id < lefts.size(); ++doc_id) {
		auto doc_left = lefts[doc_id];
		auto doc_right = rights[doc_id];
		if(doc_left.size() != doc_right.size()) {
		    match = false;
		    break;
		}

		if(doc_left.size() > 3) {
		    long_group_seen = true;
		}

		int last_left_offset = -1;
		for(size_t left_ix = 1, right_ix = 0;
		    left_ix < doc_left.size() and right_ix < doc_right.size() - 1;
		    left_ix++, right_ix++) {
		    int right_offset = doc_right[right_ix];
		    int left_offset = doc_left[left_ix];

		    if((left_offset <= right_offset) or
		       (last_left_offset >= right_offset)) {
			match = false;
			break;
		    }

		    last_left_offset = left_offset;

		    if(left_offset - right_offset > left_table.left_match_length) {
			match = false;
			break;
		    }

		    if(offset_delta == -1) {
			offset_delta = left_offset - right_offset;
		    } else {
			if(left_offset - right_offset != offset_delta) {
			    match = false;
			    break;
			}
		    }

		}
	    }
	    if(match and long_group_seen) {
		Table t;
		t.left_match_length = left_table.left_match_length;
		t.left_extendables = left_table.left_extendables;
		t.right_match_length = right_table.right_match_length;
		t.right_extendables = right_table.right_extendables;
		good_tables.push_back(t);
		// if(good_tables.size() > max_tables) {
		//     tables = good_tables;
		//     return false;
		// }
	    }
	}
    }
    tables = good_tables;
    return true;
}

bool CommonRepeatFinder::extendable(vector<int> &offsets,
				    int delta) {

    for(unsigned int i=0; i < offsets.size() - 2 ; ++i) {
	if(offsets[i] + delta < 0 or
	   offsets[i + 1] + delta < 0 or
	   offsets[i] + delta >= combined_texts.size() or
	   offsets[i + 1] + delta >= combined_texts.size()) {
	    return false; // out of bounds
	} else if(combined_texts[offsets[i] + delta] !=
		  combined_texts[offsets[i + 1] + delta]) {
	    return false; // No extension
	}
    }

    return true;
}

int CommonRepeatFinder::has_extension(vector<int> &starts,
				      vector<int> &rests,
				      int delta) {
    // There is extension for rests
    if(rests.size() < 3) {
	return 0;
    }

    if(not extendable(rests, delta)) {
	return 0;
    }

    int first_rest = rests[0];

    for(unsigned int i=0; i < starts.size() - 2 ; ++i) {

	if(starts[i] + delta < 0 or
	   starts[i + 1] + delta < 0 or
	   starts[i] + delta >= combined_texts.size() or
	   starts[i + 1] + delta >= combined_texts.size()) {
	    return 0; // out of bounds
	} else if(combined_texts[starts[i] + delta] ==
		  combined_texts[first_rest + delta]) {
	    return 0;
	}
    }

    return 1;
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
