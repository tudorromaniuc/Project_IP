#include <graphics.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <conio.h>
#include <stdio.h>

using namespace std;

struct Nod
{
    char info[50];
    Nod* stanga;
    Nod* dreapta;
};


bool esteOperatorSimplu(char c)
{
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
}

bool esteOperatorElem(const char elem[])
{
    return (
               strcmp(elem, "+")==0 || strcmp(elem,"-")==0 ||
               strcmp(elem,"*")==0 || strcmp(elem,"/")==0 ||
               strcmp(elem,"^")==0
           );
}

bool esteParantezaDeschisa(const char elem[])
{
    return (strcmp(elem,"(")==0);
}

bool esteParantezaInchisa(const char elem[])
{
    return (strcmp(elem,")")==0);
}

bool esteNumar(const char elem[])
{
    if (strlen(elem) == 0) return false;
    int puncte = 0;
    for (int i=0; i<(int)strlen(elem); i++)
    {
        /// cifre sau numar zecimal
        if (!((elem[i]>='0' && elem[i]<='9') || elem[i]=='.'))
            return false;
        if (elem[i]=='.')
            puncte++;
        if (puncte>1)
            return false;
    }
    return true;
}

bool esteVariabila(const char elem[])
{
    /// "x" singura variabila
    return (strcmp(elem,"x")==0);
}

bool esteFunctieMatematica(const char elem[])
{
    const char* functii[] =
    {
        "sin", "cos", "tg", "ctg",
        "arcsin","arccos","arctg","arcctg",
        "log","ln","lg","log10","log2",
        "sqrt","exp"
    };
    int nrFunctii = (int)(sizeof(functii)/sizeof(functii[0]));
    for (int i=0; i<nrFunctii; i++)
    {
        if (strcmp(elem, functii[i])==0)
            return true;
    }
    return false;
}

int prioritate(const char op[])
{
    if (strcmp(op,"+")==0 || strcmp(op,"-")==0)
        return 1;
    if (strcmp(op,"*")==0 || strcmp(op,"/")==0)
        return 2;
    if (strcmp(op,"^")==0)
        return 3;
    if (esteFunctieMatematica(op))
        return 4;
    return -1;
}

void pushElem(char stiva[][50], int &top, const char elem[])
{
    top++;
    strcpy(stiva[top], elem);
}

void popElem(int &top)
{
    if (top>=0) top--;
}

void topElem(char stiva[][50], int top, char rezultat[])
{
    if (top>=0) strcpy(rezultat, stiva[top]);
}

void remove1StarFromString(char *str)
{
    /// daca avem "1*" lasam fara el
    char *p = str;
    while ((p = strstr(p, "1*")) != NULL)
    {
        /// daca avem "(+-/*^", eliminam "1*".
        if (p == str)
        {
            memmove(p, p+2, strlen(p+2)+1);
        }
        else
        {
            char c = *(p-1);
            if (c == '(' || c == '+' || c == '-' || c == '/' || c == '*' || c == '^')
            {
                memmove(p, p+2, strlen(p+2)+1);
            }
            else
            {
                p++;
            }
        }
    }
}
int element(char *expr, char elems[][50])
{
    int len = (int)strlen(expr);
    int cnt = 0;
    int pos = 0;
    for (int i=0; i<=len; i++)
    {
        char c = expr[i];
        if (c==' ' || c=='\t' || c=='\0')
        {
            if (pos>0)
            {
                elems[cnt][pos] = '\0';
                cnt++;
                pos=0;
            }
        }
        else if (esteOperatorSimplu(c) || c=='(' || c==')')
        {
            if (pos>0)
            {
                elems[cnt][pos] = '\0';
                cnt++;
                pos=0;
            }
            elems[cnt][0] = c;
            elems[cnt][1] = '\0';
            cnt++;
        }
        else
        {
            elems[cnt][pos] = c;
            pos++;
        }
    }
    return cnt;
}

void infixInPostfix(char elems[][50], int n, char postfix[][50], int &m)
{
    char stiva[100][50];
    int top=-1;
    m=0;

    for (int i=0; i<n; i++)
    {
        if (esteNumar(elems[i]) || esteVariabila(elems[i]))
        {
            strcpy(postfix[m], elems[i]);
            m++;
        }
        else if (esteParantezaDeschisa(elems[i]))
        {
            pushElem(stiva, top, elems[i]);
        }
        else if (esteParantezaInchisa(elems[i]))
        {
            char op[50];
            topElem(stiva, top, op);
            while(top>=0 && !esteParantezaDeschisa(op))
            {
                strcpy(postfix[m], op);
                m++;
                popElem(top);
                topElem(stiva, top, op);
            }
            if (top>=0 && esteParantezaDeschisa(op))
                popElem(top);
        }
        else if (esteOperatorElem(elems[i]) || esteFunctieMatematica(elems[i]))
        {
            char op[50];
            topElem(stiva, top, op);
            while(top>=0 && prioritate(op)>=prioritate(elems[i]))
            {
                strcpy(postfix[m], op);
                m++;
                popElem(top);
                topElem(stiva, top, op);
            }
            pushElem(stiva, top, elems[i]);
        }
    }

    while(top>=0)
    {
        char op[50];
        topElem(stiva, top, op);
        strcpy(postfix[m], op);
        m++;
        popElem(top);
    }
}

Nod* creareNod(const char val[])
{
    Nod* nod = (Nod*)malloc(sizeof(Nod));
    strcpy(nod->info, val);
    nod->stanga = NULL;
    nod->dreapta = NULL;
    return nod;
}

Nod* construireArboreDinPostfix(char postfix[][50], int m)
{
    Nod* stiva[100];
    int top=-1;

    for (int i=0; i<m; i++)
    {
        if (esteNumar(postfix[i]) || esteVariabila(postfix[i]))
        {
            Nod* nod = creareNod(postfix[i]);
            top++;
            stiva[top] = nod;
        }
        else if (esteFunctieMatematica(postfix[i]))
        {
            Nod* nod = creareNod(postfix[i]);
            if(top>=0)
            {
                Nod* st = stiva[top];
                top--;
                nod->stanga = st;
                top++;
                stiva[top] = nod;
            }
        }
        else if (esteOperatorElem(postfix[i]))
        {
            Nod* nod = creareNod(postfix[i]);
            if (top>=1)
            {
                Nod* dreapta = stiva[top];
                top--;
                Nod* stanga = stiva[top];
                top--;
                nod->stanga = stanga;
                nod->dreapta = dreapta;
                top++;
                stiva[top] = nod;
            }
        }
    }

    if (top>=0)
        return stiva[top];
    return NULL;
}

bool eFrunzaNumar(Nod* nod)
{
    if (!nod) return false;
    return (nod->stanga == NULL && nod->dreapta == NULL && esteNumar(nod->info));
}

bool eFrunzaX(Nod* nod)
{
    if (!nod) return false;
    return (nod->stanga == NULL && nod->dreapta == NULL && esteVariabila(nod->info));
}

