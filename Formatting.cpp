#include "Formatting.h"

using namespace std;  // Add this to use standard library types and functions without std::

// Function to Format XML content
string FormattingFunction(const string& input) {
    string output;
    stack<string> tagStack; // Stack to track tags for indentation
    string currentTag;      // Temporary storage for current tag
    bool insideTag = false; // To check if parsing inside a tag
    int indentationLevel = 0;

    for (size_t i = 0; i < input.size(); ++i) {
        char ch = input[i];

        if (ch == '<') {
            // Start of a tag
            if (!currentTag.empty()) {
                // If there's text content, write it with proper indentation
                if (!output.empty() && output.back() != '\n') {
                    output += '\n';
                }
                output += string(indentationLevel * 2, ' ') + currentTag + '\n';
                currentTag.clear();
            }
            insideTag = true;
            currentTag += ch;
        } else if (ch == '>') {
            // End of a tag
            insideTag = false;
            currentTag += ch;

            if (currentTag[1] == '/') {
                // Closing tag
                if (!tagStack.empty()) {
                    tagStack.pop();
                    indentationLevel--; // Reduce indentation level
                }
                output += string(indentationLevel * 2, ' ') + currentTag + '\n';
            } else {
                // Opening tag or self-closing tag
                size_t spacePos = currentTag.find(' ');
                string tagName = (spacePos != string::npos)
                                     ? currentTag.substr(1, spacePos - 1)
                                     : currentTag.substr(1, currentTag.size() - 2);

                if (currentTag.back() == '/') {
                    // Self-closing tag
                    output += string(indentationLevel * 2, ' ') + currentTag + '\n';
                } else {
                    // Opening tag
                    output += string(indentationLevel * 2, ' ') + currentTag + '\n';
                    tagStack.push(tagName);
                    indentationLevel++;
                }
            }
            currentTag.clear();
        } else if (insideTag) {
            // Inside a tag, continue building it
            currentTag += ch;
        } else {
            // Outside of tags, handle text content
            if (!isspace(static_cast<unsigned char>(ch)) || (!output.empty() && output.back() != '\n')) {
                currentTag += ch;
            }
        }
    }

    // Add any remaining content in `currentTag` to the output
    if (!currentTag.empty()) {
        if (!output.empty() && output.back() != '\n') {
            output += '\n';
        }
        output += string(indentationLevel * 2, ' ') + currentTag + '\n';
    }

    return output;
}
