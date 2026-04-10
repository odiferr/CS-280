
/*
 * o_diaz_parser.cpp
 * Programming Assignment 2
 * Spring 2026
 */

#include "parserSP26.h"

map<string, bool> defVar;
map<string, bool> defConst;

namespace Parser {
bool pushed_back = false;
LexItem pushed_token;

static LexItem GetNextToken(istream &in, int &line) {
  if (pushed_back) {
    pushed_back = false;
    return pushed_token;
  }
  return getNextToken(in, line);
}

static void PushBackToken(LexItem &t) {
  if (pushed_back) {
    abort();
  }
  pushed_back = true;
  pushed_token = t;
}

} // namespace Parser

static int error_count = 0;

int ErrCount() { return error_count; }

void ParseError(int line, string msg) {
  ++error_count;
  cout << line << ": " << msg << endl;
}

#include <algorithm>
#include <vector>

static vector<string> declaredVarOrder;
static vector<string> definedConstOrder;
static int lastConsumedLine = 1;

// Forward declarations
bool Expr(istream &in, int &line);
bool SimpleExpr(istream &in, int &line);
bool Term(istream &in, int &line);
bool SFactor(istream &in, int &line);
bool Factor(istream &in, int &line, int sign);
bool VarList(istream &in, int &line);
bool ExprList(istream &in, int &line);
bool Variable(istream &in, int &line);
bool DeclStmt(istream &in, int &line);
bool ConstDef(istream &in, int &line);
bool Stmt(istream &in, int &line);
bool ConstPart(istream &in, int &line);
bool VarPart(istream &in, int &line);
bool DeclPart(istream &in, int &line);
bool IdentList(istream &in, int &line);
bool Block(istream &in, int &line);
bool CompStmt(istream &in, int &line);
bool SimpleStmt(istream &in, int &line);
bool StructuredStmt(istream &in, int &line);
bool IfStmt(istream &in, int &line);
bool AssignStmt(istream &in, int &line);
bool WriteLnStmt(istream &in, int &line);
bool WriteStmt(istream &in, int &line);
bool ReadLnStmt(istream &in, int &line);

bool Prog(istream &in, int &line) {
  LexItem tok = Parser::GetNextToken(in, line);

  if (tok != PROGRAM) {
    ParseError(line, "Missing Program Name.");
    return false;
  }

  tok = Parser::GetNextToken(in, line);
  if (tok != IDENT) {
    ParseError(line, "Missing Program Name.");
    return false;
  }

  string progName = tok.GetLexeme();

  tok = Parser::GetNextToken(in, line);
  if (tok != SEMICOL) {
    ParseError(line, "Missing semicolon after program name.");
    return false;
  }

  if (!Block(in, line)) {
    ParseError(line, "Incorrect Program Body.");
    return false;
  }

  tok = Parser::GetNextToken(in, line);
  if (tok != DOT) {
    ParseError(line, "Incorrect Program Body.");
    return false;
  }

  vector<string> sortedVars = declaredVarOrder;
  sort(sortedVars.begin(), sortedVars.end());

  vector<string> sortedConsts = definedConstOrder;
  sort(sortedConsts.begin(), sortedConsts.end());

  cout << "Program Name: " << progName << endl;
  cout << "Declared Variables:" << endl;
  for (size_t i = 0; i < sortedVars.size(); i++) {
    if (i > 0)
      cout << ", ";
    cout << sortedVars[i];
  }
  cout << endl;
  cout << endl;
  cout << "Defined Constants:" << endl;
  for (size_t i = 0; i < sortedConsts.size(); i++) {
    if (i > 0)
      cout << ", ";
    cout << sortedConsts[i];
  }
  cout << endl;
  cout << endl;
  cout << "DONE" << endl;

  return true;
}

bool Block(istream &in, int &line) {
  LexItem tok = Parser::GetNextToken(in, line);
  Parser::PushBackToken(tok);

  if (tok == CONST || tok == VAR) {
    if (!DeclPart(in, line)) {
      return false;
    }
  }

  if (!CompStmt(in, line)) {
    ParseError(line, "Incorrect Program Body.");
    return false;
  }

  return true;
}