bool eFunctie(Nod* nod)
{
    if (!nod) return false;
    return esteFunctieMatematica(nod->info);
}

Nod* copiazaArbore(Nod* rad)
{
    if (!rad) return NULL;
    Nod* nou = creareNod(rad->info);
    nou->stanga = copiazaArbore(rad->stanga);
    nou->dreapta = copiazaArbore(rad->dreapta);
    return nou;
}

char* minusUnu(char numar[])
{
    int n = (int)strlen(numar);
    for(int i=n-1; i>=0; i--)
    {
        if(numar[i]>'0')
        {
            numar[i]--;
            break;
        }
        else
            numar[i]='9';
    }
    if(numar[0]=='0')
        numar[0] = '1';
    return numar;
}

Nod* construiesteFPrim(const char f[], Nod* g)
{
    /// daca incepe cu '-'
    if (f[0]=='-' && strncmp(f,"-1/sin^2(",9)!=0 &&
            strncmp(f,"-1/sqrt(1-",9)!=0 &&
            strncmp(f,"-1/(1+",6)!=0 )
    {
        Nod* mul = creareNod("*");
        Nod* minusUnuNode = creareNod("-1");
        Nod* funcNod = creareNod(f+1);

        funcNod->stanga = copiazaArbore(g);

        mul->stanga = minusUnuNode;
        mul->dreapta = funcNod;
        return mul;
    }

    /// pentru ctg(x) => -1/sin^2(x)
    if (strncmp(f,"-1/sin^2(",9)==0)
    {
        /// construim -1 / sin^2(x)
        Nod* sinNode = creareNod("sin");
        sinNode->stanga = copiazaArbore(g);

        Nod* powerNode = creareNod("^");
        powerNode->stanga = sinNode;
        powerNode->dreapta = creareNod("2");

        Nod* div = creareNod("/");
        div->stanga = creareNod("1");
        div->dreapta = powerNode;

        Nod* mul = creareNod("*");
        mul->stanga = creareNod("-1");
        mul->dreapta = div;
        return mul;
    }
    /// arcsin(x) => 1/sqrt(1 - x^2)
    else if (strncmp(f,"1/sqrt(1-",8)==0)
    {
        Nod* unu = creareNod("1");
        Nod* minusN = creareNod("-");
        minusN->stanga = creareNod("1");

        Nod* powx = creareNod("^");
        powx->stanga = copiazaArbore(g);
        powx->dreapta = creareNod("2");
        minusN->dreapta = powx;

        Nod* sqrtNode = creareNod("sqrt");
        sqrtNode->stanga = minusN;

        Nod* div = creareNod("/");
        div->stanga = unu;
        div->dreapta = sqrtNode;
        return div;
    }
    /// arccos(x) => -1/sqrt(1 - x^2)
    else if (strncmp(f,"-1/sqrt(1-",9)==0)
    {
        Nod* unu = creareNod("1");
        Nod* minusN = creareNod("-");
        minusN->stanga = creareNod("1");

        Nod* powx = creareNod("^");
        powx->stanga = copiazaArbore(g);
        powx->dreapta = creareNod("2");
        minusN->dreapta = powx;

        Nod* sqrtNode = creareNod("sqrt");
        sqrtNode->stanga = minusN;

        Nod* div = creareNod("/");
        div->stanga = unu;
        div->dreapta = sqrtNode;

        Nod* mul = creareNod("*");
        mul->stanga = creareNod("-1");
        mul->dreapta = div;
        return mul;
    }
    /// arctg(x) => 1/(1 + x^2)
    else if (strncmp(f,"1/(1+",5)==0)
    {
        Nod* unu = creareNod("1");
        Nod* plusN = creareNod("+");
        plusN->stanga = creareNod("1");

        Nod* powx = creareNod("^");
        powx->stanga = copiazaArbore(g);
        powx->dreapta = creareNod("2");
        plusN->dreapta = powx;

        Nod* div = creareNod("/");
        div->stanga = unu;
        div->dreapta = plusN;
        return div;
    }
    /// arcctg(x) => -1/(1 + x^2)
    else if (strncmp(f,"-1/(1+",6)==0)
    {
        Nod* unu = creareNod("1");
        Nod* plusN = creareNod("+");
        plusN->stanga = creareNod("1");

        Nod* powx = creareNod("^");
        powx->stanga = copiazaArbore(g);
        powx->dreapta = creareNod("2");
        plusN->dreapta = powx;

        Nod* div = creareNod("/");
        div->stanga = unu;
        div->dreapta = plusN;

        Nod* mul = creareNod("*");
        mul->stanga = creareNod("-1");
        mul->dreapta = div;
        return mul;
    }
    /// tg(x) => 1/cos^2(x)
    else if (strncmp(f,"1/cos^2(",7)==0)
    {
        Nod* cosNode = creareNod("cos");
        cosNode->stanga = copiazaArbore(g);

        Nod* powerNode = creareNod("^");
        powerNode->stanga = cosNode;
        powerNode->dreapta = creareNod("2");

        Nod* div = creareNod("/");
        div->stanga = creareNod("1");
        div->dreapta = powerNode;
        return div;
    }
    /// sin^2
    else if (strncmp(f,"1/sin^2(",7)==0)
    {
        Nod* sinNode = creareNod("sin");
        sinNode->stanga = copiazaArbore(g);

        Nod* powerNode = creareNod("^");
        powerNode->stanga = sinNode;
        powerNode->dreapta = creareNod("2");

        Nod* div = creareNod("/");
        div->stanga = creareNod("1");
        div->dreapta = powerNode;
        return div;
    }
    /// sqrt => (1/(2*sqrt(...)
    else if (strncmp(f,"(1/(2*sqrt",10)==0)
    {
    }
    else if (strncmp(f,"1/(",3)==0)
    {
        Nod* div = creareNod("/");
        div->stanga = creareNod("1");
        div->dreapta = copiazaArbore(g);
        return div;
    }
    else if (strncmp(f,"1/(ln(10)*",10)==0)
    {
        /// => 1 / [ln(10)* g]
        Nod* ln10 = creareNod("ln");
        ln10->stanga = creareNod("10");

        Nod* mul = creareNod("*");
        mul->stanga = ln10;
        mul->dreapta = copiazaArbore(g);

        Nod* div = creareNod("/");
        div->stanga = creareNod("1");
        div->dreapta = mul;
        return div;
    }
    else if (strncmp(f,"1/(ln(2)*",9)==0)
    {
        /// => 1 / [ln(2)* g]
        Nod* ln2 = creareNod("ln");
        ln2->stanga = creareNod("2");

        Nod* mul = creareNod("*");
        mul->stanga = ln2;
        mul->dreapta = copiazaArbore(g);

        Nod* div = creareNod("/");
        div->stanga = creareNod("1");
        div->dreapta = mul;
        return div;
    }

    /// functie normala
    Nod* func = creareNod(f);
    if (esteFunctieMatematica(f))
    {
        func->stanga = copiazaArbore(g);
    }
    else
    {
        func->dreapta = copiazaArbore(g);
    }
    return func;
}

