# include <stdio.h>
# include <stdlib.h>
# include <iostream>
# include <string.h>
# include <vector>
# include <string>

using namespace std ;

int gCol = 0 ;
int gLine = 1 ;
int gErrCol = 0 ;
int gErrLine = 0 ;
bool gEnd = false ;
bool gCL_AfterDot = true ;
bool gEOF = false ;
bool gError = false ;

char gNextBuf = '\0' ;

enum TokenType {

  EMPTY,
  LP,
  RP,
  INT,
  STRING,
  DOT,
  FLOAT,
  NIL,
  T,
  QUOTE,
  EXECUTE_QUOTE,
  SYMBOL,
  LC,
  OPERATOR,
  OPERATOR_PRINT,
  DEFINE,
  CLEAN

} ; // TokenType Define

enum ExpType {

  EXP_ATOM,
  EXP_RP,
  EOL,
  EEOF,
  END,
  NON_LIST,
  NOA, // incorrect number of arguments
  WRONG_TYPE,
  NON_FUNCTION,
  NO_RETURN_VALUE,
  UNBOUND_SYMBOL,
  DEFINE_FORMAT,
  DIV_ZERO,
  COND_FORMAT, // before 13 don't need
  CLEAN_LEVEL,
  DEFINE_LEVEL,
  EXIT_LEVEL

} ; // ExpType Define

bool Isspace( char temp ) ;

bool Isalpha( char temp ) ;

bool Isdigit( char tmep ) ;

// to change a enum to string

void PrintEnumStr( int type ) ;

// to get the next char in stream

char GetNextChar() ;

// to get the next char which is not space in stream

char GetNextNonSpaceChar() ;

class Token {

  private :

  public :

  vector<char> mtoken ;
  int mtype ;
  int mcol ;
  int mline ;

  /*
   * the constructor of Token
  */

  Token() {

    mtoken.clear() ;
    mtype = EMPTY ;
    mcol = 0 ;
    mline = 0 ;

  } // Token()

  Token( int tempType, char tempToken ) {

    mtype = tempType ;
    mtoken.push_back( tempToken ) ;
    mtoken.push_back( '\0' ) ;

  } // Token()

} ; // class Token

class Node {

  private :

  public :

  Token mtoken ;

  Node *mleft ;
  Node *mright ;

  Node() {

    mtoken = Token() ;
    mleft = NULL ;
    mright = NULL ;

  } // Node()

} ; // class Node

typedef Node *NodePtr ;

class Scanner {

  private :

  public :

  char mbuffer ;

  vector<Token> mtokenString ;

  Scanner() {

    mbuffer = '\0' ;
    mtokenString.clear() ;

  } // Scanner()

  bool GetToken( bool lp ) ;

} ; // class Scanner

class Parser {

  private :

  public :

  int mtokenCount ;

  vector<Token> ms_exp ;

  NodePtr mtree ;

  Parser() {

    mtokenCount = 0 ;
    ms_exp.clear() ;

    mtree = NULL ;

  } // Parser()

  bool S_exp( vector<Token> tokenString, bool recursion ) ;

  bool Atom( vector<Token> tokenString ) ;

  vector<Token> S_expTrans( vector<Token> tokenString, bool &firstQuote, bool req ) ;

  NodePtr BuildTree( NodePtr head, vector<Token> tokenString, int &pos ) ;

} ; // class Parser

class SymTable {

  private :

  public :

  Token mSymbol ;
  NodePtr mBound ;

  SymTable() {

    mSymbol = Token() ;
    mBound = NULL ;

  } // SymTable()

} ; // class SymTable

vector<SymTable> gSymTable ;

int NumOfParameter( NodePtr head ) ;

void ClearTree( NodePtr head ) ;

void PrintTree( NodePtr head, bool right, bool printLP, int &count,
                  bool firstQuote, bool qt, int qtCount ) ;

NodePtr CloneTree( NodePtr root ) ;

NodePtr Define( NodePtr head ) ;

NodePtr EvnClean() ;

NodePtr Cons( NodePtr head ) ;

NodePtr List( NodePtr head ) ;

NodePtr Quote( NodePtr head ) ;

NodePtr Cdr( NodePtr head ) ;

NodePtr Car( NodePtr head ) ;

NodePtr ElementIsAtom( NodePtr head ) ;

NodePtr IsAtom( NodePtr head ) ;

NodePtr IsPair( NodePtr head, bool needEval ) ;

NodePtr IsList( NodePtr head, bool needEval ) ;

NodePtr IsNull( NodePtr head ) ;

NodePtr IsInteger( NodePtr head ) ;

NodePtr IsReal( NodePtr head ) ;

NodePtr IsNumber( NodePtr head ) ;

NodePtr IsString( NodePtr head ) ;

NodePtr IsBoolean( NodePtr head ) ;

NodePtr IsSymbol( NodePtr head ) ;

NodePtr Add( NodePtr head ) ;

NodePtr Sub( NodePtr head ) ;

NodePtr Mult( NodePtr head ) ;

NodePtr Div( NodePtr head ) ;

NodePtr Not( NodePtr head ) ;

NodePtr And( NodePtr head ) ;

NodePtr Or( NodePtr head ) ;

NodePtr StringAppend( NodePtr head ) ;

NodePtr BiggerThan( NodePtr head ) ;

NodePtr BiggerEqu( NodePtr head ) ;

NodePtr LessThan( NodePtr head ) ;

NodePtr LessEqu( NodePtr head ) ;

NodePtr NumberEqu( NodePtr head ) ;

NodePtr StringBigger( NodePtr head ) ;

NodePtr StringLess( NodePtr head ) ;

NodePtr StringEqu( NodePtr head ) ;

NodePtr AddressEqu( NodePtr head ) ;

NodePtr ContentEqu( NodePtr head ) ;

NodePtr Begin( NodePtr head ) ;

// condi

NodePtr If( NodePtr head ) ;

NodePtr Cond( NodePtr head, NodePtr errorHead ) ;

NodePtr Eval( NodePtr head, bool eqv, bool topCall ) ;

class Exception {

  private :

  public :

  Exception() { } // Exception()

  Exception( Token errToken, int type ) {

    char debug ;

    gError = true ;

    if ( type == EXP_ATOM ) {

      cout << "> ERROR (unexpected token) : atom or '(' expected when token at Line "
           << errToken.mline << " Column " << errToken.mcol
           << " is >>" << errToken.mtoken.data() << "<<" << endl << endl ;

      if ( gCol != 0 ) {

        debug = GetNextChar() ;

        while ( debug != '\n' && debug != '\0' )
          debug = GetNextChar() ;

      } // if


      gLine = 1 ;
      gCol = 0 ;

      gCL_AfterDot = true ;

    } // if

    else if ( type == EXP_RP ) {

      cout << "> ERROR (unexpected token) : ')' expected when token at Line "
           << errToken.mline << " Column " << errToken.mcol
           << " is >>" << errToken.mtoken.data() << "<<" << endl << endl ;

      if ( gCol != 0 ) {

        debug = GetNextChar() ;

        while ( debug != '\n' && debug != '\0' )
          debug = GetNextChar() ;

      } // if

      gLine = 1 ;
      gCol = 0 ;

    } // else if

    else if ( type == UNBOUND_SYMBOL ) {

      cout << "ERROR (unbound symbol) : " << errToken.mtoken.data() << endl << endl ;

      gLine = 1 ;
      gCol = 0 ;

    } // else if

  } // Exception()

  Exception( int type ) {

    gError = true ;

    if ( type == EEOF ) {

      cout << "> ERROR (no more input) : END-OF-FILE encountered" << endl  ;

      gEnd = true ;
      gEOF = true ;

    } // if

    else if ( type == EOL ) {

      cout << "> ERROR (no closing quote) : END-OF-LINE encountered at Line "
           << gErrLine << " Column " << gErrCol << endl << endl ;

    } // else if

    else if ( type == END )
      gEnd = true ;

    else if ( type == DIV_ZERO )
      cout << "ERROR (division by zero) : /" << endl << endl ;

    else if ( type == CLEAN_LEVEL )
      cout << "ERROR (level of CLEAN-ENVIRONMENT)" << endl << endl ;

    else if ( type == DEFINE_LEVEL )
      cout << "ERROR (level of DEFINE)" << endl << endl ;
      
    else if ( type == EXIT_LEVEL )
      cout << "ERROR (level of EXIT)" << endl << endl ;

    gLine = 1 ;
    gCol = 0 ;

  } // Exception()

  Exception( NodePtr head, int type ) {

    int count = 0 ;

    NodePtr walk = head ;

    if ( type == NON_LIST ) {

      cout << "ERROR (non-list) : " ;
      PrintTree( walk, false, false, count, false, false, 0 ) ;
      ClearTree( walk ) ;

    } // if

    else if ( type == NOA ) {

      cout << "ERROR (incorrect number of arguments) : " ;
      PrintTree( walk, false, false, count, false, false, 0 ) ;
      ClearTree( walk ) ;

    } // else if

    else if ( type == NON_FUNCTION ) {

      cout << "ERROR (attempt to apply non-function) : " ;
      PrintTree( walk, false, false, count, false, false, 0 ) ;
      ClearTree( walk ) ;

    } // else if

    else if ( type == NO_RETURN_VALUE ) {

      cout << "ERROR (no return value) : " ;
      PrintTree( walk, false, false, count, false, false, 0 ) ;
      ClearTree( walk ) ;

    } // else if

    else if ( type == DEFINE_FORMAT ) {

      cout << "ERROR (DEFINE format) : " ;
      PrintTree( walk, false, false, count, false, false, 0 ) ;
      ClearTree( walk ) ;

    } // else if
    
    else if ( type == COND_FORMAT ) {

      cout << "ERROR (COND format) : " ;
      PrintTree( walk, false, false, count, false, false, 0 ) ;
      ClearTree( walk ) ;

    } // else if
    
    cout << endl ;
    
    gLine = 1 ;
    gCol = 0 ;

  } // Exception()

  Exception( char errFun[100], NodePtr errArg ) {

    int count = 0 ;

    NodePtr tempErrArg = errArg ;

    cout << "ERROR (" << errFun << " with incorrect argument type) : " ;
    PrintTree( tempErrArg, false, false, count, false, false, 0 ) ;
    ClearTree( tempErrArg ) ;
    
    cout << endl ;
    
    gLine = 1 ;
    gCol = 0 ;

  } // Exception()

} ; // class Exception

// ================================    main

int main() {

  int pos = 0 ;
  int uTestNum = 0 ;
  int count = 0 ;
  bool firstQuote = false ;
  bool isLP = false ;
  char temp = '\0' ;

  cin >> uTestNum ;

  Scanner scan = Scanner() ;
  Parser par = Parser() ;

  NodePtr head = new Node() ;
  NodePtr result = NULL ;

  cout << "Welcome to OurScheme!" << endl << endl ;

  try {

    isLP = scan.GetToken( isLP ) ;

  } catch ( Exception e ) {

    scan.mtokenString.clear() ;

  } // catch

  while ( !gEnd ) {

    result = NULL ;
    head = NULL ;
    pos = 0 ;
    count = 0 ;

    try {

      if ( par.S_exp( scan.mtokenString, false ) ) {

        if ( scan.mbuffer == '\n' ) {

          gCol = 0 ;
          gLine = 1 ;

        } // if

        else if ( scan.mbuffer == ';' ) {

          scan.mbuffer = GetNextChar() ;

          while ( scan.mbuffer != '\n' && scan.mbuffer != '\0' )
            scan.mbuffer = GetNextChar() ;

          gCol = 0 ;
          gLine = 1 ;

        } // else if

        else if ( Isspace( scan.mbuffer ) ) {

          gCol = 1 ;

          scan.mbuffer = GetNextChar() ;

          while ( ( scan.mbuffer == ' ' || scan.mbuffer == '\t' ) && scan.mbuffer != '\0' )
            scan.mbuffer = GetNextChar() ;

          if ( scan.mbuffer == '\n' ) {

            gCol = 0 ;
            gLine = 1 ;

          } // if

          else if ( scan.mbuffer == ';' ) {

            scan.mbuffer = GetNextChar() ;

            while ( scan.mbuffer != '\n' && scan.mbuffer != '\0' )
              scan.mbuffer = GetNextChar() ;

            gCol = 0 ;
            gLine = 1 ;

          } // else if

          else {

            gLine = 1 ;

          } // else

        } // else if

        else {

          gCol = 1 ;
          gLine = 1 ;

        } // else

        cout << "> " ;

        par.ms_exp = par.S_expTrans( scan.mtokenString, firstQuote, false ) ;

        // for ( int i = 0 ; i < par.ms_exp.size() ; i++ )
          // cout << par.ms_exp[i].mtoken.data() ;

        // cout << "\n" ;

        head = par.BuildTree( head, par.ms_exp, pos ) ;
        // PrintTree( head, false, false, count, firstQuote, false, 0 ) ;

        result = Eval( head, false, true ) ;

        if ( result == NULL ) {

          throw Exception( head, NO_RETURN_VALUE ) ;

        } // if

        else if ( result->mtoken.mtype == OPERATOR ) {

          cout << "#<procedure " << result->mtoken.mtoken.data() << ">" << endl ;

          ClearTree( head ) ;

        } // else if

        else if ( result->mtoken.mtype == CLEAN )
          ClearTree( head ) ;
          
        else if ( result->mleft != NULL && result->mleft->mtoken.mtype == QUOTE ) {

          PrintTree( result, false, false, count, true, false, 0 ) ;

          ClearTree( head ) ;

        } // else if

        else if ( result->mtoken.mtype != DEFINE ) {

          PrintTree( result, false, false, count, false, false, 0 ) ;

          ClearTree( head ) ;

        } // else if

        cout << "\n" ;

        scan.mtokenString.clear() ;
        par.ms_exp.clear() ;
        par.mtokenCount = 0 ;
        firstQuote = false ;

      } // if

      else
        par.mtokenCount = 0 ;

      isLP = scan.GetToken( isLP ) ;

    } catch ( Exception e ) {

      scan.mtokenString.clear() ;

    } // catch

  } // while

  if ( gEOF )
    cout << "Thanks for using OurScheme!" ;

  else
    cout << endl << "Thanks for using OurScheme!" ;

} // main()

