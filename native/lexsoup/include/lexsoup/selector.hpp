#pragma once
#include <string>
#include <vector>
#include "lexsoup/element.hpp"
#include <lexbor/selectors/selectors.h>

namespace novelua::lexsoup {
class Selector {
public:
    Selector();
    ~Selector();
    std::vector<Element> Select(Element root, const std::string& css);
private:
    lxb_selectors_t* m_selectors;
};
}