bool DeclPart(istream &in, int &line) {
  LexItem tok = Parser::GetNextToken(in, line);
  Parser::PushBackToken(tok);

  if (tok == CONST) {
    if (!ConstPart(in, line)) {
      ParseError(line, "Incorrect Constant Definition Part.");
      return false;
    }
    tok = Parser::GetNextToken(in, line);
    Parser::PushBackToken(tok);
  }

  if (tok == VAR) {
    if (!VarPart(in, line)) {
      ParseError(line, "Incorrect Declaration Part.");
      return false;
    }
  }

  return true;
}

bool ConstPart(istream &in, int &line) {
  LexItem tok = Parser::GetNextToken(in, line);

  if (tok != CONST) {
    ParseError(line, "Expected CONST keyword.");
    return false;
  }

  if (!ConstDef(in, line)) {
    ParseError(line, "Syntactic error in Constants Definitions Part.");
    return false;
  }

  tok = Parser::GetNextToken(in, line);
  while (tok == SEMICOL) {
    LexItem peek = Parser::GetNextToken(in, line);
    if (peek.GetToken() == IDENT) {
      Parser::PushBackToken(peek);
      if (!ConstDef(in, line)) {
        ParseError(line, "Syntactic error in Constants Definitions Part.");
        return false;
      }
      tok = Parser::GetNextToken(in, line);
    } else {
      Parser::PushBackToken(peek);
      return true;
    }
  }

  ParseError(line, "Missing semicolon at end of ConstPart.");
  return false;
}

bool ConstDef(istream &in, int &line) {
  LexItem tok = Parser::GetNextToken(in, line);

  if (tok != IDENT) {
    ParseError(line, "Incorrect constant definition syntax.");
    return false;
  }

  string constName = tok.GetLexeme();

  if (defConst.find(constName) != defConst.end()) {
    ParseError(line, "Constant Redefinition: " + constName);
    return false;
  }

  tok = Parser::GetNextToken(in, line);
  if (tok != EQ) {
    ParseError(line, "Incorrect constant definition syntax.");
    return false;
  }

  if (!Expr(in, line)) {
    ParseError(line, "Incorrect constant definition syntax.");
    return false;
  }

  defConst[constName] = true;
  definedConstOrder.push_back(constName);

  return true;
}

bool VarPart(istream &in, int &line) {
  LexItem tok = Parser::GetNextToken(in, line);

  if (tok != VAR) {
    ParseError(line, "Expected VAR keyword.");
    return false;
  }

  if (!DeclStmt(in, line)) {
    ParseError(line, "Syntactic error in Declaration Block.");
    return false;
  }

  tok = Parser::GetNextToken(in, line);
  while (tok == SEMICOL) {
    LexItem peek = Parser::GetNextToken(in, line);
    if (peek.GetToken() == IDENT) {
      Parser::PushBackToken(peek);
      if (!DeclStmt(in, line)) {
        ParseError(line, "Syntactic error in Declaration Block.");
        return false;
      }
      tok = Parser::GetNextToken(in, line);
    } else {
      Parser::PushBackToken(peek);
      return true;
    }
  }

  ParseError(line, "Missing semicolon at end of VarPart.");
  return false;
}

bool IdentList(istream &in, int &line) { return true; }

