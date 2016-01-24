#include <iostream>
#include <pcrecpp.h>

int main(int argc, char ** argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " pattern text\n";
        return 1;
    }

    pcrecpp::RE oPattern(argv[1]);
    if (oPattern.FullMatch(argv[2]))
    {
        std::cout << argv[2] << " fully matches " << argv[1] << "\n";
    }
    else if (oPattern.PartialMatch(argv[2]))
    {
        std::cout << argv[2] << " partially matches " << argv[1] << "\n";
    }
    else
    {
        std::cout << argv[2] << " dose not match " << argv[1] << "\n";
    }

    return 0;
}
