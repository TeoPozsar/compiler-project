#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>



enum{ID, END, CT_INT, ASSIGN, SEMICOLON , CT_REAL, EQUAL , CT_CHAR , CT_STRING,
COMMA , LPAR , RPAR , LBRACKET , RBRACKET , LACC, RACC,ADD, SUB , MUL , DIV , DOT ,AND, OR,
NOT , LESS, LESS_EQ , MORE , MORE_EQ , DIFF }; // tokens codes 
 
typedef struct _Token{ 
 int   code;     // code (name) 
 union{ 
  char   *text;   // used for ID, CT_STRING (dynamically allocated) 
  long int  i;    // used for CT_INT, CT_CHAR 
  double  r;    // used for CT_REAL 
  }; 
 int   line;     // the input file line 
 struct _Token *next;   // link to the next token 
 }Token; 


 void err(const char *fmt,...) 
{ 
va_list  
va; 
va_start(va,fmt); 
fprintf(stderr,"error: "); 
vfprintf(stderr,fmt,va); 
fputc('\n',stderr); 
va_end(va); 
exit(-1); 
}


void tkerr(const Token *tk,const char *fmt,...) {
va_list  
va; 
va_start(va,fmt); 
fprintf(stderr,"error in line %d: ",tk->line); 
vfprintf(stderr,fmt,va); 
fputc('\n',stderr); 
va_end(va); 
exit(-1); 
} 

#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)err("not enough memory");

