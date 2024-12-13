using namespace std;  // Add this to use standard library types and functions without std::

#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <string>  // Include string header

string MinifyingFunction(const string& input) {
    string output;
    bool insideTag = false;
    
    for (size_t i = 0; i < input.size(); ++i) {
        char ch = input[i];
        
        if (ch == '<') {
            // Start of a new tag
            if (!output.empty() && output.back() != '\n') {
                output += '\n'; // Add a newline before starting a new tag
            }
            insideTag = true;
            output += ch;
        }
        else if (ch == '>') {
            // End of a tag
            insideTag = false;
            output += ch;
            output += '\n'; // Add a newline after closing the tag
        }
        else if (insideTag) {
            // Inside a tag, preserve all characters
            output += ch;
        }
        else {
            // Outside tags (text content)
            if (!isspace(static_cast<unsigned char>(ch)) || (output.empty() || output.back() != '\n')) {
                // Preserve text content but trim excessive whitespace
                output += ch;
            }
        }
    }
    return output;
}