bool DeclStmt(istream &in, int &line) {
  vector<string> varNames;
  LexItem tok = Parser::GetNextToken(in, line);

  if (tok != IDENT) {
    ParseError(line, "Expected identifier in declaration statement.");
    return false;
  }

  string firstName = tok.GetLexeme();

  if (defConst.find(firstName) != defConst.end()) {
    ParseError(line,
               "Illegal use of a constant name as a variable: " + firstName);
    ParseError(line, "Incorrect identifiers list in Declaration Statement.");
    return false;
  }

  if (defVar.find(firstName) != defVar.end()) {
    ParseError(line, "Variable Redefinition: " + firstName);
    ParseError(line, "Incorrect identifiers list in Declaration Statement.");
    return false;
  }

  varNames.push_back(firstName);

  tok = Parser::GetNextToken(in, line);
  while (tok == COMMA) {
    tok = Parser::GetNextToken(in, line);
    if (tok != IDENT) {
      ParseError(line, "Missing comma in declaration statement.");
      ParseError(line, "Incorrect identifiers list in Declaration Statement.");
      return false;
    }
    string vn = tok.GetLexeme();

    if (defConst.find(vn) != defConst.end()) {
      ParseError(line, "Illegal use of a constant name as a variable: " + vn);
      ParseError(line, "Incorrect identifiers list in Declaration Statement.");
      return false;
    }

    if (defVar.find(vn) != defVar.end()) {
      ParseError(line, "Variable Redefinition: " + vn);
      ParseError(line, "Incorrect identifiers list in Declaration Statement.");
      return false;
    }

    varNames.push_back(vn);
    tok = Parser::GetNextToken(in, line);
  }

  if (tok != COLON) {
    ParseError(line, "Missing comma in declaration statement.");
    ParseError(line, "Incorrect identifiers list in Declaration Statement.");
    return false;
  }

  tok = Parser::GetNextToken(in, line);
  Token varType = tok.GetToken();

  if (varType != INTEGER && varType != REAL && varType != BOOLEAN &&
      varType != CHAR && varType != STRING) {
    ParseError(line, "Incorrect Declaration Type: " + tok.GetLexeme());
    return false;
  }

  for (const string &varName : varNames) {
    defVar[varName] = false;
    declaredVarOrder.push_back(varName);
  }

  tok = Parser::GetNextToken(in, line);
  if (tok == ASSOP) {
    if (!Expr(in, line))
      return false;
    tok = Parser::GetNextToken(in, line);
  }

  Parser::PushBackToken(tok);

  return true;
}

bool Stmt(istream &in, int &line) {
  LexItem tok = Parser::GetNextToken(in, line);
  Parser::PushBackToken(tok);

  if (tok == IF || tok == BEGIN) {
    if (!StructuredStmt(in, line)) {
      ParseError(line, "Incorrect Structured Statement.");
      return false;
    }
    return true;
  } else {
    if (!SimpleStmt(in, line)) {
      ParseError(line, "Incorrect Simple Statement.");
      return false;
    }
    return true;
  }
}

bool StructuredStmt(istream &in, int &line) {
  LexItem tok = Parser::GetNextToken(in, line);
  Parser::PushBackToken(tok);

  if (tok == IF) {
    return IfStmt(in, line);
  } else if (tok == BEGIN) {
    return CompStmt(in, line);
  }

  ParseError(line, "Expected IF or BEGIN for structured statement.");
  return false;
}

bool CompStmt(istream &in, int &line) {
  LexItem tok = Parser::GetNextToken(in, line);

  if (tok != BEGIN) {
    ParseError(line, "Expected BEGIN for compound statement.");
    return false;
  }

  {
    LexItem peek = Parser::GetNextToken(in, line);
    if (peek == ELSE) {
      ParseError(line, "Illegal Else-clause.");
      return false;
    }
    Parser::PushBackToken(peek);
  }

  if (!Stmt(in, line)) {
    ParseError(line, "Syntactic error in the statement.");
    return false;
  }

  int stmtLine = lastConsumedLine;
  tok = Parser::GetNextToken(in, line);

  while (tok == SEMICOL) {
    LexItem peek = Parser::GetNextToken(in, line);
    if (peek == END) {
      return true;
    }

    if (peek.GetToken() == DONE || peek.GetToken() == ERR) {
      ParseError(line, "Missing end of compound statement.");
      return false;
    }

    if (peek == ELSE) {
      ParseError(peek.GetLinenum(), "Illegal Else-clause.");
      return false;
    }
    Parser::PushBackToken(peek);

    if (!Stmt(in, line)) {
      ParseError(line, "Syntactic error in the statement.");
      return false;
    }
    stmtLine = lastConsumedLine;
    tok = Parser::GetNextToken(in, line);
  }

  if (tok == END) {
    return true;
  }

  if (tok.GetToken() == DONE || tok.GetToken() == ERR) {
    ParseError(line, "Missing end of compound statement.");
    return false;
  }

  line = stmtLine;
  ParseError(line, "Missing semicolon.");
  return false;
}

