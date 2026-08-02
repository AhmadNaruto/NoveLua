#include "lexsoup/selector.hpp"
#include <lexbor/dom/interfaces/element.h>
#include <lexbor/css/css.h>
#include <algorithm>
#include <cctype>

namespace novelua::lexsoup {

Selector::Selector() {
    m_selectors = lxb_selectors_create();
    lxb_selectors_init(m_selectors);
}

Selector::~Selector() {
    if (m_selectors) {
        lxb_selectors_destroy(m_selectors, true);
    }
}


static lxb_status_t find_callback(lxb_dom_node_t *node, lxb_css_selector_specificity_t spec, void *ctx) {
    auto* results = static_cast<std::vector<Element>*>(ctx);
    results->push_back(Element(node));
    return LXB_STATUS_OK;
}

std::vector<Element> Selector::Select(Element root, const std::string& css) {
    std::vector<Element> results;
    if (css.empty() || !root.GetNativeNode()) return results;

    lxb_css_parser_t* parser = lxb_css_parser_create();
    if (!parser) return results;

    lxb_status_t status = lxb_css_parser_init(parser, NULL);
    if (status != LXB_STATUS_OK) {
        lxb_css_parser_destroy(parser, true);
        return results;
    }

    lxb_css_selector_list_t* list = lxb_css_selectors_parse(parser, (const lxb_char_t*)css.c_str(), css.length());
    if (list) {
        lxb_selectors_find(m_selectors, root.GetNativeNode(), list, find_callback, &results);
        lxb_css_selector_list_destroy(list);
    }

    lxb_css_parser_destroy(parser, true);
    return results;
}

}