// ================================    funtion implement

bool Isspace( char temp ) {

  if ( temp == ' ' || temp == '\n' || temp == '\t' )
    return true ;

  return false ;

} // Isspace()

bool Isalpha( char temp ) {

  if ( ( temp >= 'a' && temp <= 'z' ) || ( temp >= 'A' && temp <= 'Z' ) )
    return true ;

  return false ;

} // Isalpha()

bool Isdigit( char temp ) {

  if ( temp >= '0' && temp <= '9' )
    return true ;

  return false ;

} // Isdigit()

void PrintEnumStr( int type ) {

  if ( type == EMPTY )
    cout << "EMPTY" << endl ;

  else if ( type == LP )
    cout << "LP" << endl ;

  else if ( type == RP )
    cout << "RP" << endl ;

  else if ( type == INT )
    cout << "INT" << endl ;

  else if ( type == STRING )
    cout << "STRING" << endl ;

  else if ( type == DOT )
    cout << "DOT" << endl ;

  else if ( type == FLOAT )
    cout << "FLOAT" << endl ;

  else if ( type == NIL )
    cout << "NIL" << endl ;

  else if ( type == T )
    cout << "T" << endl ;

  else if ( type == QUOTE )
    cout << "QUOTE" << endl ;

  else if ( type == SYMBOL )
    cout << "SYMBOL" << endl ;

  else if ( type == LC )
    cout << "LC" << endl ;

  else if ( type == OPERATOR )
    cout << "OPERATOR" << endl ;

} // PrintEnumStr()

char GetNextChar() {

  char next = '\0' ;
  int eof = 0 ;

  eof = scanf( "%c", &next ) ;

  gCol++ ;

  gErrCol = gCol ;
  gErrLine = gLine ;

  if ( next == '\n' ) {

    gLine++ ;
    gCol = 0 ;

  } // if encounter a newline

  if ( eof == -1 )
    return '\0' ;

  return next ;

} // GetNextChar()

char GetNextNonSpaceChar() {

  char next = GetNextChar() ;

  while ( Isspace( next ) && next != '\0' )
    next = GetNextChar() ;

  return next ;

} // GetNextNonSpaceChar()

// ============================    class Scanner implementation

bool Scanner::GetToken( bool lp ) {

  bool escape = false ;
  bool isLP = false ;
  int haveDot = 0 ;

  gCL_AfterDot = false ;

  Token tempToken = Token() ;

  if ( gError ) {

    mbuffer = '\0' ;
    gError = false ;

  } // if

  if ( gNextBuf != '\0' ) {

    mbuffer = gNextBuf ;
    gNextBuf = '\0' ;

  } // if

  if ( Isspace( mbuffer ) )
    mbuffer = GetNextNonSpaceChar() ;

  else if ( mbuffer == '\0' )
    mbuffer = GetNextNonSpaceChar() ;

  if ( mbuffer == '\0' )
    throw Exception( EEOF ) ;

  if ( mbuffer == '(' ) {

    tempToken.mtoken.push_back( mbuffer ) ;
    tempToken.mtype = LP ;
    tempToken.mcol = gCol ;
    tempToken.mline = gLine ;

    mbuffer = GetNextNonSpaceChar() ;

    if ( mbuffer == ')' ) {

      tempToken.mtoken[0] = 'f' ;
      tempToken.mtype = NIL ;
      mbuffer = GetNextChar() ;

    } // if

    isLP = true ;

    tempToken.mtoken.push_back( '\0' ) ;

  } // if LP

  else if ( mbuffer == ')' ) {

    tempToken.mtoken.push_back( mbuffer ) ;
    tempToken.mtype = RP ;
    tempToken.mcol = gCol ;
    tempToken.mline = gLine ;
    mbuffer = GetNextChar() ;

    tempToken.mtoken.push_back( '\0' ) ;

  } // else if RP

  else if ( mbuffer == '\'' ) {

    tempToken.mtoken.push_back( mbuffer ) ;
    tempToken.mtype = QUOTE ;
    tempToken.mcol = gCol ;
    tempToken.mline = gLine ;
    mbuffer = '\0' ;

    tempToken.mtoken.push_back( '\0' ) ;

  } // else if QUOTE

  else if ( mbuffer == '\"' ) {

    tempToken.mtoken.push_back( mbuffer ) ;
    tempToken.mtype = STRING ;
    tempToken.mcol = gCol ;
    tempToken.mline = gLine ;

    mbuffer = GetNextChar();

    while ( ( mbuffer != '\"' && mbuffer != '\n' && mbuffer != '\0' ) ||
            ( mbuffer == '\"' && escape ) ) {

      if ( mbuffer == '\\' ) {

        mbuffer = GetNextChar() ;
        escape = true ;

      } // if

      if ( escape && mbuffer == 'n' ) {

        escape = false ;
        tempToken.mtoken.push_back( '\n' ) ;

      } // if

      else if ( escape && mbuffer == '\"' ) {

        escape = false ;
        tempToken.mtoken.push_back( mbuffer ) ;

      } // else if

      else if ( escape && mbuffer == '\t' ) {

        escape = false ;
        tempToken.mtoken.push_back( '\t' ) ;

      } // else if

      else if ( escape && mbuffer == '\\' ) {

        escape = false ;
        tempToken.mtoken.push_back( mbuffer ) ;

      } // else if

      else if ( escape ) {

        escape = false ;
        tempToken.mtoken.push_back( '\\' ) ;
        tempToken.mtoken.push_back( mbuffer ) ;

      } // else if

      else
        tempToken.mtoken.push_back( mbuffer ) ;

      mbuffer = GetNextChar() ;

    } // while

    if ( mbuffer == '\"' ) {

      tempToken.mtoken.push_back( mbuffer ) ;

      mbuffer = GetNextChar() ;

    } // if

    else if ( mbuffer == '\n' )
      throw Exception( EOL ) ;

    tempToken.mtoken.push_back( '\0' ) ;

  } // else if STRING

  else if ( mbuffer == ';' ) {

    tempToken.mtoken.push_back( mbuffer ) ;
    tempToken.mcol = gCol ;
    tempToken.mline = gLine ;

    mbuffer = GetNextChar() ;

    while ( mbuffer != '\0' && mbuffer != '\n' ) {

      // if the first char is comment
      // after this char should be comment

      tempToken.mtoken.push_back( mbuffer ) ;
      mbuffer = GetNextChar() ;

    } // while

    tempToken.mtoken.push_back( '\0' ) ;

    tempToken.mtype = LC ;
    mbuffer = '\0' ;

  } // else if LC

  else if ( Isalpha( mbuffer ) ) {

    tempToken.mtoken.push_back( mbuffer ) ;
    tempToken.mtype = SYMBOL ;
    tempToken.mcol = gCol ;
    tempToken.mline = gLine ;

    mbuffer = GetNextChar() ;

    while ( !Isspace( mbuffer ) &&
            mbuffer != '(' && mbuffer != ')' &&
            mbuffer != ';' && mbuffer != '\0' &&
            mbuffer != '\'' && mbuffer != '\"' ) {

      // the first char is alpha
      // this token might be symbol

      tempToken.mtoken.push_back( mbuffer ) ;
      tempToken.mtype = SYMBOL ;

      mbuffer = GetNextChar() ;

    } // while

    tempToken.mtoken.push_back( '\0' ) ;

    if ( strcmp( tempToken.mtoken.data(), "nil" ) == 0 )
      tempToken.mtype = NIL ;

    else if ( strcmp( tempToken.mtoken.data(), "t" ) == 0 )
      tempToken.mtype = T ;

    else if ( strcmp( tempToken.mtoken.data(), "quote" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "cons" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "list" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "define" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "car" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "cdr" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "atom?" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "pair?" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "list?" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "null?" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "integer?" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "real?" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "number?" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "string?" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "boolean?" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "symbol?" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "not" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "and" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "or" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "string-append" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "string>?" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "string<?" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "string=?" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "eqv?" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "equal?" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "begin" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "if" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "cond" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "clean-environment" ) == 0 )
      tempToken.mtype = OPERATOR ;

    else if ( strcmp( tempToken.mtoken.data(), "exit" ) == 0 )
      tempToken.mtype = OPERATOR ;

  } // else if firsr char is alpha

  else if ( Isdigit( mbuffer ) ) {

    tempToken.mtoken.push_back( mbuffer ) ;
    tempToken.mtype = INT ;
    tempToken.mcol = gCol ;
    tempToken.mline = gLine ;

    mbuffer = GetNextChar() ;

    while ( !Isspace( mbuffer ) &&
            mbuffer != '(' && mbuffer != ')' &&
            mbuffer != ';' && mbuffer != '\0' &&
            mbuffer != '\'' && mbuffer != '\"' ) {

      if ( Isalpha( mbuffer ) ) {

        // the first char is digit
        // appear a char is alpha
        // this token might be a symbol

        tempToken.mtoken.push_back( mbuffer ) ;
        tempToken.mtype = SYMBOL ;
        mbuffer = '\0' ;

      } // if

      else if ( mbuffer == '.' ) {

        // the first char is digit
        // appear a char is dot
        // and this token is not a symbol
        // this token might be a float

        tempToken.mtoken.push_back( mbuffer ) ;
        haveDot++ ;

        if ( haveDot == 1 && tempToken.mtype == INT )
          tempToken.mtype = FLOAT ;

        else
          tempToken.mtype = SYMBOL ;

      } // else if

      else if ( Isdigit( mbuffer ) ) {

        // the first token is digit
        // appear a char still a digit
        // and this token is not symbol or float
        // this token might be a INT

        tempToken.mtoken.push_back( mbuffer ) ;
        mbuffer = '\0' ;

      } // else if

      else {

        // encounter the other case
        // this token might be a symbol

        tempToken.mtoken.push_back( mbuffer ) ;
        tempToken.mtype = SYMBOL ;
        mbuffer = '\0' ;

      } // else

      mbuffer = GetNextChar() ;

    } // while there is not a space

    tempToken.mtoken.push_back( '\0' ) ;

  } // else if first char is a digit

  else if ( mbuffer == '.' ) {

    tempToken.mtoken.push_back( mbuffer ) ;
    tempToken.mcol = gCol ;
    tempToken.mline = gLine ;

    mbuffer = GetNextChar() ;

    if ( Isspace( mbuffer ) || mbuffer == '(' || mbuffer == '\"' || mbuffer == ')' ) {

      // the first char is a dot
      // and it stands alone
      // this token might be a dot

      if ( mbuffer == '\n' )
        gCL_AfterDot = true ;

      else
        gCL_AfterDot = false ;

      tempToken.mtype = DOT ;

    } // if

    while ( !Isspace( mbuffer ) &&
            mbuffer != '(' && mbuffer != ')' &&
            mbuffer != ';' && mbuffer != '\0' &&
            mbuffer != '\'' && mbuffer != '\"' ) {

      if ( Isalpha( mbuffer ) ) {

        // the first token is a dot
        // and encounter a alpha
        // this token might be a symbol

        tempToken.mtoken.push_back( mbuffer ) ;
        tempToken.mtype = SYMBOL ;
        mbuffer = '\0' ;

      } // if

      else if ( Isdigit( mbuffer ) ) {

        // the first token is a dot
        // and encounter a digit
        // and this token still not defined a type
        // this token might be a float

        tempToken.mtoken.push_back( mbuffer ) ;

        if ( tempToken.mtype == EMPTY )
          tempToken.mtype = FLOAT ;

        mbuffer = '\0' ;

      } // else if

      else {

        // encounter the other case
        // this token might be a symbol

        tempToken.mtoken.push_back( mbuffer ) ;
        tempToken.mtype = SYMBOL ;
        mbuffer = '\0' ;

      } // else

      mbuffer = GetNextChar() ;

      if ( tempToken.mtype == EMPTY )
        tempToken.mtype = SYMBOL ;

    } // while

    tempToken.mtoken.push_back( '\0' ) ;

  } // else if first char is a dot

  else if ( mbuffer == '#' ) {

    tempToken.mtoken.push_back( mbuffer ) ;
    tempToken.mtype = SYMBOL ;
    tempToken.mcol = gCol ;
    tempToken.mline = gLine ;

    mbuffer = GetNextChar() ;

    while ( !Isspace( mbuffer ) &&
            mbuffer != '(' && mbuffer != ')' &&
            mbuffer != ';' && mbuffer != '\0' &&
            mbuffer != '\'' && mbuffer != '\"' ) {

      // the first char is a sharp
      // this token might be a symbol

      tempToken.mtoken.push_back( mbuffer ) ;
      tempToken.mtype = SYMBOL ;

      mbuffer = GetNextChar() ;

    } // while

    tempToken.mtoken.push_back( '\0' ) ;

    if ( strcmp( tempToken.mtoken.data(), "#t" ) == 0 )
      tempToken.mtype = T ;

    else if ( strcmp( tempToken.mtoken.data(), "#f" ) == 0 )
      tempToken.mtype = NIL ;

  } // else if first char is a sharp

  else if ( mbuffer == '+' || mbuffer == '-' ) {

    tempToken.mtoken.push_back( mbuffer ) ;
    tempToken.mcol = gCol ;
    tempToken.mline = gLine ;

    mbuffer = GetNextChar() ;

    while ( !Isspace( mbuffer ) &&
            mbuffer != '(' && mbuffer != ')' &&
            mbuffer != ';' && mbuffer != '\0' &&
            mbuffer != '\'' && mbuffer != '\"' ) {

      if ( mbuffer == '.' ) {

        // the first token is '+'
        // and encounter a '.'
        // this token might be a float

        tempToken.mtoken.push_back( mbuffer ) ;
        haveDot++ ;

        if ( haveDot == 1 && tempToken.mtype == INT )
          tempToken.mtype = FLOAT ;

        else if ( haveDot > 1 )
          tempToken.mtype = SYMBOL ;

        mbuffer = '\0' ;

      } // if

      else if ( Isdigit( mbuffer ) ) {

         // the first token is '+'
         // and encounter a digit
         // and this token still not decide a type
         // this token might be a INT

        tempToken.mtoken.push_back( mbuffer ) ;

        if ( haveDot == 1 && tempToken.mtype != SYMBOL )
          tempToken.mtype = FLOAT ;

        else if ( tempToken.mtype != SYMBOL )
          tempToken.mtype = INT ;

        else
          tempToken.mtype = SYMBOL ;

        mbuffer = '\0' ;

      } // else if

      else {

        // the other case this token might be a symbol

        tempToken.mtoken.push_back( mbuffer ) ;
        tempToken.mtype = SYMBOL ;

        mbuffer = '\0' ;

      } // else

      mbuffer = GetNextChar() ;

    } // while

    tempToken.mtoken.push_back( '\0' ) ;

    if ( tempToken.mtype == EMPTY )
      tempToken.mtype = OPERATOR ;

  } // else if first char is a '+'

  else {

    // no matter encounter what
    // this token must be a SYMBOL

    tempToken.mtoken.push_back( mbuffer ) ;
    tempToken.mtype = SYMBOL ;
    tempToken.mcol = gCol ;
    tempToken.mline = gLine ;

    mbuffer = GetNextChar() ;

    while ( !Isspace( mbuffer ) &&
            mbuffer != '(' && mbuffer != ')' &&
            mbuffer != ';' && mbuffer != '\0' &&
            mbuffer != '\'' && mbuffer != '\"' ) {

      tempToken.mtoken.push_back( mbuffer ) ;

      mbuffer = GetNextChar() ;

    } // while

    tempToken.mtoken.push_back( '\0' ) ;

  } // else first char is other case

  if ( strcmp( tempToken.mtoken.data(), "*" ) == 0 )
    tempToken.mtype = OPERATOR ;

  else if ( strcmp( tempToken.mtoken.data(), "/" ) == 0 )
    tempToken.mtype = OPERATOR ;

  else if ( strcmp( tempToken.mtoken.data(), ">" ) == 0 )
    tempToken.mtype = OPERATOR ;

  else if ( strcmp( tempToken.mtoken.data(), ">=" ) == 0 )
    tempToken.mtype = OPERATOR ;

  else if ( strcmp( tempToken.mtoken.data(), "=" ) == 0 )
    tempToken.mtype = OPERATOR ;

  else if ( strcmp( tempToken.mtoken.data(), "<" ) == 0 )
    tempToken.mtype = OPERATOR ;

  else if ( strcmp( tempToken.mtoken.data(), "<=" ) == 0 )
    tempToken.mtype = OPERATOR ;

  if ( lp && tempToken.mtype == QUOTE )
    tempToken.mtype = EXECUTE_QUOTE ;

  if ( tempToken.mtype != LC )
    mtokenString.push_back( tempToken ) ;

  return isLP ;

} // Scanner::GetToken()

