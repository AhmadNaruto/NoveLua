#pragma once
#include <string>
#include <lexbor/html/parser.h>
#include <lexbor/html/serialize.h>
#include "lexsoup/element.hpp"

namespace novelua::lexsoup {

class Document {
public:
    Document();
    ~Document();

    bool Parse(const std::string& html);
    
    std::string Title() const;
    Element Head() const;
    Element Body() const;
    
    std::string Html() const;
    std::string OuterHtml() const;
    std::string Text() const;

    Element CreateElement(const std::string& tag);
    
    lxb_html_document_t* GetNativeDocument() const { return m_document; }

private:
    lxb_html_document_t* m_document;
};
}
