#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>

using :: std::string;
using :: std::cout;
using :: std::cin;
using :: std::ifstream;
using :: std::endl;

// Function to convert to uppercase
string toUpperCase(string str) {
    for (char &c : str) {
        c = toupper(c);
    }
    return str;
}

// Function to trim leading whitespace
string trimLeft(string str) {
    size_t pos = str.find_first_not_of(" \t");
    return (pos == string::npos) ? "" : str.substr(pos);
}

// Function to check if line is empty or contains whitespace only
bool isEmpty(const string& line) {
    return line.find_first_not_of(" \t") == string::npos;
}

// Function to check if line is a comment
bool isComment(const string& line) {
    string trimmed = trimLeft(line);

    // Check if line starts with "::"
    if (trimmed.length() >= 2 && trimmed.substr(0, 2) == "::") {
       return true;
    }
    
    // Check if it is case-insensitive "REM"
    string upper = toUpperCase(trimmed);
    
    //check to stay within bounds
    if (upper.length() >= 3 && upper.substr(0, 3) == "REM") {
        if (upper.length() == 3 || isspace(upper[3])) {
            return true;
        }
    }

    return false;
}

//Function to extract the first word
string getFirstWord(const string& line) {
    string trimmed = trimLeft(line);
    size_t start = trimmed.find_first_of(" \t"); //Find first space, not non-space
    if (start == string::npos) {
        return trimmed;
    }
    return trimmed.substr(0, start);
}

//Main function
int main() {
    string filename; 
    string line;

    cout << "Enter the name of a file to read from: " << endl; 
    cin >> filename;

    ifstream infile(filename);
    if (!infile) {
        //Output format per PDF 
        cout << "File cannot be opened " << filename << endl;
        return 1; 
    }

    int total_lines = 0;
    int comment_lines = 0;
    int command_lines = 0;
    int invalid_command_lines = 0;
    int cd_count = 0;
    int dir_count = 0;
    int delete_count = 0;
    int copy_count = 0;

    while (getline(infile, line)) {
        total_lines++; // increment total lines for every line read

        // skip empty lines first 
        if (isEmpty(line)) {
            continue;
        }

        //check for comments 
        if (isComment(line)) {
            comment_lines++; 
            continue;
        }

        string command = getFirstWord(line);
        string upper = toUpperCase(command);

        //check if its a valid command : CD, DIR, COPY, DEL
        if (upper == "CD") {
            command_lines++;
            cd_count++;
        } else if (upper == "DEL") {
            command_lines++;
            delete_count++;
        } else if (upper == "COPY") {
            command_lines++;
            copy_count++;
        } else if (upper == "DIR") {
            command_lines++;
            dir_count++;
        } else {
            //Error message format per PDF 
            cout << "Error: Unrecognizable command in line " << total_lines << ": " << command << endl;
            invalid_command_lines++;
        }
    }

    infile.close();

    // Output of results as per : PDF
    cout << "Total lines: " << total_lines << endl;
    cout << "Commented lines: " << comment_lines << endl;
    cout << "Valid Command lines: " << command_lines << endl;
    cout << "Invalid Command lines: " << invalid_command_lines << endl;
    cout << "DIR commands: " << dir_count << endl;
    cout << "CD commands: " << cd_count << endl;
    cout << "COPY commands: " << copy_count << endl;
    cout << "DEL commands: " << delete_count << endl;
    
    return 0;
}