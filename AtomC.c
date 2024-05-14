#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

int line = 1;;
char input[50001] = "123";

const char *pCrtCh = input;
enum { ID, BREAK, CHAR, DOUBLE, ELSE, FOR, IF, INT, RETURN, STRUCT, VOID, WHILE, CT_INT, CT_REAL, CT_CHAR, CT_STRING, COMMA, SEMICOLON, LPAR, RPAR, LBRACKET, RBRACKET, LACC, RACC, ADD, SUB, MUL, DIV, DOT, AND, OR, NOT, ASSIGN, EQUAL, NOTEQ, LESS, LESSEQ, GREATER, GREATEREQ, SPACE, LINECOMMENT, COMMENT, END };
char *tokenName[] = { "ID", "BREAK", "CHAR", "DOUBLE", "ELSE", "FOR", "IF", "INT", "RETURN", "STRUCT", "VOID", "WHILE", "CT_INT", "CT_REAL", "CT_CHAR", "CT_STRING", "COMMA", "SEMICOLON", "LPAR", "RPAR", "LBRACKET", "RBRACKET", "LACC", "RACC", "ADD", "SUB", "MUL", "DIV", "DOT", "AND", "OR", "NOT", "ASSIGN", "EQUAL", "NOTEQ", "LESS", "LESSEQ", "GREATER", "GREATEREQ", "SPACE", "LINECOMMENT", "COMMENT", "END" };
void err(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);
    va_end(va);
    exit(-1);
}
#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)err("not enough memory");

// Token structure
typedef struct _Token {
    int code;               // Token code
    union {
        char *text;         // For ID, CT_STRING (dynamically allocated)
        long int i;         // For CT_INT, CT_CHAR
        double r;           // For CT_REAL
    };
    int line;               // Input file line
    struct _Token *next;    // Pointer to the next token
} Token;

Token *lastToken, *tokens;
Token *crtTk, *consumedTk;
Token *startTk;
// Function to add a new token
Token *addTk(int code)
{
 Token *tk;
    SAFEALLOC(tk,Token);
    tk->code=code;
    tk->line=line;
    tk->next=NULL;
        if(lastToken){
            lastToken->next=tk;
    }else{
        tokens=tk;
}
    lastToken=tk;
    return tk;
}

// Token-related error handling function
void tkerr(const Token *tk, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error in line %d: ", tk->line);
    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);
    va_end(va);
    exit(-1);
}

char* createString(const char* start, const char* stop) {
    int n = stop - start;
    char* buffer = (char*)malloc(sizeof(char) * (n + 1));
    if (buffer == NULL) {
        printf("Eroare alocare\n");
        exit(1);
    }

    memcpy(buffer, start, n);
    buffer[n] = '\0';
    return buffer;
}

