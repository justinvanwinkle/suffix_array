#include "repeats.hpp"
#include <iostream>

using namespace std;

int main() {
  vector<string> ss = {
    "1368226\",\"dpci\":\"246-02-0788",
    "4104910\",\"dpci\":\"068-07-0065",
    "3350721\",\"dpci\":\"248-63-1547",
    "1843493\",\"dpci\":\"246-04-5372",
    "3247206\",\"dpci\":\"248-60-0627",
    "4255101\",\"dpci\":\"243-30-6501",
    "4375553\",\"dpci\":\"040-03-0872",
    "3567775\",\"dpci\":\"070-05-0341",
    "4170911\",\"dpci\":\"086-04-0259",
    "4307328\",\"dpci\":\"248-81-4755",
    "2237077\",\"dpci\":\"246-05-1835",
    "4358735\",\"dpci\":\"074-00-0030",
    "3775398\",\"dpci\":\"244-22-3012",
    "3763893\",\"dpci\":\"242-17-5065",
    "2870702\",\"dpci\":\"248-52-0775",
    "3221691\",\"dpci\":\"244-20-6656",
    "1354607\",\"dpci\":\"244-00-6031",
    "4077605\",\"dpci\":\"060-03-0312",
    "3804229\",\"dpci\":\"244-22-4137",
    "3890157\",\"dpci\":\"248-78-1622",
    "1691132\",\"dpci\":\"246-04-3258",
    "4420449\",\"dpci\":\"026-01-1827",
    "1567749\",\"dpci\":\"248-09-2062",
    "4362542\",\"dpci\":\"241-27-3133",
    "3715382\",\"dpci\":\"248-73-0576",
    "4157298\",\"dpci\":\"243-29-1957",
    "3104684\",\"dpci\":\"243-19-8713"};

  for(int i=0; i<1000; ++i){
    auto rf = RepeatFinder(ss, 3);
    rf.rstr();
    cout << rf.match_length << '\n';
  }


}