// ============================    class Parser implementation

bool Parser::Atom( vector<Token> tokenString ) {

  int tempToken = mtokenCount ;

  if ( tempToken >= tokenString.size() ) {

    mtokenCount = tempToken ;

    return false ;

  } // if

  else if ( tokenString[tempToken].mtype == SYMBOL ) {

    mtokenCount++ ;

    return true ;

  } // else if

  else if ( tokenString[tempToken].mtype == INT ) {

    mtokenCount++ ;

    return true ;

  } // else if

  else if ( tokenString[tempToken].mtype == FLOAT ) {

    mtokenCount++ ;

    return true ;

  } // else if

  else if ( tokenString[tempToken].mtype == STRING ) {

    mtokenCount++ ;

    return true ;

  } // else if

  else if ( tokenString[tempToken].mtype == NIL ) {

    mtokenCount++ ;

    return true ;

  } // else if

  else if ( tokenString[tempToken].mtype == T ) {

    mtokenCount++ ;

    return true ;

  } // else if

  else if ( tokenString[tempToken].mtype == LP ) {

    if ( tempToken + 1 < tokenString.size() && tokenString[tempToken+1].mtype == RP ) {

      mtokenCount = tempToken + 2 ;

      return true ;

    } // if

    else
      return false ;

  } // else if

  else if ( tokenString[tempToken].mtype == OPERATOR ) {

    mtokenCount++ ;

    return true ;

  } // else if

  else {

    return false ;

  } // else

} // Parser::Atom()

bool Parser::S_exp( vector<Token> tokenString, bool recursion ) {

  bool moreS_exp = false ;
  int tempCount = mtokenCount ;

  if ( tempCount >= tokenString.size() ) {

    mtokenCount = tempCount ;

    return false ;

  } // if

  else if ( Atom( tokenString ) )
    return true ;

  else if ( tokenString[tempCount].mtype == LP ) {

    tempCount++ ;

    if ( tempCount < tokenString.size() ) {

      mtokenCount = tempCount ;

      if ( S_exp( tokenString, true ) ) {

        if ( mtokenCount >= tokenString.size() ) {

          return false ;

        } // if

        tempCount = mtokenCount ;

        while ( S_exp( tokenString, true ) )
          moreS_exp = true ;

        if ( moreS_exp )
          tempCount = mtokenCount ;

        if ( tempCount < tokenString.size() ) {

          if ( tokenString[tempCount].mtype == DOT ) {

            tempCount++ ;

            if ( tempCount >= tokenString.size() ) {

              mtokenCount = tempCount ;

              return false ;

            } // if

            mtokenCount = tempCount ;

            if ( !S_exp( tokenString, true ) ) {

              // this token string is not meet grammar
              // there is an excpetion here

              // if ( !recursion && mtokenCount < tokenString.size() )
              // throw Exception( tokenString[mtokenCount], EXP_ATOM ) ;

              if ( mtokenCount < tokenString.size() )
                throw Exception( tokenString[mtokenCount], EXP_ATOM ) ;

              return false ;

            } // if

          } // if

        } // if

        else {

          mtokenCount = tempCount ;

          return false ;

        } // else

        if ( tempCount >= tokenString.size() ) {

          mtokenCount = tempCount ;

          return false ;

        } // if

        if ( mtokenCount < tokenString.size() && tokenString[mtokenCount].mtype == RP ) {

          mtokenCount++ ;

          return true ;

        } // if

        else {

          // this token string is not meet grammar
          // there is an excpetion here

          // if ( !recursion && mtokenCount < tokenString.size() )
          // throw Exception( tokenString[mtokenCount], EXP_RP ) ;


          if ( mtokenCount < tokenString.size() )
            throw Exception( tokenString[mtokenCount], EXP_RP ) ;

          return false ;

        } // else

      } // if

      else {

        // this token string is not meet grammar
        // there is an excpetion here


        // if ( !recursion && mtokenCount < tokenString.size() )
        // throw Exception( tokenString[mtokenCount], EXP_ATOM ) ;

        if ( mtokenCount < tokenString.size() )
          throw Exception( tokenString[mtokenCount], EXP_ATOM ) ;

        return false ;

      } // else

    } // if

    else {

      mtokenCount = tempCount ;

      return false ;

    } // else

  } // else if

  else if ( mtokenCount < tokenString.size() &&
            ( tokenString[mtokenCount].mtype == QUOTE ||
              tokenString[mtokenCount].mtype == EXECUTE_QUOTE ) ) {

    tempCount++ ;

    if ( tempCount < tokenString.size() ) {

      mtokenCount = tempCount ;

      if ( S_exp( tokenString, true ) ) {

        return true ;

      } // if

      else {

        // this token string is not meet grammar
        // there is an excpetion here

        // if ( !recursion && mtokenCount < tokenString.size() )
        // throw Exception( tokenString[mtokenCount], EXP_ATOM ) ;

        if ( mtokenCount < tokenString.size() )
          throw Exception( tokenString[mtokenCount], EXP_ATOM ) ;

        return false ;

      } // else

    } // if

    else {

      mtokenCount = tempCount ;
      return false ;

    } // else

  } // else if

  else {

    // this token string is not meet grammar
    // there is an exception here

    if ( !recursion && tempCount < tokenString.size() )
      throw Exception( tokenString[tempCount], EXP_ATOM ) ;

    else if ( tempCount < tokenString.size() &&
              tokenString[tempCount-1].mtype == LP &&
              tokenString[tempCount].mtype == DOT )
      throw Exception( tokenString[tempCount], EXP_ATOM ) ;

    return false ;

  } // else

} // Parser::S_exp()