// Function to tokenize input
int getNextToken() {
    int state = 0;
    int nCh;
    char ch;
    char* aux;
    const char *pStartCh;
    Token *tk;

    while (1) {
        ch = *pCrtCh;
        switch (state) {
            case 0:
               if (isdigit(ch) && ch == '0') {//0
                pStartCh = pCrtCh; // memoreaza inceputul numarului
                pCrtCh++;
                state = 1;
                }
                else if (isdigit(ch) && (ch >= '1' && ch <= '9')) {//1-9
                pStartCh = pCrtCh; // memoreaza inceputul numarului
                pCrtCh++;
                state = 3;
                }
                else if (ch == '\'') {//'
                pStartCh = pCrtCh; // memoreaza inceputul CT_CHAR
                pCrtCh++;
                state = 14;
                }
                else if (ch == '"') {//"
                pStartCh = pCrtCh; // memoreaza inceputul CT_STRING
                pCrtCh++;
                state = 19;
                }
                else if (isalpha(ch) || ch == '_') {//inceput ID
                pStartCh = pCrtCh; // memoreaza inceputul ID-ului
                pCrtCh++; // consuma caracterul
                state = 24; // trece la noua stare
                }
                else if (ch == ',') {//inceput COMMA
                pCrtCh++;
                state = 26;
                }
                else if (ch == ';') {//inceput SEMICOLON
                pCrtCh++;
                state = 27;
                }
                else if (ch == '(') {//inceput LPAR
                pCrtCh++;
                state = 28;
                }
                else if (ch == ')') {//inceput RPAR
                pCrtCh++;
                state = 29;
                }
                else if (ch == '[') {//inceput LBRACKET
                pCrtCh++;
                state = 30;
                }
                else if (ch == ']') {//inceput RBRACKET
                pCrtCh++;
                state = 31;
                }
                else if (ch == '{') {//inceput LACC
                pCrtCh++;
                state = 32;
                }
                else if (ch == '}') {//inceput RACC
                pCrtCh++;
                state = 33;
                }
                else if (ch == '/') {//inceput DIV sau COMMENT
                pCrtCh++;
                state = 36;
                }
                else if (ch == ' ' || ch == '\r' || ch == '\t') {
                pCrtCh++; // consuma caracterul si ramane in starea 0
                state = 0;
                }
                else if (ch == '\n') { // tratat separat pentru a actualiza linia curenta
                line++;
                pCrtCh++;
                state = 0;
                }
                else if (ch == '+') {//inceput ADD
                pCrtCh++;
                state = 39;
                }
                else if (ch == '-') {//inceput SUB
                pCrtCh++;
                state = 40;
                }
                else if (ch == '*') {//inceput MUL
                pCrtCh++;
                state = 41;
                }
                else if (ch == '.') {//inceput DOT
                pCrtCh++;
                state = 42;
                }
                else if (ch == '<') {//inceput LESS sau LESSEQ
                pCrtCh++;
                state = 43;
                }
                else if (ch == '>') {//inceput GREATER sau GREATEREQ
                pCrtCh++;
                state = 46;
                }
                else if (ch == '&') {//inceput AND
                pCrtCh++;
                state = 49;
                }
                else if (ch == '|') {//inceput OR
                pCrtCh++;
                state = 50;
                }
                else if (ch == '!') {//inceput NOT sau NOTEQ
                pCrtCh++;
                state = 51;
                }
                else if (ch == '=') {//inceput ASSIGN sau EQUAL
                pCrtCh++;
                state = 54;
                }
                else if (ch == 0) { // sfarsit de sir
                addTk(END);
                return END;
                }
                else
                tkerr(addTk(END), "caracter invalid");
                break;
            case 1:
                if (ch == 'x') {
                pCrtCh++;
                state = 4;
                }
                else if(isdigit(ch) && (ch == '8' || ch == '9')){
                pCrtCh++;
                state = 7;
                }
                else
                state = 5;
                break;
            case 2:
            tk = addTk(CT_INT);
            aux = createString(pStartCh, pCrtCh);
            tk->i = atoi(aux);
            free(aux);
            return CT_INT;
                break;
            case 3:
                if (isdigit(ch)) {
                pCrtCh++;
                state = 3;
                }
                else if(ch == '.'){
                pCrtCh++;
                state = 8;
                }
                else if(ch == 'E' || ch == 'e'){
                pCrtCh++;
                state = 10;
                }
                else
                state = 2;
                break;
            case 4:
                if (isdigit(ch) || (ch >='a' && ch<='f') || (ch >='A' && ch<='F')) {//0-9a-fA-F
                pCrtCh++;
                state = 6;
                }
                break;
                
            case 5:
                if (isdigit(ch) && ch <= '7') {//0-7
                pCrtCh++;
                state = 5;
                }else if(ch == '.'){
                pCrtCh++;
                state = 8;
                }else if(ch == 'E' || ch == 'e'){
                pCrtCh++;
                state = 10;
                }
                else
                 state = 2;
                break;
            case 6:
                if(isdigit(ch) || (isalpha(ch) && ((ch >= 'a' && ch <='f') || (ch >= 'A' && ch <='F')) )){
                pCrtCh++;
                state = 6;
                }
                else 
                state = 2;
                break;
            case 7:
                if(ch == '.'){
                pCrtCh++;
                state = 8;
                }else if(ch == 'E' || ch == 'e'){
                pCrtCh++;
                state = 10;
                }
                break;
            case 8:
                if (isdigit(ch)) {
                pCrtCh++;
                state = 9;
                }
                break;
            case 9:
            if (isdigit(ch)) {
                pCrtCh++;
                state = 9;
                }else if (ch == 'e' || ch == 'E') {
                pCrtCh++;
                state = 10;
                }
                else 
                state = 13;
                break;
            case 10:
                if (ch == '+' || ch == '-') {
                pCrtCh++;
                state = 11;
                }
                else
                state = 12;
                break;
            case 11:
                if (isdigit(ch)) {
                pCrtCh++;
                state = 12;
                }
                break;
            case 12:
            if (isdigit(ch)) {
                pCrtCh++;
                state = 12;
                }
                else
                state = 13; 
                break;
            case 13:
            tk = addTk(CT_REAL);
            aux = createString(pStartCh, pCrtCh);
            tk->r = atof(aux);
            free(aux);
            return CT_REAL;
                break;
            case 14:
            if (ch == '\\') {
                pCrtCh++;
                state = 15;
                }
                else if (ch != '\'\\') {//[^'\\]
                pCrtCh++;
                state = 16;
                }
                break;
            case 15://testarea tranzitii posibile pentru starea 15
            if (ch == 'a' || ch == 'b' || ch == 'f' || ch == 'n' || ch == 'r' || ch == 't' || ch == 'v' || ch == '\'' || ch == '?' || ch == '"' || ch == '\0' || ch == '\\') {//abfnrtv'?"\\0
                pCrtCh++;
                state = 17;
                }
                break;
            case 16://testarea tranzitii posibile pentru starea 16
            if (ch == '\'') {
                pCrtCh++;
                state = 18;
                }
                break;
            case 17://testarea tranzitii posibile pentru starea 17
            if (ch == '\'') {
                pCrtCh++;
                state = 18;
                }
                break;
            case 18://starea finala CT_CHAR
            aux = createString(pStartCh, pCrtCh);
            tk = addTk(CT_CHAR);
            tk->i = aux[1];
            free(aux);
            return CT_CHAR;
            break;
            case 19://testarea tranzitii posibile pentru starea 19
            if (ch == '\\') {
                pCrtCh++;
                state = 22;
                }
                else if (ch != '\"\\') {//[^"\\]
                pCrtCh++;
                state = 20;
                }
                break;
            case 20://testarea tranzitii posibile pentru starea 20
            if (ch == '"') {//"
                pCrtCh++;
                state = 21;
                }
                else
                state = 19;
                break;
            case 21:
            tk = addTk(CT_STRING);
            tk->text = createString(pStartCh, pCrtCh);
            return CT_STRING;
                break;
            case 22://testarea tranzitii posibile pentru starea 22
            if (ch == 'a' || ch == 'b' || ch == 'f' || ch == 'n' || ch == 'r' || ch == 't' || ch == 'v' || ch == '\'' || ch == '?' || ch == '"' || ch == '\\0') {//abfnrtv'?"\\0
                pCrtCh++;
                state = 23;
                }
                break;
            case 23://testarea tranzitii posibile pentru starea 23
            if (ch == '"') {//"
                pCrtCh++;
                state = 21;
                }
                else
                state = 19;
                break;
            case 24: //continuare ID
            if (isalnum(ch) || ch == '_')
                pCrtCh++;
                else
                state = 25;
                break;
            case 25:
                nCh = pCrtCh - pStartCh; // lungimea cuvantului gasit
                // teste cuvinte cheie
                if (nCh == 5 && !memcmp(pStartCh, "break", 5))tk = addTk(BREAK);
                else if (nCh == 4 && !memcmp(pStartCh, "char", 4))tk = addTk(CHAR);
                else if (nCh == 6 && !memcmp(pStartCh, "double", 6))tk = addTk(DOUBLE);
                else if (nCh == 4 && !memcmp(pStartCh, "else", 4))tk = addTk(ELSE);
                else if (nCh == 3 && !memcmp(pStartCh, "for", 3))tk = addTk(FOR);
                else if (nCh == 2 && !memcmp(pStartCh, "if", 2))tk = addTk(IF);
                else if (nCh == 3 && !memcmp(pStartCh, "int", 3))tk = addTk(INT);
                else if (nCh == 6 && !memcmp(pStartCh, "return", 6))tk = addTk(RETURN);
                else if (nCh == 6 && !memcmp(pStartCh, "struct", 6))tk = addTk(STRUCT);
                else if (nCh == 4 && !memcmp(pStartCh, "void", 4))tk = addTk(VOID);
                else if (nCh == 5 && !memcmp(pStartCh, "while", 5))tk = addTk(WHILE);
                else { // daca nu este un cuvant cheie, atunci e un ID
                //printf("ID gasit\n");
                tk = addTk(ID);
                tk->text = createString(pStartCh, pCrtCh);
                }
                return tk->code;
                break;
            case 26://starea finala COMMA
                addTk(COMMA);
                return COMMA;
                break;
            case 27://starea finala SEMICOLON
                addTk(SEMICOLON);
                return SEMICOLON;
                break;
            case 28://starea finala LPAR
                addTk(LPAR);
                return  LPAR;
                break;
            case 29://starea finala RPAR
                addTk(RPAR);
                return RPAR;
                break;
            case 30://starea finala LBRACKET
                addTk(LBRACKET);
                return  LBRACKET;
                break;
            case 31://starea finala RBRACKET
                addTk(RBRACKET);
                return RBRACKET;
                break;
            case 32://starea finala LACC
                addTk(LACC);
                return LACC;
                break;
            case 33://starea finala RACC
                addTk(RACC);
                return RACC;
                break;
            case 34://starea finala DIV
                addTk(DIV);
                return DIV;
                break;
            case 35:
            if (ch != '\r' || ch != '\0' || ch != '\n') {//^\r\t
                pCrtCh++;
                state = 35;
                }
                else if (ch == '\n') { 
                line++;
                pCrtCh++;
                state = 0;
                }
                else {
                pCrtCh++;
                state = 0;
                }
                break;
            case 36:
            if (ch == '/') {
                pCrtCh++;
                state = 35;
                }
                else if (ch == '*') {
                pCrtCh++;
                state = 37;
                }
                else {
                state = 34;
                }
                break;
            case 37:
            if (ch == '*') {
                pCrtCh++;
                state = 38;
                }
                else if (ch != '*') {
                pCrtCh++;
                state = 37;
                }
                break;
            case 38:
            if (ch == '*') {
                pCrtCh++;
                state = 38;
                }
                else if (ch != '*' || ch != '/') {
                pCrtCh++;
                state = 37;
                }
                else if (ch == '/') {
                pCrtCh++;
                state = 0;
                }
                break;
            case 39://starea finala ADD
                addTk(ADD);
                return ADD;
                break;
            case 40://starea finala SUB
                addTk(SUB);
                return SUB;
                break;
            case 41://starea finala MUL
                addTk(MUL);
                return MUL;
                break;
            case 42://starea finala DOT
                addTk(DOT);
                return DOT;
                break;
            case 43:
                if (ch == '=') {
                pCrtCh++;
                state = 45;
                }
                else
                state = 44;
                break;
            case 44://starea finala LESS
                addTk(LESS);
                return LESS;
                break;
            case 45://starea finala LESSEQ
                addTk(LESSEQ);
                return LESSEQ;
                break;
            case 46:
                if (ch == '=') {
                pCrtCh++;
                state = 48;
                }
                else
                state = 47;
                break;
            case 47://starea finala GREATER
                addTk(GREATER);
                return GREATER;
                break;
            case 48://starea finala GREATEREQ
                addTk(GREATEREQ);
                return GREATEREQ;
                break;
            case 49:
                if (ch == '&') {
                pCrtCh++;
                state = 57;
                }
                break;
            case 50:
                if (ch == '|') {
                pCrtCh++;
                state = 58;
                }
                break;
            case 51:
                if (ch == '=') {
                pCrtCh++;
                state = 53;
                }
                else
                state = 52;
                break;
            case 52://starea finala NOT
                addTk(NOT);
                return NOT;
                break;
            case 53://starea finala NOTEQ
                addTk(NOTEQ);
                return NOTEQ;
                break;
                case 54:
                if (ch == '=') {
                pCrtCh++;
                state = 56;
                }
                else
                state = 55;
                break;
            case 56://starea finala EQUAL
                addTk(EQUAL);
                return EQUAL;
                break;
                case 55://starea finala ASSIGN
                addTk(ASSIGN);
                return ASSIGN;
                break;
            case 57://starea finala AND
                addTk(AND);
                return AND;
                break;
            case 58:
                addTk(OR);
                return OR;
                break;
        }   }
  
      
}

