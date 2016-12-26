#ifndef TYPE_H
#define TYPE_H

#include <string>

enum class ReferenceType {
  RT_FUNCTION_DECL,
  RT_FUNCTION_DEFI,
  RT_FUNCTION_REF
};

const std::string referenceTypeToString(ReferenceType type);

#endif /* TYPE_H */
