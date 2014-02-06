#include "repeats.hpp"
#include <iostream>

using namespace std;

int main() {
  vector<string> ss = {
      "xxxaba",
      "xxxcbc",
      "xxxddd",
      "xxxfgf",
      "xxxnon"};


  for(int i=0; i<1000; ++i){
    auto rf = RepeatFinder(ss, 3);
    auto result = rf.rstr();
    cout << result->match_length << '\n';
  }


}