void showToken(Token* tk) {
    printf("%d %s", tk->line, tokenName[tk->code]);               
    if (tk->code == CT_INT || tk->code == CT_CHAR ) {
        printf(":%ld ", tk->i);
    }
    else if (tk->code == CT_REAL) {
        printf(":%lf ", tk->r);
    }
    else if (tk->code == CT_STRING || tk->code == ID) {
        printf(":%s ", tk->text);
    }
}

void showTokens() {
    Token* aux = tokens;
    while (aux != NULL) {
        showToken(aux);
        printf("\n");
        aux = aux->next;
    }
}

int unit();
int declStruct();
int declFunc();
int declVar();
int typeBase();
int arrayDecl();
int fnDef();
int fnParam();
int stm();
int stmCompound();
int expr();
int exprAssign();
int exprOr();
int exprAnd();
int exprEq();
int exprRel();
int exprAdd();
int exprMul();
int exprCast();
int exprUnary();
int exprPostfix();
int exprPrimary();
int exprAndPrim();
int exprOr();
int exprOrPrim();



int consume(int code)
{
    //printf("Sunt aici3 %d ",code);
	if(crtTk->code == code)
	{
        //printf("Sunt aici4");
		consumedTk=crtTk;
		crtTk=crtTk->next;
        
		return 1;
	}

    // printf("Sunt aici5");
return 0;
}

