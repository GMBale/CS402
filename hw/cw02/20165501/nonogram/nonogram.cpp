#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <numeric>

#define MTRUE 0xffffffffffffffff
#define VTRUE 0xfffffffffffffffe

using namespace std;

int n, m;
int MAX;
int vars;
vector<vector<int> *> rows;
vector<vector<int> *> cols;

string satInput = "p cnf";


class Clause {
    private:
    public:
        unsigned long long neg;
        unsigned long long pos;
        Clause() {
            neg = 0;
            pos = 0;
        }
};

class CNF {
    private:
    public:
        vector<Clause*> clauses;
		void addClause(unsigned long long t, Clause* c){
            vector<Clause*>::iterator it = clauses.begin();
			while(it != clauses.end()){
                unsigned long long imply = (~((*it)->neg) | c->neg) & (~((*it)->pos) | c->pos);
                unsigned long long rimply = ((*it)->neg | ~(c->neg)) & ((*it)->pos | ~(c->pos));
                if(imply == t){
                    return;
                }
                if(rimply == t){
                    it = clauses.erase(it);
                }else{
                    it++;
                }
			}
            clauses.push_back(c);
            return;
            /*if(limit == MAX) return;
            int cnt = 0;
            while(true){
                cnt++;
                bool fixed = true;
                vector<Clause*>::iterator it = clauses.begin();
                while(it != clauses.end()){
                    bool exist = false;
                    bool uniq = true;
                    int lit;
                    bool pos;
                    for(int i = st; i < limit; i++){
                        if((*it)->neg[i]){
                            if(!exist){
                                lit = i;
                                pos = false;
                            }else{
                                uniq = false;
                                break;
                            }
                            exist = true;
                        }
                        if((*it)->pos[i]){
                            if(!exist){
                                lit = i;
                                pos = true;
                            }else{
                                uniq = false;
                                break;
                            }
                            exist = true;
                        }
                    }
                    if(!uniq){
                        ++it;
                        continue;
                    }
                    printf("HERE2 %d %d\n",lit,pos);
                    vector<Clause*>::iterator it2 = clauses.begin();
                    while(it2 != clauses.end()){
                        if(pos){
                            if((*it2)->neg[lit]){
                                printf("HERE2 %d\n",lit);
                                (*it2)->neg[lit] = false;
                                fixed = false;
                                ++it2;
                                continue;
                            }
                        }else{
                            if((*it2)->pos[lit]){
                                printf("HERE2 %d\n",lit);
                                (*it2)->pos[lit] = false;
                                fixed = false;
                                ++it2;
                                continue;
                            }
                        }
                        ++it2;
                    }
                    ++it;
                }
                if(fixed) break;
            }
            printf("iter = %d\n", cnt);
            */
		}
        string toString()
        {
            string form = "";
            int len = clauses.size();
            for(int i = 0; i < len; i++){
                Clause* ci = clauses[i];
                if(i>0) form += " & ";
                form += "(";
                bool first = true;

                unsigned long long tneg = ci->neg;
                unsigned long long tpos = ci->pos;
                for(int j = 0; tneg != 0 || tpos != 0; j++){
                    if(tneg & 1){
                        if(!first) form += " | ";
                        if(j == 0) form += "-";
                        form += to_string(-j);
                        first = false;
                    }
                    if(tpos & 1){
                        if(!first) form += " | ";
                        form += to_string(j);
                        first = false;
                    }
                    tneg>>=1;
                    tpos>>=1;
                }
                form += ")";
            }
            return form;
        }
};

class FinalClause {
    private:
    public:
        vector<bool> neg;
        vector<bool> pos;
        FinalClause(int n) {
            for(int i = 0; i < n; i++){
                neg.push_back(false);
                pos.push_back(false);
            }
        }
        string toString(){
            string ret = "";
            bool first = true;
            for(int j = 1; j < vars; j++){
                if(neg[j]){
                    if(!first) ret += " ";
                    ret += to_string(-j);
                    first = false;
                }
                if(pos[j]){
                    if(!first) ret += " ";
                    ret += to_string(j);
                    first = false;
                }
            }
            return ret;
        }
};

class FinalCNF {
    private:
    public:
        vector<FinalClause*> clauses;
		void addClause(FinalClause* c){
            vector<FinalClause*>::iterator it = clauses.begin();
			while(it != clauses.end()){
                bool imply =  true;
                bool rimply = true;
                int len = (*it)->neg.size();
                int cnt1 = 0, cnt2 = 0;
                for(int i = 0; i < len; i++){
                    if((*it)->neg[i]) cnt1++;
                    if((*it)->pos[i]) cnt1++;

                    if(c->neg[i]) cnt2++;
                    if(c->pos[i]) cnt2++;

                    imply &= (!((*it)->neg[i]) || c->neg[i]) && (!((*it)->pos[i]) || c->pos[i]);
                    rimply &= ((*it)->neg[i] || !(c->neg[i])) && ((*it)->pos[i] || !(c->pos[i]));
                }
                if(imply){
                    return;
                }
                if(rimply){
                    it = clauses.erase(it);
                }else{
                    it++;
                }
			}
            clauses.push_back(c);
        }
};

