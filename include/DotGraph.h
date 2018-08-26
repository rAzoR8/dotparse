#pragma once

#include "DotNode.h"
#include <unordered_map>

class DotGraph
{
public:
    DotGraph(const std::string& _sName = {}, const size_t _uSize = 128u) :
        m_sName(_sName) { m_Nodes.reserve(_uSize); };

    DotGraph(const DotGraph& _Other) :
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

    const std::vector<DotNode>& GetNodes() const { return m_Nodes; }
    std::vector<DotNode>& GetNodes() { return m_Nodes; }

    const std::string& GetName() const { return m_sName; }
    void SetName(const std::string& _sName) { m_sName = _sName; }

private:
    std::string m_sName;
    std::vector<DotNode> m_Nodes;
    // name -> node
    std::unordered_map<std::string, DotNode*> m_NodeMap;
};