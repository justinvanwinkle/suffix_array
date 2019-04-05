import glob
from itertools import combinations
import suffix_array
from os.path import isfile


fns = [fn for fn in glob.glob('**/*', recursive=True) if isfile(fn)]

# for fn1, fn2 in combinations(fns, 2):
#     print(fn1, fn2)
#     print(fn1, fn2, suffix_array.file_bisect_distance(fn1, fn2))

texts = ['bananas', 'bananas']
print(texts)

print(suffix_array.rstr_max(texts))
for texts in (['bub'], ['b'], ['abcd'], ['dcba'], ['aaaaa', 'aaaaa'],
              ['abcdefghij'] * 4, ['abcdefghij'] * 3, ['abcde', 'abcde']):
    print(texts)
    print(repr(suffix_array.longest_common_substring(texts)))


# ix:0 lcp:0 let_to_end:0 last_left_to_end:0
# ix:1 lcp:5 let_to_end:5 last_left_to_end:0
# ix:2 lcp:0 let_to_end:4 last_left_to_end:5
# ix:3 lcp:4 let_to_end:4 last_left_to_end:4
# ix:4 lcp:0 let_to_end:3 last_left_to_end:4
# ix:5 lcp:3 let_to_end:3 last_left_to_end:3
# ix:6 lcp:0 let_to_end:2 last_left_to_end:3
# ix:7 lcp:2 let_to_end:2 last_left_to_end:2
# ix:8 lcp:0 let_to_end:1 last_left_to_end:2
# ix:9 lcp:1 let_to_end:1 last_left_to_end:1
# 5 0 6 1 7 2 8 3 9 4
# 0 5 0 4 0 3 0 2 0 1
# a b c d e a b c d e
# LCP:[0, 5, 0, 4, 0, 3, 0, 2, 0, 1]
# push
# lcp_len:5 string_ix:0 end_ix:10 lcp_diff:-5 top:5
# Stack:[(5, 0, 10)]
# remove_many
# lcp_len:0 string_ix:6 end_ix:6 lcp_diff:5 top:0
# Stack:[]
# push
# lcp_len:4 string_ix:1 end_ix:10 lcp_diff:-4 top:4
# Stack:[(4, 2, 10)]
# remove_many
# lcp_len:0 string_ix:7 end_ix:7 lcp_diff:4 top:0
# Stack:[]
# push
# lcp_len:3 string_ix:2 end_ix:10 lcp_diff:-3 top:3
# Stack:[(3, 4, 10)]
# remove_many
# lcp_len:0 string_ix:8 end_ix:8 lcp_diff:3 top:0
# Stack:[]
# push
# lcp_len:2 string_ix:3 end_ix:10 lcp_diff:-2 top:2
# Stack:[(2, 6, 10)]
# remove_many
# lcp_len:0 string_ix:9 end_ix:9 lcp_diff:2 top:0
# Stack:[]
# push
# lcp_len:1 string_ix:4 end_ix:10 lcp_diff:-1 top:1
# Stack:[(1, 8, 10)]
# b'bcde'
