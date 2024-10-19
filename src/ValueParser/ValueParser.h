#ifndef VALUE_PARSER_H
#define VALUE_PARSER_H

#include <string>

namespace cpp_cli {

template <typename T>
T parse(const std::string &toParse);
// {
//   static_assert(
//       false,
//       "Please provide a parse implementation that returns a shared ptr of type () with signature "
//       "std::shared_ptr<your-type>(const char*)\n"
//   );

//   return std::shared_ptr<T>{};
// }
}  // namespace cpp_cli

#endif
