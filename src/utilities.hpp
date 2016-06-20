#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <experimental/optional>
#include <string>

std::experimental::optional<std::string> file_contents(const char* filename);

#endif // !UTILITIES_HPP
