#ifndef MYSTACK_HPP
#define MYSTACK_HPP

#include <stack>

template <typename T>
class my_stack : public std::stack<T> {
  public:
    using std::stack<T>::c; // expose the container
};


#endif