bool SimpleStmt(istream &in, int &line) {
  LexItem tok = Parser::GetNextToken(in, line);
  Parser::PushBackToken(tok);

  if (tok == IDENT) {
    return AssignStmt(in, line);
  } else if (tok == READLN) {
    return ReadLnStmt(in, line);
  } else if (tok == WRITELN) {
    return WriteLnStmt(in, line);
  } else if (tok == WRITE) {
    return WriteStmt(in, line);
  }

  ParseError(line, "Invalid statement.");
  return false;
}

bool WriteLnStmt(istream &in, int &line) {
  LexItem tok = Parser::GetNextToken(in, line);

  if (tok != WRITELN) {
    ParseError(line, "Expected WRITELN.");
    return false;
  }

  tok = Parser::GetNextToken(in, line);
  if (tok != LPAREN) {
    ParseError(line, "Missing Left Parenthesis in WriteLn statement.");
    return false;
  }

  if (!ExprList(in, line))
    return false;

  tok = Parser::GetNextToken(in, line);
  if (tok != RPAREN) {
    ParseError(line, "Missing right parenthesis in WRITELN statement.");
    return false;
  }

  return true;
}

bool WriteStmt(istream &in, int &line) {
  LexItem tok = Parser::GetNextToken(in, line);

  if (tok != WRITE) {
    ParseError(line, "Expected WRITE.");
    return false;
  }

  tok = Parser::GetNextToken(in, line);
  if (tok != LPAREN) {
    ParseError(line, "Missing Left Parenthesis in Write statement.");
    return false;
  }

  if (!ExprList(in, line))
    return false;

  tok = Parser::GetNextToken(in, line);
  if (tok != RPAREN) {
    ParseError(line, "Missing right parenthesis in WRITE statement.");
    return false;
  }

  return true;
}

bool IfStmt(istream &in, int &line) {
  LexItem tok = Parser::GetNextToken(in, line);

  if (tok != IF) {
    ParseError(line, "Expected IF.");
    return false;
  }

  tok = Parser::GetNextToken(in, line);
  if (tok == THEN) {
    ParseError(line, "If Statement Syntax Error.");
    return false;
  }
  Parser::PushBackToken(tok);

  if (!Expr(in, line)) {
    ParseError(line, "Missing if statement Logic Expression.");
    return false;
  }

  tok = Parser::GetNextToken(in, line);
  if (tok != THEN) {
    ParseError(line, "If Statement Syntax Error.");
    return false;
  }

  if (!Stmt(in, line)) {
    ParseError(line, "Missing Statement for If-Then-Part.");
    return false;
  }

  tok = Parser::GetNextToken(in, line);
  if (tok == ELSE) {
    if (!Stmt(in, line)) {
      ParseError(line, "Missing Statement for Else-Part.");
      return false;
    }
  } else {
    Parser::PushBackToken(tok);
  }

  return true;
}

bool AssignStmt(istream &in, int &line) {
  if (!Variable(in, line)) {
    ParseError(line,
               "Missing Left-Hand Side Variable in Assignment statement.");
    return false;
  }

  LexItem tok = Parser::GetNextToken(in, line);
  if (tok != ASSOP) {
    ParseError(line, "Missing Assignment Operator.");
    return false;
  }

  if (!Expr(in, line)) {
    ParseError(line, "Missing Expression in Assignment Statement.");
    return false;
  }

  return true;
}

bool Variable(istream &in, int &line) {
  LexItem tok = Parser::GetNextToken(in, line);

  if (tok != IDENT) {
    ParseError(line, "Expected variable name (IDENT).");
    return false;
  }

  string varName = tok.GetLexeme();

  if (defConst.find(varName) != defConst.end()) {
    ParseError(line,
               "Illegal use of a constant name as a variable: " + varName);
    return false;
  }

  if (defVar.find(varName) == defVar.end()) {
    ParseError(line, "Undeclared Variable: " + varName);
    return false;
  }

  return true;
}

bool ExprList(istream &in, int &line) {
  if (!Expr(in, line))
    return false;

  LexItem tok = Parser::GetNextToken(in, line);
  while (tok == COMMA) {
    if (!Expr(in, line))
      return false;
    tok = Parser::GetNextToken(in, line);
  }

  Parser::PushBackToken(tok);
  return true;
}

