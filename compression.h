#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <string>
#include <vector>

// Structure to store character frequencies
struct CharFrequency {
    char ch;
    int freq;
};

// Structure to store Huffman codes
struct CharCode {
    char ch;
    std::string code;
};

// Huffman Tree Node
struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;
    HuffmanNode(char c, int f);
};

// Reads the content of a file
std::string readFile(const std::string& filename);

// Writes decompressed data to a file
void writeFile(const std::string& filename, const std::string& data);

// Builds the Huffman Tree
HuffmanNode* buildHuffmanTree(const std::vector<CharFrequency>& frequencies);

// Generates Huffman codes from the tree
void generateCodes(HuffmanNode* root, const std::string& str, std::vector<CharCode>& codes);

// Encodes input data using Huffman codes
std::vector<bool> encode(const std::string& input, const std::vector<CharCode>& codes);

// Decodes binary data using the Huffman tree
std::string decode(const std::vector<bool>& input, HuffmanNode* root);

// Compresses the input file
void compress(const std::string& inputFile, const std::string& compressedFile);

// Decompresses the compressed file
void decompress(const std::string& compressedFile, const std::string& decompressedFile);

#endif // HUFFMAN_H
