#include "lexsoup/selector.hpp"
#include <lexbor/dom/interfaces/element.h>
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

static bool MatchesSelector(Element el, const std::string& css) {
    if (css.empty()) return false;
    std::string tag = css;
    std::string class_filter;
    std::string id_filter;

    size_t hash_pos = css.find('#');
    size_t dot_pos = css.find('.');

    if (hash_pos != std::string::npos) {
        tag = css.substr(0, hash_pos);
        id_filter = css.substr(hash_pos + 1);
    } else if (dot_pos != std::string::npos) {
        tag = css.substr(0, dot_pos);
        class_filter = css.substr(dot_pos + 1);
    }

    if (!id_filter.empty() && el.Id() != id_filter) return false;
    if (!class_filter.empty() && el.ClassName().find(class_filter) == std::string::npos) return false;

    if (!tag.empty()) {
        std::string elTag = el.TagName();
        std::transform(elTag.begin(), elTag.end(), elTag.begin(), [](unsigned char c){ return std::tolower(c); });
        std::transform(tag.begin(), tag.end(), tag.begin(), [](unsigned char c){ return std::tolower(c); });
        if (elTag != tag) return false;
    }

    return true;
}

static void WalkTree(lxb_dom_node_t* node, const std::string& css, std::vector<Element>& results) {
    if (!node) return;

    if (node->type == LXB_DOM_NODE_TYPE_ELEMENT) {
        Element el(node);
        if (MatchesSelector(el, css)) {
            results.push_back(el);
        }
    }

    for (lxb_dom_node_t* child = node->first_child; child; child = child->next) {
        WalkTree(child, css, results);
    }
}

std::vector<Element> Selector::Select(Element root, const std::string& css) {
    std::vector<Element> results;
    WalkTree(root.GetNativeNode(), css, results);
    return results;
}

}
