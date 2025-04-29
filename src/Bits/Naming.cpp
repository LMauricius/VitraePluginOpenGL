#include "VitraePluginOpenGL/Bits/Naming.hpp"

#include "Vitrae/TypeConversion/StringCvt.hpp"

namespace Vitrae
{

String convert2GLSLTypeName(StringView name)
{
    String ret;

    bool modified = false;

    for (char c : name) {
        if (c == '<') {
            ret += '4';
            modified = true;
        } else if (c == '>') {
            ret += '7';
            modified = true;
        } else if (c == '*') {
            ret += '2';
            modified = true;
        } else if (c == ',') {
            ret += "__";
            modified = true;
        } else if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
            ret += c;
        } else {
            // skip
            modified = true;
        }
    }

    if (modified) {
        std::size_t hash = std::hash<StringView>{}(name);
        ret += "_" + toHexString(hash);
    }

    return ret;
}

} // namespace Vitrae