bool VarList(istream &in, int &line) {
  if (!Variable(in, line))
    return false;

  LexItem tok = Parser::GetNextToken(in, line);
  while (tok == COMMA) {
    if (!Variable(in, line))
      return false;
    tok = Parser::GetNextToken(in, line);
  }

  Parser::PushBackToken(tok);
  return true;
}

bool ReadLnStmt(istream &in, int &line) {
  LexItem tok = Parser::GetNextToken(in, line);

  if (tok != READLN) {
    ParseError(line, "Expected READLN.");
    return false;
  }

  tok = Parser::GetNextToken(in, line);
  if (tok != LPAREN) {
    ParseError(line, "Missing Left Parenthesis in ReadLn statement.");
    return false;
  }

  if (!VarList(in, line))
    return false;

  tok = Parser::GetNextToken(in, line);
  if (tok != RPAREN) {
    ParseError(line, "Missing right parenthesis in READLN statement.");
    return false;
  }

  return true;
}

bool Expr(istream &in, int &line) {
  if (!SimpleExpr(in, line))
    return false;

  LexItem tok = Parser::GetNextToken(in, line);
  if (tok == EQ || tok == LTHAN || tok == GTHAN) {
    if (!SimpleExpr(in, line)) {
      ParseError(line, "Illegal Relational Expression.");
      return false;
    }

    tok = Parser::GetNextToken(in, line);
    if (tok == EQ || tok == LTHAN || tok == GTHAN) {
      ParseError(line, "Illegal Relational Expression.");
      return false;
    }
    Parser::PushBackToken(tok);
  } else {
    Parser::PushBackToken(tok);
  }

  return true;
}

bool SimpleExpr(istream &in, int &line) {
  if (!Term(in, line))
    return false;

  LexItem tok = Parser::GetNextToken(in, line);
  while (tok == PLUS || tok == MINUS || tok == OR) {
    if (!Term(in, line))
      return false;
    tok = Parser::GetNextToken(in, line);
  }

  if (tok.GetToken() == ERR) {
    ParseError(line, "Unrecognized Input Pattern.");
    cout << "(" << tok.GetLexeme() << ")" << endl;
    return false;
  }

  Parser::PushBackToken(tok);
  return true;
}

bool Term(istream &in, int &line) {
  if (!SFactor(in, line))
    return false;

  LexItem tok = Parser::GetNextToken(in, line);
  while (tok == MULT || tok == DIV || tok == IDIV || tok == MOD || tok == AND) {
    if (!SFactor(in, line))
      return false;
    tok = Parser::GetNextToken(in, line);
  }

  Parser::PushBackToken(tok);
  return true;
}

bool SFactor(istream &in, int &line) {
  int sign = 1;
  LexItem tok = Parser::GetNextToken(in, line);

  if (tok == MINUS) {
    sign = -1;
  } else if (tok == PLUS) {
    sign = 1;
  } else if (tok == NOT) {
    sign = 0;
  } else {
    Parser::PushBackToken(tok);
  }

  return Factor(in, line, sign);
}

bool Factor(istream &in, int &line, int sign) {
  LexItem tok = Parser::GetNextToken(in, line);

  lastConsumedLine = tok.GetLinenum();

  if (tok.GetToken() == ERR) {
    ParseError(line, "Unrecognized Input Pattern.");
    cout << "(" << tok.GetLexeme() << ")" << endl;
    return false;
  }

  if (tok == IDENT) {
    string identName = tok.GetLexeme();

    if (defConst.find(identName) != defConst.end()) {
      return true;
    }

    if (defVar.find(identName) == defVar.end()) {
      ParseError(line, "Undeclared Variable: " + identName);
      return false;
    }
    return true;
  } else if (tok == ICONST || tok == RCONST || tok == SCONST || tok == BCONST ||
             tok == CCONST) {
    return true;
  } else if (tok == LPAREN) {
    if (!Expr(in, line)) {
      ParseError(line, "Missing expression after Left Parenthesis.");
      return false;
    }
    tok = Parser::GetNextToken(in, line);
    if (tok != RPAREN) {
      ParseError(line, "Missing right parenthesis after expression.");
      return false;
    }
    return true;
  }

  ParseError(line, "Missing operand after operator.");
  return false;
}