// handle the typeBase --> typeBase: INT | DOUBLE | CHAR | STRUCT ID ;

int typeBase()
{
     //printf("Sunt aici5");
	if(consume(INT) || consume(DOUBLE) || consume(CHAR) || (consume(STRUCT) && consume(ID)))
		return 1;
	return 0;
}

//handle the array declaration  --> arrayDecl: LBRACKET expr? RBRACKET ;
int arrayDecl()
{
	if(!consume(LBRACKET))
		return 0;
	if(expr())
	{
		//it does nothing if there is or not an expr
	}
	if(!consume(RBRACKET))
		tkerr(crtTk,"missing ] after when declaring the array");
	return 0 ;
}


// handle a variable declaration or a multiple variables declaration 
// declVar:  typeBase ID arrayDecl? ( COMMA ID arrayDecl? )* SEMICOLON ;

int declVar()
{
     //printf("Sunt aiciVar");
	if(!typeBase()) return 0;
	if(!consume(ID)) tkerr(crtTk,"missing ID after TYPE"); 
	if(arrayDecl())
	{
		//it does nothing if there is or not an array declaration
	}
	while(1)
	{
		if(!consume(COMMA))break;
		if(!consume(ID)) tkerr(crtTk,"missing ID after COMMA"); 
		if(arrayDecl())
		{
			//it does nothing if there is or not an array declaration
		}
	}
	
	if(!consume(SEMICOLON)) tkerr(crtTk,"missing ; after declaration");
	
	return 1;
}

