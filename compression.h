#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <string>
#include <vector>


using namespace std;

// Structure to store character frequencies
struct CharFrequency {
    char ch;
    int freq;
};

// Structure to store Huffman codes
struct CharCode {
    char ch;
    string code;
};

// Huffman Tree Node
struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;
    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

// Reads the content of a file
string readFile(const string& filename);

// Writes decompressed data to a file
void writeFile(const string& filename, const string& data);

// Builds the Huffman Tree
HuffmanNode* buildHuffmanTree(const vector<CharFrequency>& frequencies);

// Generates Huffman codes from the tree
void generateCodes(HuffmanNode* root, const string& str, vector<CharCode>& codes);

// Encodes input data using Huffman codes
vector<bool> encode(const string& input, const vector<CharCode>& codes);

// Decodes binary data using the Huffman tree
string decode(const vector<bool>& input, HuffmanNode* root);

// Compresses the input file
void compress(const string& inputFile, const string& compressedFile);

// Decompresses the compressed file
void decompress(const string& compressedFile, const string& decompressedFile);

#endif // HUFFMAN_H