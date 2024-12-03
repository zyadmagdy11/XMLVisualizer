#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <cctype>
#include <sstream>

using namespace std;

// Stack Implementation for XML Consistency Check
struct Node {
    string data;
    Node *next;

    Node(string val) {
        data = val;
        next = nullptr;
    }
};

class Stack {
private:
    Node *top;

public:
    Stack() {
        top = nullptr;
    }

    ~Stack() {
        while (!isEmpty())
            pop();
    }

    bool isEmpty() {
        return top == nullptr;
    }

    void push(string value) {
        Node *newNode = new Node(value);
        newNode->next = top;
        top = newNode;
    }

    string pop() {
        if (isEmpty()) {
            return "";
        }
        Node *temp = top;
        string poppedValue = temp->data;
        top = top->next;
        delete temp;
        return poppedValue;
    }

    string peek() {
        if (isEmpty()) {
            return "";
        }
        return top->data;
    }
};

// Check XML Consistency
bool checkXMLConsistency(string xml) {
    Stack tagStack;
    int i = 0;

    while (i < xml.length()) {
        string tag;

        if (xml[i] == '<' && xml[i + 1] != '/') {
            tag = "";
            i++;
            while (i < xml.length() && xml[i] != '>') {
                tag += xml[i];
                i++;
            }

            if (!tag.empty())
                tagStack.push(tag);
            i++;
        } else if (xml[i] == '<' && xml[i + 1] == '/') {
            tag = "";
            i += 2;
            while (i < xml.length() && xml[i] != '>') {
                tag += xml[i];
                i++;
            }

            if (tagStack.isEmpty() || tagStack.peek() != tag)
                return false;
            tagStack.pop();
            i++;
        } else {
            i++;
        }
    }

    return tagStack.isEmpty();
}

// JSON Conversion
struct JsonNode {
    string tag, value;
    vector<JsonNode> children;

    string toJson(int indent = 0) const {
        string padding(indent, ' '), result;
        if (!tag.empty()) result += padding + "\"" + tag + "\": ";
        if (!children.empty()) {
            result += "{\n";
            for (size_t i = 0; i < children.size(); ++i) {
                result += children[i].toJson(indent + 2);
                if (i < children.size() - 1) result += ",";
                result += "\n";
            }
            result += padding + "}";
        } else {
            result += "\"" + value + "\"";
        }
        return result;
    }
};

class XmlToJsonConverter {
public:
    string convertToJson(const string &xml) {
        stack<JsonNode> nodes;
        JsonNode root;
        string tag, value;
        bool insideTag = false, closingTag = false;

        for (char ch : xml) {
            if (ch == '<') {
                if (!value.empty()) {
                    trim(value);
                    if (!value.empty() && !nodes.empty())
                        nodes.top().value = value;
                    value.clear();
                }
                insideTag = true;
                closingTag = false;
            } else if (ch == '>') {
                insideTag = false;
                if (closingTag) {
                    JsonNode completed = nodes.top();
                    nodes.pop();
                    if (!nodes.empty())
                        nodes.top().children.push_back(completed);
                    else
                        root = completed;
                } else {
                    nodes.push(JsonNode{tag});
                }
                tag.clear();
            } else if (insideTag) {
                if (ch == '/')
                    closingTag = true;
                else
                    tag += ch;
            } else {
                value += ch;
            }
        }
        return "{\n" + root.toJson(2) + "\n}";
    }

    void saveToFile(const string &json, const string &filename) {
        ofstream file(filename);
        if (file)
            file << json;
        else
            cerr << "Error: Could not open file for writing." << endl;
    }

private:
    void trim(string &str) {
        size_t start = 0, end = str.size();
        while (start < end && isspace(str[start]))
            ++start;
        while (end > start && isspace(str[end - 1]))
            --end;
        str = str.substr(start, end - start);
    }
};

// Read XML File
string readXMLFile(string fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();

    string content = buffer.str();

    string compressedContent;
    for (char ch : content) {
        if (!isspace(ch)) {
            compressedContent += ch;
        }
    }

    return compressedContent;
}

int main() {
    string fileName;
    cout << "Enter the XML file name (with extension): ";
    cin >> fileName;

    string xml = readXMLFile(fileName);
    if (xml.empty()) {
        cout << "Error reading the XML file." << endl;
        return 1;
    }
    cout << "Number of characters in the XML file: " << xml.length() << endl;

    // XML Consistency Check
    cout << "Checking XML file consistency..." << endl;
    bool isConsistent = checkXMLConsistency(xml);
    cout << "XML Consistency: " << (isConsistent ? "Consistent" : "Inconsistent") << endl;

    // XML to JSON Conversion
    if (isConsistent) {
        XmlToJsonConverter converter;
        string json = converter.convertToJson(xml);
        cout << "Converted JSON:\n" << json << endl;

        // Save JSON to File
        converter.saveToFile(json, "output.json");
        cout << "JSON output saved to 'output.json'." << endl;
    } else {
        cout << "XML file is inconsistent. JSON conversion skipped." << endl;
    }

    return 0;
}
