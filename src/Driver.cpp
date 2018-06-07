#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <list>

using namespace std;


#include "AstStructure.hpp"

extern FILE *yyin;
extern int yyparse();

extern int yylineno;
extern char* yytext;


char* sourcefile;
int numerrors = 0;

MainClass* AST;
using namespace llvm;

Value *ErrorV(const char *Str) { cout<<Str<<endl; return 0; }

int main(int argc, char* argv[])
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    exit(0);
  }

  if (strcmp(argv[1], "-") == 0) {
    // take input from stdin
    yyin = stdin;
    sourcefile = (char *)malloc(strlen("stdin")+1);
    strcpy(sourcefile, "stdin");    
  } 
    else if ((yyin = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Error in opening file %s\n", argv[1]);
    exit(0);
  } //file input
  else {
    sourcefile = (char *)malloc(strlen(argv[1])+1);
    strcpy(sourcefile, argv[1]);
  }
  if (yyparse() || (numerrors > 0)) {
    printf("%d %s contains one or more syntax errors.\n",numerrors, argv[1]);
  }
  else {
    AST->print();
    AST->Codegen();
  }
}