// handle the declaration of a structure --> declStruct: STRUCT ID LACC declVar* RACC SEMICOLON ;
int declStruct(){
	
	Token *start= crtTk;
	if(consume(STRUCT)){
		if(consume(ID)){
			if(consume(LACC)){
				while(1){
					if(declVar()){
						
					}
					else break;
				}
				if(consume(RACC)){
					if(consume(SEMICOLON)){
						return 1;
					}
				}
			}
		}
	}
	crtTk=start; //ne intoarcem la pozitia initiala daca vreun if nu sa indeplinit
	return 0;
}

// handel the unit --> unit: ( declStruct | declFunc | declVar )* END ;

int unit(){
	Token *start= crtTk;

    //printf("------------------------%d-----------------", start->code);
	while(1){
		if(declStruct()){
						
		}else if(declFunc()){}
		else if(declVar()){}
		else break;
		
	}
	if(consume(END)){
		return 1;
	}
	crtTk=start;
	return 0;
	
}

//handle the typeName --> typeName: typeBase arrayDecl? ;
int typeName()
{
	if(!typeBase())return 0;
	arrayDecl();
	return 1;
}

//handle the funcArg --> funcArg: typeBase ID arrayDecl? ;
int funcArg()
{
	if(!typeBase())return 0;
	if(!consume(ID)) tkerr(crtTk,"missing ID after type of argument");
	arrayDecl();
	return 1;
}

// handle a function declaration
//declFunc: ( typeBase MUL? | VOID ) ID 
//                       LPAR ( funcArg ( COMMA funcArg )* )? RPAR 
//                       stmCompound ;
int declFunc()
{
  startTk = crtTk;
   //printf("Sunt aiciFUNC");
	if(typeBase())
	{
		if(consume(MUL))
		{
			// do nothing if there is or not a MUL token
		}
	}else
		if(!consume(VOID)) return 0;
	
	if(!consume(ID)) tkerr(crtTk,"missing ID after type of function");
	if(!consume(LPAR)) { //tkerr(crtTk,"missing ( after the ID");
	  crtTk=startTk;
	  return 0;
	}
	
	if(funcArg())
	{
		// do nothing if there is or not an argument
		while(1)
		{
			if(!consume(COMMA))break;
			if(!funcArg()) tkerr(crtTk,"missing argument after COMMA");
		}
	}
	if(!consume(RPAR)) tkerr(crtTk,"missing ) at the end of function declaration");
	stmCompound();
	return 1;
}

// check the if correctness --> IF LPAR expr RPAR stm ( ELSE stm )?
int ruleIf()
{
	if(!consume(IF))return 0;
	if(!consume(LPAR))tkerr(crtTk,"missing ( after if");
	if(!expr())tkerr(crtTk,"invalid expression after (");
	if(!consume(RPAR))tkerr(crtTk,"missing )");
	if(!stm())tkerr(crtTk,"missing if statement");
	if(consume(ELSE))
	{
	if(!stm()) tkerr(crtTk,"missing else statement");
	}	
	return 1;
}

