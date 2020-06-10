#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minako.h"

#define BUFFERSIZE 64

int Debug = 0;

void die(char* msg) {
   printf("%s\n", msg);
   exit(-1);
}

void debug(char* msg, const char* arg) {
   if(Debug)
      printf("%s%s\n", msg, arg);
}

//append b to a
char* append(char* a, char* b) {
   a = realloc(a, strlen(a) + 1 + strlen(b));
   if(!a)
      die("Couldn't allocate memory for input string");
   return strcat(a, b);
}

char* inputString;
int numberCharsRead = 0;

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
   //debug("checking ", toCheck);
   return (strncmp(toCheck, inputString, strlen(toCheck))==0);
}

int checkInt(const int intToCheck) {
   char toCheck[16];
   memset(toCheck, '\0', 16);
   sprintf(toCheck, "%d", intToCheck);
   return check(toCheck);
}

//terminate the programm with an error if the next sequence of chars does not equal "toEat", otherwise move the pointer on the input String
void eat(char* toEat) {
   if(!check(toEat)) {
      char badToken[strlen(toEat)];
      strncpy(badToken, inputString, strlen(toEat));
      badToken[strlen(toEat)] = '\0';
      printf("token expected: \"%s\", read this instead: \"%s\", %d chars read\n", toEat, badToken, numberCharsRead);
      die("Parser error");
   }
   inputString += strlen(toEat);
   numberCharsRead += strlen(toEat);
   debug("ate ", toEat);
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
   debug("starting ", "P");
   FD(), eat("eof");
}

//check if the next symbol is a type
int checkType() {
   return (checkInt(KW_BOOLEAN)||checkInt(KW_INT)||checkInt(KW_FLOAT)||checkInt(KW_VOID));
}

void FD() {
   debug("starting ", "FD");
   while(checkType()) {
      TY(), eatInt(ID), eat("("), FD2(), eat(")"), FD3(), SL(), FD4();
   }
   debug("completed ", "FD");
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
   return (checkInt(ID)||checkInt(CONST_BOOLEAN)||checkInt(CONST_INT)||checkInt(CONST_FLOAT)||check("-"));
}

void FC() {
   debug("starting ", "FC");
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
   debug("completed ", "FC");
}

//check if next symbol is in first-set of block
int checkBlock() {
   return (checkInt(ID)||checkInt(KW_IF)||checkInt(KW_RETURN)||checkInt(KW_PRINTF)||checkInt(KW_BEGIN)||check("{"));
}

void SL() {
   debug("starting ", "SL");
   while(checkBlock()) {
      B();
   }
   debug("completed ", "SL");
}

void B() {
   debug("starting ", "B");
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
   debug("completed ", "B");
}

void S() {
   debug("starting ", "S");
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
   debug("completed ", "S");
}

void IF() {
   debug("starting ", "IF");
   eatInt(KW_IF), eat("("), A(), eat(")"), B();
   debug("completed ", "IF");
}

void R() {
   debug("starting ", "R");
   eatInt(KW_RETURN);
   //assignment after return statement is optional
   if(checkAssignment())
      A();
   debug("completed ", "R");
}

void PR() {
   debug("starting ", "PR");
   eatInt(KW_PRINTF), eat("("), A(), eat(")");
   debug("completed ", "PR");
}

void TY() {
   debug("starting ", "TY");
   if(checkInt(KW_BOOLEAN))
      eatInt(KW_BOOLEAN);
   else if(checkInt(KW_INT))
      eatInt(KW_INT);
   else if(checkInt(KW_FLOAT))
      eatInt(KW_FLOAT);
   else
      eatInt(KW_VOID);
   debug("completed ", "TY");
}

void SA() {
   debug("starting ", "SA");
   eatInt(ID), eat("="), A();
   debug("completed ", "SA");
}

void A() {
   debug("starting ", "A");
   //recursivly call A for further assignements
   char idAsign[5];
   sprintf(idAsign, "%d=", ID);
   if(check(idAsign)) {
      eatInt(ID), eat("="), A();
   }
   else {
      E();
   }
   debug("completed ", "A");
}

void E() {
   debug("starting ", "E");
   SE();
   if(checkInt(EQ)) {
      eatInt(EQ), SE();
   }
   else if(checkInt(NEQ)) {
      eatInt(NEQ), SE();
   }
   else if(checkInt(LEQ)) {
      eatInt(LEQ), SE();
   }
   else if(checkInt(GEQ)) {
      eatInt(GEQ), SE();
   }
   else if(checkInt(LSS)) {
      eatInt(LSS), SE();
   }
   else if(checkInt(GRT)) {
      eatInt(GRT), SE();
   }
   //nothing also possible
   debug("completed ", "E");
}

void SE() {
   debug("starting ", "SE");
   if(check("-")) {
      eat("-");
   }
   T();
   while(1) {
      if(check("+")) {
         eat("+"), T();
      }
      else if(check("-")) {
         eat("-"), T();
      }
      else if(checkInt(OR)) {
         eatInt(OR), T();
      }
      else {
         //no further terms
         break;
      }
   }
   debug("completed ", "SE");
}

void T() {
   debug("starting ", "T");
   F();
   while(1) {
      if(check("*")) {
         eat("*"), F();
      }
      else if(check("/")) {
         eat("/"), F();
      }
      else if(checkInt(AND)) {
         eatInt(AND), F();
      }
      else {
         //no further factors
         break;
      }
   }
   debug("completed ", "T");
}

void F() {
   debug("starting ", "F");
   char fc[5];
   sprintf(fc, "%d(", ID);

   if(checkInt(CONST_BOOLEAN))
      eatInt(CONST_BOOLEAN);
   else if(checkInt(CONST_INT))
      eatInt(CONST_INT);
   else if(checkInt(CONST_FLOAT))
      eatInt(CONST_FLOAT);
   else if(check(fc)) {
      FC();
   }
   else if(checkInt(ID))
      eatInt(ID);
   else {
      eat("("), A(), eat(")");
   }
   debug("completed ", "F");
}

//check if the grammar of the input string is valid
void parseInput() {
   debug("input: ", inputString);
   P();
   printf("Parsing complete, no errors detected.\n");
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
