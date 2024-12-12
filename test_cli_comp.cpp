#include "compression.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        cerr << "Usage: xml_editor <command> -i <input_file> [-o <output_file>] [-f]\n";
        return 1;
    }

    string command = argv[1];
    string inputFile, outputFile;
    bool fixErrors = false;

    for (int i = 2; i < argc; ++i)
    {
        if (string(argv[i]) == "-i" && i + 1 < argc)
        {
            inputFile = argv[++i];
        }
        else if (string(argv[i]) == "-o" && i + 1 < argc)
        {
            outputFile = argv[++i];
        }
        else if (string(argv[i]) == "-f")
        {
            fixErrors = true;
        }
    }

    if (inputFile.empty())
    {
        cerr << "Error: Input file not specified.\n";
        return 1;
    }    
    if (command == "compress")
    {
        if (outputFile.empty())
        {
            cerr << "Error: Output file not specified for compression.\n";
            return 1;
        }
        compress(inputFile, outputFile);
    }
    else if (command == "decompress")
    {
        if (outputFile.empty())
        {
            cerr << "Error: Output file not specified for decompression.\n";
            return 1;
        }
        decompress(inputFile, outputFile);
    }
    else
    {
        cerr << "Invalid command.\n";
        return 1;
    }

    return 0;
}
