//
//  longestCommonStrand.cpp
//  
//
//  Created by Kyle on 4/14/21.
//

#include <stdio.h>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

// Returns: the length of the longest common strand,
// and changes offsets in main (by reference) to the index where the strand is found
// Parameters: strings, length of strings, ints to store offset where the strand is found in each string
int longestCommonStrand(char* first, char* second, int length1, int length2, int &offset1, int &offset2) {
    // 2D array stores length of longest common prefix of
    // first[i .. length1-1] and second[j .. length2-1]
    int longestCommonPrefix[length1 + 1][length2 + 1];
    // max: current length of longest common prefix
    int max = 0;
    
    // initialize when no characters in either substring
    for (int i = 0; i <= length1; i++) {
        longestCommonPrefix[i][length2] = 0;
    }
    for (int j = 0; j <= length2; j++) {
        longestCommonPrefix[length1][j] = 0;
    }
    for (int i = length1 - 1; i >= 0; i--) {
        for (int j = length2 - 1; j >= 0; j--) {
            if (first[i] == second[j]) {
                longestCommonPrefix[i][j] = longestCommonPrefix[i + 1][j + 1] + 1;
                if (longestCommonPrefix[i][j] > max) {
                    max = longestCommonPrefix[i][j];
                    offset1 = i;
                    offset2 = j;
                }
            }
            else {
                longestCommonPrefix[i][j] = 0;
            }
        }
    }
    return (max);
}


// Reads char from file one at a time
// Returns file contents as char*
// changes length in main to length of the string (by reference)
char* parse(int fileDescriptor, int &length) {
    char tempChar;
    string buffer;
    size_t bytesRead;
    length = 0;
    
    // Attempt to Read at Least Once, Don't Repeat if No Bytes Read
    do {
        bytesRead = read(fileDescriptor, &tempChar, 1);
        // if byte read, add char to end of buffer
        if (bytesRead > 0) {
            if (tempChar == char_traits<char>::eof())
                break;
            buffer.push_back(tempChar);
            length++;
        }
    }
    while (bytesRead > 0);
    return (strdup(buffer.c_str()));
}

// Prints the length of the longest common strand in the given input files
// Prints the file names where the largest strand appears and the offset
// Returns 0 if successful
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "longestCommonStrand: must have at least 2 input files\n";
        return(1);
    }
    int max = 0;
    char* file1;
    char* file2;
    int offset1 = 0;
    int offset2 = 0;
    char* contents[argc - 1];
    int length[argc - 1];
    int fileDescriptor;
    
    // open files and call parse to store as char*
    for (int i = 0; i < argc - 1; i++) {
        fileDescriptor = open(argv[i + 1], O_RDONLY);
        if (fileDescriptor < 0) {
            cerr << "longestCommonStrand: cannot open file\n";
            return(2);
        }
        int tempLength;
        contents[i] = parse(fileDescriptor, tempLength);
        length[i] = tempLength;
        close(fileDescriptor);
    }
    
    // Find longest common strand between each pair of files
    for (int i = 0; i < argc - 2; i++) {
        for (int j = i + 1; j < argc - 1; j++) {
            int tempOffset1;
            int tempOffset2;
            int tempMax = longestCommonStrand(contents[i], contents[j], length[i], length[j], tempOffset1, tempOffset2);
            if (tempMax > max) {
                max = tempMax;
                file1 = strdup(argv[i + 1]);
                file2 = strdup(argv[j + 1]);
                offset1 = tempOffset1;
                offset2 = tempOffset2;
            }
        }
    }
    if (max == 0)
        cout << "No common strand found\n";
    else {
        cout << "Length of longest strand: " << max << endl;
        cout << "Files containing strand:" << endl;
        cout << file1 << " at offset " << offset1 << endl;
        cout << file2 << " at offset " << offset2 << endl;
    }
    return(0);
}