Nod* derivaArbore(Nod* rad)
{
    if (!rad) return NULL;

    if (eFrunzaX(rad))
    {
        /// d/dx [x] = 1
        return creareNod("1");
    }

    if (eFrunzaNumar(rad))
    {
        /// d/dx [constant] = 0
        return creareNod("0");
    }

    if (eFunctie(rad))
    {
        Nod* inner = rad->stanga;
        Nod* dInner = derivaArbore(inner);

        char fname[50];
        strcpy(fname, rad->info);

        if (strcmp(fname,"sin")==0)         strcpy(fname,"cos");
        else if (strcmp(fname,"cos")==0)    strcpy(fname,"-sin");
        else if (strcmp(fname,"tg")==0)     strcpy(fname,"1/cos^2(");
        else if (strcmp(fname,"ctg")==0)    strcpy(fname,"-1/sin^2(");
        else if (strcmp(fname,"arcsin")==0) strcpy(fname,"1/sqrt(1-");
        else if (strcmp(fname,"arccos")==0) strcpy(fname,"-1/sqrt(1-");
        else if (strcmp(fname,"arctg")==0)  strcpy(fname,"1/(1+");
        else if (strcmp(fname,"arcctg")==0) strcpy(fname,"-1/(1+");
        // LOG:
        else if (strcmp(fname,"ln")==0 ||
                 strcmp(fname,"log")==0)
        {
            /// baza e
            strcpy(fname,"1/(");
        }
        else if (strcmp(fname,"log10")==0 ||
                 strcmp(fname,"lg")==0)
        {
            strcpy(fname,"1/(ln(10)*");
        }
        else if (strcmp(fname,"log2")==0)
        {
            strcpy(fname,"1/(ln(2)*");
        }
        else if (strcmp(fname,"exp")==0)
        {
            strcpy(fname,"exp");
        }
        else if (strcmp(fname,"sqrt")==0)
        {
            strcpy(fname,"(1/(2*sqrt");
        }
        else strcpy(fname,"0");

        Nod* fp = construiesteFPrim(fname, inner);
        Nod* mul = creareNod("*");
        mul->stanga = dInner;
        mul->dreapta = fp;
        return mul;
    }

    /// daca este +, -, *, /, ^
    if (esteOperatorElem(rad->info))
    {
        char op[50];
        strcpy(op, rad->info);

        Nod* dLeft = derivaArbore(rad->stanga);
        Nod* dRight = derivaArbore(rad->dreapta);

        if (strcmp(op,"+")==0)
        {
            /// (f+g)' = f' + g'
            Nod* p = creareNod("+");
            p->stanga = dLeft;
            p->dreapta = dRight;
            return p;
        }
        if (strcmp(op,"-")==0)
        {
            /// (f-g)' = f' - g'
            Nod* m = creareNod("-");
            m->stanga = dLeft;
            m->dreapta = dRight;
            return m;
        }
        if (strcmp(op,"*")==0)
        {
            /// (f*g)' = f'*g + f*g'
            Nod* stCopy = copiazaArbore(rad->stanga);
            Nod* drCopy = copiazaArbore(rad->dreapta);

            Nod* term1 = creareNod("*");
            term1->stanga = dLeft;
            term1->dreapta = drCopy;

            Nod* term2 = creareNod("*");
            term2->stanga = stCopy;
            term2->dreapta = dRight;

            Nod* sumNode = creareNod("+");
            sumNode->stanga = term1;
            sumNode->dreapta = term2;
            return sumNode;
        }
        if (strcmp(op,"/")==0)
        {
            /// (f/g)' = [f'*g - f*g'] / g^2
            Nod* numerator1 = creareNod("*");
            numerator1->stanga = dLeft;
            numerator1->dreapta = copiazaArbore(rad->dreapta);

            Nod* numerator2 = creareNod("*");
            numerator2->stanga = copiazaArbore(rad->stanga);
            numerator2->dreapta = dRight;

            Nod* num = creareNod("-");
            num->stanga = numerator1;
            num->dreapta = numerator2;

            Nod* g2 = creareNod("^");
            g2->stanga = copiazaArbore(rad->dreapta);
            g2->dreapta = creareNod("2");

            Nod* div = creareNod("/");
            div->stanga = num;
            div->dreapta = g2;
            return div;
        }
        if (strcmp(op,"^")==0)
        {
            /// (f^g)' => cazuri multiple
            Nod* df = dLeft;
            Nod* dg = dRight;
            Nod* stCopy = copiazaArbore(rad->stanga);
            Nod* drCopy = copiazaArbore(rad->dreapta);

            /// x^c => c*x^(c-1)
            if (esteNumar(drCopy->info) && !esteNumar(stCopy->info))
            {
                Nod* copie = creareNod(drCopy->info);
                Nod* putere = creareNod("^");
                putere->stanga = stCopy;
                Nod* puterenoua = creareNod(minusUnu(copie->info));
                putere->dreapta = puterenoua;

                Nod* mul = creareNod("*");
                mul->stanga = creareNod(drCopy->info);
                mul->dreapta = putere;

                Nod* rezultat = creareNod("*");
                rezultat->stanga = mul;
                rezultat->dreapta = df;
                return rezultat;
            }
            /// c^x => c^x * ln(c) * x'
            else if (!esteNumar(drCopy->info) && esteNumar(stCopy->info))
            {
                Nod* lnNode = creareNod("ln");
                lnNode->stanga = copiazaArbore(rad->stanga);

                Nod* mul = creareNod("*");
                mul->stanga = lnNode;
                mul->dreapta = dg;

                Nod* putere = creareNod("^");
                putere->stanga = stCopy;
                putere->dreapta = drCopy;

                Nod* rezultat = creareNod("*");
                rezultat->stanga = putere;
                rezultat->dreapta = mul;
                return rezultat;
            }
            /// f^g => f^g [ g'(x)*ln(f) + g*f'(x)/f(x) ]
            else
            {
                Nod* fg = creareNod("^");
                fg->stanga = copiazaArbore(rad->stanga);
                fg->dreapta = copiazaArbore(rad->dreapta);

                Nod* lnNode = creareNod("ln");
                lnNode->stanga = copiazaArbore(rad->stanga);

                Nod* part1 = creareNod("*");
                part1->stanga = dg;
                part1->dreapta = lnNode;

                Nod* divf = creareNod("/");
                divf->stanga = df;
                divf->dreapta = copiazaArbore(rad->stanga);

                Nod* part2 = creareNod("*");
                part2->stanga = copiazaArbore(rad->dreapta);
                part2->dreapta = divf;

                Nod* sum = creareNod("+");
                sum->stanga = part1;
                sum->dreapta = part2;

                Nod* mul = creareNod("*");
                mul->stanga = fg;
                mul->dreapta = sum;
                return mul;
            }
        }
    }

    return creareNod("0");
}

