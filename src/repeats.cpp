/**
 * Local Variables:
 * flycheck-clang-language-standard: "c++11"
 * flycheck-clang-standard-library: "libc++"
 * flycheck-clang-include-path: ("../include")
 * flycheck-clang-warnings: ("all" "extra")
 * End:
 */

#include "repeats.hpp"


namespace RepeatFinder {

    using namespace std;


    CommonRepeatFinder::~CommonRepeatFinder() {};

    void CommonRepeatFinder::evaluate_match(int nb, int match_len, int start_ix,
					    RepeatFinderResult& result) {
	if((match_len < 2) or (nb < num_texts))
	    return;

	vector<vector<int>> positions(num_texts, vector<int>());
	vector<int> all_offsets;
	for(int o=start_ix; o < start_ix + nb; ++o) {
	    int offset_global = sa.suffix_array[o];
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
	    good_table = true;
	}

	if(has_extension(rights, right_rests, 1)) {
	    table.right_extendables = positions;
	    table.right_match_length = match_len;
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
		    auto &doc_left = lefts[doc_id];
		    auto &doc_right = rights[doc_id];
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

			// if(left_offset - right_offset > left_table.left_match_length) {
			// 	match = false;
			// 	break;
			// }

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
	       offsets[i] + delta >= (int)combined_texts.size() or
	       offsets[i + 1] + delta >= (int)combined_texts.size()) {
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
	       starts[i] + delta >= (int)combined_texts.size() or
	       starts[i + 1] + delta >= (int)combined_texts.size()) {
		return 0; // out of bounds
	    } else if(combined_texts[starts[i] + delta] ==
		      combined_texts[first_rest + delta]) {
		return 0;
	    }
	}

	return 1;
    }

}