vector<Token> Parser::S_expTrans( vector<Token> tokenString, bool &firstQuote, bool req ) {

  int numOfLP = 0 ;
  int numOfRP = 0 ;
  int setLP = 0 ;
  bool noDot = true ;
  bool notFirst = true ;

  Token leftParen = Token( LP, '(' ) ;
  Token rightParen = Token( RP, ')' ) ;
  Token dot = Token( DOT, '.' ) ;
  Token nil = Token( NIL, 'f' ) ;

  vector<Token> tokenStringSeg ;
  vector<Token> transTokenString ;
  vector<Token> finalTokenString ;

  if ( tokenString.size() == 1 )
    return tokenString ;

  else if ( !req && tokenString.size() == 3 && strcmp( tokenString[1].mtoken.data(), "exit" ) == 0 )
    throw Exception( END ) ;

  else if ( tokenString.size() == 5 &&
            strcmp( tokenString[1].mtoken.data(), "exit" ) == 0 &&
            tokenString[3].mtype == NIL )
    throw Exception( END ) ;

  if ( tokenString[0].mtype == QUOTE )
    firstQuote = true ;

  if ( tokenString[0].mtype != LP ) {

    tokenString.insert( tokenString.begin(), leftParen ) ;
    tokenString.insert( tokenString.end(), rightParen ) ;

  } // if

  for ( int i = 1 ; i < tokenString.size() - 1 ; i++ ) {

    // the original tokenString which is meet the grammar

    if ( tokenString[i].mtype == LP ) {

      numOfLP++ ;

      tokenStringSeg.push_back( tokenString[i] ) ;

      // segment the element between '(' and ')'

      while ( numOfLP != numOfRP && i < tokenString.size() ) {

        i++ ;

        if ( tokenString[i].mtype == LP )
          numOfLP++ ;

        if ( tokenString[i].mtype == RP )
          numOfRP++ ;

        tokenStringSeg.push_back( tokenString[i] ) ;

      } // while

      numOfLP = 0 ;
      numOfRP = 0 ;

      // the segment of token string
      // should be translate into  ( ¡¼ . ( ¡º . ( ¡· . ( ¡½ . ¡º ))))

      // for ( int j = 0 ; j < tokenStringSeg.size() ; j++ )
        // PrintEnumStr( tokenStringSeg[j].mtype ) ;

      tokenStringSeg = S_expTrans( tokenStringSeg, notFirst, true ) ;

      for ( int j = 0 ; j < tokenStringSeg.size() - 1 ; j++ )
        transTokenString.push_back( tokenStringSeg[j] ) ;

      tokenStringSeg.clear() ;

      // after translate the segment
      // index will stop at ')' and forget to see it
      // so go back one index
      // the next round will see ')'

      i-- ;

    } // if

    else if ( tokenString[i].mtype == QUOTE ) {

      transTokenString.push_back( tokenString[i] ) ;
      transTokenString.push_back( dot ) ;

    } // else if

    else if ( tokenString[i+1].mtype == DOT ) {

      // peek the next token is a dot
      // this expression has a dot
      // do not need to add another dot

      transTokenString.push_back( tokenString[i] ) ;

      noDot = false ;

    } // else if

    else if ( tokenString[i].mtype == DOT ) {

      // this token is a dot
      // after this token should not be a dot
      // do not need to add another dot

      transTokenString.push_back( tokenString[i] ) ;

      noDot = false ;

    } // else if

    // the size of tokenString -1 is ')'
    // the size of tokenString -2 is last meaningful token
    // don't need to add a dot after last token

    else if ( i == tokenString.size() - 2 )
      transTokenString.push_back( tokenString[i] ) ;

    else {

      // between each element shhould have a dot

      transTokenString.push_back( tokenString[i] ) ;
      transTokenString.push_back( dot ) ;

    } // else

  } // for

  if ( noDot ) {

    // there is no dot in this expression
    // should add a dot and a nil token

    transTokenString.push_back( dot ) ;
    transTokenString.push_back( nil ) ;

  } // if

  // cout << "each element : " ;

  // for ( int i = 0 ; i < transTokenString.size() ; i++ )
    // cout << transTokenString[i].mtoken.data() ;

  finalTokenString.push_back( leftParen ) ;
  setLP++ ;

  for ( int i = 0 ; i < transTokenString.size() ; i++ ) {

    if ( transTokenString[i].mtype == LP ) {

      numOfLP++ ;

      finalTokenString.push_back( transTokenString[i] ) ;

      // between '(' and ')' should be seen as a element
      // no dot need add any left paren into a element

      while ( numOfLP != numOfRP && i < transTokenString.size() ) {

        i++ ;

        if ( transTokenString[i].mtype == LP )
          numOfLP++ ;

        if ( transTokenString[i].mtype == RP )
          numOfRP++ ;

        finalTokenString.push_back( transTokenString[i] ) ;

      } // while

      numOfLP = 0 ;
      numOfRP = 0 ;

    } // if

    // there is a dot which is the last dot
    // no dot need add left paren

    else if ( transTokenString[i].mtype == DOT &&
              i + 2 >= transTokenString.size() )
      finalTokenString.push_back( transTokenString[i] ) ;

    else if ( transTokenString[i].mtype == DOT &&
              i + 1 < transTokenString.size() &&
              transTokenString[i+1].mtype == LP ) {

      numOfLP++ ;

      finalTokenString.push_back( transTokenString[i] ) ;

      for ( int j = i + 2 ;
            numOfLP != numOfRP && j < transTokenString.size() ;
            j++ ) {

        if ( transTokenString[j].mtype == LP )
          numOfLP++ ;

        if ( transTokenString[j].mtype == RP )
          numOfRP++ ;

        if ( j + 2 >= transTokenString.size() )
          j += 2 ;

        if ( numOfLP == numOfRP ) {

          finalTokenString.push_back( leftParen ) ;

          setLP++ ;

        } // if

      } // for

      numOfLP = 0 ;
      numOfRP = 0 ;

    } // else if

    else if ( transTokenString[i].mtype == DOT ) {

      // there is a dot which is not the last dot
      // should add a left paren

      finalTokenString.push_back( transTokenString[i] ) ;
      finalTokenString.push_back( leftParen ) ;

      setLP++ ;

    } // else if

    else
      finalTokenString.push_back( transTokenString[i] ) ;

  } // for

  // add right paren to the bottom

  for ( int i = 0 ; i < setLP ; i++ )
    finalTokenString.push_back( rightParen ) ;

  return finalTokenString ;

} // Parser::S_expTrans()

NodePtr Parser::BuildTree( NodePtr head, vector<Token> tokenString, int &pos ) {

  NodePtr walk = NULL ;

  if ( pos >= tokenString.size() )
    return NULL ;

  else if ( head == NULL ) {

    head = new Node() ;

  } // else if

  walk = head ;

  if ( tokenString[pos].mtype == LP ) {

    pos++ ;

    walk->mleft = BuildTree( walk->mleft, tokenString, pos ) ;

  } // if

  if ( tokenString[pos].mtype == DOT ) {

    pos++ ;

    walk->mright = BuildTree( walk->mright, tokenString, pos ) ;

  } // if

  if ( tokenString[pos].mtype == RP ) {

    pos++ ;

    return head ;

  } // if

  else {

    walk->mtoken = tokenString[pos] ;

    pos++ ;

    return head ;

  } // else

} // Parser::BuildTree()

void PrintTree( NodePtr head, bool right, bool printLP, int &count,
                bool firstQuote, bool qt, int qtCount ) {

  // cout << "print Re \n" ;

  NodePtr walk = head ;
  bool pLP = false ;
  bool q = false ;

  if ( walk == NULL )
    ;

  else {

    if ( firstQuote )
      ;

    else if ( walk->mtoken.mtype == EMPTY && !right ) {

      if ( !printLP ) {

        for ( int i = 0 ; i < count ; i++ )
          printf( "  " ) ;

      } // if

      printf( "( " ) ;
      count++ ;

      pLP = true ;

    } // else if only a empty node and not at right

    if ( walk->mleft != NULL &&
         ( walk->mleft->mtoken.mtype == QUOTE || walk->mleft->mtoken.mtype == EXECUTE_QUOTE ) )
      q = true ;

    PrintTree( walk->mleft, false, pLP, count, false, q, 0 ) ;

    if ( walk->mtoken.mtype != EMPTY && !right ) {

      if ( !printLP ) {

        for ( int i = 0 ; i < count ; i++ )
          printf( "  " ) ;

      } // if

      if ( walk->mtoken.mtype == QUOTE || walk->mtoken.mtype == EXECUTE_QUOTE )
        printf( "( quote\n" ) ;

      else if ( walk->mtoken.mtype == FLOAT )
        printf( "%.3f\n", atof( walk->mtoken.mtoken.data() ) ) ;

      else if ( walk->mtoken.mtype == INT )
        printf( "%d\n", atoi( walk->mtoken.mtoken.data() ) ) ;

      else if ( walk->mtoken.mtype == T )
        cout << "#t\n" ;

      else if ( walk->mtoken.mtype == NIL )
        cout << "nil\n" ;

      else if ( walk->mtoken.mtype == OPERATOR_PRINT )
        cout << "#<procedure " << walk->mtoken.mtoken.data() << ">" << endl ;

      else
        cout << walk->mtoken.mtoken.data() << endl ;

    } // if this is a token

    else if ( walk->mtoken.mtype != EMPTY && right ) {

      if ( walk->mtoken.mtype != NIL ) {

        for ( int i = 0 ; i < count ; i++ )
          printf( "  " ) ;

        cout << ".\n" ;

        for ( int i = 0 ; i < count ; i++ )
          printf( "  " ) ;

        if ( walk->mtoken.mtype == FLOAT )
          printf( "%.3f\n", atof( walk->mtoken.mtoken.data() ) ) ;

        else if ( walk->mtoken.mtype == INT )
          printf( "%d\n", atoi( walk->mtoken.mtoken.data() ) ) ;

        else if ( walk->mtoken.mtype == T )
          cout << "#t\n" << endl ;

        else if ( walk->mtoken.mtype == OPERATOR_PRINT )
          cout << "#<procedure " << walk->mtoken.mtoken.data() << ">" << endl ;

        else
          cout << walk->mtoken.mtoken.data() << endl ;

      } // else if

    } // else if there is a token and at right

    if ( walk->mleft != NULL &&
         ( walk->mleft->mtoken.mtype == QUOTE || walk->mleft->mtoken.mtype == EXECUTE_QUOTE ) )
      count++ ;

    if ( walk->mleft != NULL && right && !q ) {

      for ( int i = 0 ; i < qtCount ; i++ ) {

        count-- ;

        for ( int i = 0 ; i < count ; i++ )
          printf( "  " ) ;

        cout << ")\n" ;

      } // for

    } // if

    if ( q )
      PrintTree( walk->mright, true, pLP, count, false, q, qtCount + 1 ) ;

    else
      PrintTree( walk->mright, true, pLP, count, false, q, 0 ) ;

    if ( pLP ) {

      count-- ;

      for ( int i = 0 ; i < count ; i++ )
        cout << "  " ;

      cout << ")\n" ;

    } // if

  } // else

} // PrintTree()

int NumOfParameter( NodePtr head ) {

  int numOfPara = 0 ;

  NodePtr walk = head ;

  while ( walk != NULL && walk->mright != NULL && walk->mright->mtoken.mtype != NIL ) {

    walk = walk->mright ;

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) ;

    else
      numOfPara++ ;

  } // while

  return numOfPara ;

} // NumOfParameter()

void ClearTree( NodePtr head ) {

  if ( head != NULL ) {

    ClearTree( head->mleft ) ;
    ClearTree( head->mright ) ;

    delete head ;

  } // if

} // ClearTree()

NodePtr CloneTree( NodePtr head, NodePtr root ) {

  NodePtr walk = NULL ;

  if ( root != NULL ) {

    if ( head == NULL ) {

      head = new Node() ;

    } // if

    walk = head ;
    walk->mtoken = root->mtoken ;

    walk->mleft = CloneTree( walk->mleft, root->mleft ) ;
    walk->mright = CloneTree( walk->mright, root->mright ) ;

  } // if

  return walk ;

} // CloneTree()

NodePtr Define( NodePtr head ) {

  SymTable sym = SymTable() ;
  bool haveSymbol = false ;
  bool samePointer = false ;
  int index = 0 ;

  NodePtr walk = head ;
  NodePtr result = new Node() ;
  NodePtr symbol = NULL ;
  NodePtr boundee = NULL ;
  
  result->mtoken.mtype = DEFINE ;

  if ( walk != NULL ) {

    symbol = walk->mleft ;
    
    walk = walk->mright ;

  } // if

  if ( walk != NULL ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE )
      boundee = Eval( walk, true, false ) ;

    else if ( walk->mleft != NULL )
      boundee = Eval( walk->mleft, true, false ) ;

  } // if

  for ( int i = 0 ; i < gSymTable.size() && !haveSymbol ; i++ ) {

    if ( strcmp( gSymTable[i].mSymbol.mtoken.data(), symbol->mtoken.mtoken.data() ) == 0 ) {

      haveSymbol = true ;
      
      index = i ;

    } // if

  } // for

  if ( haveSymbol ) {

    for ( int i = 0 ; i < gSymTable.size() - 1 && !samePointer ; i++ ) {

      if ( gSymTable[i].mBound == gSymTable[i+1].mBound )
        samePointer = true ;

    } // for

    // if ( !samePointer )
      // ClearTree( gSymTable[index].mBound ) ;

    gSymTable[index].mBound = boundee ;

  } // if

  else {

    sym.mSymbol = symbol->mtoken ;
    sym.mBound = boundee ;

    gSymTable.push_back( sym ) ;

  } // else

  cout << symbol->mtoken.mtoken.data() << " defined\n" ;

  return result ;

} // Define()

