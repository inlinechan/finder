#include "Type.h"

const std::string referenceTypeToString(ReferenceType type) {
  switch (type) {
  case ReferenceType::RT_FUNCTION_DECL:
    return "RT_FUNCTION_DECL";
  case ReferenceType::RT_FUNCTION_DEFI:
    return "RT_FUNCTION_DEFI";
  case ReferenceType::RT_FUNCTION_REF:
    return "RT_FUNCTION_REF";
  }
  return "Unknown";
}
