#pragma once
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
