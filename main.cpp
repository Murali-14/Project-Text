#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <dirent.h>

using namespace std;

// Function to load text files from directory
void loadTextFiles(const string& dirPath, vector<string>& fileNames) {
    DIR* dir = opendir(dirPath.c_str());
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string fileName = entry->d_name;
        if (fileName.find(".txt") != string::npos) {
            fileNames.push_back(dirPath + "/" + fileName);
        }
    }
    closedir(dir);
}

// Function to read file content
string readFileContent(const string& fileName) {
    ifstream file(fileName);
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    return content;
}

// Function to create frequency vector (word count)
unordered_map<string, int> createFrequencyVector(const string& content) {
    unordered_map<string, int> frequencyMap;
    string word;
    for (char c : content) {
        if (isalnum(c)) {
            word += tolower(c);
        } else if (!word.empty()) {
            frequencyMap[word]++;
            word = "";
        }
    }
    if (!word.empty()) {
        frequencyMap[word]++;
    }
    return frequencyMap;
}

// Function to calculate Cosine Similarity between two frequency vectors
double cosineSimilarity(const unordered_map<string, int>& freqVecA, const unordered_map<string, int>& freqVecB) {
    double dotProduct = 0.0, normA = 0.0, normB = 0.0;
    for (const auto& pair : freqVecA) {
        dotProduct += pair.second * freqVecB.at(pair.first);
        normA += pow(pair.second, 2);
    }
    for (const auto& pair : freqVecB) {
        normB += pow(pair.second, 2);
    }
    return dotProduct / (sqrt(normA) * sqrt(normB));
}

// Main function to find similar pairs
int main() {
    string dirPath = "./textbooks"; // Directory containing the 64 textbook files
    vector<string> fileNames;
    loadTextFiles(dirPath, fileNames);

    if (fileNames.size() < 64) {
        cout << "Not enough text files in the directory." << endl;
        return 1;
    }

    vector<unordered_map<string, int>> freqVectors;
    for (const auto& file : fileNames) {
        string content = readFileContent(file);
        freqVectors.push_back(createFrequencyVector(content));
    }

    vector<tuple<double, int, int>> similarityScores;
    for (int i = 0; i < freqVectors.size(); ++i) {
        for (int j = i + 1; j < freqVectors.size(); ++j) {
            double similarity = cosineSimilarity(freqVectors[i], freqVectors[j]);
            similarityScores.push_back(make_tuple(similarity, i, j));
        }
    }

    sort(similarityScores.begin(), similarityScores.end(), greater<>());

    cout << "Top 10 similar pairs:" << endl;
    for (int i = 0; i < 10 && i < similarityScores.size(); ++i) {
        auto [similarity, indexA, indexB] = similarityScores[i];
        cout << "Pair " << i + 1 << ": " << fileNames[indexA] << " and " << fileNames[indexB]
             << " with similarity: " << similarity << endl;
    }

    return 0;
}
