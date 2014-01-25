#include <vector>

using namespace std;

void
make_lcp(const unsigned char *s,
	 const int *sa,
	 int sa_length,
	 vector<int> &lcp) {
    lcp.resize(sa_length);
    vector<int> rank(sa_length);

    for(int i=0; i < sa_length; ++i)
	rank[sa[i]] = i;

    int l = 0;

    lcp[0] = -1;
    for(int j=0; j < sa_length; ++j) {
	if(l != 0)
	    l = l - 1;
	int i = rank[j];
	int j2 = sa[i - 1];
	if(i != 0) {
	    while(l + j < sa_length &&
		  l + j2 < sa_length &&
		  s[j + l] == s[j2 + l]) {
		l += 1;
	    }
	    lcp[i] = l;
	} else {
	    l = 0;
	}
    }
}