NodePtr EvnClean() {

  NodePtr result = new Node() ;
  result->mtoken.mtype = CLEAN ;

  for ( int i = 0 ; i < gSymTable.size() ; i++ ) {
    
    for ( int j = i ; j < gSymTable.size() ; j++ ) {

      if ( gSymTable[i].mBound == gSymTable[j].mBound )
        gSymTable[j].mBound = NULL ;

    } // for

    if ( gSymTable[i].mBound != NULL ) {
      
      ClearTree( gSymTable[i].mBound ) ;
      gSymTable[i].mBound = NULL ;
      
    } // if

  } // for

  gSymTable.clear() ;

  cout << "environment cleaned\n" ;

  return result ;

} // EvnClean()

NodePtr Cons( NodePtr head ) {

  NodePtr walk = head ;
  NodePtr result = new Node() ;

  if ( walk != NULL ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {
      
      result->mleft = Eval( walk, false, false ) ;
      
      walk = walk->mright ;
      
    } // if

    else
      result->mleft = Eval( walk->mleft, false, false ) ;

  } // if
  
  walk = walk->mright ;
  
  if ( walk != NULL ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

      result->mright = Eval( walk, false, false ) ;

      walk = walk->mright ;

    } // if

    else
      result->mright = Eval( walk->mleft, false, false ) ;

  } // if

  if ( result != NULL &&
       result->mleft != NULL &&
       result->mleft->mtoken.mtype == OPERATOR )
    result->mleft->mtoken.mtype = OPERATOR_PRINT ;

  if ( result != NULL &&
       result->mright != NULL &&
       result->mright->mtoken.mtype == OPERATOR )
    result->mright->mtoken.mtype = OPERATOR_PRINT ;

  return result ;

} // Cons()

NodePtr List( NodePtr head ) {

  // ############ need modify ?

  NodePtr result = new Node() ;
  NodePtr walkResult = result ;
  NodePtr walk = head ;

  while ( walk != NULL ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {
      
      walkResult->mleft = Eval( walk, false, false ) ;
      
      walk = walk->mright ;
      
    } // if

    else
      walkResult->mleft = Eval( walk->mleft, false, false ) ;

    if ( walkResult != NULL &&
         walkResult->mleft != NULL &&
         walkResult->mleft->mtoken.mtype == OPERATOR )
      walkResult->mleft->mtoken.mtype = OPERATOR_PRINT ;

    walkResult->mright = new Node() ;
    walkResult = walkResult->mright ;
    walk = walk->mright ;

  } // while

  if ( head->mtoken.mtype == NIL )
    result->mtoken.mtype = NIL ;

  return result ;

} // List()

NodePtr Quote( NodePtr head ) {

  NodePtr walk = head ;

  return walk->mright ;

} // Quote()

NodePtr Cdr( NodePtr head ) {

  char name[100] = "cdr\0" ;

  NodePtr walk = head ;
  NodePtr result = NULL ;

  if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE )
    result = Eval( walk, false, false ) ;

  else if ( walk->mleft != NULL )
    result = Eval( walk->mleft, false, false ) ;

  if ( IsPair( result, false )->mtoken.mtype == NIL )
    throw Exception( name, result ) ;

  return result->mright ;

} // Cdr()

NodePtr Car( NodePtr head ) {

  char name[100] = "car\0" ;

  NodePtr walk = head ;
  NodePtr result = NULL ;

  if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE )
    result = Eval( walk, false, false ) ;

  else if ( walk->mleft != NULL )
    result = Eval( walk->mleft, false, false ) ;

  if ( IsPair( result, false )->mtoken.mtype == NIL )
    throw Exception( name, result ) ;

  return result->mleft ;

} // Car()

NodePtr ElementIsAtom( NodePtr head ) {

  NodePtr walk = head ;
  NodePtr temp = new Node() ;

  if ( walk->mtoken.mtype == SYMBOL )
    temp->mtoken.mtype = T ;

  else if ( walk->mtoken.mtype == INT )
    temp->mtoken.mtype = T ;

  else if ( walk->mtoken.mtype ==  FLOAT )
    temp->mtoken.mtype = T ;

  else if ( walk->mtoken.mtype == STRING )
    temp->mtoken.mtype = T ;

  else if ( walk->mtoken.mtype == NIL )
    temp->mtoken.mtype = T ;

  else if ( walk->mtoken.mtype == T )
    temp->mtoken.mtype = T ;

  else
    temp->mtoken.mtype = NIL ;

  return temp ;

} // ElementIsAtom()

NodePtr IsAtom( NodePtr head ) {

  NodePtr walk = head ;
  NodePtr result = NULL ;
  NodePtr temp = NULL ;

  if ( walk != NULL ) {
    
    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE )
      result = Eval( walk, false, false ) ;
    
    else
      result = Eval( walk->mleft, false, false ) ;
    
  } // if

  temp = ElementIsAtom( result ) ;

  return temp ;

} // IsAtom()

NodePtr IsPair( NodePtr head, bool needEval ) {

  NodePtr walk = head ;
  NodePtr temp = new Node() ;
  NodePtr result = NULL ;

  if ( walk != NULL && needEval ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE )
      result = Eval( walk, false, false ) ;

    else
      result = Eval( walk->mleft, false, false ) ;

  } // if
  
  else if ( walk != NULL && !needEval )
    result = walk ;

  if ( result->mleft != NULL && result->mright != NULL )
    temp->mtoken.mtype = T ;

  else
    temp->mtoken.mtype = NIL ;

  return temp ;

} // IsPair()

NodePtr IsList( NodePtr head, bool needEval ) {

  NodePtr walk = head ;
  NodePtr temp = new Node() ;
  NodePtr result = NULL ;

  if ( walk != NULL && needEval ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE )
      result = Eval( walk, false, false ) ;

    else
      result = Eval( walk->mleft, false, false ) ;

  } // if

  else if ( walk != NULL && !needEval )
    result = walk ;

  while ( result != NULL && result->mright != NULL && result->mright->mright != NULL )
    result = result->mright ;

  if ( result != NULL && result->mright != NULL && result->mright->mtoken.mtype == NIL )
    temp->mtoken.mtype = T ;

  else
    temp->mtoken.mtype = NIL ;

  return temp ;

} // IsList()

NodePtr IsNull( NodePtr head ) {

  NodePtr walk = head ;
  NodePtr result = NULL ;
  NodePtr temp = new Node() ;

  if ( walk != NULL ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE )
      result = Eval( walk, false, false ) ;

    else
      result = Eval( walk->mleft, false, false ) ;

  } // if

  if ( result != NULL && result->mtoken.mtype == NIL )
    temp->mtoken.mtype = T ;

  else
    temp->mtoken.mtype = NIL ;

  return temp ;

} // IsNull()

NodePtr IsInteger( NodePtr head ) {

  NodePtr walk = head ;
  NodePtr result = NULL ;
  NodePtr temp = new Node() ;

  if ( walk != NULL ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE )
      result = Eval( walk, false, false ) ;

    else
      result = Eval( walk->mleft, false, false ) ;

  } // if

  if ( result != NULL && result->mtoken.mtype == INT )
    temp->mtoken.mtype = T ;

  else
    temp->mtoken.mtype = NIL ;

  return temp ;

} // IsInteger()

NodePtr IsReal( NodePtr head ) {

  NodePtr walk = head ;
  NodePtr result = NULL ;
  NodePtr temp = new Node() ;

  if ( walk != NULL ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE )
      result = Eval( walk, false, false ) ;

    else
      result = Eval( walk->mleft, false, false ) ;

  } // if

  if ( result != NULL && ( result->mtoken.mtype == INT || result->mtoken.mtype == FLOAT ) )
    temp->mtoken.mtype = T ;

  else
    temp->mtoken.mtype = NIL ;

  return temp ;

} // IsReal()

NodePtr IsNumber( NodePtr head ) {

  NodePtr walk = head ;
  NodePtr result = NULL ;
  NodePtr temp = new Node() ;

  if ( walk != NULL ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE )
      result = Eval( walk, false, false ) ;

    else
      result = Eval( walk->mleft, false, false ) ;

  } // if

  if ( result != NULL && ( result->mtoken.mtype == INT || result->mtoken.mtype == FLOAT ) )
    temp->mtoken.mtype = T ;

  else
    temp->mtoken.mtype = NIL ;

  return temp ;

} // IsNumber()

NodePtr IsString( NodePtr head ) {

  NodePtr walk = head ;
  NodePtr result = NULL ;
  NodePtr temp = new Node() ;

  if ( walk != NULL ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE )
      result = Eval( walk, false, false ) ;

    else
      result = Eval( walk->mleft, false, false ) ;

  } // if

  if ( result != NULL && result->mtoken.mtype == STRING )
    temp->mtoken.mtype = T ;

  else
    temp->mtoken.mtype = NIL ;

  return temp ;

} // IsString()

NodePtr IsBoolean( NodePtr head ) {

  NodePtr walk = head ;
  NodePtr result = NULL;
  NodePtr temp = new Node() ;

  if ( walk != NULL ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE )
      result = Eval( walk, false, false ) ;

    else
      result = Eval( walk->mleft, false, false ) ;

  } // if

  if ( result != NULL && ( result->mtoken.mtype == T || result->mtoken.mtype == NIL ) )
    temp->mtoken.mtype = T ;

  else
    temp->mtoken.mtype = NIL ;

  return temp ;

} // IsBoolean()

NodePtr IsSymbol( NodePtr head ) {

  NodePtr walk = head ;
  NodePtr result = NULL ;
  NodePtr temp = new Node() ;

  if ( walk != NULL ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE )
      result = Eval( walk, false, false ) ;

    else
      result = Eval( walk->mleft, false, false ) ;

  } // if

  if ( result != NULL && result->mtoken.mtype == SYMBOL )
    temp->mtoken.mtype = T ;

  else
    temp->mtoken.mtype = NIL ;

  return temp ;

} // IsSymbol()

NodePtr Add( NodePtr head ) {

  vector<float> f_array ;

  bool isFloat = false ;
  float evalResult = 0 ;
  int intEvalResult = 0 ;
  char error[100] = "+\0" ;
  char ans[100] = "\0" ;

  NodePtr walk = head ;
  NodePtr test = walk ;
  NodePtr eval = NULL ;
  NodePtr result = new Node() ;

  while ( test != NULL && test->mtoken.mtype != NIL ) {

    // to test if all the parameters are match to the protocal

    if ( test->mleft != NULL && test->mleft->mtoken.mtype == QUOTE ) {
      
      eval = Eval( test, false, false ) ;
      
      test = test->mright ;
      
    } // if
    
    else if ( test->mleft != NULL ) {

      eval = Eval( test->mleft, false, false ) ;

    } // else if

    if ( eval->mtoken.mtype != INT && eval->mtoken.mtype != FLOAT ) {

      delete result ;
      result = NULL ;

      throw Exception( error, eval ) ;

    } // if

    test = test->mright ;

  } // while

  while ( walk != NULL && walk->mtoken.mtype != NIL ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

      eval = Eval( walk, false, false ) ;

      walk = walk->mright ;

    } // if

    else if ( walk->mleft != NULL )
      eval = Eval( walk->mleft, false, false ) ;

    if ( eval->mtoken.mtype != INT && eval->mtoken.mtype != FLOAT ) {

      delete result ;
      result = NULL ;
      f_array.clear() ;

      throw Exception( error, eval ) ;

    } // if

    if ( eval != NULL ) {

      f_array.push_back( atof( eval->mtoken.mtoken.data() ) ) ;

      if ( eval->mtoken.mtype == FLOAT )
        isFloat = true ;

    } // if

    walk = walk->mright ;

  } // while
  
  for ( int i = 0 ; i < f_array.size() ; i++ )
    evalResult += f_array[i] ;

  if ( isFloat ) {

    result->mtoken.mtype = FLOAT ;
    sprintf( ans, "%f", evalResult ) ;

    for ( int i = 0 ; i < strlen( ans ) ; i++ )
      result->mtoken.mtoken.push_back( ans[i] ) ;

  } // if

  else {

    result->mtoken.mtype = INT ;
    intEvalResult = ( int ) evalResult ;
    sprintf( ans, "%d", intEvalResult ) ;

    for ( int i = 0 ; i < strlen( ans ) ; i++ )
      result->mtoken.mtoken.push_back( ans[i] ) ;

  } // else

  result->mtoken.mtoken.push_back( '\0' ) ;

  return result ;

} // Add()

