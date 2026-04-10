#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <unordered_set>

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::isalpha;
using std::map;
using std::string;
using std::unordered_set;


void parserString(string &fileName, char* argv[], int argc,bool& flagAll, bool& flagF1, bool& flagF2, bool& flagF3,bool& flagNone) {
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i]; //

        if (arg == "-all") {
        flagAll = true;
        continue;
        } else if (arg == "-t1") {
        flagF1 = true;
        } else if (arg == "-t2") {
        flagF2 = true;
        } else if (arg == "-t3") {
        flagF3 = true;
        } else {
            fileName = arg;
        }
    }
    if(!flagAll && !flagF1 && !flagF2 && !flagF3 ){
        flagNone = true;

    }
}



int main(int argc, char* argv[]){

        string fileName;
        bool flagAll = false;
        bool flagF1 = false;
        bool flagF2 = false;
        bool flagF3 = false;
        bool flagNone = false;

        int totalWords = 0;

        parserString(fileName, argv, argc, flagAll,flagF1, flagF2, flagF3,flagNone);


        if (fileName.empty()) {
        cout << "NO SPECIFIED INPUT FILE NAME." << endl;
        return 1;
        }

        ifstream file(fileName);

        if (!file.is_open()) {
        cout << "CANNOT OPEN THE FILE " << fileName << endl;
        return 1;
        }

        if (file.peek() == EOF) {
        cout << "The file is empty." << endl;
        return 1;
        }

        string word;

        int f1 = 0;
        int f2 = 0;
        int f3 = 0;
        unordered_set<string> f1Unique = {};
        unordered_set<string> f2Unique = {};
        unordered_set<string> f3Unique = {};


        while (file >> word) {
            totalWords++;
            bool specialWord = false;

            //check prefix
            if(word[0] == '_' || word[0] == '@' || word[0] == '#'){
                if(word.size() > 1){
                    specialWord = true;

                } // if lenght > 1 assume its valid
            } else {
                continue;
            }

                for (int c =1; c < word.size(); ++c) {
                    if(!isalpha(word[c]) && !isdigit(word[c]) && word[c] != '_'){
                        specialWord = false;
                        break;

                    }
                }


                if (word[0] == '_' && specialWord == true) {
                    f1++;
                    f1Unique.insert(word);

                    } else if (word[0] == '@' && specialWord == true) {
                        f2++;
                        f2Unique.insert(word);
                    } else if (word[0] == '#' && specialWord == true) {
                        f3++;
                        f3Unique.insert(word);

                    }


                }
\
        if (flagAll || flagNone) {
            cout << "Total number of words: " << totalWords << endl;
        }

        if (flagAll) {
            cout << "Occurrences of Type1 Names (Starting with '_' character): " << f1 << endl;
            cout << "Occurrences of Type2 Names (Starting with '@' character): " << f2 << endl;
            cout << "Occurrences of Type3 Names (Starting with '#' character): " << f3 << endl;
        }


        if (flagF1) {
            cout << "Count of Type1 Unique Names: " << f1Unique.size() << endl;
        }
        if (flagF2) {
            cout << "Count of Type2 Unique Names: " << f2Unique.size() << endl;
        }
        if (flagF3) {
            cout << "Count of Type3 Unique Names: " << f3Unique.size() << endl;
        }


        return 0;

        }