FinalCNF cnf;

unordered_map<string, CNF*> mem;

void split(vector<string>* tokens, const string& str, const string& delim) {
    tokens->clear();
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens->push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
}

CNF* distr(CNF* n1, CNF* n2){
    CNF* ret = new CNF();
    if(n1->clauses.size() == 0){
        for(vector<Clause*>::iterator it2 = n2->clauses.begin(); it2 != n2->clauses.end(); ++it2){
            Clause *c = new Clause();
            c->neg = (*it2)->neg;
            c->pos = (*it2)->pos;
            if((c->neg & c->pos) != 0) continue;
            ret->addClause(MTRUE, c);
        }
    }else if(n2->clauses.size() == 0){
        for(vector<Clause*>::iterator it = n1->clauses.begin(); it != n1->clauses.end(); ++it){
            Clause *c = new Clause();
            c->neg = (*it)->neg;
            c->pos = (*it)->pos;
            if((c->neg & c->pos) != 0) continue;
            ret->addClause(MTRUE, c);
        }
    }
    for(vector<Clause*>::iterator it = n1->clauses.begin(); it != n1->clauses.end(); ++it){
        for(vector<Clause*>::iterator it2 = n2->clauses.begin(); it2 != n2->clauses.end(); ++it2){
            Clause *c = new Clause();
            c->neg = (*it)->neg | (*it2)->neg;
            c->pos = (*it)->pos | (*it2)->pos;
            if((c->neg & c->pos) != 0) continue;
            ret->addClause(MTRUE, c);
        }
    }
    return ret;
}

CNF* assign(int n, vector<int> *boxes, vector<int>::iterator st){
    string state = "";
    state.append(to_string(n));
    int len = 0;
    int sum = 0;
    for(vector<int>::iterator it = st; it != boxes->end(); ++it){
        state.append(" ");
        state.append(to_string(*it));
        sum += *it;
        ++len;
    }
    sum += len - 1;
    //printf("%s %d %d\n", state.c_str(), sum, len);
    auto t = mem.find(state);
    if (t == mem.end()) {
        if(n < sum){
            mem[state] = NULL;
            return NULL;
        }
        if(len == 0){
            CNF *ret = new CNF();
            for(int i = 0; i < n; i++){
                Clause *c = new Clause();
                c->neg = 1 << i;
                ret->addClause(MTRUE, c);
            }
            mem[state] = ret;
            return ret;
        }else{
            int front = *st;
            int m = n - sum;
            vector<CNF*> ors;
            for(int i = 0; i <= m; i++){
                int gap = len > 1 ? 1 : 0;
                CNF *rest = assign(n - i - front - gap, boxes, st+1);
                if(rest == NULL) continue;
                CNF* one = new CNF();
                for(int j = 0; j < i; j++){
                    Clause *c = new Clause();
                    c->neg = 1 << j;
                    one->addClause(MTRUE, c);
                }
                for(int j = i; j < i+front; j++){
                    Clause *c = new Clause();
                    c->pos = 1 << j;
                    one->addClause(MTRUE, c);
                }
                if(len > 1){
                    Clause *c = new Clause();
                    c->neg = 1 << (i+front);
                    one->addClause(MTRUE, c);
                }
                int last = i+front+gap;
                for(vector<Clause*>::iterator it = rest->clauses.begin(); it != rest->clauses.end(); ++it){
                    Clause *c = new Clause();
                    c->neg = (*it)->neg << last;
                    c->pos = (*it)->pos << last;
                    one->addClause(MTRUE, c);
                }
                ors.push_back(one);
            }
            CNF *ret = new CNF();
            int tmp = len;
            int len = ors.size();
            //printf("DISTR %d %d %d %lu %lu\n", n, tmp, len, ret->clauses.size(), ors[0]->clauses.size());
            for(int i = 0; i < len; i++){
                ret = distr(ret, ors[i]);
                //printf("ors[%d]: %s\n", i, ors[i]->toString().c_str());
                //printf("ret: %lu\n", ret->clauses.size());
                //printf("ret: %s\n", ret->toString().c_str());
            }
            //printf("DISTR %d %d %lu %lu\n", n, len, ret->clauses.size(), ors[0]->clauses.size());
            //printf("%s\n", ret->toString().c_str());
            mem[state] = ret;
            return ret;
        }
    }else{
        return t->second;
    }
}

