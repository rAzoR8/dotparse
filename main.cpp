#include "include/DotParser.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
        return 1;

    DotGraph graph = DotParser::ParseFromFile(argv[1]);

    return 0;
}