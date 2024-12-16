#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <string>
#include <stack>

using namespace std;

bool        checkXMLConsistency    (string xml);
vector<int> findMismatchedTags     (string xml);
string      correctMismatchedTags  (string xml, vector<int> tag_index);
string      readXMLFile            (string fileName);

void heapSort  (vector<int>& arr , int n);
void buildHeap (vector<int>& arr, int n);
void heapify   (vector<int>& arr, int n , int i);

void heapify(vector<int>& arr, int n , int i)
{
    int l = 2*i + 1;
    int r = 2*i + 2;
    int max = i;
    if (l<n && arr[l] > arr[max])  max = l;
    if (r<n && arr[r] > arr[max])  max = r;
    if (max != i)
    {
        swap(arr[i] , arr[max]);
        heapify(arr,n,max);
    } 
}
void buildHeap (vector<int>& arr, int n)
{
    for (int i = n/2 -1 ; i>=0 ; i--) heapify(arr,n,i);
}
void heapSort(vector<int>& arr , int n)
{
    buildHeap(arr,n);
    for (int i = n-1; i>=0 ; i--)
    {
        swap(arr[0],arr[i]);
        heapify(arr,i,0);
    }
}



bool checkXMLConsistency(string xml)               
{
    stack<string> tagStack;  
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

                if (tagStack.empty() || tagStack.top() != tagName) 
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

    return tagStack.empty();
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
    heapSort(mismatchedPositions, mismatchedPositions.size());
    return mismatchedPositions;
}

string correctMismatchedTags(string xml, vector<int> tag_index)
{
    vector<string> tagContent;

    string closingTag;
    int closePos;
    int pos = 0;
    int offset = 0; // To keep track of changes in the string length

    for (int p : tag_index)
    {
        closePos = xml.find('>', p);
        if (xml[p + 1] != '/')
        {
            tagContent.push_back(xml.substr(p + 1, closePos - p - 1));
        }
        else
        {
            tagContent.push_back(xml.substr(p + 2, closePos - p - 2));
        }
    }

    for (int i = 0; i < tagContent.size(); i++)
    {
        pos = tag_index[i] + offset;
        if (xml[pos + 1] != '/') // opentag without closetag
        {
            closingTag = "</" + tagContent[i] + ">";
            closePos = xml.find('>', pos) + 1;
            xml.insert(closePos, closingTag);
            offset += closingTag.length();
        }
        else // closetag without opentag
        {
            closingTag = "<" + tagContent[i] + ">";
            closePos = pos;
            xml.insert(closePos, closingTag);
            offset += closingTag.length();
        }
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