// check the while correctness --> WHILE LPAR expr RPAR stm
int ruleWhile()
{
	if(!consume(WHILE))return 0;
	if(!consume(LPAR))tkerr(crtTk,"missing ( after while");
	if(!expr())tkerr(crtTk,"invalid expression after (");
	if(!consume(RPAR))tkerr(crtTk,"missing )");
	if(!stm())tkerr(crtTk,"missing while statement");
	return 1;
}


// check the for correctness --> FOR LPAR expr? SEMICOLON expr? SEMICOLON expr? RPAR stm
int ruleFor()
{
   printf("#rulefOR %d(%d)\n",crtTk->line,crtTk->code);
	if(!consume(FOR))return 0;
	if(!consume(LPAR))tkerr(crtTk,"missing ( after for");
	
	if(expr())
	{
		//while(expr());//it does nothing if there is or not an expr
	}
	
	if(!consume(SEMICOLON))tkerr(crtTk,"missing ; after first expr");
	
	if(expr())
	{
		//it does nothing if there is or not an expr
	}
	
	if(!consume(SEMICOLON))tkerr(crtTk,"missing ; after second expr");
	
	if(expr())
	{
		//it does nothing if there is or not an expr
	}

	if(!consume(RPAR))tkerr(crtTk,"missing )");
	
	if(!stm())tkerr(crtTk,"missing for statement");
	return 1;
}

// check the break correctness --> BREAK SEMICOLON
int ruleBreak()
{
   printf("#break %d(%d)\n",crtTk->line,crtTk->code);
	if(!consume(BREAK))return 0;
	if(!consume(SEMICOLON)) tkerr(crtTk,"missing ;");
	return 1;
}

//check return correctness --> RETURN expr? SEMICOLON
int ruleReturn()
{
   printf("#return %d(%d)\n",crtTk->line,crtTk->code);
	if(!consume(RETURN))return 0;
	expr();
	if(!consume(SEMICOLON)) tkerr(crtTk,"missing ;");
	return 1;
}

// handle expr --> expr: exprAssign ;
int expr()
{
   printf("#expr %d(%d)\n",crtTk->line,crtTk->code);
	if(!exprAssign())
		return 0;
	return 1;
}

//handle exprAssign --> exprAssign: exprUnary ASSIGN exprAssign | exprOr ;
int exprAssign() 
{
   printf("#ass %d(%d)\n",crtTk->line,crtTk->code);
  Token *start = crtTk;
	if(exprUnary())  
           if(consume(ASSIGN)) //tkerr(crtTk, "Expected ASSIGN after expression");
            if(exprAssign()) //tkerr(crtTk, "Expected Expression in expression");
            return 1;
	   
    crtTk = start;
    if(!exprOr()){ return 0;}
    return 1;
}

////handle exprOr --> exprOr: exprOr OR exprAnd | exprAnd
					// Remove left recursion:
					//     exprOr: exprAnd exprOr1
					//     exprOr1: OR exprAnd exprOr1
void exprOr1() 
{
   printf("#Or1 %d(%d)\n",crtTk->line,crtTk->code);
    if(consume(OR)) {
        if(!exprAnd()) 
            {
               // tkerr(crtTk,"missing expression after OR"); 
            }
        exprOr1();
    }
}

int exprOr() 
{
   printf("#Or %d(%d)\n",crtTk->line,crtTk->code);
    if(!exprAnd()) return 0;
    exprOr1();
    return 1;
}



// handle exprAnd --> exprAnd: exprAnd AND exprEq | exprEq
						// Remove left recursion:
						//     exprAnd: exprEq exprAnd1
						//     exprAnd1: AND exprEq exprAnd1
void exprAnd1()
 {
    printf("#and1 %d(%d)\n",crtTk->line,crtTk->code);
    if(consume(AND)) {
        if(!exprEq()) 
            {
                //tkerr(crtTk,"missing expression after AND"); 
            }
        exprAnd1();
    }
}

int exprAnd()
 {
    printf("#and %d(%d)\n",crtTk->line,crtTk->code);
    if(!exprEq()) return 0;
    exprAnd1();
    return 1;
}



// handle exprEq --> exprEq: exprEq ( EQUAL | NOTEQ ) exprRel | exprRel
					// Remove left recursion:
					//     exprEq: exprRel exprEq1
					//     exprEq1: ( EQUAL | NOTEQ ) exprRel exprEq1
