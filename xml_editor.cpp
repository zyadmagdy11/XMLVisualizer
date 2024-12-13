#include "Formatting.cpp"
#include "Minifying.cpp"
#include "XML_Consistency.cpp"
#include "xml2json.cpp"
#include "compression.cpp"

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

    if (command == "verify")
    {
        string xml = readXMLFile(inputFile);
        if (xml.empty())
        {
            cerr << "Error: Failed to read input file.\n";
            return 1;
        }

        if (checkXMLConsistency(xml))
        {
            cout << "Output: XML is valid.\n";
        }
        else
        {
            cout << "Output: XML is invalid.\n";
            vector<int> errors = findMismatchedTags(xml);
            cout << "Number of errors: " << errors.size() << "\n";
            for (int line : errors)
            {
                cout << "Error at line: " << line << "\n";
            }
            if (fixErrors && !outputFile.empty())
            {
                string correctedXml = correctMismatchedTags(xml, errors);
                ofstream outFile(outputFile);
                if (!outFile.is_open())
                {
                    cerr << "Error: Failed to write to output file.\n";
                    return 1;
                }
                outFile << correctedXml;
                cout << "Errors fixed. Corrected file saved as: " << outputFile << "\n";
            }
        }
    }
    else if (command == "format")
    {
        string xml = readXMLFile(inputFile);
        if (xml.empty())
        {
            cerr << "Error: Failed to read input file.\n";
            return 1;
        }

        string formattedXml = FormattingFunction(xml);
        ofstream outFile(outputFile);
        if (!outFile.is_open())
        {
            cerr << "Error: Failed to write to output file.\n";
            return 1;
        }
        outFile << formattedXml;
        cout << "Formatted XML saved to " << outputFile << "\n";
    }
    else if (command == "json")
    {
        string xml = readXMLFile(inputFile);
        if (xml.empty())
        {
            cerr << "Error: Failed to read input file.\n";
            return 1;
        }

        XmlToJsonConverter converter;
        string json = converter.convertToJson(xml);
        ofstream outFile(outputFile);
        if (!outFile.is_open())
        {
            cerr << "Error: Failed to write to output file.\n";
            return 1;
        }
        outFile << json;
        cout << "Converted JSON saved to " << outputFile << "\n";
    }
    else if (command == "mini")
    {
        string xml = readXMLFile(inputFile);
        if (xml.empty())
        {
            cerr << "Error: Failed to read input file.\n";
            return 1;
        }

        string minifiedXml = MinifyingFunction(xml);
        ofstream outFile(outputFile);
        if (!outFile.is_open())
        {
            cerr << "Error: Failed to write to output file.\n";
            return 1;
        }
        outFile << minifiedXml;
        cout << "Minified XML saved to " << outputFile << "\n";
    }
    else if (command == "compress")
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
