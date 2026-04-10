    #include <cctype>
    #include <fstream>
    #include <iostream>
    #include <sstream>
    #include <string>
    #include <vector>

    using std::cout;
    using std::endl;
    using std::ifstream;
    using std::istringstream;
    using std::stod;
    using std::stoll;
    using std::string;
    using std::vector;

    vector<string> intStrings;
    vector<long long> intValues;

    vector<string> realStrings;
    vector<double> realValues;

    vector<string> allStrings;
    vector<int> allCounts;

    int findIndex(vector<string> const &vec, string const &target) {
    for (int i = 0; i < (int)vec.size(); ++i)
        if (vec[i] == target)
        return i;
    return -1;
    }

    void storeAll(string const &lit) {
    int idx = findIndex(allStrings, lit);
    if (idx == -1) {
        allStrings.push_back(lit);
        allCounts.push_back(1);
    } else
        allCounts[idx]++;
    }

    void storeInt(string const &lit) {
    if (findIndex(intStrings, lit) == -1) {
        intStrings.push_back(lit);
        intValues.push_back(stoll(lit));
    }
    storeAll(lit);
    }

    void storeReal(string const &lit) {
    if (findIndex(realStrings, lit) == -1) {
        realStrings.push_back(lit);
        realValues.push_back(stod(lit));
    }
    storeAll(lit);
    }

    void sortParallelInt(vector<string> &strs, vector<long long> &vals) {
    for (int i = 1; i < (int)strs.size(); ++i) {
        string ks = strs[i];
        long long kv = vals[i];
        int j = i - 1;
        while (j >= 0 && strs[j] > ks) {
        strs[j + 1] = strs[j];
        vals[j + 1] = vals[j];
        --j;
        }
        strs[j + 1] = ks;
        vals[j + 1] = kv;
    }
    }

    void sortParallelReal(vector<string> &strs, vector<double> &vals) {
    for (int i = 1; i < (int)strs.size(); ++i) {
        string ks = strs[i];
        double kv = vals[i];
        int j = i - 1;
        while (j >= 0 && strs[j] > ks) {
        strs[j + 1] = strs[j];
        vals[j + 1] = vals[j];
        --j;
        }
        strs[j + 1] = ks;
        vals[j + 1] = kv;
    }
    }

    void sortParallelAll(vector<string> &strs, vector<int> &vals) {
    for (int i = 1; i < (int)strs.size(); ++i) {
        string ks = strs[i];
        int kv = vals[i];
        int j = i - 1;
        while (j >= 0 && strs[j] > ks) {
        strs[j + 1] = strs[j];
        vals[j + 1] = vals[j];
        --j;
        }
        strs[j + 1] = ks;
        vals[j + 1] = kv;
    }
    }

    void parseToken(string const &token, int lineNum) {

    int start = -1;
    for (int i = 0; i < (int)token.size(); ++i) {
        if (isdigit((unsigned char)token[i])) {
        start = i;
        break;
        }
    }
    if (start == -1)
        return;

    int i = start;
    int n = (int)token.size();
    string lit;
    int dotCount = 0;
    bool hasFraction = false;
    bool hasExponent = false;
    bool isError = false;

    while (i < n && isdigit((unsigned char)token[i]))
        lit += token[i++];

    if (i < n && token[i] == '.') {

        if (i + 1 < n && isdigit((unsigned char)token[i + 1])) {
        lit += token[i++];
        ++dotCount;
        hasFraction = true;

        while (i < n && isdigit((unsigned char)token[i]))
            lit += token[i++];

        if (i < n && token[i] == '.') {
            lit += token[i++];
            ++dotCount;
            cout << "Line " << lineNum << ": Invalid floating-point literal \""
                << lit << "\"" << endl;
            isError = true;
        }

        } else {
        // dot not followed by digit — not part of this literal, just ignore it
        }
    }

    if (!isError && i < n && (token[i] == 'E' || token[i] == 'e')) {

        lit += token[i++];
        int expCount = 1;

        if (i < n && (token[i] == 'E' || token[i] == 'e')) {
        lit += token[i++];
        ++expCount;
        cout << "Line " << lineNum
            << ": Invalid exponent for a numeric literal: \"" << lit << "\""
            << endl;
        isError = true;
        }

        if (!isError && i < n && (token[i] == '+' || token[i] == '-')) {
        lit += token[i++];
        int signCount = 1;

        if (i < n && (token[i] == '+' || token[i] == '-')) {
            lit += token[i++];
            ++signCount;
            cout << "Line " << lineNum
                << ": Invalid exponent for a numeric literal: \"" << lit << "\""
                << endl;
            isError = true;
        }
        }

        if (!isError) {
        if (i < n && isdigit((unsigned char)token[i])) {
            while (i < n && isdigit((unsigned char)token[i]))
            lit += token[i++];

            if (i < n && (token[i] == 'E' || token[i] == 'e')) {
            lit += token[i++];
            cout << "Line " << lineNum
                << ": Invalid exponent for a numeric literal: \"" << lit << "\""
                << endl;
            isError = true;
            } else {
            hasExponent = true;
            }

        } else {

            cout << "Line " << lineNum
                << ": Invalid exponent for a numeric literal: \"" << lit << "\""
                << endl;
            isError = true;
        }
        }
    }

    if (!isError) {
        if (hasFraction || hasExponent)
        storeReal(lit);
        else
        storeInt(lit);
    }

    // Continue scanning the rest of the token for more literals
    string remaining = token.substr(i);
    if (!remaining.empty())
        parseToken(remaining, lineNum);
    }

    void validateIntent(int const &argc, char *argv[], bool &allFlag, bool &intFlag,
                        bool &realFlag, string &fileName, bool &hasFileFlag) {

    for (int i = 1; i < argc; ++i) {
        string userArg = argv[i];

        if (userArg == "-all") {
        allFlag = true;
        } else if (userArg == "-int") {
        intFlag = true;
        } else if (userArg == "-real") {
        realFlag = true;
        } else if (!userArg.empty() && userArg[0] == '-') {
        cout << "UNRECOGNIZED FLAG " << userArg << endl;
        exit(1);
        } else {
        if (!hasFileFlag) {
            hasFileFlag = true;
            fileName = userArg;
        }
        }
    }

    if (!hasFileFlag) {
        cout << "NO SPECIFIED INPUT FILE NAME." << endl;
        exit(1);
    }
    }

    void validateIOS(string const &fileName) {

    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "CANNOT OPEN THE FILE " << fileName << endl;
        exit(1);
    } else if (file.peek() == std::ifstream::traits_type::eof()) {
        cout << "File is empty." << endl;
        exit(1);
    }
    }

    void processFile(string const &fileName, int &totalLines) {

    ifstream file(fileName);
    string line;
    totalLines = 0;

    while (getline(file, line)) {
        ++totalLines;
        istringstream ss(line);
        string token;
        while (ss >> token) {
        parseToken(token, totalLines);
        }
    }
    }

    void displayResults(bool const &allFlag, bool const &intFlag,
                        bool const &realFlag, int const &totalLines) {

    sortParallelAll(allStrings, allCounts);
    sortParallelInt(intStrings, intValues);
    sortParallelReal(realStrings, realValues);

    cout << "Total Number of Lines: " << totalLines << endl;
    cout << "Number of Integer Literals: " << intStrings.size() << endl;
    cout << "Number of Floating-Point Literals: " << realStrings.size() << endl;

    if (allFlag) {
        cout << endl;
        cout << "List of All Numeric Literals and their Number of Occurrences:"
            << endl;
        for (int i = 0; i < (int)allStrings.size(); ++i)
        cout << "\"" << allStrings[i] << "\": " << allCounts[i] << endl;
    }

    if (intFlag) {
        cout << endl;
        cout << "List of Integer Literals and their Values:" << endl;
        for (int i = 0; i < (int)intStrings.size(); ++i)
        cout << "\"" << intStrings[i] << "\": " << intValues[i] << endl;
    }

    if (realFlag) {
        cout << endl;
        cout << "List of Floating-Point Literals and their Values:" << endl;
        for (int i = 0; i < (int)realStrings.size(); ++i)
        cout << "\"" << realStrings[i] << "\": " << realValues[i] << endl;
    }
    }

    int main(int argc, char *argv[]) {

    bool allFlag = false;
    bool intFlag = false;
    bool realFlag = false;
    bool hasFileFlag = false;
    string fileName;
    int totalLines = 0;

    validateIntent(argc, argv, allFlag, intFlag, realFlag, fileName, hasFileFlag);
    validateIOS(fileName);
    processFile(fileName, totalLines);
    displayResults(allFlag, intFlag, realFlag, totalLines);

    return 0;
    }
