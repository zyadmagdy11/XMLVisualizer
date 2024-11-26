#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

struct Node
{
    string data;
    Node *next;

    Node(string val)
    {
        data = val;
        next = nullptr;
    }
};

class Stack
{
private:
    Node *top;

public:
    Stack()
    {
        top = nullptr;
    }

    ~Stack()
    {
        while (!isEmpty())
            pop();
    }

    bool isEmpty()
    {
        return top == nullptr;
    }

    void push(string value)
    {
        Node *newNode = new Node(value);
        newNode->next = top;
        top = newNode;
    }

    string pop()
    {
        if (isEmpty())
        {
            return "";
        }
        Node *temp = top;
        string poppedValue = temp->data;
        top = top->next;
        delete temp;
        return poppedValue;
    }

    string peek()
    {
        if (isEmpty())
        {
            return "";
        }
        return top->data;
    }

    void display()
    {
        if (isEmpty())
        {
            return;
        }
        Node *temp = top;
        while (temp != nullptr)
        {
            temp = temp->next;
        }
    }
};

bool checkXMLConsistency(string xml)
{
    Stack tagStack;
    int i = 0;

    while (i < xml.length())
    {
        string tag;

        if (xml[i] == '<' && xml[i + 1] != '/')
        {
            tag = "";
            i++;
            while (i < xml.length() && xml[i] != '>')
            {
                tag += xml[i];
                i++;
            }

            if (!tag.empty())
                tagStack.push(tag);
            i++;
        }
        else if (xml[i] == '<' && xml[i + 1] == '/')
        {
            tag = "";
            i += 2;
            while (i < xml.length() && xml[i] != '>')
            {
                tag += xml[i];
                i++;
            }

            if (tagStack.isEmpty() || tagStack.peek() != tag)
                return false;
            tagStack.pop();
            i++;
        }
        else
        {
            i++;
        }
    }

    return tagStack.isEmpty();
}

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
    for (char ch : content)
    {
        if (!isspace(ch))
        {
            compressedContent += ch;
        }
    }

    return compressedContent;
}

int main()
{
    string fileName = "sample.xml";
    string xml = readXMLFile(fileName);

    if (xml.empty())
    {
        cout << "Error reading the XML file." << endl;
        return 1;
    }
    cout << "number of characters = " << xml.length() << endl;

    cout << "Checking XML file consistency..." << endl;
    bool isConsistent = checkXMLConsistency(xml);
    cout << "XML consistency: " << (isConsistent ? "Consistent" : "Inconsistent") << endl;

    return 0;
}
