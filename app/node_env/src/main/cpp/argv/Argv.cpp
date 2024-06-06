#include "Argv.hpp"

Argv::Argv(std::vector<std::string> argv) {
  count_ = argv.size();
  if (count_ > 0) {
    argv_ = new char *[count_]();
    for (uint32_t i = 0; i < count_; i++) {
      size_t argv_string_size = argv[i].size() + 1;
      argv_[i] = new char[argv_string_size]();
      memcpy(argv_[i], argv[i].c_str(), argv_string_size);
    }
  }
}

char **Argv::get() const { return argv_; }

size_t Argv::count() const { return count_; }

Argv::~Argv() {
  while (count_--) {
    delete[] argv_[count_];
  }
  delete[] argv_;

  argv_ = nullptr;
}