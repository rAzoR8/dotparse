#pragma once

#include "DotNode.h"
#include <unordered_map>

enum EGlobalAttrib : uint32_t 
{
    kGlobalAttrib_Node = 0,
    kGlobalAttrib_NumOf
};

static const std::vector<std::string> GraphTypes = { "graph", "digraph" };
static const std::vector<std::string> EdgeTypes = { "--", "->" };
static const std::vector<std::string> GlobalAttribKeywords = { "node" };
inline bool IsGlobalAttrib(const std::string& _sKeyword) { return std::find(GlobalAttribKeywords.begin(), GlobalAttribKeywords.end(), _sKeyword) != GlobalAttribKeywords.end(); }

enum EGraphType : uint32_t
{
    kGraphType_Undirected = 0,
    kGraphType_Directed = 1
};

using TGlobalAttributes = std::unordered_map<std::string, TAttributes>;

class DotGraph
{
public:
    using Nodes = std::vector<DotNode>;

    DotGraph(const EGraphType _kType = kGraphType_Undirected, const std::string& _sName = {}, const size_t _uSize = 128u) :
        m_kType(_kType), m_sName(_sName) { m_Nodes.reserve(_uSize); };

    DotGraph(const DotGraph& _Other) :
        m_kType(_Other.m_kType),
        m_sName(_Other.m_sName)
    {
        m_Nodes.reserve(_Other.m_Nodes.size());
        for (const DotNode& node : _Other.m_Nodes)
        {
            DotNode* pNode = AddNode(node.GetName())->AddAttributes(node.GetAttributes());
            for (const DotNode::Successor& succ : node.GetSuccessors())
            {
                pNode->AddSuccessor(AddNode(succ.pNode->GetName())->AddAttributes(succ.pNode->GetAttributes()), succ.Attributes);
            }
        }
    }

    DotGraph(DotGraph&& _Other) : 
        m_kType(_Other.m_kType),
        m_sName(_Other.m_sName),
        m_NodeMap(std::move(_Other.m_NodeMap)),
        m_Nodes(std::move(_Other.m_Nodes))
    {    
    }

    ~DotGraph() {};

    template <class Name, class ...Args>
    DotNode* AddNode(Name&& _sName, Args&& ... _Args)
    {
        if (auto it = m_NodeMap.find(_sName); it != m_NodeMap.end())
        {
            return it->second;
        }

        DotNode* pNode = &m_Nodes.emplace_back(std::forward<Name>(_sName), std::forward<Args>(_Args)...);
        m_NodeMap.insert({ _sName, pNode });
        return pNode;
    }

    // applies global node attribues to all nodes
    void ApplyGlobalNodeAttributes();

    const Nodes& GetNodes() const { return m_Nodes; }
    Nodes& GetNodes() { return m_Nodes; }

    typename Nodes::iterator begin() noexcept { return m_Nodes.begin(); }
    typename Nodes::iterator end() noexcept { return m_Nodes.end(); }

    typename Nodes::const_iterator begin() const noexcept { return m_Nodes.begin(); }
    typename Nodes::const_iterator end() const noexcept { return m_Nodes.end(); }

    const std::string& GetName() const { return m_sName; }
    void SetName(const std::string& _sName) { m_sName = _sName; }

    EGraphType GetType() const { return m_kType; }
    void SetType(const EGraphType _kType) { m_kType = _kType; }
  
    const TGlobalAttributes& GetGlobalAttributes() const { return m_GlobalAttributes; }
    TGlobalAttributes& GetGlobalAttributes() { return m_GlobalAttributes; }

private:
    EGraphType m_kType = kGraphType_Undirected;
    std::string m_sName;
    Nodes m_Nodes;
    // name -> node
    std::unordered_map<std::string, DotNode*> m_NodeMap;
    TGlobalAttributes m_GlobalAttributes;
};

inline void DotGraph::ApplyGlobalNodeAttributes()
{
    if (auto it = m_GlobalAttributes.find(GlobalAttribKeywords[kGlobalAttrib_Node]); it != m_GlobalAttributes.end())
    {
        for (const auto&[k, v] : it->second)
        {
            for (DotNode& node : m_Nodes)
            {
                node.SetAttribute(k, v, false);
            }
        }
    }    
}