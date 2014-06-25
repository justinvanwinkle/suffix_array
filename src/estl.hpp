#ifndef ESTL_HPP
#define ESTL_HPP

#include <dirent.h>
#include <glob.h>
#include <sys/stat.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <iostream>


template <typename T>
std::ostream &operator<<(std::ostream &s, std::pair<T, T> t) {
    s << "(" << t.first << ", " << t.second << ")";
    return s;
}

template <typename T>
std::ostream &operator<<(std::ostream &s, std::vector<T> t) {
    s << "[";
    for (unsigned int i = 0; i < t.size(); i++)
        s << t[i] << (i == t.size() - 1 ? "" : ", ");
    return s << "]";
}

template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std
{
  template<typename S, typename T> struct hash<pair<S, T>>
  {
    inline size_t operator()(const pair<S, T> & v) const
    {
      size_t seed = 0;
      hash_combine(seed, v.first);
      hash_combine(seed, v.second);
      return seed;
    }
  };
}


namespace estl {

using namespace std;

using strings = vector<string>;
using ints = vector<int>;


template <class C, class Function>
void enumerate(C &vec, Function fn) {
    size_t ix = 0;
    for (auto &val : vec) {
        fn(ix, val);
        ++ix;
    }
}


bool is_dir(string fn) {
    struct stat statbuf;
    stat(fn.c_str(), &statbuf);
    if (S_ISDIR(statbuf.st_mode))
        return true;
    return false;
}


string read_file(string fn) {
    ifstream in(fn, ios::in | ios::binary);
    if (in) {
        string contents;
        in.seekg(0, ios::end);
        contents.resize(in.tellg());
        in.seekg(0, ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return (contents);
    }
    throw(errno);
}


inline vector<string> glob(string pattern, bool only_files = false) {
    unique_ptr<glob_t, decltype(&globfree)> glob_buffer(new glob_t(), globfree);

    glob(pattern.c_str(), GLOB_TILDE, NULL, glob_buffer.get());

    strings fns;
    for (size_t i = 0; i < glob_buffer->gl_pathc; ++i) {
        fns.push_back(string(glob_buffer->gl_pathv[i]));
    }

    if (only_files) {
        strings file_fns;
        for (auto &fn : fns) {
            if (not is_dir(fn))
                file_fns.push_back(fn);
        }
        return file_fns;
    }
    return fns;
}


strings list_dir(string path, bool only_files = false) {
    strings fns;
    unique_ptr<DIR, decltype(&closedir)> dir(opendir(path.c_str()), closedir);
    if (not dir) {
        throw system_error(errno, system_category());
    }

    while (struct dirent *entry = readdir(dir.get())) {
        fns.push_back(string(entry->d_name));
    }

    if (only_files) {
        strings file_fns;
        for (auto &fn : fns) {
            if (not is_dir(fn))
                file_fns.push_back(fn);
        }
        return file_fns;
    }
    return fns;
}

strings read_files(strings fns) {
    strings file_contents;
    for (auto &fn : fns)
        file_contents.push_back(read_file(fn));
    return file_contents;
}

// strings read_files(initializer_list<string> &fns) {
//     return read_files(fns);
// }

};

#endif
