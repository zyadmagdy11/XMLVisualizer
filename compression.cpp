#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <string>
#include <vector>

using namespace std;

// Structure to store character frequencies
struct CharFrequency
{
    char ch;
    int freq;
};

// Structure to store Huffman codes
struct CharCode
{
    char ch;
    string code;
};

// Huffman Tree Node
struct HuffmanNode
{
    char ch;
    int freq;
    HuffmanNode *left;
    HuffmanNode *right;
    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

string readFile(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Could not open file " << filename << endl;
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Function to write the decompressed data to a file
void writeFile(const string &filename, const string &data)
{
    ofstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Could not write to file " << filename << endl;
        return;
    }
    file << data;
}

// Comparison object for priority queue
struct Compare
{
    bool operator()(HuffmanNode *lhs, HuffmanNode *rhs)
    {
        return lhs->freq > rhs->freq;
    }
};

// Build the Huffman Tree
HuffmanNode *buildHuffmanTree(const vector<CharFrequency> &frequencies)
{
    priority_queue<HuffmanNode *, vector<HuffmanNode *>, Compare> huff;

    for (const auto &cf : frequencies)
    {
        huff.push(new HuffmanNode(cf.ch, cf.freq));
    }

    while (huff.size() > 1)
    {
        HuffmanNode *left = huff.top();
        huff.pop();
        HuffmanNode *right = huff.top();
        huff.pop();
        HuffmanNode *newNode = new HuffmanNode('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        huff.push(newNode);
    }

    return huff.top();
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

// Function to compress the input file
void compress(const string &inputFile, const string &compressedFile)
{
    string inputData = readFile(inputFile);
    if (inputData.empty())
        return;

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

// Function to decompress the compressed file
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
    writeFile(decompressedFile, decompressedText);

    cout << "Decompression complete. Decompressed data saved to: " << decompressedFile << endl;
}
