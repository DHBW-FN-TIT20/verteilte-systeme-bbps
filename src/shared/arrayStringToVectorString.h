#ifndef ARRAY_STRING_TO_VECTOR_STRING_H
#define ARRAY_STRING_TO_VECTOR_STRING_H

#include <vector>
#include <string>
#include <sstream>

using namespace std;

/**
 * @brief Converts a array notation string to a vector of strings.
 * Example: "[a, b, c]" -> {"a", "b", "c"}
 * 
 * @param stringToConvert String in array notation.
 * @return vector<string> 
 */
static vector<string> arrayStringToVectorString(const string& stringToConvert) {
    vector<string> result;

    // Remove the square brackets from the input string
    string str = stringToConvert.substr(1, stringToConvert.length() - 2);

    // Create a string stream to parse the comma-separated values
    stringstream ss(str);
    string token;

    // Extract each value separated by comma and add it to the vector
    while (getline(ss, token, ',')) {
        // Remove leading/trailing white spaces from each value
        token.erase(0, token.find_first_not_of(" "));
        token.erase(token.find_last_not_of(" ") + 1);
        result.push_back(token);
    }

    return result;
};

#endif // ARRAY_STRING_TO_VECTOR_STRING_H