Token *tokens = NULL;
Token *lastToken = NULL;
int line = 1;
char *pCrtCh;


 Token  *addTk(int code) 
{ 
 Token  *tk; 
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


int  getNextToken() 
{ 
 int state=0;
 char ch; 
 const char *pStartCh; //primul caracter din sir
 Token *tk; 
 
 while(1){  
  ch=*pCrtCh; //pointer to the current character in the input string
    
  switch(state){ 
   case 0:    
    
    if (ch == ',') { pCrtCh++; state = 21; }
    else if (ch == ';') { pCrtCh++; state = 22; }
    else if (ch == '(') { pCrtCh++; state = 23; }
    else if (ch == ')') { pCrtCh++; state = 24; }
    else if (ch == '[') { pCrtCh++; state = 25; }
    else if (ch == ']') { pCrtCh++; state = 26; }
    else if (ch == '{') { pCrtCh++; state = 27; }
    else if (ch == '}') { pCrtCh++; state = 28; }
    else if (ch == '+') { pCrtCh++; state = 30; }
    else if (ch == '-') { pCrtCh++; state = 31; }
    else if (ch == '*') { pCrtCh++; state = 32; }
    else if (ch == '/') 
    { pStartCh=pCrtCh;
      pCrtCh++;
      state = 33;
    
    }
    else if (ch == '.') { pCrtCh++; state = 34; }
    else if (ch == '&') { pCrtCh++; state = 35; }
    else if (ch == '|') { pCrtCh++; state = 37; }
    else if (ch == '!') { pCrtCh++; state = 39; }
    else if (ch == '=') { pCrtCh++; state = 42; }
    else if (ch == '<') { pCrtCh++; state = 45; }
    else if (ch == '>') { pCrtCh++; state = 48; }
    else if (ch == ' ' || ch == '\r' || ch == '\t')
    {   //space return tab
        pCrtCh++;
    }
    else if (ch == '\n')
    {
        line++;
        pCrtCh++;
    }


    //numbers
    else if (ch >= '1' && ch <= '9')
    {
        pStartCh = pCrtCh; //punem pointerul catre primul caracter din sir si ii atribuim caracterul pe care am fost tocmai
        pCrtCh++; // iar cel curent il trecem pe urmatorul
        state = 1;
    }

    else if (ch== '0')
    {
        pStartCh =pCrtCh;
        pCrtCh++;
        state=9;
    }

    // ID 
    else if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
    {
       pStartCh=pCrtCh;
       pCrtCh++;
       state=51;
    }

    //char
    else if (ch == '\'')
    {
        pStartCh=pCrtCh;
        pCrtCh++;
        state=15;
    }

    else if (ch == '"')
    {
        pStartCh=pCrtCh;
        pCrtCh++;
        state=18;
    }

    else if (ch =='\0')
    {
        addTk(END);
        return END;
    }

    else 
    {
        tkerr(addTk(END), "invalid ");
    }
    break;


    case 1:
      if ( ch >= '0' && ch <= '9')
      {
         pCrtCh++;
         state=1;
      }

      else if ( ch == '.')
      {
        pCrtCh++;
        state=3;
      }

      else if ( ch == 'e' || ch == 'E')
      {
        pCrtCh++;
        state=5;
      }

      else state=2;

      break;

    case 2: //final state

      tk = addTk(CT_INT);
      tk->i= strtol(pStartCh,NULL,0); //converts hexa si octa in baza 10
      return CT_INT;


      //strtol = converts strings in intigers ( supports multiple bases)

    case 3: //merge doar in 4
      if( ch >= '0' && ch<= '9')
      {
        pCrtCh++;
        state=4;
      }

      else tkerr(addTk(END), "invalid ");
      break;

    case 4:
      if( ch >= '0' && ch<= '9')
      {
        pCrtCh++;
        //ramane tot in state 4
      }

      else if (ch == 'e' || ch == 'E')
      {
        pCrtCh++;
        state=5;
      }

      else 
      { state=8; }
      break;

      

      case 5:
       if(ch >= '0' && ch <= '9')
       {
         pCrtCh++;
         state=7;
       }
       else if(ch == '-' || ch == '+')
       {
         pCrtCh++;
         state=6;
       }
       else {
        tkerr(addTk(END), "invalid exponent in real constant");
         }
         break;

       case 6:
        if(ch >= '0' && ch <= '9')
       {
         pCrtCh++;
         state=7;
       }   
        else {
        tkerr(addTk(END), "invalid exponent in real constant");
         }
        break;

       case 7:
        if(ch >= '0' && ch <= '9')
       {
         pCrtCh++;
         //ramane in state 7
       }
       else state=8;
       break;

       case 8: //final  state
        tk = addTk(CT_REAL);
        tk->r = atof(pStartCh);  //din string in double (si alea cu e le scrie frumos ca nr)
        return CT_REAL;

       //numere care incep cu 0
        case 9: 
        if (ch == '\0') //cazul in care numarul era doar 0
        {
        tk = addTk(CT_INT);
        tk->i = strtol(pStartCh,NULL,0);
        return CT_INT;
        }
        
        else if (ch == 'x' || ch =='X')
        {
            pCrtCh++;
            state=10;
        }

        else if ( ch >= '1' && ch <= '7')
        {
            pCrtCh++;
            state=13;
        }

        else if(ch >= '0' && ch <= '9')
        {
            pCrtCh++;
            state=13;
        }

        else if( ch == '.')
        {
          pCrtCh++;
          state=3;
        }

        else if (ch == 'e' || ch == 'E')
      {
        pCrtCh++;
        state=5;
      }

      else {
        tk = addTk(CT_INT);
        tk->i = strtol(pStartCh,NULL,0);
        return CT_INT;
      }

        break;

      case 10:
       if( (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
       {
         pCrtCh++;
         state=11;
       }

       else tkerr(addTk(END), "invalid hexa constant ");

       break;
       
       case 11:
       if( (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
       {
         pCrtCh++;
       }

       else state=2;

       break;



      case 13:
        if (ch >= '0' && ch <= '7') {
        pCrtCh++;
       }
        else if (ch == '8' || ch == '9') {
         pCrtCh++;
         state=1;
       }

       else if( ch =='.')
       {
         pCrtCh++;
         state=3;
       }
       else {
        state = 2;
        }
       break;

    case 15:
    if (ch == '\\') {
        pCrtCh++;
        state = 53;   // escaped char , nu i in diagrama
    }
    else if (ch != '\'' && ch != '\0' && ch != '\n') {
        pCrtCh++;
        state = 16;   // normal char
    }
    else {
        tkerr(addTk(END), "invalid char");
    }
    break;


    case 16:
    if(ch == '\'')
    {
      pCrtCh++;
      state=17;
    }

    else tkerr(addTk(END), "invalid closing char ");
    break;

    case 17:
{
    tk = addTk(CT_CHAR); //create a new token de tip char constant
    //pt file 8
    if (pStartCh[1] == '\\') { //verific daca e escape sequence  ('\n')
        switch (pStartCh[2]) {  //daca da , handle the escape sequence
            case 'n': tk->i = '\n'; break;
            case 't': tk->i = '\t'; break; //stored as actual tab character
            case '0': tk->i = '\0'; break;
            case '\\': tk->i = '\\'; break;
            case '\'': tk->i = '\''; break;
            case '"': tk->i = '"'; break;
            default: tkerr(tk, "invalid escape in char");
        }
    } else {
        tk->i = pStartCh[1]; //normal character
    }

    return CT_CHAR;
}


    case 18:
    if (ch == '"') {
        pCrtCh++;
        state = 20;   // empty string
    }
    else if (ch == '\\') {
        pCrtCh++;
        state = 54;   // escaped char inside string
    }
    else if (ch != '\0' && ch != '\n') { //orice inafara de "
        pCrtCh++;
        state = 19;
    }
    else {
        tkerr(addTk(END), "invalid string");
    }
    break;


    case 19:
    if (ch == '"') {
        pCrtCh++;
        state = 20;
    }
    else if (ch == '\\') {
        pCrtCh++;
        state = 54;
    }
    else if (ch != '\0' && ch != '\n') {
        pCrtCh++; //daca ii orice inafara de " atunci e un caracter normal si continua string ul
    }
    else {
        tkerr(addTk(END), "unterminated string");
    }
    break;

    case 20:
{
    const char *src = pStartCh + 1; // pointer cu care citesc textul dintre ""
    char *dst; //poinyter used later to write into the new string
    int len = 0; //how many characters the final code will have

    while (src < pCrtCh - 1) { //counts how many characters the final decoded string will contain
        if (*src == '\\') { //count the whole escape as one final character
            src++;
            if (src >= pCrtCh - 1)  //make sure that there is still one more character after it
              tkerr(addTk(END), "invalid escape in string");
        }
        len++; //daca src nu e \ , atunci trecem la urm caracter
        src++;
    }

    tk = addTk(CT_STRING); //create a token
    tk->text = (char*)malloc(len + 1); //allocate memory for len characters +1 pt \0
    if (!tk->text) err("not enough memory");

    src = pStartCh + 1; //src incepe iar la caracteru de dupa "
    dst = tk->text; //points to where i write the decoded string

    while (src < pCrtCh - 1) { //actually decode and copy
        if (*src == '\\') { //daca e escape sequence
            src++; //trecem la esvcaped char
            switch (*src) { //si il transformam in efectiv ce e
                case 'n': *dst = '\n'; break;
                case 't': *dst = '\t'; break;
                case '0': *dst = '\0'; break;
                case '\\': *dst = '\\'; break;
                case '\'': *dst = '\''; break;
                case '"': *dst = '"'; break;
                default: tkerr(tk, "invalid escape in string");
            }
        } else {
            *dst = *src; //normal caracter , si destinatia primeste ce e in sursa
        }
        dst++;
        src++;
    }

    *dst = '\0'; //end of string
    return CT_STRING;



// src reads the original string
// len counts decoded size
// malloc(len+1) allocates memory
// dst writes decoded chars
// escapes like \n, \t, \", \\ become real characters
// '\0' ends the final string
}
    
    case 21: addTk(COMMA); return COMMA;
    case 22: addTk(SEMICOLON); return SEMICOLON;
    case 23: addTk(LPAR); return LPAR;
    case 24: addTk(RPAR); return RPAR;
    case 25: addTk(LBRACKET); return LBRACKET;
    case 26: addTk(RBRACKET); return RBRACKET;
    case 27: addTk(LACC); return LACC;
    case 28: addTk(RACC); return RACC;
    case 30: addTk(ADD); return ADD;
    case 31: addTk(SUB); return SUB;
    case 32: addTk(MUL); return MUL;

    case 33: 
    if (ch == '/') {
        pCrtCh++;      // consume second '/'
        state = 52;    // go to comment state
    } else {
        addTk(DIV);
        return DIV;
    }
    break;


    case 34: addTk(DOT); return DOT;

    case 35:
     if(ch == '&')
     {
       pCrtCh++;
       state=36;
     }

     else err("invalid &");
     break;

    case 36: addTk(AND); return AND;

    case 37 :
     if(ch == '|')
     {
      pCrtCh++;
      state=38;
     }
     else err("invalid |");
     break;

    case 38: addTk(OR); return OR;
     
    case 39:
     if (ch == '=') {
      pCrtCh++;
      state=41;
     }
     else{
      state=40;
     }

     
     break;

    case 40: addTk(NOT); return NOT;
    case 41: addTk(DIFF); return DIFF;
    
    case 42:
     if(ch == '=')
     {
      pCrtCh++;
      state=44;
     }
     else {
      state=43;
     }
     
     break;

    
    case 43: addTk(ASSIGN); return ASSIGN;
    case 44: addTk(EQUAL); return EQUAL;

    case 45:
     if(ch == '=')
     {
      pCrtCh++;
      state=47;
     }
     else {
      state=46;
     }
    
     break;


    case 46: addTk(LESS); return LESS;
    case 47: addTk(LESS_EQ); return LESS_EQ;

    case 48:
     if(ch == '=')
     {
      pCrtCh++;
      state=50;
     }
     else {
      state=49;
     }
     
     break;

    case 49: addTk(MORE); return MORE;
    case 50: addTk(MORE_EQ); return MORE_EQ;

    case 51: //identifier
     if( (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ( ch== '_'))
      {
        pCrtCh++;
      } //cat timp ch e parte din identifier

     else { //cand ch nu e part of the identifier
      int len = (int)(pCrtCh - pStartCh);
      tk = addTk(ID);
      tk->text = (char*)malloc(len + 1); //allocate space for the text
      if (!tk->text) err("not enough memory");
      memcpy(tk->text, pStartCh, len); //copy the identifier
      tk->text[len] = '\0'; 
      return ID;
     }

     break;

    case 52:
    if (ch == '\n') {
        line++;
        pCrtCh++;
        state = 0;     // go back to start
    }
    else if (ch == '\0') {
        state = 0;     // end of file
    }
    else {
        pCrtCh++;      // skip characters
    }
    break;


    case 53:
    if (ch == 'n' || ch == 't' || ch == '\\' || ch == '\'' || ch == '"' || ch == '0') {
        pCrtCh++;
        state = 16;
    } else {
        tkerr(addTk(END), "invalid escape in char");
    }
    break;

    case 54:
    if (ch == 'n' || ch == 't' || ch == '0' || ch == '\\' || ch == '\'' || ch == '"') {
        pCrtCh++;
        state = 19;
    } else {
        tkerr(addTk(END), "invalid escape in string");
    }
    break;


   } 
  } 
} 

const char *codeName(int code)
{
    switch (code) {
        case ID: return "ID";
        case END: return "END";
        case CT_INT: return "CT_INT";
        case ASSIGN: return "ASSIGN";
        case SEMICOLON: return "SEMICOLON";
        case CT_REAL: return "CT_REAL";
        case EQUAL: return "EQUAL";
        case CT_CHAR: return "CT_CHAR";
        case CT_STRING: return "CT_STRING";
        case COMMA: return "COMMA";
        case LPAR: return "LPAR";
        case RPAR: return "RPAR";
        case LBRACKET: return "LBRACKET";
        case RBRACKET: return "RBRACKET";
        case LACC: return "LACC";
        case RACC: return "RACC";
        case ADD: return "ADD";
        case SUB: return "SUB";
        case MUL: return "MUL";
        case DIV: return "DIV";
        case DOT: return "DOT";
        case AND: return "AND";
        case OR: return "OR";
        case NOT: return "NOT";
        case LESS: return "LESS";
        case LESS_EQ: return "LESS_EQ";
        case MORE: return "MORE";
        case MORE_EQ: return "MORE_EQ";
        case DIFF: return "DIFF";
        default: return "UNKNOWN";
    }
}

void printTokens(const Token *tk) //trec prin token list si print line number , token type si token value 
{
    while (tk) {
        printf("%d\t%s", tk->line, codeName(tk->code));

        switch (tk->code) {
            case ID:
            case CT_STRING:
                printf(":%s", tk->text);
                break;

            case CT_INT:
                printf(":%ld", tk->i);
                break;

            case CT_CHAR:
                printf(":%ld ('%c')", tk->i, (char)tk->i);
                break;

            case CT_REAL:
                printf(":%g", tk->r);
                break;
        }

        printf("\n");
        tk = tk->next;
    }
}

char *loadFile(const char *fileName)
{
    FILE *f;
    long size;
    char *buf;

    f = fopen(fileName, "rb");
    if (!f) err("cannot open file %s", fileName);

    if (fseek(f, 0, SEEK_END) != 0)  //go to the end of the file ca sa aflam the size
      err("cannot seek file %s", fileName);
    size = ftell(f); //getting the size of the file
    if (size < 0) err("cannot get size of file %s", fileName);
    rewind(f); //go back to the start of the file

    buf = (char*)malloc(size + 1);
    if (!buf) err("not enough memory for file %s", fileName);

    if (fread(buf, 1, size, f) != (size_t)size)
        err("cannot read file %s", fileName);

    buf[size] = '\0';
    fclose(f);
    return buf;
}

void freeTokens(Token *tk)
{
    Token *aux;
    while (tk) {
        aux = tk;
        tk = tk->next;

        if (aux->code == ID || aux->code == CT_STRING) {
            free(aux->text);
        }
        free(aux);
    }
}

int main(int argc, char **argv)
{
    char *buf;

    if (argc != 2) {
        fprintf(stderr, "usage: %s file.c\n", argv[0]);
        return 1;
    }

    buf = loadFile(argv[1]);

    pCrtCh = buf;
    line = 1;
    tokens = NULL;
    lastToken = NULL;

    while (getNextToken() != END) {
    }

    printTokens(tokens);
    freeTokens(tokens);
    free(buf);

    return 0;
}