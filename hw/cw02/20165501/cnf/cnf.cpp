#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<string> tokens;

void split(const string& str, const string& delim)
{
    tokens.clear();
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
}

class Formula {
    private:
    public:
};

class Literal {
    private:
    public:
        string name;
        string toString() {
            return name;
        }
};
enum Ops {
    AND,
    OR,
    IMPLY,
    RIMPLY,
    EQUIV,
    NOT
};
class Op {
    private:
    public:
        Ops opType;
        string toStringOp() {
            switch (opType) {
                case AND:
                    return "&";
                case OR:
                    return "|";
                case IMPLY:
                    return ">";
                case RIMPLY:
                    return "<";
                case EQUIV:
                    return "=";
                case NOT:
                    return "-";
            }
        }
};

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

    split(str, " ");
    printf("%lu", tokens.size());
    return 0;
}
