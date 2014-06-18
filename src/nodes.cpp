
#include "nodes.hpp"
#include <fstream>
#include "estl.hpp"

using namespace Nodes;
using namespace estl;

std::string get_file_contents(const char *filename)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in)
  {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return(contents);
  }
  throw(errno);
}




int main() {
    strings texts {"thesexxxare", "theseyyyare", "thesezzzare"};
    auto node = construct(texts);

    cout << node->texts << endl;
    cout << node->length_of_data() << endl;
    cout << list_dir("/Users/jvanwink/scratch") << endl;
    cout << glob("~/scratch/*") << endl;
}
