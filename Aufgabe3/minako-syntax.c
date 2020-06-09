#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minako.h"

#define BUFFERSIZE 64

void die(char* msg) {
   printf("%s\n", msg);
   exit(-1);
}

//append b to a
char* append(char* a, char* b) {
   a = realloc(a, strlen(a) + 1 + strlen(b));
   if(!a)
      die("Couldn't allocate memory for input string");
   return strcat(a, b);
}

char* inputString;
int readChars = 0;

void callLexer() {
   int token;
   inputString = malloc(1);

   while ((token = yylex()) != EOF)
	{
      char buf[BUFFERSIZE];
      //clear buffer
      memset(buf, '\0', BUFFERSIZE);
      //append as char
		if (token <= 255) {
			sprintf(buf, "%c", token);
         inputString = append(inputString, buf);
      }
      //append as token
		else {
			sprintf(buf, "%d", token);
         inputString = append(inputString, buf);
      }
	}
   //append "EOF token"
   inputString = append(inputString, "eof");
}

//check if the next sequence of chars in the input String equals "toCheck"
//returns 1 if the sequence matches, 0 if it doesn't
int check(const char* toCheck) {
   printf("checking %s\n", toCheck);
   return (strncmp(toCheck, inputString, strlen(toCheck))==0);
}

int checkInt(const int intToCheck) {
   char toCheck[16];
   sprintf(toCheck, "%d", intToCheck);
   return check(toCheck);
}

//terminate the programm with an error if the next sequence of chars does not equal "toEat", otherwise move the pointer on the input String
void eat(char* toEat) {
   if(!check(toEat)) {
      printf("token expected: %s after %d chars read\n", toEat, readChars);
      die("Parser error");
   }
   inputString += strlen(toEat);
   readChars += strlen(toEat);
}

void eatInt(const int intToEat) {
   char toEat[16];
   sprintf(toEat, "%d", intToEat);
   eat(toEat);
}

// --- rules --- //

typedef void metasym_f(void);
metasym_f P, FD, FD2, FD3, FD4, Id, FC, SL, B, S, IF, R, A1, PR, TY, SA, A, E, E1, SE, SE1, SE2, SE3, SE4, T, T1, T2, T3, F;

void P() {
   FD(), eat("eof");
}

//check if the next symbol is a type
int checkType() {
   return (checkInt(KW_BOOLEAN)||checkInt(KW_INT)||checkInt(KW_FLOAT)||checkInt(KW_VOID));
}

void FD() {
   while(checkType()) {
      TY(), eatInt(ID), eat("("), FD2(), eat(")"), FD3(), SL(), FD4();
   }
}

void FD2() {
   if(checkInt(ID)) {
      Id();
   }
}

void Id() {
   eatInt(ID);
   char idComma[16];
   sprintf(idComma, ",%d", ID);
   //get additional IDs
   while(check(idComma)) {
      eat(idComma);
   }
}

void FD3() {
   if(check("{"))
      eat("{");
   else
      eatInt(KW_BEGIN);
}

void FD4() {
   if(check("}"))
      eat("}");
   else
      eatInt(KW_END);
}

//check if next symbol is in first-set of assignment
int checkAssignment() {
   return (checkInt(ID)||checkInt(KW_BOOLEAN)||checkInt(KW_INT)||checkInt(KW_FLOAT)||check("\"-\""));
}

void FC() {
   eatInt(ID), eat("(");
   //if there is at least one arg
   if(checkAssignment()) {
      A();
   }
   //handle more args, if there are more
   while(check(",")) {
      eat(","), A();
   }
   eat(")");
}

//check if next symbol is in first-set of block
int checkBlock() {
   return (checkInt(ID)||checkInt(KW_BOOLEAN)||checkInt(KW_INT)||checkInt(KW_FLOAT)||check("-")||check("("));
}

void SL() {
   while(checkBlock()) {
      B();
   }
}

void B() {
   if(check("{"))
      eat("{");
   else if(checkInt(KW_BEGIN))
      eatInt(KW_BEGIN);
   //must be statement, execute only that and no furthe rules from here
   else {
      S();
      return;
   }
   SL();
   if(check("}"))
      eat("}");
   else
      eatInt(KW_END);
}

void S() {
   //both function call and asignment start with an Id, check one step further
   char idAsign[16];
   sprintf(idAsign, "%d=", ID);
   //if statement
   if(checkInt(KW_IF))
      IF();
   //return
   else if(checkInt(KW_RETURN))
      R(), eat(";");
   //printf
   else if(checkInt(KW_PRINTF))
      PR(), eat(";");
   //statassignment
   else if(check(idAsign))
      SA(), eat(";");
   //fuction call
   else
      FC(), eat(";");
}

void TY() {}
void A() {}

//check if the grammar of the input string is valid
void parseInput() {
   printf("%s\n", inputString);
   P();
}

int main(int argc, char* argv[]) {
   //file input
   if(argc>1) {
      yyin = fopen(argv[1], "r");
      if(!yyin)
         die("Couldn't open file");
   }
   //stdin
   else {
      yyin = stdin;
   }

   callLexer();
   parseInput();

   return 0;
}