FinalCNF* add(CNF* f, int base, int offset){
    //printf("add size = %lu\n", f->clauses.size());
    FinalCNF* ret = new FinalCNF();
    for(vector<Clause*>::iterator it = f->clauses.begin(); it != f->clauses.end(); ++it){
        FinalClause* c = new FinalClause(vars);
        unsigned long long tneg = (*it)->neg;
        unsigned long long tpos = (*it)->pos;
        for(int i = 0; i < MAX; i++,tneg>>=1,tpos>>=1){
            //printf("HERE %d %d %d %d\n", base, offset, i, base + i*offset);
            //printf("tneg = %lld  tpos = %lld\n", tneg ,tpos);
            if(tneg & 1){
                c->neg[base + i*offset] = true;
            }
            if(tpos & 1){
                c->pos[base + i*offset] = true;
            }
        }

        //printf("addFinalClause: %s\n", c->toString().c_str());
        ret->addClause(c);
        //printf("after add size : %lu\n", ret->clauses.size());
    }
    return ret;
}

int main(int argc, char *argv[])
{
    switch(argc){
        case 1:
            break;
        default:
            printf("Wrong number of arguments!\n");
            return 0;
    }
    ifstream file("input.cwd");
    string line;

    getline(file, line);
    n = atoi( line.c_str() );

    getline(file, line);
    m = atoi( line.c_str() );

    vector<string> tokens;
    for(int i = 0; i < n; i++){
        getline(file, line);
        split(&tokens, line, " ");
        vector<int> *row = new vector<int>();
        int len = tokens.size();
        for(int j = 0; j < len; j++){
            row->push_back(atoi( tokens[j].c_str() ));
        }
        if(row->size() == 1 && row->at(0) == 0){row->clear();}
        rows.push_back(row);
    }
    for(int i = 0; i < m; i++){
        getline(file, line);
        split(&tokens, line, " ");
        vector<int> *col = new vector<int>();
        int len = tokens.size();
        for(int j = 0; j < len; j++){
            col->push_back(atoi( tokens[j].c_str() ));
        }
        if(col->size() == 1 && col->at(0) == 0){col->clear();}
        cols.push_back(col);
    }

    MAX = n > m ? n : m;
    vars = n*m+1;
    satInput += " " + to_string(vars-1);
    for(int i = 0; i < n; i++){
        //printf("row %d size = %lu\n", i, rows[i]->size());
        //break;
        FinalCNF* row = add(assign(m, rows[i], rows[i]->begin()), 1+i*m, 1);
        for(vector<FinalClause*>::iterator it = row->clauses.begin(); it != row->clauses.end(); ++it){
            //printf("GFinalClause: %s\n", (*it)->toString().c_str());
            cnf.addClause(*it);
        }
        //printf("row %d CNF size = %lu\n", i, cnf.clauses.size());
    }
    for(int i = 0; i < m; i++){
        //printf("col %d size = %lu\n", i, cols[i]->size());
        //break;
        FinalCNF* col = add(assign(n, cols[i], cols[i]->begin()), 1+i, m);
        for(vector<FinalClause*>::iterator it = col->clauses.begin(); it != col->clauses.end(); ++it){
            cnf.addClause(*it);
        }
    }
    int len = cnf.clauses.size();
    string form = "";
    for(int i = 0; i < len; i++){
        FinalClause* ci = cnf.clauses[i];
        if(i>0) form += " & ";
        form += "(";
        bool first = true;


        for(int j = 1; j < vars; j++){
            if(ci->neg[j]){
                if(!first) form += " | ";
                form += to_string(-j);
                first = false;
            }
            if(ci->pos[j]){
                if(!first) form += " | ";
                form += to_string(j);
                first = false;
            }
        }
        form += ")";
    }
    //printf("%s\n", form.c_str());
//    printf("%s\n", satInput.c_str());

    FILE *out = fopen("input.sat", "w");
    satInput += " ";
    satInput += to_string(len);
    satInput += "\n";

    for(int i = 0; i < len; i++){
        FinalClause* ci = cnf.clauses[i];
        bool first = true;
        for(int j = 1; j < vars; j++){
            if(ci->neg[j]){
                if(!first) satInput += " ";
                satInput += to_string(-j);
                first = false;
            }
            if(ci->pos[j]){
                if(!first) satInput += " ";
                satInput += to_string(j);
                first = false;
            }
        }
        satInput += " 0\n";
    }

    fprintf(out, "%s", satInput.c_str());
    fclose(out);
    system("./minisat input.sat output.txt > sat.log");
    FILE *in = fopen("output.txt", "r");
    char sat[100];
    fscanf(in, "%s", sat);
    int lit;
    for(int i = 0 ; i < n; i++){
        for(int j = 0; j < m; j++){
            fscanf(in, "%d", &lit);
            if(lit < 0){
                printf(".");
            }else{
                printf("#");
            }
        }
        printf("\n");
    }
    return 0;
}