void printInfixSimpl(Nod* rad, char out[], int parentPrec, bool isRightChild)
{
    if (!rad) return;
    if (eFrunzaNumar(rad) || eFrunzaX(rad))
    {
        strcat(out, rad->info);
        return;
    }

    if (eFunctie(rad))
    {
        strcat(out, rad->info);
        strcat(out, "(");
        printInfixSimpl(rad->stanga, out, 4, false);
        strcat(out, ")");
        return;
    }

    int currentPrec = prioritate(rad->info);
    bool needParen = (currentPrec < parentPrec);
    if (needParen) strcat(out, "(");

    /// copil stang
    if (rad->stanga)
    {
        printInfixSimpl(rad->stanga, out, currentPrec, false);
    }

    /// operator
    strcat(out, rad->info);

    /// copil drept
    if (rad->dreapta)
    {
        bool rightIsRightChild = false;
        if (strcmp(rad->info,"^")==0)
        {
            rightIsRightChild = true;
        }
        printInfixSimpl(rad->dreapta, out, currentPrec + (rightIsRightChild?1:0), true);
    }

    if (needParen) strcat(out, ")");
}

void afiseazaArboreInfixInSir(Nod* rad, char out[])
{
    out[0] = '\0';
    printInfixSimpl(rad, out, -1, false);
    remove1StarFromString(out);
}

Nod* simplificaArbore(Nod* rad);

bool sameFunctionNodes(Nod* a, Nod* b)
{
    if(!a || !b) return false;

    if(a->stanga == NULL && a->dreapta == NULL && b->stanga == NULL && b->dreapta == NULL)
    {
        return (strcmp(a->info, b->info) == 0);
    }

    if(eFunctie(a) && eFunctie(b))
    {
        if(strcmp(a->info, b->info) == 0)
        {
            return sameFunctionNodes(a->stanga, b->stanga);
        }
    }

    return false;
}

struct FactorPair
{
    Nod* factorNode;
    int multiplicity;
};

