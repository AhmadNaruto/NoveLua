import os

def create_file(path, content):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w') as f:
        f.write(content)

src_dir = "native/lexsoup/src"

doc_cpp = """#include "lexsoup/document.hpp"
#include <lexbor/html/html.h>
#include <lexbor/html/interfaces/element.h>

namespace novelua::lexsoup {

Document::Document() {
    m_document = lxb_html_document_create();
}

Document::~Document() {
    if (m_document) {
        lxb_html_document_destroy(m_document);
    }
}

bool Document::Parse(const std::string& html) {
    if (!m_document) return false;
    lxb_status_t status = lxb_html_document_parse(m_document, (const lxb_char_t*)html.c_str(), html.length());
    return status == LXB_STATUS_OK;
}

std::string Document::Title() const {
    size_t len = 0;
    lxb_char_t *title = lxb_html_document_title(m_document, &len);
    if (!title) return "";
    return std::string((const char*)title, len);
}

Element Document::Head() const {
    return Element(lxb_dom_interface_node(lxb_html_document_head(m_document)));
}

Element Document::Body() const {
    return Element(lxb_dom_interface_node(lxb_html_document_body(m_document)));
}

std::string Document::Html() const {
    return Element(lxb_dom_interface_node(m_document)).Html();
}

std::string Document::OuterHtml() const {
    return Html();
}

std::string Document::Text() const {
    return Element(lxb_dom_interface_node(m_document)).Text();
}

Element Document::CreateElement(const std::string& tag) {
    lxb_dom_element_t* el = lxb_dom_document_create_element(lxb_dom_interface_document(m_document), 
                                                           (const lxb_char_t*)tag.c_str(), tag.length(), NULL);
    return Element(lxb_dom_interface_node(el));
}

}
"""

elem_cpp = """#include "lexsoup/element.hpp"
#include <lexbor/html/interfaces/element.h>

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
    // This requires proper lexbor serialization which is complex for innerHTML
    return OuterHtml(); 
}

std::string Element::OuterHtml() const {
    return "";
}

std::string Element::Attr(const std::string& key) const {
    return "";
}

bool Element::SetAttr(const std::string& key, const std::string& value) { return false; }
bool Element::HasAttr(const std::string& key) const { return false; }
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
"""

sel_cpp = """#include "lexsoup/selector.hpp"

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

std::vector<Element> Selector::Select(Element root, const std::string& css) {
    return {};
}

}
"""

create_file(f"{src_dir}/document.cpp", doc_cpp)
create_file(f"{src_dir}/element.cpp", elem_cpp)
create_file(f"{src_dir}/selector.cpp", sel_cpp)
