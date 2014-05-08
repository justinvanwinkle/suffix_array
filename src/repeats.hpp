
#include <math.h>
#include "flott/flott.h"
#include "flott/flott_nid.h"

#if !defined(REPEATS_HPP)
double ntc_distance(char *bytes1,size_t length1,char *bytes2,size_t length2);

double nti_distance(char *bytes1,size_t length1,char *bytes2,size_t length2);

flott_result get_entropy(char *bytes,size_t length);

inline double dlog(int x){
    return log(x + 1) - log(x);
};

#endif