NodePtr Sub( NodePtr head ) {

  bool isFloat = false ;
  bool isFirst = true ;
  char error[100] = "-\0" ;
  char ans[100] = "\0" ;
  float evalResult = 0 ;
  int intEvalResult = 0 ;

  NodePtr walk = head ;
  NodePtr eval = NULL ;
  NodePtr result = new Node() ;

  while ( walk != NULL && walk->mtoken.mtype != NIL ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

      eval = Eval( walk, false, false ) ;

      walk = walk->mright ;

    } // if

    else if ( walk->mleft != NULL )
      eval = Eval( walk->mleft, false, false ) ;

    if ( eval->mtoken.mtype != INT && eval->mtoken.mtype != FLOAT ) {

      delete result ;
      result = NULL ;

      throw Exception( error, eval ) ;

    } // if

    if ( isFirst && eval != NULL ) {

      evalResult = atof( eval->mtoken.mtoken.data() ) ;

      if ( eval->mtoken.mtype == FLOAT )
        isFloat = true ;

      isFirst = false ;

    } // if

    else if ( eval != NULL ) {

      evalResult -= atof( eval->mtoken.mtoken.data() ) ;

      if ( eval->mtoken.mtype == FLOAT )
        isFloat = true ;

    } // else if

    walk = walk->mright ;

  } // while

  if ( isFloat ) {

    result->mtoken.mtype = FLOAT ;
    sprintf( ans, "%f", evalResult ) ;

    for ( int i = 0 ; i < strlen( ans ) ; i++ )
      result->mtoken.mtoken.push_back( ans[i] ) ;

  } // if

  else {

    result->mtoken.mtype = INT ;
    intEvalResult = ( int ) evalResult ;
    sprintf( ans, "%d", intEvalResult ) ;

    for ( int i = 0 ; i < strlen( ans ) ; i++ )
      result->mtoken.mtoken.push_back( ans[i] ) ;

  } // else

  result->mtoken.mtoken.push_back( '\0' ) ;

  return result ;

} // Sub()

NodePtr Mult( NodePtr head ) {

  bool isFloat = false ;
  bool isFirst = true ;
  char error[100] = "*\0" ;
  char ans[100] = "\0" ;
  float evalResult = 0 ;
  int intEvalResult = 0 ;

  NodePtr walk = head ;
  NodePtr eval = NULL ;
  NodePtr result = new Node() ;

  while ( walk != NULL && walk->mtoken.mtype != NIL ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

      eval = Eval( walk, false, false ) ;

      walk = walk->mright ;

    } // if

    else if ( walk->mleft != NULL )
      eval = Eval( walk->mleft, false, false ) ;

    if ( eval->mtoken.mtype != INT && eval->mtoken.mtype != FLOAT ) {

      delete result ;
      result = NULL ;

      throw Exception( error, eval ) ;

    } // if

    if ( eval != NULL && isFirst ) {

      evalResult = atof( eval->mtoken.mtoken.data() ) ;

      if ( eval->mtoken.mtype == FLOAT )
        isFloat = true ;

      isFirst = false ;

    } // if

    else if ( eval != NULL ) {

      evalResult *= atof( eval->mtoken.mtoken.data() ) ;

      if ( eval->mtoken.mtype == FLOAT )
        isFloat = true ;

    } // else if

    walk = walk->mright ;

  } // while

  if ( isFloat ) {

    result->mtoken.mtype = FLOAT ;
    sprintf( ans, "%f", evalResult ) ;

    for ( int i = 0 ; i < strlen( ans ) ; i++ )
      result->mtoken.mtoken.push_back( ans[i] ) ;

  } // if

  else {

    result->mtoken.mtype = INT ;
    intEvalResult = ( int ) evalResult ;
    sprintf( ans, "%d", intEvalResult ) ;

    for ( int i = 0 ; i < strlen( ans ) ; i++ )
      result->mtoken.mtoken.push_back( ans[i] ) ;

  } // else

  result->mtoken.mtoken.push_back( '\0' ) ;

  return result ;

} // Mult()

NodePtr Div( NodePtr head ) {

  bool isFloat = false ;
  bool isFirst = true ;
  char error[100] = "/\0" ;
  char ans[100] = "\0" ;
  float evalResult = 0 ;
  int intEvalResult = 0 ;

  NodePtr walk = head ;
  NodePtr test = head ;
  NodePtr eval = NULL ;
  NodePtr result = new Node() ;

  while ( test != NULL ) {

    if ( test->mleft != NULL ) {

      if ( strcmp( Eval( test->mleft, false, false )->mtoken.mtoken.data(), "0" ) == 0 ) {

        delete result ;
        result = NULL ;

        throw Exception( DIV_ZERO ) ;

      } // if

    } // if

    test = test->mright ;

  } // while

  while ( walk != NULL && walk->mtoken.mtype != NIL ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

      eval = Eval( walk, false, false ) ;

      walk = walk->mright ;

    } // if

    else if ( walk->mleft != NULL )
      eval = Eval( walk->mleft, false, false ) ;

    if ( eval->mtoken.mtype != INT && eval->mtoken.mtype != FLOAT ) {

      delete result ;
      result = NULL ;

      throw Exception( error, eval ) ;

    } // if

    if ( eval != NULL && isFirst ) {

      evalResult = atof( eval->mtoken.mtoken.data() ) ;

      if ( eval->mtoken.mtype == FLOAT )
        isFloat = true ;

      isFirst = false ;

    } // if

    else if ( eval != NULL ) {

      evalResult /= atof( eval->mtoken.mtoken.data() ) ;

      if ( eval->mtoken.mtype == FLOAT )
        isFloat = true ;

    } // else if

    walk = walk->mright ;

  } // while

  if ( isFloat ) {

    result->mtoken.mtype = FLOAT ;
    sprintf( ans, "%f", evalResult ) ;

    for ( int i = 0 ; i < strlen( ans ) ; i++ )
      result->mtoken.mtoken.push_back( ans[i] ) ;

  } // if

  else {

    result->mtoken.mtype = INT ;
    intEvalResult = ( int ) evalResult ;
    sprintf( ans, "%d", intEvalResult ) ;

    for ( int i = 0 ; i < strlen( ans ) ; i++ )
      result->mtoken.mtoken.push_back( ans[i] ) ;

  } // else

  result->mtoken.mtoken.push_back( '\0' ) ;

  return result ;

} // Div()

NodePtr Not( NodePtr head ) {

  NodePtr walk = head ;
  NodePtr eval = NULL ;
  NodePtr result = new Node() ;

  if ( walk != NULL ) {

    if ( walk->mleft != NULL )
      eval = Eval( walk->mleft, false, false ) ;

    if ( eval != NULL ) {

      if ( eval->mtoken.mtype == NIL )
        result->mtoken.mtype = T ;

      else
        result->mtoken.mtype = NIL ;

    } // if

  } // if

  return result ;

} // Not()

// 31 regulation

NodePtr And( NodePtr head ) {

  bool isNil = false ;

  NodePtr walk = head ;
  NodePtr eval = NULL ;
  NodePtr result = new Node() ;

  while ( walk != NULL && !isNil ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

      eval = Eval( walk, false, false ) ;

      walk = walk->mright ;

    } // if

    else if ( walk->mleft != NULL )
      eval = Eval( walk->mleft, false, false ) ;

    if ( eval != NULL && eval->mtoken.mtype == NIL )
      isNil = true ;

    walk = walk->mright ;

  } // while

  if ( isNil )
    result->mtoken.mtype = NIL ;

  else
    result = eval ;

  return result ;

} // And()

NodePtr Or( NodePtr head ) {

  bool isT = false ;

  NodePtr walk = head ;
  NodePtr eval = NULL ;
  NodePtr result = new Node() ;

  while ( walk != NULL && !isT ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

      eval = Eval( walk, false, false ) ;

      walk = walk->mright ;

    } // if

    else if ( walk->mleft != NULL )
      eval = Eval( walk->mleft, false, false ) ;

    if ( eval != NULL && eval->mtoken.mtype != NIL )
      isT = true ;

    walk = walk->mright ;

  } // while

  if ( isT )
    result = eval ;

  else
    result->mtoken.mtype = NIL ;

  return result ;

} // Or()

NodePtr StringAppend( NodePtr head ) {

  char error[100] = "string-append\0" ;

  NodePtr walk = head ;
  NodePtr eval = NULL ;
  NodePtr result = new Node() ;

  result->mtoken.mtoken.push_back( '\"' ) ;

  while ( walk != NULL && walk->mtoken.mtype != NIL ) {
    
    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

      eval = Eval( walk, false, false ) ;

      walk = walk->mright ;

    } // if

    else if ( walk->mleft != NULL )
      eval = Eval( walk->mleft, false, false ) ;

    if ( eval != NULL && eval->mtoken.mtype != STRING ) {

      delete result ;
      result = NULL ;

      throw Exception( error, eval ) ;

    } // if

    else if ( walk->mleft != NULL ) {

      for ( int i = 1 ; i < eval->mtoken.mtoken.size() - 2 ; i++ )
        result->mtoken.mtoken.push_back( eval->mtoken.mtoken[i] ) ;

    } // else if

    walk = walk->mright ;

  } // while

  result->mtoken.mtype = STRING ;
  result->mtoken.mtoken.push_back( '\"' ) ;
  result->mtoken.mtoken.push_back( '\0' ) ;

  return result ;

} // StringAppend()

NodePtr BiggerThan( NodePtr head ) {

  NodePtr walk = head ;

  vector<float> f_array ;
  char error[100] = ">\0" ;
  NodePtr eval1 = NULL ;
  NodePtr result = new Node() ;

  bool lessOrEqual = false ;

  while ( walk != NULL &&
          walk->mtoken.mtype != NIL ) {

    if ( walk->mleft != NULL ) {

      if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

        eval1 = Eval( walk, false, false ) ;

        walk = walk->mright ;

      } // if

      else if ( walk->mleft != NULL )
        eval1 = Eval( walk->mleft, false, false ) ;

      if ( eval1->mtoken.mtype != INT && eval1->mtoken.mtype != FLOAT ) {

        delete result ;
        result = NULL ;

        throw Exception( error, eval1 ) ;

      } // if

      if ( eval1 != NULL )
        f_array.push_back( atof( eval1->mtoken.mtoken.data() ) ) ;

    } // if

    walk = walk->mright ;

  } // while

  for ( int i = 0 ; i < f_array.size() - 1 && !lessOrEqual ; i++ ) {

    if ( f_array[i] <= f_array[i+1] )
      lessOrEqual = true ;

  } // for

  if ( lessOrEqual )
    result->mtoken.mtype = NIL ;

  else
    result->mtoken.mtype = T ;

  return result ;

} // BiggerThan()

NodePtr BiggerEqu( NodePtr head ) {

  NodePtr walk = head ;

  vector<float> f_array ;
  char error[100] = ">=\0" ;
  NodePtr eval1 = NULL ;
  NodePtr result = new Node() ;

  bool less = false ;

  while ( walk != NULL &&
          walk->mtoken.mtype != NIL ) {

    if ( walk->mleft != NULL ) {

      if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

        eval1 = Eval( walk, false, false ) ;

        walk = walk->mright ;

      } // if

      else if ( walk->mleft != NULL )
        eval1 = Eval( walk->mleft, false, false ) ;

      if ( eval1->mtoken.mtype != INT && eval1->mtoken.mtype != FLOAT ) {

        delete result ;
        result = NULL ;

        throw Exception( error, eval1 ) ;

      } // if

      if ( eval1 != NULL )
        f_array.push_back( atof( eval1->mtoken.mtoken.data() ) ) ;

    } // if

    walk = walk->mright ;

  } // while

  for ( int i = 0 ; i < f_array.size() - 1 && !less ; i++ ) {

    if ( f_array[i] < f_array[i+1] )
      less = true ;

  } // for

  if ( less )
    result->mtoken.mtype = NIL ;

  else
    result->mtoken.mtype = T ;

  return result ;

} // BiggerEqu()

