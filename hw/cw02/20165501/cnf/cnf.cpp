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
        virtual string toString(){
            string ret = "";
            return ret;
        }
};

class Literal : public Formula {
    private:
    public:
        string name;
        Literal(string _name){
            name = _name;
        }
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
class Op : public Formula{
    private:
    public:
        Ops opType;
        Op(Ops type) {
            opType = type;
        }
        vector<Formula*> operands;
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
        string toString() {
            string ret = "";
            ret += toStringOp();
            for(int i = 0; i < operands.size(); i++){
                ret += " ";
                ret += operands[i]->toString();
            }
            return ret;
        }
};

int pos;

enum FormulaType {
    Lit,
    BinOp,
    UnOp
};

Formula* parseFormula()
{
//    if(tokens.size() == 0) return NULL;
    string cur = tokens.front();
    tokens.erase(tokens.begin());
    FormulaType type;
    Ops opType;
    if(cur.compare("&") == 0){
        type = BinOp;
        opType = AND;
    }else if(cur.compare("|") == 0){
        type = BinOp;
        opType = OR;
    }else if(cur.compare(">") == 0){
        type = BinOp;
        opType = IMPLY;
    }else if(cur.compare("<") == 0){
        type = BinOp;
        opType = RIMPLY;
    }else if(cur.compare("=") == 0){
        type = BinOp;
        opType = EQUIV;
    }else if(cur.compare("-") == 0){
        type = UnOp;
        opType = NOT;
    }else{
        type = Lit;
    }
    if(type == Lit){
        return new Literal(cur);
    }
    Op* ret = new Op(opType);
    Formula* left = parseFormula();
    ret->operands.push_back(left);
    if(type == BinOp){
        Formula* right = parseFormula();
        ret->operands.push_back(right);
    }
    return ret;
}

Formula* implyFree(Formula *f) {
    if(Literal *l = dynamic_cast<Literal *>(f)){
        return f;
    }else{
        Op *o = dynamic_cast<Op *>(f);
        if(o->opType == IMPLY || o->opType == RIMPLY){
            Op *ret = new Op(OR);
            Op *nf = new Op(NOT);
            if(o->opType == IMPLY){
                nf->operands.push_back(implyFree(o->operands[0]));
                ret->operands.push_back(nf);
                ret->operands.push_back(implyFree(o->operands[1]));
            }else{
                nf->operands.push_back(implyFree(o->operands[1]));
                ret->operands.push_back(implyFree(o->operands[0]));
                ret->operands.push_back(nf);
            }
            return ret;
        }else if(o->opType == NOT){
            Op *nf = new Op(NOT);
            nf->operands.push_back(implyFree(o->operands[0]));
            return nf;
        }else{
            Op *ret = new Op(o->opType);
            ret->operands.push_back(implyFree(o->operands[0]));
            ret->operands.push_back(implyFree(o->operands[1]));
            return ret;
        }
    }
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

    split(str, " ");
    printf("%lu\n", tokens.size());

    int size = tokens.size();
    Formula *F = parseFormula();
    printf("%s\n", F->toString().c_str());

    Formula *IF = implyFree(F);
    printf("%s\n", IF->toString().c_str());
    return 0;
}
