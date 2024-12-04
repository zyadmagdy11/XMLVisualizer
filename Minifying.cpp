#include "Minifying.h"

using namespace std;  // Add this to use standard library types and functions without std::

// Function to minify XML content using a stack
string MinifyingFunction(const string& input) {
    string output;
    stack<char> charStack;  // Stack to process characters
    bool insideTag = false;

    for (size_t i = 0; i < input.size(); ++i) {
        char ch = input[i];

        if (ch == '<') {
            // Start of a new tag
            if (!output.empty() && output.back() != '\n') {
                output += '\n'; // Add a newline before starting a new tag
            }
            charStack.push(ch); // Push '<' to the stack
            insideTag = true;
            output += ch;
        } else if (ch == '>') {
            // End of a tag
            insideTag = false;
            while (!charStack.empty() && charStack.top() == '<') {
                charStack.pop(); // Pop matched '<' from the stack
            }
            output += ch;
            output += '\n'; // Add a newline after closing the tag
        } else if (insideTag) {
            // Inside a tag
            charStack.push(ch); // Push tag characters
            output += ch;
        } else {
            // Outside tags (text content) .... isspace to check if it is character or space 
            // .back() It returns a reference to the last element of the container.
            if (!isspace(static_cast<unsigned char>(ch)) || (output.empty() || output.back() != '\n')) {
                // Preserve text content but trim excessive whitespace
                charStack.push(ch); // Push text content
                output += ch;
            }
        }
    }
    return output;
}