void exprEq1() 
{
   printf("#eq1 %d(%d)\n",crtTk->line,crtTk->code);
    if(consume(EQUAL)) {}
    else if(consume(NOTEQ)) {}
    else return;
    if(!exprRel()) 
        {
        }//tkerr(crtTk,"missing expressiong after =");
    exprEq1();
}

int exprEq()
 {
    printf("#eq %d(%d)\n",crtTk->line,crtTk->code);
    if(!exprRel()) return 0;
    exprEq1();
    return 1;
}



// handle exprRel --> exprRel: exprRel ( LESS | LESSEQ | GREATER | GREATEREQ ) exprAdd | exprAdd
					// Remove left recursion:
					//     exprRel: exprAdd exprRel1
					//     exprRel1: ( LESS | LESSEQ | GREATER | GREATEREQ ) exprAdd exprRel1
void exprRel1() 
{
   printf("#exprrel1 %d(%d)\n",crtTk->line,crtTk->code);
    if(consume(LESS)) {}
    else if(consume(LESSEQ)) {}
    else if(consume(GREATER)) {}
    //else if(consume(GREATEREQ)) {}
    else return;
    if(!exprAdd())
    {

    }// tkerr(crtTk,"missing expression after relationship");
    exprRel1();
}

int exprRel() 
{
   printf("#exprrel %d(%d)\n",crtTk->line,crtTk->code);
    if(!exprAdd()) return 0;
    exprRel1();
    return 1;
}



//handle exprAdd --> exprAdd: exprAdd ( ADD | SUB ) exprMul | exprMul
					// Remove left recursion:
					//     exprAdd: exprMul exprAdd1
					//     exprAdd1: ( ADD | SUB ) exprMul exprAdd1
void exprAdd1()
 {
    printf("#expradd1 %d(%d)\n",crtTk->line,crtTk->code);
    if(consume(ADD)) {}
    else if(consume(SUB)) {}
    else return;
    if(!exprMul())
    {

    } //tkerr(crtTk,"missing expressiong after + or -");
    exprAdd1();
}

int exprAdd() 
{
   printf("#expradd %d(%d)\n",crtTk->line,crtTk->code);
    if(!exprMul()) return 0;
    exprAdd1();
    return 1;
}



//handle exprMul --> exprMul: exprMul ( MUL | DIV ) exprCast | exprCast
					// Remove left recursion:
					//     exprMul: exprCast exprMul1
					//     exprMul1: ( MUL | DIV ) exprCast exprMul1
void exprMul1() 
{
   printf("#mul1 %d(%d)\n",crtTk->line,crtTk->code);
    if(consume(MUL)) {}
    else if(consume(DIV)) {}
    else return;
    if(!exprCast())
    {

    } //tkerr(crtTk,"missing expressiong after * or /");
    exprMul1();
}

int exprMul()
 {
    printf("#mul %d(%d)\n",crtTk->line,crtTk->code);
    if(!exprCast()) return 0;
    exprMul1();
    return 1;
}



//handle exprCast --> exprCast: LPAR typeName RPAR exprCast | exprUnary
int exprCast()
 {
    printf("#cast %d(%d)\n",crtTk->line,crtTk->code);
   if(exprUnary()) return 1;
    if(!consume(LPAR)) return 0;
    if(!typeName()) tkerr(crtTk,"missing type after LPAR");

    if(consume(RPAR)) tkerr(crtTk,"missing RPAR after type");
    if(exprCast()) { return 1; }
    
	return 0;
}

//handle exprUnary --> exprUnary: ( SUB | NOT ) exprUnary | exprPostfix
int exprUnary() 
{
    printf("#UNARY %d(%d)\n",crtTk->line,crtTk->code);
  if(exprPostfix()) return 1;
  
    if(consume(SUB))
      if(exprUnary()) return 1;
    if(consume(NOT)) 
      if(exprUnary()) return 1;
	
	return 0;
    
}

//handle exprPostfix -->  exprPostfix: exprPostfix LBRACKET expr RBRACKET
//            | exprPostfix DOT ID 
//            | exprPrimary
				// Remove left recursion:
				//     exprPostfix: exprPrimary exprPostfix1
				//     exprPostfix1: ( LBRACKET expr RBRACKET | DOT ID ) exprPostfix1
void exprPostfix1() 
{
   printf("#postfix1 %d(%d)\n",crtTk->line,crtTk->code);
    if(consume(LBRACKET)) {
        if(!expr()) tkerr(crtTk,"missing expression after (");
        if(!consume(RBRACKET)) tkerr(crtTk,"missing ) after expression");
    } else if(consume(DOT)) {
        if(!consume(ID)) tkerr(crtTk,"error");
    } else return;
    exprPostfix1();
}

