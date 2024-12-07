#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <sstream>

// Function to read the content of a file
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

// Function to write the decompressed data to a file
void writeFile(const std::string& filename, const std::string& data) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not write to file " << filename << std::endl;
        return;
    }
    file << data;
}

// Huffman coding node
struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    //Struct constructor
    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

// Comparison object for priority queue
struct Compare {
    bool operator()(HuffmanNode* lhs, HuffmanNode* rhs) {
        return lhs->freq > rhs->freq;
    }
};

//Character-frequency pair
struct CharFrequency {
    char ch;
    int freq;
};


// Character-code pair
struct CharCode {
    char ch;
    std::string code;
};

// Build the Huffman Tree
HuffmanNode* buildHuffmanTree(const std::vector<CharFrequency>& frequencies) {
    //Priority Queue
    // HuffmanNode* is the type of elements stored in the priority queue
    // Since priority queue is a container adapter
    // the  Container is a vector of HuffmanNode* objects
    // Compare is the compare function used by the priority queue
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> huff;

    //Start pushing the frequencies vector and it will be automatically arranged
    // Lowest frequency at the top  
    for (const auto& cf : frequencies) {
        huff.push(new HuffmanNode(cf.ch, cf.freq));
    }



    while (huff.size() > 1) {
        HuffmanNode* left = huff.top(); huff.pop();
        HuffmanNode* right = huff.top(); huff.pop();
        
        //Create a new node with placeholder character '\0' and it will hold the 
        // combined frequency of its two child nodes
        HuffmanNode* newNode = new HuffmanNode('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        huff.push(newNode);
    }
    //Return the root node of the tree
    //Priority queue will only contain this one node
    // And we will use the left and right pointers of this node to traverse the created tree
    return huff.top();
}

// Generate Huffman codes from the Huffman tree
void generateCodes(HuffmanNode* root, const std::string& str, std::vector<CharCode>& codes) {

    //*AFTER* a leaf node is reached
    // root->left and root-> right will be nullptr
    // Thus return will be executed
    if (!root) return;

    //*WHEN* a leaf node is reached, this condition will be true
    //Push to the codes (character-code pair) vector the current character
    // along with the corresponding huffman code
    if (root->ch != '\0') {
        codes.push_back({root->ch, str});
    }

    generateCodes(root->left, str + "0", codes);
    generateCodes(root->right, str + "1", codes);
}

// Encode input data using Huffman codes
std::vector<bool> encode(const std::string& input, const std::vector<CharCode>& codes) {
    std::vector<bool> result;
    //Iterate over each character in the stringified input file
    for (char ch : input) {
        //Iterate over the character-code pairs
        for (const auto& code : codes) {
            //If a match is found
            if (code.ch == ch) {
                //Push back the corresponding character code as boolean values
                for (char bit : code.code) {
                    result.push_back(bit == '1');
                }
                break;
            }
        }
    }
    return result;
}

// Decode binary data using Huffman codes
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

// Function to compress the input file
void compress(const std::string& inputFile, const std::string& compressedFile) {
    std::string inputData = readFile(inputFile);
    if (inputData.empty()) return;

    //Here we will calculate the character frequency
    // Through a dynamic array (vector) of CharFrequency struct instances
    // which contains character (ch) with coressponding frequency (freq)
    std::vector<CharFrequency> frequencies;
    for (char ch : inputData) {
        bool found = false;
        //range-based for with "auto" keyword to automatically deduce the type of
        // "frequencies" 
        for (auto& cf : frequencies) {
            if (cf.ch == ch) {
                cf.freq++;
                found = true;
                break;
            }
        }
        //If an element is found for the first time,
        // it enters this if condition
        if (!found) {
            frequencies.push_back({ch, 1});
        }
    }


    //Build the Huffman tree
    HuffmanNode* root = buildHuffmanTree(frequencies);
    

    //A vector of CharCode (character and coressponding huffman code)
    std::vector<CharCode> huffmanCodes;

    //Generate the codes and put them back into huffmanCodes vector
    generateCodes(root, "", huffmanCodes);

    //CompressedData is a vector of bools
    // encode(inputData,huffmanCodes) will be assigned to it
    // inputData is our to-be-compressed file as a string
    std::vector<bool> compressedData = encode(inputData, huffmanCodes);

    //Open the the output file for writing
    std::ofstream file(compressedFile, std::ios::binary);

    //Not open check
    if (!file.is_open()) {
        std::cerr << "Error: Could not write to file " << compressedFile << std::endl;
        return;
    }

    //Now for the writing to output file part

    //Header:
    //We will begin by storing the size of the frequencies vector
    // which represents the number of unique characters in the file
    // in the numSymbols variable
    size_t numSymbols = frequencies.size();

    //Convert numSymbols into raw data and write it to the file
    file.write(reinterpret_cast<const char*>(&numSymbols), sizeof(numSymbols));
    for (const auto& cf : frequencies) {
        file.put(cf.ch);
        file.write(reinterpret_cast<const char*>(&cf.freq), sizeof(cf.freq));
    }

    //Actual Compressed data:
    // std::vector<bool> is a specialized container that stores bits in a compressed format, 
    // not as individual bool objects. Therefore, it cannot be directly written to a file 
    // as raw bytes. To write the bits to a file, we manually pack them into bytes (8 bits)
    // and write those bytes to the file using file.put(). This ensures the bits are correctly 
    //  represented and stored in the file.

    unsigned char byte = 0;
    int bitCount = 0;
    for (bool bit : compressedData) {
        byte = (byte << 1) | bit;
        ++bitCount;
        if (bitCount == 8) {
            file.put(byte);
            byte = 0;
            bitCount = 0;
        }
    }

    if (bitCount > 0) {
        byte <<= (8 - bitCount);
        file.put(byte);
    }

    std::cout << "Compression complete. Compressed data saved to: " << compressedFile << std::endl;
}

// Function to decompress the compressed file
void decompress(const std::string& compressedFile, const std::string& decompressedFile) {
    std::ifstream file(compressedFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << compressedFile << std::endl;
        return;
    }

    //Read the number of unique character symbols:
    size_t numSymbols;
    file.read(reinterpret_cast<char*>(&numSymbols), sizeof(numSymbols));

    //Readt the corresponding frequencies
    std::vector<CharFrequency> frequencies(numSymbols);
    for (size_t i = 0; i < numSymbols; ++i) {
        file.get(frequencies[i].ch);
        file.read(reinterpret_cast<char*>(&frequencies[i].freq), sizeof(frequencies[i].freq));
    }

    //Put the compressed Data into a vector of bools
    std::vector<bool> decompressedData;
    char byte;
    while (file.get(byte)) {
        for (int i = 7; i >= 0; --i) {
            decompressedData.push_back((byte >> i) & 1);
        }
    }

    HuffmanNode* root = buildHuffmanTree(frequencies);
    std::string decompressedText = decode(decompressedData, root);
    writeFile(decompressedFile, decompressedText);

    std::cout << "Decompression complete. Decompressed data saved to: " << decompressedFile << std::endl;
}

int main() {
    std::string inputFile = "sample2.xml";
    std::string compressedFile = "output.comp";
    std::string decompressedFile = "decompressed.xml";

    compress(inputFile, compressedFile);
    decompress(compressedFile, decompressedFile);

    return 0;
}
