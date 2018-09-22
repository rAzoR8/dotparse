#pragma once

#include "DotGraph.h"
#include <ostream>

class DotWriter
{
public:
    DotWriter() {};
    ~DotWriter() {};

    static void WriteToStream(const DotGraph& _Graph, std::ostream& _OutStream);

    static void WriteAttributesToStream(const TAttributes& _Attribs, std::ostream& _OutStream);
};

inline void DotWriter::WriteToStream(const DotGraph& _Graph, std::ostream& _OutStream)
{
    _OutStream << GraphTypes[_Graph.GetType()] << ' ' << _Graph.GetName() << '{' << std::endl;

    // Graph attributes
    for (const auto& GA : _Graph.GetGlobalAttributes())
    {
        _OutStream << '\t' << GA.first;
        WriteAttributesToStream(GA.second, _OutStream);
        _OutStream << std::endl;
    }

    for (const DotNode& N : _Graph)
    {
        // Node attributes
        if (N.GetAttributes().empty() == false || N.GetSuccessors().empty())
        {
            _OutStream << '\t' << N.GetName() << ' ';
            WriteAttributesToStream(N.GetAttributes(), _OutStream);
            _OutStream << std::endl;
        }

        // edges
        for (const DotNode::Successor& Succ : N.GetSuccessors())
        {
            _OutStream << '\t' << N.GetName() << ' ' << EdgeTypes[_Graph.GetType()] << ' ' << Succ.pNode->GetName();
             WriteAttributesToStream(Succ.Attributes, _OutStream);
            _OutStream << ';' << std::endl;
        }
    }

    _OutStream << '}' << std::endl;
}

inline void DotWriter::WriteAttributesToStream(const TAttributes& _Attribs, std::ostream& _OutStream)
{
    if (_Attribs.empty() == false)
    {
        _OutStream << '[';

        for (auto it = _Attribs.begin(), end = _Attribs.end(); it != end;)
        {
            _OutStream << it->first << '=' << it->second;
            it = ++it;

            if (it != end)
            {
                _OutStream << ',';
            }
        }

        _OutStream << ']';    
    }
}
