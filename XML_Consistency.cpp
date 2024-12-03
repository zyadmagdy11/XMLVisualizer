#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <string>


using namespace std;


template <class t>
class Stack
{
    struct Node
    {
        t data;
        Node *next;

        Node(t val)
        {
            data = val;
            next = nullptr;
        }
    };
private:
    Node *top;
    int length = 0;

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
    int len()
    {
        return length;
    }
    void push(t value)
    {
        Node *newNode = new Node(value);
        newNode->next = top;
        top = newNode;
        length++;
    }

    t pop()
    {
        if (!isEmpty())
        {
            Node *temp = top;
            t poppedValue = temp->data;
            top = top->next;
            delete temp;
            length--;
            return poppedValue;

        }
        
    }

    t peak()
    {
        if (!isEmpty())
        {
            return top->data;
        }
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
    Stack<string> tagStack;  
    int i = 0;

    while (i < xml.length()) 
    {
        if (xml[i] == '<') 
        {
            int closePos = xml.find('>', i);

            string tagContent = xml.substr(i + 1, closePos - i - 1);            // Extract tag content between '<' and '>'

            if (tagContent[0] == '/') 
            {
                string tagName = tagContent.substr(1);                         // Remove '/' from the tag

                if (tagStack.isEmpty() || tagStack.peak() != tagName) 
                {
                    return false;
                }
                tagStack.pop();  
            } 
            else    tagStack.push(tagContent);
            
            i = closePos + 1;

        } 
        else    i++;
        
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

    while (i < n) {
        if (xml[i] == '<') {

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



int correctindex(string xml,int i) 
{
    vector<string> tagStack; 
    int n = xml.length();
    bool flag = false;

    while(i<n) 
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
                    return i-tagStack.back().length()-2; // Found two consecutive opening tags
                }
                tagStack.push_back(tagName);
                flag = true;
            }

            i = closePos+1;
        }
        else i++;
    
    }
    return -1; // No mismatched closing tag found
}






string correctMismatchedTags(string xml) 
{
    vector<int>     mismatchedPositions = findMismatchedTags(xml);
    vector<string>  tagContent;
    vector<int>     tag_index;
    int offset = 0; // To keep track of changes in the string length
    


    for (int pos : mismatchedPositions) 
    {
        pos += offset; // Adjust position based on previous modifications

        if (xml[pos + 1] == '/') 
        {
            // It's a closing tag, remove it
            int closePos = xml.find('>', pos);
            xml.erase(pos, closePos - pos + 1);
            offset -= (closePos - pos + 1);

            // Insert the correct closing tag
            string closingTag = "</" + tagContent.back() + ">";
            xml.insert(pos, closingTag);
            offset += closingTag.length();
            tagContent.pop_back();
            tag_index.pop_back();
        } 
        else 
        {
            int closePos = xml.find('>', pos);
            tagContent.push_back(xml.substr(pos + 1, closePos - pos - 1));
            tag_index.push_back(pos);
        }
       
    }
    string closingTag;
    int closePos; 
    bool f = false;
    int len = 0;
    while (!tagContent.empty() && !tag_index.empty()) 
    {
        
        int i;
        for(i = 0 ; i<tagContent.size() ; i++)
        {
            if(f) break;
            closingTag = "</" + tagContent[i] + ">";
            closePos = xml.find('>', tag_index[i]+len) + 1;

            if (xml[closePos] != '<')
            {
                closePos = xml.find('<', closePos);
                xml.insert(closePos, closingTag);
                len += closingTag.length();

                tagContent.erase(tagContent.begin() + i);
                tag_index.erase(tag_index.begin() + i);
                i--;
            }
            
        }
        f = true;
        closingTag = "</" + tagContent.back() + ">";
        closePos = xml.find('>', tag_index.back()+len) + 1;
        closePos = correctindex(xml,closePos);
        xml.insert(closePos, closingTag);
        len += closingTag.length();
        
        tagContent.pop_back();
        tag_index.pop_back();
    }
    return xml;
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


    for (int i=0 ; i<content.length() ; i++)
    {
        if (!isspace(content[i]))
        {
            compressedContent += content[i];
        }
        else if ( isalpha(content[i-1]) || isalpha(content[i+1])  )    //keep spaces between tags 
        {
            compressedContent += content[i];
        }
    }

    return compressedContent;
}






int main()
{
    string fileName = "sample2.xml";
    string xml = readXMLFile(fileName);

    if (xml.empty())
    {
        cout << "Error reading the XML file." << endl;
        return 1;
    }

    bool isConsistent       = checkXMLConsistency(xml);
    cout << "Checking XML file consistency..."<<"XML consistency: "<<(isConsistent ? "Consistent" : "Inconsistent") << endl;


    
    if (!isConsistent)
    {
        cout << "Detect XML file errors..." << endl;
        vector<int> error_index = findMismatchedTags (xml);

        for(int i = 0 ; i<error_index.size() ; i++)
        {
            cout<<"error index = "<<error_index[i]<<" first 11 chars of it: ";
            cout<<xml[error_index[i]]<<xml[error_index[i]+1]<<xml[error_index[i]+2]<<xml[error_index[i]+3]<<xml[error_index[i]+4]<<xml[error_index[i]+5]<<xml[error_index[i]+6]<<xml[error_index[i]+7]<<xml[error_index[i]+8]<<xml[error_index[i]+9]<<xml[error_index[i]+10]<<xml[error_index[i]+11]<<endl;
        }
        cout << "Correct XML file errors..." << endl;
        xml = correctMismatchedTags(xml);              //it doesn't handle errors of ( <  >  / ) 
        ofstream outFile("newSample.txt");
        if (outFile.is_open()) 
        {
            outFile << xml <<endl;
            
            outFile.close();

            cout << "xml written to newfile.txt successfully!" <<endl;
        } 
        else 
        {
            std::cerr << "Failed to open file for writing!" <<endl;
        }
    }
    

    

    

   
    return 0;
}
