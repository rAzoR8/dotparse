#pragma once

#include <vector>
#include <unordered_map>
#include <string>

using TAttributes = std::unordered_map<std::string, std::string>;

class DotNode
{
public:
    struct Successor
    {
        Successor(DotNode* _pNode = nullptr, const TAttributes& _Attributes = {}) :
         pNode(_pNode), Attributes(_Attributes) {}

        DotNode* pNode = nullptr;
        TAttributes Attributes; // Edge Attributes
    };

    DotNode(const std::string& _sName = {}, const std::vector<Successor>& _Successors = {}, const TAttributes& _Attributes = {}):
        m_sName(_sName), m_Successors(_Successors), m_Attributes(_Attributes){}

    DotNode(DotNode&& _Other) :
        m_sName(std::move(_Other.m_sName)),
        m_Attributes(std::move(_Other.m_Attributes)),
        m_Successors(std::move(_Other.m_Successors))
    {}

    ~DotNode() {};

    DotNode* AddSuccessor(DotNode* _pNode, const TAttributes& _Attributes = {})
    {
        m_Successors.emplace_back(_pNode, _Attributes);
        return this;
    }

    DotNode* AddAttributes(const TAttributes& _Attributes, const bool _bOverride = false)
    {
        for (const auto&[k, v] : _Attributes)
        {
            SetAttribute(k, v, _bOverride);
        }
        return this;
    }

    DotNode*  SetAttribute(const std::string& _sKey, const std::string& _sValue, const bool _bOverride = false)
    {
        if (_bOverride || m_Attributes.count(_sKey) == 0u)
        {
            m_Attributes[_sKey] = _sValue;        
        }
        return this;
    }

    const std::string& GetName() const { return m_sName; }
    const TAttributes& GetAttributes() const { return m_Attributes; }
    const std::vector<Successor>& GetSuccessors() const { return m_Successors; }

private:
    std::string m_sName;

    TAttributes m_Attributes; // Node Attributes
    std::vector<Successor> m_Successors;
};