NodePtr LessThan( NodePtr head ) {

  NodePtr walk = head ;

  vector<float> f_array ;
  char error[100] = "<\0" ;
  NodePtr eval1 = NULL ;
  NodePtr result = new Node() ;

  bool biggerOrEqual = false ;

  while ( walk != NULL &&
          walk->mtoken.mtype != NIL ) {

    if ( walk->mleft != NULL ) {

      if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

        eval1 = Eval( walk, false, false ) ;

        walk = walk->mright ;

      } // if

      else if ( walk->mleft != NULL )
        eval1 = Eval( walk->mleft, false, false ) ;

      if ( eval1->mtoken.mtype != INT && eval1->mtoken.mtype != FLOAT ) {

        delete result ;
        result = NULL ;

        throw Exception( error, eval1 ) ;

      } // if

      if ( eval1 != NULL )
        f_array.push_back( atof( eval1->mtoken.mtoken.data() ) ) ;

    } // if

    walk = walk->mright ;

  } // while

  for ( int i = 0 ; i < f_array.size() - 1 && !biggerOrEqual ; i++ ) {

    if ( f_array[i] >= f_array[i+1] )
      biggerOrEqual = true ;

  } // for

  if ( biggerOrEqual )
    result->mtoken.mtype = NIL ;

  else
    result->mtoken.mtype = T ;

  return result ;

} // LessThan()

NodePtr LessEqu( NodePtr head ) {

  NodePtr walk = head ;

  vector<float> f_array ;
  char error[100] = "<=\0" ;
  NodePtr eval1 = NULL ;
  NodePtr result = new Node() ;

  bool bigger = false ;

  while ( walk != NULL &&
          walk->mtoken.mtype != NIL ) {

    if ( walk->mleft != NULL ) {

      if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

        eval1 = Eval( walk, false, false ) ;

        walk = walk->mright ;

      } // if

      else if ( walk->mleft != NULL )
        eval1 = Eval( walk->mleft, false, false ) ;

      if ( eval1->mtoken.mtype != INT && eval1->mtoken.mtype != FLOAT ) {

        delete result ;
        result = NULL ;

        throw Exception( error, eval1 ) ;

      } // if

      if ( eval1 != NULL )
        f_array.push_back( atof( eval1->mtoken.mtoken.data() ) ) ;

    } // if

    walk = walk->mright ;

  } // while

  for ( int i = 0 ; i < f_array.size() - 1 && !bigger ; i++ ) {

    if ( f_array[i] > f_array[i+1] )
      bigger = true ;

  } // for

  if ( bigger )
    result->mtoken.mtype = NIL ;

  else
    result->mtoken.mtype = T ;

  return result ;

} // LessEqu()

NodePtr NumberEqu( NodePtr head ) {

  NodePtr walk = head ;

  vector<float> f_array ;
  char error[100] = "=\0" ;
  NodePtr eval1 = NULL ;
  NodePtr result = new Node() ;

  bool notEqual = false ;

  while ( walk != NULL &&
          walk->mtoken.mtype != NIL ) {

    if ( walk->mleft != NULL ) {

      if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

        eval1 = Eval( walk, false, false ) ;

        walk = walk->mright ;

      } // if

      else if ( walk->mleft != NULL )
        eval1 = Eval( walk->mleft, false, false ) ;

      if ( eval1->mtoken.mtype != INT && eval1->mtoken.mtype != FLOAT ) {

        delete result ;
        result = NULL ;

        throw Exception( error, eval1 ) ;

      } // if

      if ( eval1 != NULL )
        f_array.push_back( atof( eval1->mtoken.mtoken.data() ) ) ;

    } // if

    walk = walk->mright ;

  } // while

  for ( int i = 0 ; i < f_array.size() - 1 && !notEqual ; i++ ) {

    if ( f_array[i] != f_array[i+1] )
      notEqual = true ;

  } // for

  if ( notEqual )
    result->mtoken.mtype = NIL ;

  else
    result->mtoken.mtype = T ;

  return result ;

} // NumberEqu()

// string 31 regulation

NodePtr StringBigger( NodePtr head ) {

  NodePtr walk = head ;

  vector<string> s_array ;
  string tempString ;
  char error[100] = "string>?\0" ;
  NodePtr eval1 = NULL ;
  NodePtr result = new Node() ;

  bool lessOrEqu = false ;

  tempString.clear() ;

  while ( walk != NULL &&
          walk->mtoken.mtype != NIL ) {

    if ( walk->mleft != NULL ) {

      if ( walk->mleft->mtoken.mtype == QUOTE ) {

        eval1 = Eval( walk, false, false ) ;

        walk = walk->mright ;

      } // if

      else
        eval1 = Eval( walk->mleft, false, false ) ;

      if ( eval1 != NULL && eval1->mtoken.mtype != STRING ) {

        delete result ;
        result = NULL ;

        throw Exception( error, eval1 ) ;

      } // if

      if ( eval1 != NULL ) {

        tempString.assign( eval1->mtoken.mtoken.data() ) ;
        s_array.push_back( tempString ) ;

      } // if

    } // if

    walk = walk->mright ;

  } // while

  for ( int i = 0 ; i < s_array.size() - 1 && !lessOrEqu ; i++ ) {

    if ( s_array[i] <= s_array[i+1] )
      lessOrEqu = true ;

  } // for

  if ( lessOrEqu )
    result->mtoken.mtype = NIL ;

  else
    result->mtoken.mtype = T ;

  return result ;

} // StringBigger()

NodePtr StringLess( NodePtr head ) {

  NodePtr walk = head ;

  vector<string> s_array ;
  string tempString ;
  char error[100] = "string<?\0" ;
  NodePtr eval1 = NULL ;
  NodePtr result = new Node() ;

  bool biggerOrEqu = false ;

  tempString.clear() ;

  while ( walk != NULL &&
          walk->mtoken.mtype != NIL ) {

    if ( walk->mleft != NULL ) {

      if ( walk->mleft->mtoken.mtype == QUOTE ) {

        eval1 = Eval( walk, false, false ) ;

        walk = walk->mright ;

      } // if

      else
        eval1 = Eval( walk->mleft, false, false ) ;

      if ( eval1 != NULL && eval1->mtoken.mtype != STRING ) {

        delete result ;
        result = NULL ;

        throw Exception( error, eval1 ) ;

      } // if

      if ( eval1 != NULL ) {

        tempString.assign( eval1->mtoken.mtoken.data() ) ;
        s_array.push_back( tempString ) ;

      } // if

    } // if

    walk = walk->mright ;

  } // while

  for ( int i = 0 ; i < s_array.size() - 1 && !biggerOrEqu ; i++ ) {

    if ( s_array[i] >= s_array[i+1] )
      biggerOrEqu = true ;

  } // for

  if ( biggerOrEqu )
    result->mtoken.mtype = NIL ;

  else
    result->mtoken.mtype = T ;

  return result ;

} // StringLess()

NodePtr StringEqu( NodePtr head ) {

  NodePtr walk = head ;

  vector<string> s_array ;
  string tempString ;
  char error[100] = "string=?\0" ;
  NodePtr eval1 = NULL ;
  NodePtr result = new Node() ;

  bool notEqu = false ;
  
  tempString.clear() ;

  while ( walk != NULL &&
          walk->mtoken.mtype != NIL ) {

    if ( walk->mleft != NULL ) {

      if ( walk->mleft->mtoken.mtype == QUOTE ) {

        eval1 = Eval( walk, false, false ) ;

        walk = walk->mright ;

      } // if

      else
        eval1 = Eval( walk->mleft, false, false ) ;

      if ( eval1 != NULL && eval1->mtoken.mtype != STRING ) {

        delete result ;
        result = NULL ;

        throw Exception( error, eval1 ) ;

      } // if

      if ( eval1 != NULL ) {

        tempString.assign( eval1->mtoken.mtoken.data() ) ;
        s_array.push_back( tempString ) ;

      } // if

    } // if

    walk = walk->mright ;

  } // while

  for ( int i = 0 ; i < s_array.size() - 1 && !notEqu ; i++ ) {

    if ( s_array[i] != s_array[i+1] )
      notEqu = true ;

  } // for

  if ( notEqu )
    result->mtoken.mtype = NIL ;

  else
    result->mtoken.mtype = T ;

  return result ;

} // StringEqu()

// eqv? only 2 parameter

NodePtr AddressEqu( NodePtr head ) {

  NodePtr walk = head ;
  NodePtr eval1, eval2 = NULL ;
  NodePtr result = new Node() ;

  if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

    eval1 = Eval( walk, true, false ) ;

    walk = walk->mright ;

  } // if

  else if ( walk->mleft != NULL )
    eval1 = Eval( walk->mleft, true, false ) ;

  walk = walk->mright ;

  if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

    eval2 = Eval( walk, true, false ) ;

    walk = walk->mright ;

  } // if

  else if ( walk->mleft != NULL )
    eval2 = Eval( walk->mleft, true, false ) ;

  if ( eval1 != NULL && eval2 != NULL ) {

    if ( ( ElementIsAtom( eval1 )->mtoken.mtype == T && eval1->mtoken.mtype != STRING ) &&
         ( ElementIsAtom( eval2 )->mtoken.mtype == T && eval2->mtoken.mtype != STRING ) ) {

      if ( strcmp( eval1->mtoken.mtoken.data(), eval2->mtoken.mtoken.data() ) == 0 )
        result->mtoken.mtype = T ;

    } // if

    else if ( eval1 == eval2 )
      result->mtoken.mtype = T ;

    else
      result->mtoken.mtype = NIL ;

  } // if

  return result ;

} // AddressEqu()

NodePtr ContentEqu( NodePtr tree1, NodePtr tree2 ) {

  NodePtr result = new Node() ;
  NodePtr leftTree = NULL ;
  NodePtr rightTree = NULL ;

  if ( tree1 == NULL && tree2 == NULL )
    result->mtoken.mtype = T ;

  else if ( tree1 == NULL || tree2 == NULL )
    result->mtoken.mtype = NIL ;

  else if ( tree1->mtoken.mtype == EMPTY && tree2->mtoken.mtype == EMPTY ) {

    leftTree = ContentEqu( tree1->mleft, tree2->mleft ) ;
    rightTree = ContentEqu( tree1->mright, tree2->mright ) ;

    if ( leftTree->mtoken.mtype == T && rightTree->mtoken.mtype == T )
      result->mtoken.mtype = T ;

    else
      result->mtoken.mtype = NIL ;

  } // else if
    
  else if ( tree1->mtoken.mtype == EMPTY || tree2->mtoken.mtype == EMPTY )
    result->mtoken.mtype = NIL ;

  else if ( strcmp( tree1->mtoken.mtoken.data(), tree2->mtoken.mtoken.data() ) == 0 ) {

    leftTree = ContentEqu( tree1->mleft, tree2->mleft ) ;
    rightTree = ContentEqu( tree1->mright, tree2->mright ) ;

    if ( leftTree->mtoken.mtype == T && rightTree->mtoken.mtype == T )
      result->mtoken.mtype = T ;

    else
      result->mtoken.mtype = NIL ;

  } // else if

  else
    result->mtoken.mtype = NIL ;

  if ( leftTree != NULL ) {

    delete leftTree ;
    leftTree = NULL ;

  } // if

  if ( rightTree != NULL ) {

    delete rightTree ;
    rightTree = NULL ;

  } // if

  return result ;

} // ContentEqu()

NodePtr Begin( NodePtr head ) {

  bool findLast = false ;

  NodePtr walk = head ;
  NodePtr result = NULL ;

  while ( walk != NULL && walk->mtoken.mtype != NIL && !findLast ) {

    if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

      if ( walk->mright != NULL && walk->mright->mright != NULL ) {

        if ( walk->mright->mright->mtoken.mtype == NIL ) {
          
          findLast = true ;
          result = Eval( walk, false, false ) ;
          
        } // if
        
        else {

          result = Eval( walk, false, false ) ;
          walk = walk->mright->mright ;

        } // else

      } // if

    } // if encounter quote
    
    else if ( walk->mleft != NULL && walk->mleft->mtoken.mtype != QUOTE ) {

      if ( walk->mright != NULL ) {

        if ( walk->mright->mtoken.mtype == NIL ) {

          findLast = true ;
          result = Eval( walk->mleft, false, false ) ;

        } // if
        
        else {
          
          result = Eval( walk->mleft, false, false ) ;
          walk = walk->mright ;
          
        } // else

      } // if

    } // else if not quote

  } // while

  return result ;

} // Begin()