int exprPostfix() 
{
   printf("#postfix %d(%d)\n",crtTk->line,crtTk->code);
    if(!exprPrimary()) return 0;
    exprPostfix1();
    return 1;
}



// exprPrimary: ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
//            | CT_INT
//            | CT_REAL 
//            | CT_CHAR 
//            | CT_STRING 
//            | LPAR expr RPAR
int exprPrimary() 
{
  Token *startTk = crtTk;
   printf("#exprPrimary %d(%d)\n",crtTk->line,crtTk->code);
    if(consume(ID)) {
	printf("id aici");
        if(consume(LPAR)) {
            if(expr()) {
                while(1) {
                    if(!consume(COMMA)) break;
                    if(!expr()) tkerr(crtTk,"missing expression after , in primary expression");
                }
            }
            if(!consume(RPAR)) tkerr(crtTk,"missing )");
        }
    }
    else if(consume(CT_INT)) { }
    else if(consume(CT_REAL)) {}
    else if(consume(CT_CHAR)) {}
    else if(consume(CT_STRING)) {}
    else if(consume(LPAR)) {
        if(!expr()) {
	  tkerr(crtTk,"missing expr after (");
        }
        if(!consume(RPAR)) tkerr(crtTk,"missing ) after expression");
    }
    else {
      crtTk = startTk;
      return 0;
    }
    return 1;
}

int factor()
{
   printf("#factor %d(%d)\n",crtTk->line,crtTk->code);
	Token *startTk=crtTk;
	if(consume(ID))
	{
		return 1;
	}
	if(consume(CT_INT))
	{
		return 1;
	}
	if(consume(LPAR))
	{
		if(expr())
		{
			if(consume(RPAR))
			{
				return 1;
			}else 
				tkerr(crtTk,"missing )");
		}else 
			tkerr(crtTk,"invalid expression after (");
		crtTk=startTk; // restore crtTk to the entry value
	}
	return 0;
}



int stmCompound()
{
   printf("#stmCmp %d(%d)\n",crtTk->line,crtTk->code);
	if(!consume(LACC))return 0;
	while(1)
	{
		if(declVar())
		{
		}
		else 
			if(stm())
			{
			}
			else break;
	}
	if(!consume(RACC))tkerr(crtTk,"missing } or syntax error");
	return 1;
}

int stm(){
	Token *startTk=crtTk;
	if(stmCompound())
		return 1;
	else
	if(consume(IF))
		{
			if(consume(LPAR))
			{
				if(expr())
				{
					if(consume(RPAR))
					{
						if(stm())
						{
							if(consume(ELSE))
							{
								stm();
							}
							return 1;
						}
					}
				}
			}
		}
	else
	if(consume(WHILE))
		{
			if(consume(LPAR))
			{
				if(expr())
				{
					if(consume(RPAR))
					{
						if(stm())
						{
							return 1;
						}
					}
				}
			}
		}
	else
	if(consume(FOR))
		{
			if(consume(LPAR))
				{
					expr();
					if(consume(SEMICOLON))
					{
						expr();
						if(consume(SEMICOLON))
						{
							expr();
							if(consume(RPAR))
							{
								if(stm())
								{
									return 1;
								}
							}
						}
					}
				}
			}
	else
	if(consume(BREAK))
		{
			if(consume(SEMICOLON))
				{
					return 1;
				}
		}
	else
	if(consume(RETURN))
		{
			expr();
			if(consume(SEMICOLON))
				{
					return 1;
				}
		}
	else
		{
			expr();
			if(consume(SEMICOLON))
				{
					return 1;
				}
		}
	crtTk=startTk;
	return 0;
}


int main()
{
    FILE* fis;
    char ch;
    char* buff;
    int i = 0; //index

    fis = fopen("0.c", "r");
    if (fis == NULL) {
        printf("eroare deschidere fisier\n");
        exit(1);
    }

    buff = (char*)malloc(sizeof(char) * 30000);
    if (buff == NULL) {
        printf("alocare incorecta\n");
        exit(1);
    }

    while ((ch = fgetc(fis)) != EOF) {
        buff[i++] = ch;
    }
    buff[i++] = '\0';

    /*
    for(int j = 0;j<= i ;j++){
        printf("%c",buff[j]);

    }
    */

    fclose(fis);

    pCrtCh = buff;
    while (getNextToken() != END) {}

    showTokens();
    crtTk = tokens;
    	if(unit()){
		printf("sintaxa buna\n");
	}else{
		tkerr(crtTk,"eroare de sintaxa");
	}

    return 0;
}

