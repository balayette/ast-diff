#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "algo.hh"
#include "heap.hh"
#include "lexer.hh"
#include "parser.hh"

void usage() {
  std::cerr << "usage: ast-diffing --diff FILENAME1 FILENAME2\n";
  std::cerr << "usage: ast-diffing --pretty-sexp FILENAME1 "
               "FILENAME2...\n";
  std::exit(1);
}

void diff(char *file1, char *file2) {
  std::ifstream f1(file1);
  if (!f1) {
    std::cerr << "Couldn't open file " << file1 << '\n';
    std::exit(2);
  }

  std::ifstream f2(file2);
  if (!f2) {
    std::cerr << "Couldn't open file " << file2 << '\n';
    std::exit(2);
  }

  Lexer l(f1);
  Lexer l2(f2);

  Parser p(l);
  Parser p2(l2);

  auto ret = p.Parse();
  ret->ComputeHeightDepth();
  auto ret2 = p2.Parse();
  ret2->ComputeHeightDepth();

  ret->PrettyPrint(std::cout) << '\n';
  ret2->PrettyPrint(std::cout) << '\n';

  std::ofstream out1("out1.dot");
  std::ofstream out2("out2.dot");

  ret->DumpDot(out1);
  ret2->DumpDot(out2);

  auto mapping = Gumtree(ret.get(), ret2.get());

  for (auto &it : mapping) {
    std::cout << it.first->GetValue() << " mapped to "
              << it.second->GetValue() << '\n';
  }

  std::ofstream map("map.dot");
  DumpMapping(map, ret.get(), ret2.get(), mapping);
}

void do_pretty(std::istream &f) {
  if (!f) {
    std::cerr << "Couldn't open file \n";
    std::exit(2);
  }

  Lexer l(f);
  Parser p(l);

  auto ret = p.Parse();
  ret->ComputeHeightDepth();

  ret->PrettyPrint(std::cout) << '\n';
}

void pretty(int nbr, char *files[]) {
  for (int i = 0; i < nbr; i++) {
    if (strcmp(files[i], "-") == 0) {
      do_pretty(std::cin);
    } else {
      std::ifstream s(files[i]);
      do_pretty(s);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2)
    usage();

  if (strcmp(argv[1], "--diff") == 0) {
    if (argc != 4)
      usage();

    diff(argv[2], argv[3]);
  } else if (strcmp(argv[1], "--pretty-sexp") == 0) {
    if (argc < 3)
      usage();

    pretty(argc - 2, argv + 2);
  } else
    usage();
}