NodePtr If( NodePtr head ) {

  NodePtr walk = head ;
  NodePtr eval = NULL ;
  NodePtr first = walk ;
  NodePtr second = walk ;
  NodePtr result = NULL ;

  first = walk->mright ;
  second = first ;
  
  while ( second->mleft->mtoken.mtype == QUOTE )
    second = second->mright ;

  second = second->mright ;

  if ( walk != NULL ) {

    eval = Eval( walk->mleft, false, false ) ;

    walk = walk->mright ;

    if ( eval->mtoken.mtype != NIL ) {
      
      if ( first->mleft != NULL && first->mleft->mtoken.mtype == QUOTE )
        result = Eval( first, false, false ) ;

      else if ( first->mleft != NULL )
        result = Eval( first->mleft, false, false ) ;
      
    } // if

    else {

      walk = walk->mright ;

      if ( second->mtoken.mtype == EMPTY ) {

        if ( second->mleft != NULL && second->mleft->mtoken.mtype == QUOTE )
          result = Eval( second, false, false ) ;

        else
          result = Eval( second->mleft, false, false ) ;

      } // if

    } // else

  } // if

  return result ;

} // If()

NodePtr Cond( NodePtr head, NodePtr errorHead ) {

  bool end = false ;

  NodePtr walk = head ;
  NodePtr eval = NULL ;
  NodePtr it = NULL ;
  NodePtr test = walk ;
  NodePtr testIt = NULL ;
  NodePtr result = NULL ;

  while ( test != NULL && test->mtoken.mtype != NIL ) {
    
    testIt = test->mleft ;
    
    if ( testIt == NULL )
      throw Exception( errorHead, COND_FORMAT ) ;

    else if ( testIt->mleft == NULL )
      throw Exception( errorHead, COND_FORMAT ) ;

    else if ( testIt->mright == NULL )
      throw Exception( errorHead, COND_FORMAT ) ;

    else if ( testIt->mright->mtoken.mtype == NIL )
      throw Exception( errorHead, COND_FORMAT ) ;
      
    test = test->mright ;
    
  } // while

  while ( walk != NULL && walk->mtoken.mtype != NIL && !end ) {

    it = walk->mleft ;

    if ( walk->mright != NULL && walk->mright->mtoken.mtype == NIL ) {

      it = walk->mleft ;
      
      if ( it->mleft->mtoken.mtype != EMPTY ) {
        
        if ( it->mleft != NULL && strcmp( it->mleft->mtoken.mtoken.data(), "else" ) == 0 ) {

          it = it->mright ;

          while ( it != NULL && it->mright != NULL ) {

            if ( it->mleft != NULL && it->mleft->mtoken.mtype == QUOTE ) {

              result = Eval( it, false, false ) ;

              it = it->mright ;

            } // if

            else if ( it->mleft != NULL )
              result = Eval( it->mleft, false, false ) ;

            it = it->mright ;

          } // while

          end = true ;

        } // if
        
      } // if

    } // if
    
    if ( it->mleft != NULL && it->mleft->mtoken.mtype == EXECUTE_QUOTE )
      it->mleft->mtoken.mtype = QUOTE ;

    if ( it->mleft != NULL && it->mleft->mtoken.mtype == QUOTE ) {
      
      eval = Eval( it, false, false ) ;
      
      it = it->mright ;
      
    } // if

    else if ( it->mleft != NULL )
      eval = Eval( it->mleft, false, false ) ;

    if ( eval != NULL && eval->mtoken.mtype != NIL ) {
      
      it = it->mright ;
      
      while ( it != NULL && it->mright != NULL ) {

        if ( it->mleft != NULL && it->mleft->mtoken.mtype == QUOTE ) {
          
          result = Eval( it, false, false ) ;
          
          it = it->mright ;
          
        } // if

        else if ( it->mleft != NULL )
          result = Eval( it->mleft, false, false ) ;

        it = it->mright ;

      } // while

      end = true ;

    } // if

    walk = walk->mright ;

  } // while

  return result ;

} // Cond()

NodePtr Eval( NodePtr head, bool eqv, bool topCall ) {

  NodePtr walk = head ;
  NodePtr temp = NULL ;
  NodePtr result = NULL ;
  NodePtr errorEval = NULL ;
  NodePtr clone = NULL ;
  NodePtr tree1 = NULL ;
  NodePtr tree2 = NULL ;

  int symIndex = 0 ;
  bool findSym = false ;

  if ( walk != NULL ) {

    temp = ElementIsAtom( walk ) ;

    if ( temp != NULL && temp->mtoken.mtype == T ) {

      if ( walk->mtoken.mtype != SYMBOL )
        return walk ;

      else {

        // this token is symbol
        // check if this token has defined

        for ( int i = 0 ; i < gSymTable.size() && !findSym ; i++ ) {

          if ( strcmp( gSymTable[i].mSymbol.mtoken.data(),
                       walk->mtoken.mtoken.data() ) == 0 ) {

            findSym = true ;
            symIndex = i ;

          } // if

        } // for

        if ( findSym && !eqv ) {

          clone = CloneTree( clone, gSymTable[symIndex].mBound ) ;
          return clone ;

        } // if

        else if ( findSym && eqv ) {
          
          return gSymTable[symIndex].mBound ;
          
        } // else if

        else {

          throw Exception( walk->mtoken, UNBOUND_SYMBOL ) ;

        } // else

      } // else

    } // if

    else if ( walk != NULL &&
              walk->mtoken.mtype == OPERATOR )
      return walk ;

    else if ( walk != NULL &&
              walk->mtoken.mtype == OPERATOR_PRINT )
      return walk ;

    else if ( walk != NULL &&
              walk->mtoken.mtype == QUOTE )
      return walk ;

    else if ( walk != NULL &&
              walk->mtoken.mtype == EXECUTE_QUOTE )
      return walk ;

    if ( walk->mleft != NULL ) {

      temp = Eval( walk->mleft, false, false ) ;

      if ( temp->mtoken.mtype == EXECUTE_QUOTE ) {

        walk = walk->mright ;

        temp = Eval( walk->mleft, false, false ) ;

      } // if

      if ( temp->mtoken.mtype == QUOTE ) {

        walk = walk->mright ;

        if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE )
          result = walk ;

        else
          result = walk->mleft ;

      } // if

      else if ( temp->mtoken.mtype != OPERATOR && temp->mtoken.mtype != OPERATOR_PRINT ) {

        throw Exception( temp, NON_FUNCTION ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "cons" ) == 0 ) {

        if ( NumOfParameter( walk ) != 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;
          
          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = Cons( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "quote" ) == 0 ) {

        if ( NumOfParameter( walk ) != 1 ) {
          
          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = walk->mleft ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "list" ) == 0 ) {
        
        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL && NumOfParameter( walk ) != 0 ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = List( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "define" ) == 0 ) {

        if ( !topCall )
          throw Exception( DEFINE_LEVEL ) ;

        if ( NumOfParameter( walk ) != 2 ) {

          throw Exception( head, DEFINE_FORMAT ) ;

        } // if

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        walk = walk->mright ;

        if ( walk->mleft != NULL && walk->mleft->mtoken.mtype != SYMBOL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, DEFINE_FORMAT ) ;

        } // if

        result = Define( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "car" ) == 0 ) {

        if ( NumOfParameter( walk ) != 1 ) {
          
          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = Car( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "cdr" ) == 0 ) {

        if ( NumOfParameter( walk ) != 1 ) {
          
          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = Cdr( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "atom?" ) == 0 ) {

        if ( NumOfParameter( walk ) != 1 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = IsAtom( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "pair?" ) == 0 ) {

        if ( NumOfParameter( walk ) != 1 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        result = IsPair( walk, true ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "list?" ) == 0 ) {

        if ( NumOfParameter( walk ) != 1 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        result = IsList( walk, true ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "null?" ) == 0 ) {

        if ( NumOfParameter( walk ) != 1 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = IsNull( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "integer?" ) == 0 ) {

        if ( NumOfParameter( walk ) != 1 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = IsInteger( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "real?" ) == 0 ) {

        if ( NumOfParameter( walk ) != 1 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = IsReal( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "number?" ) == 0 ) {

        if ( NumOfParameter( walk ) != 1 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = IsNumber( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "string?" ) == 0 ) {

        if ( NumOfParameter( walk ) != 1 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = IsString( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "boolean?" ) == 0 ) {

        if ( NumOfParameter( walk ) != 1 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = IsBoolean( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "symbol?" ) == 0 ) {

        if ( NumOfParameter( walk ) != 1 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = IsSymbol( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "+" ) == 0 ) {

        if ( NumOfParameter( walk ) < 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = Add( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "-" ) == 0 ) {

        if ( NumOfParameter( walk ) < 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = Sub( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "*" ) == 0 ) {

        if ( NumOfParameter( walk ) < 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = Mult( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "/" ) == 0 ) {

        if ( NumOfParameter( walk ) < 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = Div( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "not" ) == 0 ) {

        if ( NumOfParameter( walk ) != 1 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = Not( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "and" ) == 0 ) {

        if ( NumOfParameter( walk ) < 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = And( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "or" ) == 0 ) {

        if ( NumOfParameter( walk ) < 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = Or( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), ">" ) == 0 ) {

        if ( NumOfParameter( walk ) < 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = BiggerThan( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), ">=" ) == 0 ) {

        if ( NumOfParameter( walk ) < 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = BiggerEqu( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "=" ) == 0 ) {

        if ( NumOfParameter( walk ) < 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = NumberEqu( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "<" ) == 0 ) {

        if ( NumOfParameter( walk ) < 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = LessThan( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "<=" ) == 0 ) {

        if ( NumOfParameter( walk ) < 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = LessEqu( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "string-append" ) == 0 ) {

        if ( NumOfParameter( walk ) < 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = StringAppend( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "string>?" ) == 0 ) {

        if ( NumOfParameter( walk ) < 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = StringBigger( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "string<?" ) == 0 ) {

        if ( NumOfParameter( walk ) < 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = StringLess( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "string=?" ) == 0 ) {

        if ( NumOfParameter( walk ) < 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = StringEqu( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "eqv?" ) == 0 ) {

        if ( NumOfParameter( walk ) != 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = AddressEqu( walk ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "equal?" ) == 0 ) {

        if ( NumOfParameter( walk ) < 2 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if
        
        if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

          tree1 = Eval( walk, false, false ) ;
          walk = walk->mright ;

        } // if
        
        else if ( walk->mleft != NULL && walk->mleft->mtoken.mtype != QUOTE )
          tree1 = Eval( walk->mleft, false, false ) ;

        walk = walk->mright ;

        if ( walk->mleft != NULL && walk->mleft->mtoken.mtype == QUOTE ) {

          tree2 = Eval( walk, false, false ) ;
          walk = walk->mright ;

        } // if

        else if ( walk->mleft != NULL && walk->mleft->mtoken.mtype != QUOTE )
          tree2 = Eval( walk->mleft, false, false ) ;

        result = ContentEqu( tree1, tree2 ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "begin" ) == 0 ) {

        if ( NumOfParameter( walk ) < 1 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        result = Begin( walk ) ;
        
        if ( result == NULL )
          throw Exception( head, NO_RETURN_VALUE ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "if" ) == 0 ) {

        if ( NumOfParameter( walk ) != 2 && NumOfParameter( walk ) != 3 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = If( walk ) ;

        if ( result == NULL )
          throw Exception( head, NO_RETURN_VALUE ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "cond" ) == 0 ) {

        if ( NumOfParameter( walk ) < 1 ) {

          throw Exception( head, COND_FORMAT ) ;

        } // if

        walk = walk->mright ;

        temp = IsList( walk, false ) ;

        if ( temp->mtoken.mtype == NIL ) {

          delete temp ;
          temp = NULL ;

          throw Exception( head, NON_LIST ) ;

        } // if

        result = Cond( walk, head ) ;
        
        if ( result == NULL )
          throw Exception( head, NO_RETURN_VALUE ) ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "clean-environment" ) == 0 ) {

        if ( !topCall )
          throw Exception( CLEAN_LEVEL ) ;

        if ( NumOfParameter( walk ) != 0 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

        result = EvnClean() ;

      } // else if

      else if ( strcmp( temp->mtoken.mtoken.data(), "exit" ) == 0 ) {

        if ( !topCall )
          throw Exception( EXIT_LEVEL ) ;

        if ( NumOfParameter( walk ) != 0 ) {

          errorEval = Eval( walk->mleft, false, false ) ;

          if ( errorEval != NULL &&
               errorEval->mtoken.mtype == OPERATOR_PRINT ) {

            errorEval->mtoken.mtype = OPERATOR ;

          } // if

          throw Exception( errorEval, NOA ) ;

        } // if

      } // else if

    } // if

  } // if

  temp = NULL ;

  return result ;

} // Eval()
