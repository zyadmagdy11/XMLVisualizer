#include "Formatting.cpp"
#include "Minifying.cpp"
#include "XML_Consistency.cpp"
#include "xml2json.cpp"
#include "compression.cpp"
#include "Graph.cpp"
#include <sstream>

using namespace std;

vector<string> splitString(const string &input, char delimiter)
{
    vector<string> tokens;
    stringstream ss(input);
    string token;

    while (getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        cerr << "Usage: xml_editor <command> -i <input_file> [-o <output_file>] [options]\n";
        return 1;
    }

    string command = argv[1];
    string inputFile, outputFile;
    bool fixErrors = false;

    // Parse input arguments
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
        cerr << "Error: Input file not specified. Use -i <input_file>.\n";
        return 1;
    }

    // Graph-related commands
    if (command == "draw" || command == "most_active" || command == "most_influencer" || command == "mutual" || command == "suggest" || command == "search")
    {
        Graph network(200);
        network.parseXML(inputFile);
        network.exportToDot("social_network.dot");

        if (command == "draw")
        {
            string pythonCommand = "python Graph_GUI.py";
            if (!outputFile.empty())
            {
                pythonCommand += " -o \"" + outputFile + "\"";
            }
            int result = system(pythonCommand.c_str());
            if (result != 0)
            {
                cerr << "Failed to render graph with Python script.\n";
                return 1;
            }
        }

        else if (command == "most_active")
        {
            User mostActiveUser = network.most_active();
            cout << "Most Active User: " << mostActiveUser.name << " (ID: " << mostActiveUser.id << ")\n";
        }
        else if (command == "most_influencer")
        {
            User mostInfluencer = network.most_influencer();
            cout << "Most Influential User: " << mostInfluencer.name << " (ID: " << mostInfluencer.id << ")\n";
        }
        else if (command == "mutual")
        {
            vector<string> userIds;

            for (int i = 2; i < argc; i++)
            {
                if (string(argv[i]) == "-ids" && i + 1 < argc)
                {
                    userIds = splitString(argv[++i], ',');
                }
            }

            vector<User> mutualFollowers = network.findMutualFollowers(userIds);

            cout << "Mutual Followers:\n";
            for (const User &user : mutualFollowers)
            {
                cout << user.name << " (ID: " << user.id << ")\n";
            }
        }
        else if (command == "suggest")
        {
            string userId;

            for (int i = 2; i < argc; i++)
            {
                if (string(argv[i]) == "-id" && i + 1 < argc)
                {
                    userId = argv[++i];
                }
            }

            vector<User> suggestedUsers = network.suggestFollowers(userId);

            cout << "Suggested Users:\n";
            for (const User &user : suggestedUsers)
            {
                cout << user.name << " (ID: " << user.id << ")\n";
            }
        }
        else if (command == "search")
        {
            string searchTerm;
            string searchType;

            for (int i = 2; i < argc; i++)
            {
                if (string(argv[i]) == "-w" && i + 1 < argc)
                {
                    searchType = "word";
                    searchTerm = argv[++i];
                }
                else if (string(argv[i]) == "-t")
                {
                    searchType = "topic";
                    searchTerm.clear();
                    for (int j = i + 1; j < argc && string(argv[j])[0] != '-'; j++, i++)
                    {
                        if (!searchTerm.empty())
                            searchTerm += " ";
                        searchTerm += argv[j];
                    }
                }
            }

            if (searchTerm.empty())
            {
                cerr << "Search term not specified. Use -w <word> or -t <topic>.\n";
                return 1;
            }

            vector<string> matchedPosts = network.searchPosts(searchTerm);

            cout << "Posts mentioning the " << searchType << " \"" << searchTerm << "\":\n";
            for (const string &post : matchedPosts)
            {
                cout << post << "\n";
            }
        }

        return 0;
    }

    // Existing XML-related commands
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
