#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<string> tokens;

void split(const string& str, const string& delim) { tokens.clear();
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
        if(o->opType == IMPLY || o->opType == RIMPLY || o->opType == EQUIV){
            Op *ret = new Op(OR);
            if(o->opType == IMPLY){
                Op *nf = new Op(NOT);
                nf->operands.push_back(implyFree(o->operands[0]));
                ret->operands.push_back(nf);
                ret->operands.push_back(implyFree(o->operands[1]));
                return ret;
            }
            if(o->opType == RIMPLY){
                Op *nf = new Op(NOT);
                nf->operands.push_back(implyFree(o->operands[1]));
                ret->operands.push_back(implyFree(o->operands[0]));
                ret->operands.push_back(nf);
            }else{
                Op *and1 = new Op(AND);
                and1->operands.push_back(implyFree(o->operands[0]));
                and1->operands.push_back(implyFree(o->operands[1]));
                Op *and2 = new Op(AND);
                Op *n1 = new Op(NOT);
                n1->operands.push_back(implyFree(o->operands[0]));
                Op *n2 = new Op(NOT);
                n2->operands.push_back(implyFree(o->operands[1]));
                and2->operands.push_back(n1);
                and2->operands.push_back(n2);
                ret->operands.push_back(and1);
                ret->operands.push_back(and2);
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
Formula* nnf(Formula *f) {
    if(Literal *l = dynamic_cast<Literal *>(f)){
        return f;
    }else{
        Op *o = dynamic_cast<Op *>(f);
        if(o->opType == NOT){
            if(Op *nf = dynamic_cast<Op *>(o->operands[0])){
                if(nf->opType == NOT){
                    return nf->operands[0];
                }
                Ops nop = AND;
                if(nf->opType == AND){
                    nop = OR;
                }
                Op *ret = new Op(nop);
                Op *nLeft = new Op(NOT);
                Op *nRight = new Op(NOT);
                nLeft->operands.push_back(nf->operands[0]);
                nRight->operands.push_back(nf->operands[1]);
                ret->operands.push_back(nLeft);
                ret->operands.push_back(nRight);
                return nnf(ret);
            }else{
                return f;
            }
        }else{
            Op *ret = new Op(o->opType);
            ret->operands.push_back(nnf(o->operands[0]));
            ret->operands.push_back(nnf(o->operands[1]));
            return ret;
        }
    }
}

Formula* distr(Formula *n1, Formula *n2){
    if(Op *o = dynamic_cast<Op *>(n1)){
        if(o->opType == AND){
            Op *_and = new Op(AND);
            _and->operands.push_back(distr(o->operands[0], n2));
            _and->operands.push_back(distr(o->operands[1], n2));
            return _and;
        }
    }
    if(Op *o = dynamic_cast<Op *>(n2)){
        if(o->opType == AND){
            Op *_and = new Op(AND);
            _and->operands.push_back(distr(n1, o->operands[0]));
            _and->operands.push_back(distr(n1, o->operands[1]));
            return _and;
        }
    }
    Op *ret = new Op(OR);
    ret->operands.push_back(n1);
    ret->operands.push_back(n2);
    return ret;
}

Formula* cnf(Formula *f) {
    if(Literal *l = dynamic_cast<Literal *>(f)){
        return f;
    }else{
        Op *o = dynamic_cast<Op *>(f);
        if(o->opType == AND){
            Op *ret = new Op(AND);
            ret->operands.push_back(cnf(o->operands[0]));
            ret->operands.push_back(cnf(o->operands[1]));
            return ret;
        }else if(o->opType == OR){
            return distr(cnf(o->operands[0]), o->operands[1]);
        }else{
            return f;
        }
    }
}

vector<vector<Formula *> *> INFIX;

void addClause(vector<Formula *>* clause, Formula *f) {
    if(Literal *l = dynamic_cast<Literal *>(f)){
        clause->push_back(f);
        return;
    }else{
        Op *o = dynamic_cast<Op *>(f);
        if(o->opType == OR){
            addClause(clause, o->operands[0]);
            addClause(clause, o->operands[1]);
            return;
        }else if(o->opType == NOT){
            clause->push_back(f);
            return;
        }else{
            printf("ERROR\n");
            return;
        }
    }
}
void infix(Formula *f) {
    if(Literal *l = dynamic_cast<Literal *>(f)){
        vector<Formula *> *clause = new vector<Formula *>();
        addClause(clause, f);
        INFIX.push_back(clause);
        return;
    }else{
        Op *o = dynamic_cast<Op *>(f);
        if(o->opType == AND){
            infix(o->operands[0]);
            infix(o->operands[1]);
            return;
        }else if(o->opType == OR){
            vector<Formula *> *clause = new vector<Formula *>();
            addClause(clause, o->operands[0]);
            addClause(clause, o->operands[1]);
            INFIX.push_back(clause);
            return;
        }else{
            vector<Formula *> *clause = new vector<Formula *>();
            addClause(clause, f);
            INFIX.push_back(clause);
            return;
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
	string str(argv[1]);

	printf("%s\n", str.c_str());

    split(str, " ");

    int size = tokens.size();
    Formula *F = parseFormula();
    printf("%s\n", F->toString().c_str());

    printf("%lu\n", tokens.size());

    Formula *IF = implyFree(F);
    printf("IF: %s\n", IF->toString().c_str());

    Formula *NNF = nnf(IF);
    printf("NNF: %s\n", NNF->toString().c_str());

    Formula *CNF = cnf(NNF);
    printf("CNF: %s\n", CNF->toString().c_str());

    infix(CNF);
    int n = INFIX.size();
    for(int i = 0; i < n; i++){
        int m = INFIX[i]->size();
        if(i > 0){
            printf(" & ");
        }
        printf("(");
        for(int j = 0; j < m; j++){
            if(j > 0){
                printf(" | ");
            }
            printf("%s", INFIX[i]->at(j)->toString().c_str());
        }
        printf(")");
    }
    printf("\n");
    bool validity = true;
    for(int i = 0; i < n; i++){
        int m = INFIX[i]->size();
        bool cvalid = false;
        for(int j = 0; j < m; j++){
            Formula *jf = INFIX[i]->at(j);
            bool nfj = false;
            string wj;
            if(Literal *l = dynamic_cast<Literal *>(jf)){
                wj = l->name;
            }else{
                Op *o = dynamic_cast<Op *>(jf);
                if(Literal *l = dynamic_cast<Literal *>(o->operands[0])){
                    nfj = true;
                    wj = l->name;
                }else{
                    printf("ERROR\n");
                }
            }
            if(INFIX[i]->at(j))
            for(int k = j + 1; k < m; k++){
                Formula *kf = INFIX[i]->at(k);
                bool nfk = false;
                string wk;
                if(Literal *l = dynamic_cast<Literal *>(kf)){
                    wk = l->name;
                }else{
                    Op *o = dynamic_cast<Op *>(kf);
                    if(Literal *l = dynamic_cast<Literal *>(o->operands[0])){
                        nfk = true;
                        wk = l->name;
                    }else{
                        printf("ERROR\n");
                    }
                }
                if(wj.compare(wk) == 0){
                    if(nfj != nfk){
                        cvalid = true;
                        break;
                    }
                }
            }
        }
        if(!cvalid){
            validity = false;
            break;
        }
    }
    if(validity){
        printf("Valid\n");
    }else{
        printf("Not Valid\n");
    }
    return 0;
}
