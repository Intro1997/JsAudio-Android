#pragma once
#include <string>
#include <vector>

class Argv {
public:
  Argv(std::vector<std::string> argv);
  ~Argv();
  char **get() const;
  size_t count() const;

private:
  char **argv_;
  size_t count_;
};