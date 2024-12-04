#include "Formatting.h"

using namespace std;  // Add this to use standard library types and functions without std::


int main() {
    string inputFileName, outputFileName;

    // Prompt user for input file name
    cout << "Enter the input XML file name (e.g., input.xml): ";
    cin >> inputFileName;

    ifstream inputFile(inputFileName);
    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open the file '" << inputFileName << "'.\n";
        return 1;
    }

    // Read the file content into a string
    ostringstream buffer;
    buffer << inputFile.rdbuf();
    string xmlContent = buffer.str();
    inputFile.close();

    // Format the XML
    string formattedXML = FormattingFunction(xmlContent);

    // Prompt user for output file name
    cout << "Enter the output XML file name (e.g., output.xml): ";
    cin >> outputFileName;

    ofstream outputFile(outputFileName);
    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open the file '" << outputFileName << "'.\n";
        return 1;
    }

    // Write the formatted XML to the output file
    outputFile << formattedXML;
    outputFile.close();

    cout << "XML file has been formatted and saved as '" << outputFileName << "'.\n";

    // Optional: Display size and line information
    cout << "Original XML size: " << xmlContent.size() << " bytes\n";
    cout << "Formatted XML size: " << formattedXML.size() << " bytes\n";

    return 0;
}