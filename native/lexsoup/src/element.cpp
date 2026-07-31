#include "lexsoup/element.hpp"
#include <lexbor/html/interfaces/element.h>
#include <lexbor/dom/interfaces/attr.h>

namespace novelua::lexsoup {

std::string Element::TagName() const {
    if (!m_node || m_node->type != LXB_DOM_NODE_TYPE_ELEMENT) return "";
    size_t len;
    const lxb_char_t* name = lxb_dom_element_qualified_name(lxb_dom_interface_element(m_node), &len);
    if (!name) return "";
    return std::string((const char*)name, len);
}

std::string Element::Id() const {
    return Attr("id");
}

std::string Element::ClassName() const {
    return Attr("class");
}

std::string Element::Text() const {
    if (!m_node) return "";
    size_t len;
    lxb_char_t* text = lxb_dom_node_text_content(m_node, &len);
    if (!text) return "";
    std::string res((const char*)text, len);
    lexbor_free(text);
    return res;
}

std::string Element::Html() const {
    return OuterHtml(); 
}

std::string Element::OuterHtml() const {
    return "";
}

std::string Element::Attr(const std::string& key) const {
    if (!m_node || m_node->type != LXB_DOM_NODE_TYPE_ELEMENT) return "";
    lxb_dom_attr_t* attr = lxb_dom_element_attr_by_name(lxb_dom_interface_element(m_node),
                                                       (const lxb_char_t*)key.c_str(),
                                                       key.length());
    if (!attr) return "";
    size_t value_len = 0;
    const lxb_char_t* val = lxb_dom_attr_value(attr, &value_len);
    if (!val) return "";
    return std::string((const char*)val, value_len);
}

bool Element::SetAttr(const std::string& key, const std::string& value) { return false; }
bool Element::HasAttr(const std::string& key) const { return !Attr(key).empty(); }
void Element::RemoveAttr(const std::string& key) {}

bool Element::Append(const std::string& html) { return false; }
bool Element::Prepend(const std::string& html) { return false; }
bool Element::Before(const std::string& html) { return false; }
bool Element::After(const std::string& html) { return false; }

bool Element::AppendChild(Element child) { return false; }
void Element::Remove() {}
void Element::Empty() {}

std::vector<Element> Element::Children() const { return {}; }
Element Element::Child(size_t index) const { return Element(nullptr); }
Element Element::Parent() const { return Element(nullptr); }
Element Element::Next() const { return Element(nullptr); }
Element Element::Previous() const { return Element(nullptr); }

}
