#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<string> *;

vector<string> split(const string& str, const string& delim)
{
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

int main(int argc, char *argv[])
{
    switch(argc){
        case 2:
            break;
        default:
            printf("Wrong number of arguments!\n");
            return 0;
    }
    printf("%s\n", argv[1]);
    int a = strlen(argv[1]);
	string str(argv[1]);

	printf("%s", str.c_str());
    return 0;
}
