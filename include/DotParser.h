#pragma once

#include <hlx/include/StringHelpers.h>
#include "DotGraph.h"

#include <fstream>
#include <unordered_set>

static const std::vector<std::string> GraphTypes = { "graph", "digraph" };
static const std::vector<std::string> EdgeTypes = { "--", "->" };
static const std::unordered_set<std::string> GraphKeywords = { "node" };
static const std::vector<std::string> StatementSeparators = { ";","\r\n", "\n" };

inline bool IsGraphKeyword(const std::string& _sKeyword) { return GraphKeywords.count(_sKeyword) != 0; }

class DotParser
{
public:
    DotParser() {};
    ~DotParser() {};

    static DotGraph ParseFromFile(const std::string& _sGraph);

    static DotGraph ParseFromString(const std::string& _sGraph);

    static TAttributes ParseAttributes(const std::string& _sAttribList);
};

inline DotGraph DotParser::ParseFromFile(const std::string& _sGraphFile)
{
    std::ifstream file(_sGraphFile);

    if (file.is_open())
    {
        const std::string sGraph((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        return ParseFromString(sGraph);
    }

    return DotGraph();
}

inline DotGraph DotParser::ParseFromString(const std::string& _sGraph)
{
    DotGraph graph;

    size_t uStart = 0u;
    size_t uEnd = 0u;

    if (hlx::get_body(_sGraph, uStart, uEnd) == false)
    {
        // failed to get {}
        return graph;
    }

    // parse name
    size_t uGraphType = 0u;
    if (size_t uGraphDesc = hlx::find_first_of(_sGraph, GraphTypes, uGraphType); uGraphDesc != std::string::npos)
    {
        const size_t uNameStart = uGraphDesc + GraphTypes[uGraphType].size() + 1u;
        const std::string sGraphName = _sGraph.substr(uNameStart, uStart - uNameStart);
        graph.SetName(hlx::trim(sGraphName));
    }
    else
    {
        return graph;
    }

    TAttributes GlobalNodeAttributes;

    // parse statements
    const std::vector<std::string> Statements = hlx::split(_sGraph, StatementSeparators, uStart + 1u, uEnd);
    for (const std::string& sStatement : Statements)
    {
        const size_t uAttribStart = sStatement.find_first_of('[');
        const size_t uAttribEnd = sStatement.find_last_of(']');

        TAttributes Attributes;
        if (uAttribStart != std::string::npos && uAttribEnd != std::string::npos)
        {
            Attributes = ParseAttributes(sStatement);
        }

        const std::string sStatementBody = sStatement.substr(0u, uAttribStart);
        std::vector<std::string> EdgesNodes = hlx::split(sStatementBody, EdgeTypes[uGraphType]);
        
        if (EdgesNodes.size() <= 1u)
        {
            const std::string sNodeName = hlx::trim(sStatementBody);

            if (IsGraphKeyword(sNodeName))
            {
                if (sNodeName == "node")
                {
                    for (const auto&[k, v] : Attributes)
                    {
                        GlobalNodeAttributes[k] = v;
                    }
                }
            }
            else
            {
                graph.AddNode(sNodeName)->AddAttributes(Attributes);
            }
        }
        else
        {
            std::string sNodeName = hlx::trim(EdgesNodes.front());
            DotNode* pPrevNode = graph.AddNode(sNodeName);

            for (size_t i = 1u; i < EdgesNodes.size(); ++i)
            {
                sNodeName = hlx::trim(EdgesNodes[i]);
                DotNode* pNode = graph.AddNode(sNodeName);
                pPrevNode->AddSuccessor(pNode, Attributes);
                pPrevNode = pNode;
            }
        }        
    }

    for (const auto&[k, v] : GlobalNodeAttributes)
    {
        for (DotNode& node : graph.GetNodes())
        {
            node.SetAttribute(k, v, false);
        }
    }

    return graph;
}

inline TAttributes DotParser::ParseAttributes(const std::string& _sAttribList)
{
    TAttributes Attribs;

    std::vector<std::string> KeyValues = hlx::split(hlx::get_body(_sAttribList, "[", "]"), ',');
    
    std::vector<std::string> KeyValue;
    for (const std::string& KV : KeyValues)
    {
        KeyValue = hlx::split(KV, '=');
        if (KeyValue.size() == 2u)
        {
            Attribs[hlx::trim(KeyValue[0])] = hlx::trim(KeyValue[1]);
        }
    }

    return Attribs;
}