void colecteazaFactori(Nod* rad, double &combinedConstant, FactorPair factorArray[], int &factorCount)
{
    if (!rad) return;

    if (strcmp(rad->info, "*") == 0)
    {
        colecteazaFactori(rad->stanga, combinedConstant, factorArray, factorCount);
        colecteazaFactori(rad->dreapta, combinedConstant, factorArray, factorCount);
    }
    else
    {
        if (eFrunzaNumar(rad))
        {
            double val = atof(rad->info);
            combinedConstant *= val;
        }
        else
        {
            bool found = false;
            for(int i=0; i<factorCount; i++)
            {
                if (sameFunctionNodes(rad, factorArray[i].factorNode))
                {
                    factorArray[i].multiplicity += 1;
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                FactorPair fp;
                fp.factorNode = rad;
                fp.multiplicity = 1;
                factorArray[factorCount] = fp;
                factorCount++;
            }
        }
    }
}

Nod* creeazaMultiplicareDinFactori(double combinedConstant, FactorPair factorArray[], int factorCount)
{
    if (factorCount == 0)
    {
        char buffer[50];
        sprintf(buffer, "%g", combinedConstant);
        return creareNod(buffer);
    }

    if (fabs(combinedConstant) < 1e-15)
    {
        return creareNod("0");
    }

    char buffer[50];
    sprintf(buffer, "%g", combinedConstant);
    Nod* current = nullptr;
    double epsilon = 1e-15;
    if (fabs(combinedConstant - 1.0) > epsilon && fabs(combinedConstant + 1.0) > epsilon)
    {
        current = creareNod(buffer);
    }
    else
    {
        if (combinedConstant < -0.9999999 && combinedConstant > -1.0000001)
        {
            current = creareNod("-1");
        }
    }

    for (int i = 0; i < factorCount; i++)
    {
        if (factorArray[i].multiplicity == 1)
        {
            if(!current)
            {
                current = factorArray[i].factorNode;
            }
            else
            {
                Nod* mul = creareNod("*");
                mul->stanga = current;
                mul->dreapta = factorArray[i].factorNode;
                current = mul;
            }
        }
        else
        {
            /// multiplicare
            Nod* powerNode = creareNod("^");
            powerNode->stanga = factorArray[i].factorNode;
            char buffMulti[50];
            sprintf(buffMulti, "%d", factorArray[i].multiplicity);
            powerNode->dreapta = creareNod(buffMulti);

            if(!current)
            {
                current = powerNode;
            }
            else
            {
                Nod* mul = creareNod("*");
                mul->stanga = current;
                mul->dreapta = powerNode;
                current = mul;
            }
        }
    }

    if(!current)
    {
        return creareNod("1");
    }

    return current;
}

Nod* flattenMultiplication(Nod* rad)
{
    double combinedConstant = 1.0;
    FactorPair factorArray[200];
    int factorCount = 0;

    colecteazaFactori(rad, combinedConstant, factorArray, factorCount);
    Nod* newRoot = creeazaMultiplicareDinFactori(combinedConstant, factorArray, factorCount);
    return newRoot;
}

Nod* simplificaArbore(Nod* rad)
{
    if (!rad) return NULL;

    rad->stanga = simplificaArbore(rad->stanga);
    rad->dreapta = simplificaArbore(rad->dreapta);

    if (!esteOperatorElem(rad->info))
        return rad;

    if (strcmp(rad->info,"+")==0)
    {
        /// 0 + x => x
        if (eFrunzaNumar(rad->stanga) && strcmp(rad->stanga->info,"0")==0)
        {
            Nod* r = rad->dreapta;
            free(rad->stanga);
            free(rad);
            return r;
        }
        /// x + 0 => x
        if (eFrunzaNumar(rad->dreapta) && strcmp(rad->dreapta->info,"0")==0)
        {
            Nod* r = rad->stanga;
            free(rad->dreapta);
            free(rad);
            return r;
        }
    }
    else if (strcmp(rad->info,"-")==0)
    {
        /// x - 0 => x
        if (eFrunzaNumar(rad->dreapta) && strcmp(rad->dreapta->info,"0")==0)
        {
            Nod* r = rad->stanga;
            free(rad->dreapta);
            free(rad);
            return r;
        }
        /// 0 - x => -x
        if (eFrunzaNumar(rad->stanga) && strcmp(rad->stanga->info,"0")==0)
        {
            if (eFrunzaNumar(rad->dreapta))
            {
                double val = atof(rad->dreapta->info);
                val = -val;
                char buffer[50];
                sprintf(buffer, "%g", val);
                strcpy(rad->info, buffer);
                free(rad->stanga);
                free(rad->dreapta);
                rad->stanga = NULL;
                rad->dreapta = NULL;
                return rad;
            }
            else
            {
                /// - x => -1*x
                Nod* mulNode = creareNod("*");
                Nod* minusOne = creareNod("-1");
                mulNode->stanga = minusOne;
                mulNode->dreapta = rad->dreapta;

                free(rad->stanga);
                free(rad);
                return simplificaArbore(mulNode);
            }
        }
    }
    else if (strcmp(rad->info,"*")==0)
    {
        /// x*0 => 0
        if (eFrunzaNumar(rad->stanga) && strcmp(rad->stanga->info,"0")==0)
        {
            free(rad->dreapta);
            free(rad->stanga);
            strcpy(rad->info,"0");
            rad->stanga=NULL;
            rad->dreapta=NULL;
            return rad;
        }
        /// 0*x => 0
        if (eFrunzaNumar(rad->dreapta) && strcmp(rad->dreapta->info,"0")==0)
        {
            free(rad->stanga);
            free(rad->dreapta);
            strcpy(rad->info,"0");
            rad->stanga=NULL;
            rad->dreapta=NULL;
            return rad;
        }
        /// x*1 => x
        if (eFrunzaNumar(rad->stanga) && strcmp(rad->stanga->info,"1")==0)
        {
            Nod* r = rad->dreapta;
            free(rad->stanga);
            free(rad);
            return r;
        }
        /// 1*x => x
        if (eFrunzaNumar(rad->dreapta) && strcmp(rad->dreapta->info,"1")==0)
        {
            Nod* r = rad->stanga;
            free(rad->dreapta);
            free(rad);
            return r;
        }
        return flattenMultiplication(rad);
    }
    else if (strcmp(rad->info,"/")==0)
    {
        /// x/1 => x
        if (eFrunzaNumar(rad->dreapta) && strcmp(rad->dreapta->info,"1")==0)
        {
            Nod* r = rad->stanga;
            free(rad->dreapta);
            free(rad);
            return r;
        }
        /// 0/x => 0
        if (eFrunzaNumar(rad->stanga) && strcmp(rad->stanga->info,"0")==0)
        {
            free(rad->dreapta);
            free(rad->stanga);
            strcpy(rad->info,"0");
            rad->stanga = NULL;
            rad->dreapta = NULL;
            return rad;
        }
    }
    else if (strcmp(rad->info,"^")==0)
    {
        /// x^1 => x
        if (eFrunzaNumar(rad->dreapta) && strcmp(rad->dreapta->info,"1")==0)
        {
            Nod* r = rad->stanga;
            free(rad->dreapta);
            free(rad);
            return r;
        }
        /// x^0 => 1
        if (eFrunzaNumar(rad->dreapta) && strcmp(rad->dreapta->info,"0")==0)
        {
            free(rad->stanga);
            free(rad->dreapta);
            strcpy(rad->info,"1");
            rad->stanga=NULL;
            rad->dreapta=NULL;
            return rad;
        }
    }

    return rad;
}

Nod* derivaDeDouaOri(Nod* rad)
{
    Nod* d1 = derivaArbore(rad);
    d1 = simplificaArbore(d1);

    Nod* d2 = derivaArbore(d1);
    d2 = simplificaArbore(d2);

    return d2;
}

int ecranCurent = 0;

struct Buton
{
    int x1,y1,x2,y2;
    char text[100];
    bool hover;
    int culoare;
};


bool clickInButon(Buton b, int mx, int my)
{
    return (mx>=b.x1 && mx<=b.x2 && my>=b.y1 && my<=b.y2);
}

void deseneazaButon(Buton b)
{
    if(!b.hover)
    {
        setfillstyle(SOLID_FILL,b.culoare);
        bar(b.x1, b.y1, b.x2, b.y2);

        setcolor(b.culoare);
        rectangle(b.x1, b.y1, b.x2, b.y2);

        int bw = b.x2 - b.x1;
        int bh = b.y2 - b.y1;
        int tw = textwidth((char*)b.text);
        int th = textheight((char*)b.text);
        int tx = b.x1 + (bw - tw)/2;
        int ty = b.y1 + (bh - th)/2;

        setbkcolor(b.culoare);
        setcolor(WHITE);
        outtextxy(tx, ty, (char*)b.text);

        setbkcolor(WHITE);
    }
    else
    {
        setfillstyle(SOLID_FILL,WHITE);
        bar(b.x1, b.y1, b.x2, b.y2);

        setcolor(b.culoare);
        rectangle(b.x1, b.y1, b.x2, b.y2);

        int bw = b.x2 - b.x1;
        int bh = b.y2 - b.y1;
        int tw = textwidth((char*)b.text);
        int th = textheight((char*)b.text);
        int tx = b.x1 + (bw - tw)/2;
        int ty = b.y1 + (bh - th)/2;

        setbkcolor(WHITE);
        setcolor(b.culoare);
        outtextxy(tx, ty, (char*)b.text);

        setbkcolor(WHITE);

    }
}


void verificaHover(Buton* b, int mx, int my)
{
    bool previousHover = b->hover;
    b->hover = (mx >= b->x1 && mx <= b->x2 && my >= b->y1 && my <= b->y2);


    if (previousHover != b->hover)
    {
        deseneazaButon(*b);
    }
}

struct TextUI
{
    char titlu[100];
    char instructiuni[200];
    char buton_confirm[50];
    char buton_inapoi[50];
    char buton_limba[50];
    char buton_postfix[50];
    char buton_originaltree[50];
    char buton_d1[50];
    char buton_d1tree[50];
    char buton_d2[50];
    char buton_d2tree[50];
    char buton_modifica[50];
    char buton_exit[50];

};
TextUI texte;

const char* limbiDisponibile[]= {"romana","engleza","spaniola","franceza"};
int limbaCurenta = 0;
bool afisareLimbi = false;
int nrlimbi=(int)(sizeof(limbiDisponibile)/sizeof(limbiDisponibile[0]));


bool incarcaTexteDinFisier(const char* numeFisier, TextUI &texte)
{
    ifstream fisier(numeFisier);
    if (!fisier.is_open())
    {
        return false;
    }
    char linie[256];
    while (fisier.getline(linie, sizeof(linie)))
    {
        char* cheie = strtok(linie, "=");
        char* valoare = strtok(NULL, "\n");
        if (strcmp(cheie, "titlu") == 0) strcpy(texte.titlu, valoare);
        else if (strcmp(cheie, "instructiuni") == 0) strcpy(texte.instructiuni, valoare);
        else if (strcmp(cheie, "buton_confirm") == 0) strcpy(texte.buton_confirm, valoare);
        else if (strcmp(cheie, "buton_inapoi") == 0) strcpy(texte.buton_inapoi, valoare);
        else if (strcmp(cheie, "buton_limba") == 0) strcpy(texte.buton_limba, valoare);
        else if (strcmp(cheie, "buton_postfix") == 0) strcpy(texte.buton_postfix, valoare);
        else if (strcmp(cheie, "buton_originaltree") == 0) strcpy(texte.buton_originaltree, valoare);
        else if (strcmp(cheie, "buton_d1") == 0) strcpy(texte.buton_d1, valoare);
        else if (strcmp(cheie, "buton_d1tree") == 0) strcpy(texte.buton_d1tree, valoare);
        else if (strcmp(cheie, "buton_d2") == 0) strcpy(texte.buton_d2, valoare);
        else if (strcmp(cheie, "buton_d2tree") == 0) strcpy(texte.buton_d2tree, valoare);
        else if (strcmp(cheie, "buton_modifica") == 0) strcpy(texte.buton_modifica, valoare);
        else if (strcmp(cheie, "buton_exit") == 0) strcpy(texte.buton_exit, valoare);
    }
    fisier.close();
    return true;
}

int lungimeMaxima = 25;


int calculeazaLungimeMaxima(const char* numeFisier)
{
    ifstream fisier(numeFisier);
    if (!fisier.is_open())
    {
        return -1; /// indică eroare la deschiderea fișierului
    }

    char linie[256];
    lungimeMaxima = 0;

    while (fisier.getline(linie, sizeof(linie)))
    {
        char* cheie = strtok(linie, "=");
        char* valoare = strtok(NULL, "\n");

        if (valoare != nullptr)
        {
            int lungime = strlen(valoare);
            if (lungime > lungimeMaxima)
            {
                lungimeMaxima = lungime;
            }
        }
    }

    fisier.close();
    return lungimeMaxima;
}

int screenWidth, screenHeight;
char functie[256];
bool functieIntrodusa = false;


char postfixArr[100][50];
int m;
Nod* radFunc = NULL;
Nod* d1 = NULL;
Nod* d2 = NULL;
Nod* d1copy = NULL;
Nod* d2Copy = NULL;

Buton butPostfix, butOriginalTree, butD1, butD1Tree, butD2, butD2Tree, butExit, butConfirm, butGoBack, butModifica, butSchimbaLimba;

int inputX1, inputY1, inputX2, inputY2;

int computeNodeTextWidth(Nod* rad)
{
    if (!rad) return 0;
    int tw = textwidth(rad->info);
       return tw + 25;
}

int computeSubtreeWidth(Nod* rad)
{
    if (!rad) return 0;

    int nodeWidth = computeNodeTextWidth(rad);

    if (!rad->stanga && !rad->dreapta)
        return nodeWidth;

    int leftW = computeSubtreeWidth(rad->stanga);
    int rightW = computeSubtreeWidth(rad->dreapta);

    int spacing = 50;
    int totalChildrenWidth = leftW + rightW + spacing;

    if (totalChildrenWidth < nodeWidth)
        totalChildrenWidth = nodeWidth;

    return totalChildrenWidth;
}

void deseneazaArboreProper(Nod* rad, int x, int y, int subtreeWidth)
{
    if (!rad) return;

        int tw = textwidth(rad->info);
    int th = textheight(rad->info);
    int radius = (tw > th ? tw : th)/2 + 10;

    setlinestyle(0,0,2);
    circle(x, y, radius);
    outtextxy(x - tw/2, y - th/2, rad->info);


    if (!rad->stanga && !rad->dreapta) return;


    int leftW = computeSubtreeWidth(rad->stanga);
    int rightW = computeSubtreeWidth(rad->dreapta);

    int spacing = 20;
    int totalChildrenWidth = leftW + rightW + spacing;

    int childY = y + 80;

    int leftX = x - (totalChildrenWidth / 2) + (leftW / 2);

    int rightX = x - (totalChildrenWidth / 2) + leftW + spacing + (rightW / 2);

    if (rad->stanga)
    {
        line(x, y+radius, leftX, childY - radius);
        deseneazaArboreProper(rad->stanga, leftX, childY, leftW);
    }
    if (rad->dreapta)
    {
        line(x, y+radius, rightX, childY - radius);
        deseneazaArboreProper(rad->dreapta, rightX, childY, rightW);
    }
}

void deseneazaArbore(Nod* rad, int startX, int startY)
{
    if (!rad) return;

    int totalW = computeSubtreeWidth(rad);

    deseneazaArboreProper(rad, startX, startY, totalW);
}

void obtineSirPostfix(char out[], char postfix[][50], int m)
{
    out[0] = '\0';
    for (int i=0; i<m; i++)
    {
        strcat(out, postfix[i]);
        strcat(out, " ");
    }
}

void obtineInfixDinArbore(Nod* rad, char out[])
{
    out[0]='\0';
    afiseazaArboreInfixInSir(rad, out);
}


void deseneazaListaLimbi()
{
    cleardevice();
    int startX = screenWidth / 2 - 100;
    int startY = screenHeight/2 -100;
    int butonHeight = 40;
    int spatiu = 10;
    int steagWidth = 30;
    int steagHeight = 20;

    const char* fisiereSteaguri[] = {"ro.bmp", "gb.bmp","es.bmp","fr.bmp"};


    for (int i = 0; i < nrlimbi; i++)
    {
        Buton butonLimba;
        butonLimba.x1 = startX;
        butonLimba.x2 = startX + 200;
        butonLimba.y1 = startY + i * (butonHeight + spatiu);
        butonLimba.y2 = butonLimba.y1 + butonHeight;
        strcpy(butonLimba.text, limbiDisponibile[i]);
        butonLimba.culoare = (i == limbaCurenta) ? GREEN : BLUE;
        butonLimba.hover = false;

        deseneazaButon(butonLimba);

        int steagX = butonLimba.x1 + 5;
        int steagY = butonLimba.y1 + (butonHeight - steagHeight) / 2;
        readimagefile(fisiereSteaguri[i], steagX, steagY, steagX + steagWidth, steagY + steagHeight);


    }
}

void deseneazaEcranPrincipal()
{
    cleardevice();
    setbkcolor(WHITE);
    setcolor(MAGENTA);


    if(!afisareLimbi)
    {
        settextstyle(TRIPLEX_FONT,0,5);

        int tw = textwidth(texte.titlu);
        outtextxy((screenWidth - tw)/2, 50, (char*)texte.titlu);

        settextstyle(TRIPLEX_FONT,0,1);
        outtextxy((screenWidth - textwidth(texte.instructiuni))/2, 110, (char*)texte.instructiuni);

        butSchimbaLimba.culoare = BLUE;
        deseneazaButon(butSchimbaLimba);

        setcolor(MAGENTA);
        rectangle(inputX1, inputY1, inputX2, inputY2);
        outtextxy(inputX1+5, inputY1+5, (char*)functie);

        butConfirm.culoare = LIGHTBLUE;
        deseneazaButon(butConfirm);


        if (functieIntrodusa)
        {
            butPostfix.culoare = MAGENTA;
            butOriginalTree.culoare = MAGENTA;
            butD1.culoare = MAGENTA;
            butD1Tree.culoare = MAGENTA;
            butD2.culoare = MAGENTA;
            butD2Tree.culoare = MAGENTA;
            butExit.culoare = LIGHTBLUE;
            butModifica.culoare = LIGHTBLUE;

            deseneazaButon(butPostfix);
            deseneazaButon(butOriginalTree);
            deseneazaButon(butD1);
            deseneazaButon(butD1Tree);
            deseneazaButon(butD2);
            deseneazaButon(butD2Tree);
            deseneazaButon(butExit);
            deseneazaButon(butModifica);
        }
    }
    else
    {
        cleardevice();
        deseneazaListaLimbi();
    }

}

void citesteFunctieInput()
{
    int len = (int)strlen(functie);
    bool done = false;
    while (!done)
    {
        if (kbhit())
        {
            char c = getch();
            if (c == '\r')
            {
                done = true;
            }
            else if (c == 8)
            {
                if (len>0)
                {
                    len--;
                    functie[len] = '\0';
                }
            }
            else if (c >= 32 && c < 127)
            {
                if (len<255)
                {
                    functie[len] = c;
                    len++;
                    functie[len] = '\0';
                }
            }
            deseneazaEcranPrincipal();
        }
        int mx = mousex();
        int my = mousey();
        verificaHover(&butConfirm, mx, my);
        verificaHover(&butSchimbaLimba, mx, my);

        if (ismouseclick(WM_LBUTTONDOWN))
        {
            int mx, my;
            getmouseclick(WM_LBUTTONDOWN, mx, my);
            if (clickInButon(butConfirm, mx, my))
            {
                done = true;
            }
        }
        delay(50);
    }
}

void afiseazaEcranText(const char *titlu, const char *continut)
{
    cleardevice();
    setbkcolor(WHITE);
    setcolor(MAGENTA);
    settextstyle(TRIPLEX_FONT,0,2);
    int tw = textwidth((char*)titlu);
    outtextxy((screenWidth - tw)/2, 50, (char*)titlu);

    settextstyle(TRIPLEX_FONT,0,2);
    int x = 100;
    int y = 150;
    int maxW = screenWidth - 200;

    char contCopy[2000];
    strcpy(contCopy, continut);

    char *p = strtok(contCopy, " ");
    char line[1024];
    line[0] = '\0';

    while (p != NULL)
    {
        char temp[1024];
        strcpy(temp, line);
        if (strlen(line)>0)
            strcat(temp," ");
        strcat(temp,p);
        if (textwidth((char*)temp)>maxW)
        {
            outtextxy(x, y, (char*)line);
            y += 20;
            strcpy(line, p);
        }
        else
        {
            strcpy(line, temp);
        }
        p = strtok(NULL, " ");
    }

    if (strlen(line)>0)
    {
        outtextxy(x, y, (char*)line);
    }

    settextstyle(TRIPLEX_FONT,0,2);

    butGoBack.culoare = MAGENTA;
    deseneazaButon(butGoBack);
}

void afiseazaEcranArbore(const char *titlu, Nod* rad)
{
    cleardevice();
    setbkcolor(WHITE);
    setcolor(MAGENTA);
    settextstyle(TRIPLEX_FONT,0,2);
    int tw = textwidth((char*)titlu);
    outtextxy((screenWidth - tw)/2, 50, (char*)titlu);

    int startX = screenWidth/2;
    int startY = 300;
    int dist = 370;
    settextstyle(SANS_SERIF_FONT,0,1);
    setcolor(MAGENTA);
    deseneazaArbore(rad, startX, startY);

    settextstyle(TRIPLEX_FONT,0,2);
    butGoBack.culoare = MAGENTA;
    deseneazaButon(butGoBack);
}

void initializeazaUI()
{
    inputX1 = screenWidth/2 - 300;
    inputX2 = screenWidth/2 + 300;
    inputY1 = 150;
    inputY2 = 180;

    butConfirm.x1 = screenWidth/2 - 70;
    butConfirm.x2 = screenWidth/2 + 70;
    butConfirm.y1 = 190;
    butConfirm.y2 = 220;
    strcpy(butConfirm.text,(char*)texte.buton_confirm);

    int bx1 = screenWidth/2 - 170 -lungimeMaxima/2;
    int bx2 = screenWidth/2 + 170 + lungimeMaxima/2;
    int by = 250;
    int bh = 40;
    int sp = 30;

    butSchimbaLimba.x1 = screenWidth / 8 - textwidth(texte.buton_limba)/2-5;
    butSchimbaLimba.x2 = screenWidth / 8 + textwidth(texte.buton_limba)/2+5;
    butSchimbaLimba.y1 = 7*screenHeight/8 - 25;
    butSchimbaLimba.y2 = 7*screenHeight/8 + 25;
    strcpy(butSchimbaLimba.text, (char*)texte.buton_limba);
    butSchimbaLimba.culoare = BLUE;
    butSchimbaLimba.hover = false;

    butPostfix.x1 = bx1;
    butPostfix.y1 = by;
    butPostfix.x2 = bx2;
    butPostfix.y2 = by+bh;
    strcpy(butPostfix.text,(char*)texte.buton_postfix);

    by += bh+sp;
    butOriginalTree.x1 = bx1;
    butOriginalTree.y1 = by;
    butOriginalTree.x2 = bx2;
    butOriginalTree.y2 = by+bh;
    strcpy(butOriginalTree.text,(char*)texte.buton_originaltree);

    by += bh+sp;
    butD1.x1 = bx1;
    butD1.y1 = by;
    butD1.x2 = bx2;
    butD1.y2 = by+bh;
    strcpy(butD1.text,(char*)texte.buton_d1);

    by += bh+sp;
    butD1Tree.x1 = bx1;
    butD1Tree.y1 = by;
    butD1Tree.x2 = bx2;
    butD1Tree.y2 = by+bh;
    strcpy(butD1Tree.text,(char*)texte.buton_d1tree);

    by += bh+sp;
    butD2.x1 = bx1;
    butD2.y1 = by;
    butD2.x2 = bx2;
    butD2.y2 = by+bh;
    strcpy(butD2.text,(char*)texte.buton_d2);

    by += bh+sp;
    butD2Tree.x1 = bx1;
    butD2Tree.y1 = by;
    butD2Tree.x2 = bx2;
    butD2Tree.y2 = by+bh;
    strcpy(butD2Tree.text,(char*)texte.buton_d2tree);

    by += bh+sp;
    butModifica.x1 = bx1;
    butModifica.y1 = by;
    butModifica.x2 = bx2;
    butModifica.y2 = by+bh;
    strcpy(butModifica.text,(char*)texte.buton_modifica);

    by += bh+sp;
    butExit.x1 = bx1;
    butExit.y1 = by;
    butExit.x2 = bx2;
    butExit.y2 = by+bh;
    strcpy(butExit.text,(char*)texte.buton_exit);

    butGoBack.x1 = 50;
    butGoBack.y1 = 50;
    butGoBack.x2 = 180;
    butGoBack.y2 = 80;
    strcpy(butGoBack.text,(char*)texte.buton_inapoi);
}

void afiseazaEcran()
{
    if (ecranCurent==0)
    {
        deseneazaEcranPrincipal();
    }
    else if (ecranCurent==1)
    {
        char rez[2000];
        obtineSirPostfix(rez, postfixArr, m);
        afiseazaEcranText(texte.buton_postfix, rez);
    }
    else if (ecranCurent==2)
    {
        afiseazaEcranArbore(texte.buton_originaltree, radFunc);
    }
    else if (ecranCurent==3)
    {
        char rez[2000];
        obtineInfixDinArbore(d1, rez);
        afiseazaEcranText(texte.buton_d1, rez);
    }
    else if (ecranCurent==4)
    {
        afiseazaEcranArbore(texte.buton_d1tree, d1);
    }
    else if (ecranCurent==5)
    {
        char rez[2000];
        obtineInfixDinArbore(d2, rez);
        afiseazaEcranText(texte.buton_d2, rez);
    }
    else if (ecranCurent==6)
    {
        afiseazaEcranArbore(texte.buton_d2tree, d2);
    }
}

void proceseazaClick(int mx, int my)
{
    if (ecranCurent==0)
    {
        if(afisareLimbi)
        {
            int startX = screenWidth/2-70;
            int startY = screenHeight/2-100;
            int bh = 40;
            int sp = 10;

            for(int i=0; i< nrlimbi; i++)
            {
                int by1 = startY+i*(bh+sp);
                int by2 = by1 +bh;

                if (mx >= startX-140 && mx <= startX + 180 && my >= by1 && my <= by2)
                {
                    limbaCurenta = i;
                    incarcaTexteDinFisier((string(limbiDisponibile[i]) + ".txt").c_str(), texte);
                    initializeazaUI();
                    afisareLimbi = false;
                    cleardevice();
                    deseneazaEcranPrincipal();
                    break;
                }
            }
        }
        else
        {

            if(clickInButon(butSchimbaLimba,mx,my))
            {
                afisareLimbi = true;
                cleardevice();
                deseneazaListaLimbi();
            }

            if (clickInButon(butConfirm, mx, my))
            {
                /// postfix
                char elems[100][50];
                int n = element(functie, elems);
                infixInPostfix(elems, n, postfixArr, m);

                /// arborele de sintaxa
                radFunc = construireArboreDinPostfix(postfixArr, m);

                /// simplifica arborele
                radFunc = simplificaArbore(radFunc);

                /// derivata intai
                d1 = derivaArbore(radFunc);
                d1 = simplificaArbore(d1);

                /// derivata a doua
                d2 = derivaArbore(d1);
                d2 = simplificaArbore(d2);

                functieIntrodusa = true;
                deseneazaEcranPrincipal();
            }
            else if (functieIntrodusa)
            {
                if (clickInButon(butPostfix, mx, my))
                {
                    ecranCurent=1;
                    afiseazaEcran();
                }
                else if (clickInButon(butOriginalTree, mx, my))
                {
                    ecranCurent=2;
                    afiseazaEcran();
                }
                else if (clickInButon(butD1, mx, my))
                {
                    ecranCurent=3;
                    afiseazaEcran();
                }
                else if (clickInButon(butD1Tree, mx, my))
                {
                    ecranCurent=4;
                    afiseazaEcran();
                }
                else if (clickInButon(butD2, mx, my))
                {
                    ecranCurent=5;
                    afiseazaEcran();
                }
                else if (clickInButon(butD2Tree, mx, my))
                {
                    ecranCurent=6;
                    afiseazaEcran();
                }
                else if (clickInButon(butExit, mx, my))
                {
                    closegraph();
                    exit(0);
                }
                else if (clickInButon(butModifica, mx, my))
                {
                    // Let the user modify
                    functie[0] = '\0';
                    functieIntrodusa = false;
                    radFunc = NULL;
                    d1 = NULL;
                    d2 = NULL;
                    deseneazaEcranPrincipal();

                    citesteFunctieInput();

                    if (!functieIntrodusa && strlen(functie)>0)
                    {
                        char elems[100][50];
                        int n = element(functie, elems);
                        infixInPostfix(elems, n, postfixArr, m);
                        radFunc = construireArboreDinPostfix(postfixArr, m);

                        radFunc = simplificaArbore(radFunc);

                        d1 = derivaArbore(radFunc);
                        d1 = simplificaArbore(d1);
                        d2 = derivaArbore(d1);
                        d2 = simplificaArbore(d2);

                        functieIntrodusa = true;
                    }
                    deseneazaEcranPrincipal();
                }
            }
        }
    }
    else
    {
        if (clickInButon(butGoBack, mx, my))
        {
            ecranCurent=0;
            deseneazaEcranPrincipal();
        }
    }
}

int main()
{
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);
    initwindow(screenWidth,screenHeight,"BGI*");

    if (!incarcaTexteDinFisier("romana.txt", texte))
    {
        cout << "Eroare la incarcarea textelor!" << endl;
        return 1;
    }

    setbkcolor(WHITE);
    cleardevice();
    setcolor(MAGENTA);
    settextstyle(TRIPLEX_FONT,0,1);

    functie[0]='\0';
    functieIntrodusa = false;

    initializeazaUI();
    deseneazaEcranPrincipal();

    citesteFunctieInput();

    if (!functieIntrodusa && strlen(functie)>0)
    {
        char elems[100][50];
        int n = element(functie, elems);
        infixInPostfix(elems, n, postfixArr, m);
        radFunc = construireArboreDinPostfix(postfixArr, m);
        radFunc = simplificaArbore(radFunc);

        d1 = derivaArbore(radFunc);
        d1copy = copiazaArbore(d1);
        d1 = simplificaArbore(d1);

        d2 = derivaArbore(d1);
        d2 = simplificaArbore(d2);

        functieIntrodusa = true;
    }

    deseneazaEcranPrincipal();

    while (true)
    {
        int mx = mousex();
        int my = mousey();

        if(ecranCurent == 0 && afisareLimbi==false)
        {

            verificaHover(&butConfirm, mx, my);
            verificaHover(&butPostfix, mx, my);
            verificaHover(&butOriginalTree, mx, my);
            verificaHover(&butD1, mx, my);
            verificaHover(&butD1Tree, mx, my);
            verificaHover(&butD2, mx, my);
            verificaHover(&butD2Tree, mx, my);
            verificaHover(&butExit, mx, my);
            verificaHover(&butModifica, mx, my);
            verificaHover(&butSchimbaLimba, mx, my);
        }

        else if (afisareLimbi==false)
        {
            verificaHover(&butGoBack, mx, my);
        }

        if (ismouseclick(WM_LBUTTONDOWN))
        {
            getmouseclick(WM_LBUTTONDOWN, mx, my);
            proceseazaClick(mx, my);
        }
        delay(10);
    }

    getch();
    closegraph();
    return 0;
}
