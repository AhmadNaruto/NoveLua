import os

def create_file(path, content):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w') as f:
        f.write(content)

include_dir = "native/lexsoup/include/lexsoup"
src_dir = "native/lexsoup/src"

doc_hpp = """#pragma once
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
"""

elem_hpp = """#pragma once
#include <string>
#include <vector>
#include <lexbor/dom/interfaces/element.h>
#include <lexbor/dom/interfaces/node.h>

namespace novelua::lexsoup {

class Element {
public:
    Element(lxb_dom_node_t* node) : m_node(node) {}
    
    std::string TagName() const;
    std::string Id() const;
    std::string ClassName() const;
    std::string Text() const;
    std::string Html() const;
    std::string OuterHtml() const;

    std::string Attr(const std::string& key) const;
    bool SetAttr(const std::string& key, const std::string& value);
    bool HasAttr(const std::string& key) const;
    void RemoveAttr(const std::string& key);

    bool Append(const std::string& html);
    bool Prepend(const std::string& html);
    bool Before(const std::string& html);
    bool After(const std::string& html);

    bool AppendChild(Element child);
    void Remove();
    void Empty();

    std::vector<Element> Children() const;
    Element Child(size_t index) const;
    Element Parent() const;
    Element Next() const;
    Element Previous() const;

    lxb_dom_node_t* GetNativeNode() const { return m_node; }

private:
    lxb_dom_node_t* m_node;
};
}
"""

sel_hpp = """#pragma once
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
"""

create_file(f"{include_dir}/document.hpp", doc_hpp)
create_file(f"{include_dir}/element.hpp", elem_hpp)
create_file(f"{include_dir}/selector.hpp", sel_hpp)
