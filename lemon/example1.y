%include {
#include <stdlib.h>
#include <assert.h>
#include <iostream>
using namespace std;
}

%token_type { double }
%left PLUS MINUS.
%left DIVIDE TIMES.

%syntax_error {
    cout << "Syntax error!\n" << endl;
    exit(1);
}

program ::= expr(A). {cout << "Result = " << A << "\n" << endl;}

expr(A) ::= expr(B) MINUS expr(C).      { A = B - C; }
expr(A) ::= expr(B) PLUS expr(C).      { A = B + C;}
expr(A) ::= expr(B) TIMES expr(C).      { A = B * C; }
expr(A) ::= expr(B) DIVIDE expr(C).      { 
        if(C != 0) {
            A = B / C;
        } else {
            std::cout << "divide by zero" << std::endl;
        }
}

expr(A) ::= INTEGER(B).     { A = B; }


%code {
int main() {
    void* pParser = ParseAlloc(malloc);
    Parse(pParser, INTEGER, 50.5);
    Parse(pParser, TIMES, 0);
    Parse(pParser, INTEGER, 125.5);
    Parse(pParser, PLUS, 0);
    Parse(pParser, INTEGER, 125.5);
    Parse(pParser, 0, 0);
    ParseFree(pParser, free);
}
}


