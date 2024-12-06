#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <sstream>
#include <queue>
#include <algorithm>
#include <filesystem>
#include <cctype>

using namespace std;

// ==================== Stack Implementation ====================
template <class T>
class Stack
{
    struct Node
    {
        T data;
        Node *next;

        Node(T val) : data(val), next(nullptr) {}
    };

private:
    Node *top;
    int length;

public:
    Stack() : top(nullptr), length(0) {}

    ~Stack()
    {
        while (!isEmpty())
            pop();
    }

    bool isEmpty()
    {
        return top == nullptr;
    }

    void push(const T &value)
    {
        Node *newNode = new Node(value);
        newNode->next = top;
        top = newNode;
        length++;
    }

    T pop()
    {
        if (isEmpty())
            return T();

        Node *temp = top;
        T poppedValue = temp->data;
        top = top->next;
        delete temp;
        length--;
        return poppedValue;
    }

    T peek()
    {
        if (!isEmpty())
            return top->data;
        return T();
    }
};

// ==================== File Operations ====================
string readXMLFile(string fileName)
{
    ifstream file(fileName);
    if (!file.is_open())
    {
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();
    string compressedContent;

    for (int i = 0; i < content.length(); i++)
    {
        if (!isspace(content[i]))
        {
            compressedContent += content[i];
        }
        else if (isalpha(content[i - 1]) || isalpha(content[i + 1])) // keep spaces between tags
        {
            compressedContent += content[i];
        }
    }

    return compressedContent;
}

// ==================== XML Consistency Functions ====================
bool checkXMLConsistency(string xml)
{
    Stack<string> tagStack;
    int i = 0;

    while (i < xml.length())
    {
        if (xml[i] == '<')
        {
            int closePos = xml.find('>', i);

            string tagContent = xml.substr(i + 1, closePos - i - 1); // Extract tag content between '<' and '>'

            if (tagContent[0] == '/')
            {
                string tagName = tagContent.substr(1); // Remove '/' from the tag

                if (tagStack.isEmpty() || tagStack.peek() != tagName)
                {
                    return false;
                }
                tagStack.pop();
            }
            else
            {
                tagStack.push(tagContent);
            }

            i = closePos + 1;
        }
        else
        {
            i++;
        }
    }

    return tagStack.isEmpty();
}

vector<int> findMismatchedTags(string xml)
{
    vector<string> tagStack;         // Vector to store open tags
    vector<int> positionStack;       // Vector to store open tags positions
    vector<int> mismatchedPositions; // Vector to store mismatched tags positions

    int i = 0;
    int n = xml.length();

    while (i < n)
    {
        if (xml[i] == '<')
        {
            int closePos = xml.find('>', i);
            string tagContent = xml.substr(i + 1, closePos - i - 1);
            bool isClosingTag = (tagContent[0] == '/');
            string tagName = isClosingTag ? tagContent.substr(1) : tagContent;

            int tagPosition = i;

            if (isClosingTag)
            {
                bool matched = false;
                for (int j = tagStack.size() - 1; j >= 0; --j)
                {
                    if (tagStack[j] == tagName)
                    {
                        tagStack.erase(tagStack.begin() + j);
                        positionStack.erase(positionStack.begin() + j);
                        matched = true;
                        break;
                    }
                }
                if (!matched)
                {
                    mismatchedPositions.push_back(tagPosition);
                }
            }
            else
            {
                tagStack.push_back(tagName);
                positionStack.push_back(tagPosition);
            }

            i = closePos + 1;
        }
        else
        {
            ++i;
        }
    }
    mismatchedPositions.insert(mismatchedPositions.end(), positionStack.begin(), positionStack.end());
    sort(mismatchedPositions.begin(), mismatchedPositions.end());

    return mismatchedPositions;
}

int correctindex(string xml, int i)
{
    vector<string> tagStack;
    int n = xml.length();
    bool flag = false;

    while (i < n)
    {
        if (xml[i] == '<')
        {
            int closePos = xml.find('>', i);

            string tagContent = xml.substr(i + 1, closePos - i - 1);
            bool isClosingTag = (tagContent[0] == '/');
            string tagName = isClosingTag ? tagContent.substr(1) : tagContent;

            if (isClosingTag)
            {
                if (tagStack.empty() || tagStack.back() != tagName)
                {
                    return i; // Found a closing tag without its corresponding opening tag
                }
                tagStack.pop_back();
                flag = false;
            }
            else
            {
                if (flag)
                {
                    return i - tagStack.back().length() - 2; // Found two consecutive opening tags
                }
                tagStack.push_back(tagName);
                flag = true;
            }

            i = closePos + 1;
        }
        else
            i++;
    }
    return -1; // No mismatched closing tag found
}

string correctMismatchedTags(string xml)
{
    vector<int> tag_index = findMismatchedTags(xml);
    vector<string> tagContent;
    int offset = 0; // To keep track of changes in the string length
    int closePos;
    string closingTag;
    string r;
    int pos = 0;

    for (int p : tag_index)
    {
        closePos = xml.find('>', p);
        tagContent.push_back(xml.substr(p + 1, closePos - p - 1));
    }

    // Error_Case 1: (no direct close tag).example : <author>Harper Lee
    for (int i = 0; i < tagContent.size(); i++)
    {
        pos = tag_index[i] + offset;
        closingTag = "</" + tagContent[i] + ">";
        closePos = xml.find('>', pos) + 1;
        r = tagContent[i + 1];

        if (xml[closePos] != '<' && r[0] != '/')
        {
            closePos = xml.find('<', closePos);
            xml.insert(closePos, closingTag);
            offset += closingTag.length();

            tagContent.erase(tagContent.begin() + i);
            tag_index.erase(tag_index.begin() + i);
            i--;
        }
        else
            tag_index[i] = tag_index[i] + offset;
    }

    offset = 0;
    int i = 0;
    vector<int> temp_tag_index;
    vector<string> temp_tagContent;

    // Error_Case 2: (wrong close tag).example : <author>Harper Lee</aaauthor>
    for (int pos : tag_index)
    {
        pos += offset; // Adjust position based on previous modifications

        if (xml[pos + 1] == '/')
        {
            // It's a closing tag, remove it
            closePos = xml.find('>', pos);
            xml.erase(pos, closePos - pos + 1);
            offset -= (closePos - pos + 1);

            // Insert the correct closing tag
            closingTag = "</" + temp_tagContent.back() + ">";
            xml.insert(pos, closingTag);
            offset += closingTag.length();

            if (!temp_tagContent.empty())
                temp_tagContent.pop_back();
            if (!temp_tag_index.empty())
                temp_tag_index.pop_back();
        }
        else
        {
            temp_tagContent.push_back(tagContent[i]);
            temp_tag_index.push_back(pos);
        }
        i++;
    }

    offset = 0;
    for (i = 0; i < temp_tagContent.size(); i++)
    {
        pos = temp_tag_index[i] + offset;
        closingTag = "</" + temp_tagContent[i] + ">";
        closePos = xml.find('>', pos) + 1;

        closePos = correctindex(xml, closePos);
        xml.insert(closePos, closingTag);
        offset += closingTag.length();

        temp_tagContent.erase(temp_tagContent.begin() + i);
        temp_tag_index.erase(temp_tag_index.begin() + i);
        i--;
    }

    if (!temp_tagContent.empty() && !temp_tag_index.empty())
        cout << "The remain Errors is not handled";
    return xml;
}

// ==================== JSON Conversion ====================

struct JsonNode
{
    string tag, value;
    vector<JsonNode> children;

    string toJson(int indent = 0) const
    {
        string padding(indent, ' ');
        string result = padding + "\"" + tag + "\": ";
        if (!children.empty())
        {
            result += "{\n";
            for (size_t i = 0; i < children.size(); ++i)
            {
                result += children[i].toJson(indent + 2);
                if (i < children.size() - 1)
                    result += ",";
                result += "\n";
            }
            result += padding + "}";
        }
        else
        {
            result += "\"" + value + "\"";
        }
        return result;
    }
};

class XmlToJsonConverter
{
public:
    string convertToJson(const string &xml)
    {
        stack<JsonNode> nodes;
        JsonNode root;
        string tag, value;
        bool insideTag = false, closingTag = false;

        for (char ch : xml)
        {
            if (ch == '<')
            {
                if (!value.empty())
                {
                    trim(value);
                    if (!value.empty() && !nodes.empty())
                        nodes.top().value = value;
                    value.clear();
                }
                insideTag = true;
                closingTag = false;
            }
            else if (ch == '>')
            {
                insideTag = false;
                if (closingTag)
                {
                    JsonNode completed = nodes.top();
                    nodes.pop();
                    if (!nodes.empty())
                        nodes.top().children.push_back(completed);
                    else
                        root = completed;
                }
                else
                {
                    nodes.push(JsonNode{tag});
                }
                tag.clear();
            }
            else if (insideTag)
            {
                if (ch == '/')
                    closingTag = true;
                else
                    tag += ch;
            }
            else
            {
                value += ch;
            }
        }
        return "{\n" + root.toJson(2) + "\n}";
    }

private:
    void trim(string &str)
    {
        size_t start = 0, end = str.size();
        while (start < end && isspace(str[start]))
            ++start;
        while (end > start && isspace(str[end - 1]))
            --end;
        str = str.substr(start, end - start);
    }
};

// ==================== XML Formatting Function ====================
string FormattingFunction(const string &input)
{
    string output;
    stack<string> tagStack; // Stack to track tags for indentation
    string currentTag;      // Temporary storage for current tag
    bool insideTag = false; // To check if parsing inside a tag
    int indentationLevel = 0;

    for (size_t i = 0; i < input.size(); ++i)
    {
        char ch = input[i];

        if (ch == '<')
        {
            // Start of a tag
            if (!currentTag.empty())
            {
                // If there's text content, write it with proper indentation
                if (!output.empty() && output.back() != '\n')
                {
                    output += '\n';
                }
                output += string(indentationLevel * 2, ' ') + currentTag + '\n';
                currentTag.clear();
            }
            insideTag = true;
            currentTag += ch;
        }
        else if (ch == '>')
        {
            // End of a tag
            insideTag = false;
            currentTag += ch;

            if (currentTag[1] == '/')
            {
                // Closing tag
                if (!tagStack.empty())
                {
                    tagStack.pop();
                    indentationLevel--; // Reduce indentation level
                }
                output += string(indentationLevel * 2, ' ') + currentTag + '\n';
            }
            else
            {
                // Opening tag or self-closing tag
                size_t spacePos = currentTag.find(' ');
                string tagName = (spacePos != string::npos)
                                     ? currentTag.substr(1, spacePos - 1)
                                     : currentTag.substr(1, currentTag.size() - 2);

                if (currentTag.back() == '/')
                {
                    // Self-closing tag
                    output += string(indentationLevel * 2, ' ') + currentTag + '\n';
                }
                else
                {
                    // Opening tag
                    output += string(indentationLevel * 2, ' ') + currentTag + '\n';
                    tagStack.push(tagName);
                    indentationLevel++;
                }
            }
            currentTag.clear();
        }
        else if (insideTag)
        {
            // Inside a tag, continue building it
            currentTag += ch;
        }
        else
        {
            // Outside of tags, handle text content
            if (!isspace(static_cast<unsigned char>(ch)) || (!output.empty() && output.back() != '\n'))
            {
                currentTag += ch;
            }
        }
    }

    // Add any remaining content in `currentTag` to the output
    if (!currentTag.empty())
    {
        if (!output.empty() && output.back() != '\n')
        {
            output += '\n';
        }
        output += string(indentationLevel * 2, ' ') + currentTag + '\n';
    }
    return output;
}

// ==================== XML Minifying Function ====================
string MinifyingFunction(const string &input)
{
    string output;
    stack<char> charStack;
    bool insideTag = false;

    for (size_t i = 0; i < input.size(); ++i)
    {
        char ch = input[i];

        if (ch == '<')
        {
            // Start of a new tag
            if (!output.empty() && output.back() != '\n')
            {
                output += '\n'; // Add a newline before starting a new tag
            }
            charStack.push(ch); // Push '<' to the stack
            insideTag = true;
            output += ch;
        }
        else if (ch == '>')
        {
            // End of a tag
            insideTag = false;
            while (!charStack.empty() && charStack.top() == '<')
            {
                charStack.pop(); // Pop matched '<' from the stack
            }
            output += ch;
            output += '\n'; // Add a newline after closing the tag
        }
        else if (insideTag)
        {
            // Inside a tag
            charStack.push(ch); // Push tag characters
            output += ch;
        }
        else
        {
            // Outside tags (text content) .... isspace to check if it is character or space
            // .back() It returns a reference to the last element of the container.
            if (!isspace(static_cast<unsigned char>(ch)) || (output.empty() || output.back() != '\n'))
            {
                // Preserve text content but trim excessive whitespace
                charStack.push(ch); // Push text content
                output += ch;
            }
        }
    }
    return output;
}
// ==================== Huffman Compression and Decompression ====================
// Huffman coding node
struct HuffmanNode
{
    char ch;
    int freq;
    HuffmanNode *left;
    HuffmanNode *right;

    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

// Comparison object for priority queue
struct Compare
{
    bool operator()(HuffmanNode *lhs, HuffmanNode *rhs)
    {
        return lhs->freq > rhs->freq;
    }
};

// Custom data structure to store character frequencies
struct CharFrequency
{
    char ch;
    int freq;
};

// Custom data structure to store Huffman codes
struct CharCode
{
    char ch;
    string code;
};

// Build the Huffman Tree
HuffmanNode *buildHuffmanTree(const vector<CharFrequency> &frequencies)
{
    priority_queue<HuffmanNode *, vector<HuffmanNode *>, Compare> minHeap;

    for (const auto &cf : frequencies)
    {
        minHeap.push(new HuffmanNode(cf.ch, cf.freq));
    }

    while (minHeap.size() > 1)
    {
        HuffmanNode *left = minHeap.top();
        minHeap.pop();
        HuffmanNode *right = minHeap.top();
        minHeap.pop();

        HuffmanNode *newNode = new HuffmanNode('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        minHeap.push(newNode);
    }

    return minHeap.top();
}

// Generate Huffman codes from the Huffman tree
void generateCodes(HuffmanNode *root, const string &str, vector<CharCode> &codes)
{
    if (!root)
        return;

    if (root->ch != '\0')
    {
        codes.push_back({root->ch, str});
    }

    generateCodes(root->left, str + "0", codes);
    generateCodes(root->right, str + "1", codes);
}

// Encode input data using Huffman codes
vector<bool> encode(const string &input, const vector<CharCode> &codes)
{
    vector<bool> result;
    for (char ch : input)
    {
        for (const auto &code : codes)
        {
            if (code.ch == ch)
            {
                for (char bit : code.code)
                {
                    result.push_back(bit == '1');
                }
                break;
            }
        }
    }
    return result;
}

// Decode binary data using Huffman codes
string decode(const vector<bool> &input, HuffmanNode *root)
{
    string result;
    HuffmanNode *current = root;

    for (bool bit : input)
    {
        current = bit ? current->right : current->left;
        if (current->ch != '\0')
        {
            result += current->ch;
            current = root;
        }
    }
    return result;
}

// Compress the input XML file
void compress(const string &inputFile, const string &compressedFile)
{
    string inputData = readXMLFile(inputFile);
    if (inputData.empty())
        return;

    // Calculate character frequencies
    vector<CharFrequency> frequencies;
    for (char ch : inputData)
    {
        bool found = false;
        for (auto &cf : frequencies)
        {
            if (cf.ch == ch)
            {
                cf.freq++;
                found = true;
                break;
            }
        }
        if (!found)
        {
            frequencies.push_back({ch, 1});
        }
    }

    HuffmanNode *root = buildHuffmanTree(frequencies);
    vector<CharCode> huffmanCodes;
    generateCodes(root, "", huffmanCodes);

    vector<bool> compressedData = encode(inputData, huffmanCodes);

    ofstream file(compressedFile, ios::binary);
    if (!file.is_open())
    {
        cerr << "Error: Could not write to file " << compressedFile << endl;
        return;
    }

    size_t numSymbols = frequencies.size();
    file.write(reinterpret_cast<const char *>(&numSymbols), sizeof(numSymbols));
    for (const auto &cf : frequencies)
    {
        file.put(cf.ch);
        file.write(reinterpret_cast<const char *>(&cf.freq), sizeof(cf.freq));
    }

    unsigned char byte = 0;
    int bitCount = 0;
    for (bool bit : compressedData)
    {
        byte = (byte << 1) | bit;
        ++bitCount;
        if (bitCount == 8)
        {
            file.put(byte);
            byte = 0;
            bitCount = 0;
        }
    }

    if (bitCount > 0)
    {
        byte <<= (8 - bitCount);
        file.put(byte);
    }

    cout << "Compression complete. Compressed data saved to: " << compressedFile << endl;
}

// Decompress the compressed XML file
void decompress(const string &compressedFile, const string &decompressedFile)
{
    ifstream file(compressedFile, ios::binary);
    if (!file.is_open())
    {
        cerr << "Error: Could not open file " << compressedFile << endl;
        return;
    }

    size_t numSymbols;
    file.read(reinterpret_cast<char *>(&numSymbols), sizeof(numSymbols));

    vector<CharFrequency> frequencies(numSymbols);
    for (size_t i = 0; i < numSymbols; ++i)
    {
        file.get(frequencies[i].ch);
        file.read(reinterpret_cast<char *>(&frequencies[i].freq), sizeof(frequencies[i].freq));
    }

    vector<bool> decompressedData;
    char byte;
    while (file.get(byte))
    {
        for (int i = 7; i >= 0; --i)
        {
            decompressedData.push_back((byte >> i) & 1);
        }
    }

    HuffmanNode *root = buildHuffmanTree(frequencies);
    string decompressedText = decode(decompressedData, root);
    ofstream outFile(decompressedFile);
    if (outFile.is_open())
    {
        outFile << decompressedText;
        outFile.close();
        cout << "Decompression complete. Decompressed data saved to: " << decompressedFile << endl;
    }
    else
    {
        cerr << "Error: Could not open file " << decompressedFile << " for writing.\n";
    }
}

// ==================== Main Function ====================
int main()
{
    int choice;
    string fileName, xml;

    do
    {
        cout << "\nOperations Menu:\n";
        cout << "1. Check XML Consistency\n";
        cout << "2. Detect and Correct Errors\n";
        cout << "3. Convert XML to JSON\n";
        cout << "4. Format XML\n";
        cout << "5. Minify XML\n";
        cout << "6. Compress XML\n";
        cout << "7. Decompress XML\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1: // Check XML Consistency
        {
            cout << "Enter the XML file name (with extension): ";
            cin >> fileName;

            xml = readXMLFile(fileName);
            if (xml.empty())
            {
                cerr << "Error: Could not read the file '" << fileName << "'.\n";
                break;
            }

            cout << "Number of characters in the XML file: " << xml.length() << endl;

            bool isConsistent = checkXMLConsistency(xml);
            cout << "XML consistency: " << (isConsistent ? "Consistent" : "Inconsistent") << endl;
            break;
        }
        case 2: // Detect and Correct Errors
        {
            cout << "Enter the XML file name (with extension): ";
            cin >> fileName;

            xml = readXMLFile(fileName);
            if (xml.empty())
            {
                cerr << "Error: Could not read the file '" << fileName << "'.\n";
                break;
            }

            bool isConsistent = checkXMLConsistency(xml);
            if (!isConsistent)
            {
                cout << "Detecting XML file errors...\n";
                vector<int> mismatches = findMismatchedTags(xml);

                for (int idx : mismatches)
                {
                    cout << "Mismatched at index: " << idx << " - Context: ";
                    cout << xml.substr(idx, min((size_t)11, xml.length() - idx)) << endl;
                }

                cout << "Correcting XML file errors...\n";
                xml = correctMismatchedTags(xml);

                ofstream outFile("MAIN_output.xml");
                if (outFile.is_open())
                {
                    outFile << xml;
                    outFile.close();
                    cout << "Corrected XML saved to 'MAIN_output.xml' successfully!\n";
                }
                else
                {
                    cerr << "Failed to open 'MAIN_output.xml' for writing!\n";
                }
            }
            else
            {
                cout << "XML is consistent. No corrections needed.\n";
            }
            break;
        }
        case 3: // Convert XML to JSON
        {
            cout << "Enter the XML file name (with extension): ";
            cin >> fileName;

            xml = readXMLFile(fileName);
            if (xml.empty())
            {
                cerr << "Error: Could not read the file '" << fileName << "'.\n";
                break;
            }

            XmlToJsonConverter converter;
            string json = converter.convertToJson(xml);

            cout << "Converted JSON:\n"
                 << json << endl;

            ofstream jsonFile("output.json");
            if (jsonFile.is_open())
            {
                jsonFile << json;
                jsonFile.close();
                cout << "JSON saved to 'output.json' successfully!\n";
            }
            else
            {
                cerr << "Failed to open 'output.json' for writing!\n";
            }
            break;
        }
        case 4: // Format XML
        {
            cout << "Enter the XML file name (with extension): ";
            cin >> fileName;

            xml = readXMLFile(fileName);
            if (xml.empty())
            {
                cerr << "Error: Could not read the file '" << fileName << "'.\n";
                break;
            }

            string formattedXML = FormattingFunction(xml);

            string outputFileName;
            cout << "Enter the output file name for formatted XML (with extension): ";
            cin >> outputFileName;

            ofstream outputFile(outputFileName);
            if (outputFile.is_open())
            {
                outputFile << formattedXML;
                outputFile.close();
                cout << "Formatted XML has been written to '" << outputFileName << "'.\n";
            }
            else
            {
                cerr << "Error: Could not open the file '" << outputFileName << "' for writing.\n";
            }
            break;
        }
        case 5: // Minify XML
        {
            cout << "Enter the XML file name (with extension): ";
            cin >> fileName;

            xml = readXMLFile(fileName);
            if (xml.empty())
            {
                cerr << "Error: Could not read the file '" << fileName << "'.\n";
                break;
            }

            string minifiedXML = MinifyingFunction(xml);

            string outputFileName;
            cout << "Enter the output file name for minified XML (with extension): ";
            cin >> outputFileName;

            ofstream outputFile(outputFileName);
            if (outputFile.is_open())
            {
                outputFile << minifiedXML;
                outputFile.close();
                cout << "Minified XML has been written to '" << outputFileName << "'.\n";
            }
            else
            {
                cerr << "Error: Could not open the file '" << outputFileName << "' for writing.\n";
            }
            break;
        }
        case 6: // Compress XML
        {
            string compressedFile;
            cout << "Enter the XML file name (with extension): ";
            cin >> fileName;
            cout << "Enter the name for the compressed file: ";
            cin >> compressedFile;
            compress(fileName, compressedFile);
            break;
        }
        case 7: // Decompress XML
        {
            string decompressedFile;
            cout << "Enter the compressed file name (with extension): ";
            cin >> fileName;
            cout << "Enter the name for the decompressed XML file: ";
            cin >> decompressedFile;
            decompress(fileName, decompressedFile);
            break;
        }
        case 8:
            cout << "Exiting program. Goodbye!\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 8);
    return 0;
}