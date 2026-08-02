#include "lexsoup/element.hpp"
#include <lexbor/html/interfaces/element.h>
#include <lexbor/dom/interfaces/attr.h>
#include <lexbor/html/serialize.h>

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
    if (!m_node) return "";
    lexbor_str_t str = {0};
    lxb_status_t status = lxb_html_serialize_deep_str(m_node, &str);
    if (status != LXB_STATUS_OK) return "";
    std::string res((const char*)str.data, str.length);
    lexbor_str_destroy(&str, false);
    return res;
}

std::string Element::OuterHtml() const {
    if (!m_node) return "";
    lexbor_str_t str = {0};
    lxb_status_t status = lxb_html_serialize_tree_str(m_node, &str);
    if (status != LXB_STATUS_OK) return "";
    std::string res((const char*)str.data, str.length);
    lexbor_str_destroy(&str, false);
    return res;
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

bool Element::SetAttr(const std::string& key, const std::string& value) {
    if (!m_node || m_node->type != LXB_DOM_NODE_TYPE_ELEMENT) return false;
    lxb_dom_attr_t* attr = lxb_dom_element_set_attribute(lxb_dom_interface_element(m_node),
                                                         (const lxb_char_t*)key.c_str(), key.length(),
                                                         (const lxb_char_t*)value.c_str(), value.length());
    return attr != nullptr;
}
bool Element::HasAttr(const std::string& key) const { return !Attr(key).empty(); }
void Element::RemoveAttr(const std::string& key) {
    if (!m_node || m_node->type != LXB_DOM_NODE_TYPE_ELEMENT) return;
    lxb_dom_element_remove_attribute(lxb_dom_interface_element(m_node),
                                     (const lxb_char_t*)key.c_str(), key.length());
}

bool Element::Append(const std::string& html) { return false; }
bool Element::Prepend(const std::string& html) { return false; }
bool Element::Before(const std::string& html) { return false; }
bool Element::After(const std::string& html) { return false; }

bool Element::AppendChild(Element child) { return false; }
void Element::Remove() {
    if (m_node) {
        lxb_dom_node_remove(m_node);
    }
}
void Element::Empty() {
    if (m_node) {
        lxb_dom_node_t* child = m_node->first_child;
        while (child) {
            lxb_dom_node_t* next = child->next;
            lxb_dom_node_remove(child);
            child = next;
        }
    }
}

std::vector<Element> Element::Children() const {
    std::vector<Element> result;
    if (!m_node) return result;
    for (lxb_dom_node_t* child = m_node->first_child; child; child = child->next) {
        if (child->type == LXB_DOM_NODE_TYPE_ELEMENT) {
            result.push_back(Element(child));
        }
    }
    return result;
}
Element Element::Child(size_t index) const {
    if (!m_node) return Element(nullptr);
    size_t current = 0;
    for (lxb_dom_node_t* child = m_node->first_child; child; child = child->next) {
        if (child->type == LXB_DOM_NODE_TYPE_ELEMENT) {
            if (current == index) {
                return Element(child);
            }
            current++;
        }
    }
    return Element(nullptr);
}
Element Element::Parent() const {
    if (!m_node) return Element(nullptr);
    return Element(m_node->parent);
}
Element Element::Next() const {
    if (!m_node) return Element(nullptr);
    for (lxb_dom_node_t* sib = m_node->next; sib; sib = sib->next) {
        if (sib->type == LXB_DOM_NODE_TYPE_ELEMENT) {
            return Element(sib);
        }
    }
    return Element(nullptr);
}
Element Element::Previous() const {
    if (!m_node) return Element(nullptr);
    for (lxb_dom_node_t* sib = m_node->prev; sib; sib = sib->prev) {
        if (sib->type == LXB_DOM_NODE_TYPE_ELEMENT) {
            return Element(sib);
        }
    }
    return Element(nullptr);
}

}
