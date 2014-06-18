#ifndef ESTL_HPP
#define ESTL_HPP

#include <dirent.h>
#include <glob.h>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <memory>

namespace estl {

using namespace std;

using strings = vector<string>;
using ints = vector<int>;


template <typename T, class Function> void enumerate(vector<T> &vec, Function fn) {
    size_t ix = 0;
    for (auto &val : vec) {
        fn(ix, val);
        ++ix;
    }
}

inline vector<string> glob(string pattern) {
    unique_ptr<glob_t, void (*) (glob_t *)> glob_buffer(new glob_t(), globfree);

    glob(pattern.c_str(), GLOB_TILDE, NULL, glob_buffer.get());

    strings fns;
    for (size_t i = 0; i < glob_buffer->gl_pathc; ++i) {
        fns.push_back(string(glob_buffer->gl_pathv[i]));
    }
    return fns;
}


strings list_dir(string path) {
    strings files;

    unique_ptr<DIR, int (*) (DIR *)> dir(opendir(path.c_str()), closedir);
    if (not dir) {
        throw system_error(errno, system_category());
    }

    while (struct dirent *entry = readdir(dir.get())) {
        files.push_back(string(entry->d_name));
    }
    return files;
}
};

#endif
