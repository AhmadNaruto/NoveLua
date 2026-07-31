#include "lexsoup/document.hpp"
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
    const lxb_char_t *title = lxb_html_document_title(m_document, &len);
    if (!title) return "";
    return std::string((const char*)title, len);
}

Element Document::Head() const {
    return Element(lxb_dom_interface_node(lxb_html_document_head_element(m_document)));
}

Element Document::Body() const {
    return Element(lxb_dom_interface_node(lxb_html_document_body_element(m_document)));
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
