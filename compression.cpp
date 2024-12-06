#include "Huffman.h"

// HuffmanNode constructor
HuffmanNode::HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}

// Reads the content of a file
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Writes decompressed data to a file
void writeFile(const std::string& filename, const std::string& data) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not write to file " << filename << std::endl;
        return;
    }
    file << data;
}

// Builds the Huffman Tree
HuffmanNode* buildHuffmanTree(const std::vector<CharFrequency>& frequencies) {
    auto compare = [](HuffmanNode* lhs, HuffmanNode* rhs) { return lhs->freq > rhs->freq; };
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, decltype(compare)> minHeap(compare);

    for (const auto& cf : frequencies) {
        minHeap.push(new HuffmanNode(cf.ch, cf.freq));
    }

    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top(); minHeap.pop();
        HuffmanNode* right = minHeap.top(); minHeap.pop();
        HuffmanNode* newNode = new HuffmanNode('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        minHeap.push(newNode);
    }

    return minHeap.top();
}

// Generates Huffman codes from the tree
void generateCodes(HuffmanNode* root, const std::string& str, std::vector<CharCode>& codes) {
    if (!root) return;
    if (root->ch != '\0') codes.push_back({root->ch, str});
    generateCodes(root->left, str + "0", codes);
    generateCodes(root->right, str + "1", codes);
}

// Encodes input data using Huffman codes
std::vector<bool> encode(const std::string& input, const std::vector<CharCode>& codes) {
    std::vector<bool> result;
    for (char ch : input) {
        for (const auto& code : codes) {
            if (code.ch == ch) {
                for (char bit : code.code) result.push_back(bit == '1');
                break;
            }
        }
    }
    return result;
}

// Decodes binary data using the Huffman tree
std::string decode(const std::vector<bool>& input, HuffmanNode* root) {
    std::string result;
    HuffmanNode* current = root;
    for (bool bit : input) {
        current = bit ? current->right : current->left;
        if (current->ch != '\0') {
            result += current->ch;
            current = root;
        }
    }
    return result;
}

// Compresses the input file
void compress(const std::string& inputFile, const std::string& compressedFile) {
    std::string inputData = readFile(inputFile);
    if (inputData.empty()) return;
    
    std::vector<CharFrequency> frequencies;
    for (char ch : inputData) {
        auto it = std::find_if(frequencies.begin(), frequencies.end(), [&](const CharFrequency& cf) { return cf.ch == ch; });
        if (it != frequencies.end()) it->freq++;
        else frequencies.push_back({ch, 1});
    }

    HuffmanNode* root = buildHuffmanTree(frequencies);
    std::vector<CharCode> huffmanCodes;
    generateCodes(root, "", huffmanCodes);

    std::vector<bool> compressedData = encode(inputData, huffmanCodes);

    std::ofstream file(compressedFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not write to file " << compressedFile << std::endl;
        return;
    }

    size_t numSymbols = frequencies.size();
    file.write(reinterpret_cast<const char*>(&numSymbols), sizeof(numSymbols));
    for (const auto& cf : frequencies) {
        file.put(cf.ch);
        file.write(reinterpret_cast<const char*>(&cf.freq), sizeof(cf.freq));
    }

    unsigned char byte = 0;
    int bitCount = 0;
    for (bool bit : compressedData) {
        byte = (byte << 1) | bit;
        if (++bitCount == 8) { file.put(byte); byte = 0; bitCount = 0; }
    }
    if (bitCount > 0) file.put(byte << (8 - bitCount));
}

// Decompresses the compressed file
void decompress(const std::string& compressedFile, const std::string& decompressedFile) {
    std::ifstream file(compressedFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << compressedFile << std::endl;
        return;
    }

    size_t numSymbols;
    file.read(reinterpret_cast<char*>(&numSymbols), sizeof(numSymbols));
    std::vector<CharFrequency> frequencies(numSymbols);
    for (size_t i = 0; i < numSymbols; ++i) {
        file.get(frequencies[i].ch);
        file.read(reinterpret_cast<char*>(&frequencies[i].freq), sizeof(frequencies[i].freq));
    }

    std::vector<bool> decompressedData;
    char byte;
    while (file.get(byte)) {
        for (int i = 7; i >= 0; --i) decompressedData.push_back((byte >> i) & 1);
    }

    HuffmanNode* root = buildHuffmanTree(frequencies);
    std::string decompressedText = decode(decompressedData, root);
    writeFile(decompressedFile, decompressedText);
}