#include "val_SP26.h"

// operator+: numeric addition OR string/char concatenation
Value Value::operator+(const Value &op) const {
  // String/char concatenation: both operands must be string or char
  if (IsString() || IsChar()) {
    if (op.IsString() || op.IsChar()) {
      string left = IsString() ? GetString() : string(1, GetChar());
      string right = op.IsString() ? op.GetString() : string(1, op.GetChar());
      return Value(left + right);
    }
    return Value(); // ERROR: incompatible types
  }

  // Numeric addition
  if (IsInt() && op.IsInt())
    return Value(GetInt() + op.GetInt());

  if (IsInt() && op.IsReal())
    return Value((double)GetInt() + op.GetReal());

  if (IsReal() && op.IsInt())
    return Value(GetReal() + (double)op.GetInt());

  if (IsReal() && op.IsReal())
    return Value(GetReal() + op.GetReal());

  // All other combinations are errors
  return Value();
}

// operator/: real division returns VREAL for numeric operands
Value Value::operator/(const Value &op) const {
  double left, right;

  if (IsInt())
    left = (double)GetInt();
  else if (IsReal())
    left = GetReal();
  else
    return Value(); // ERROR

  if (op.IsInt())
    right = (double)op.GetInt();
  else if (op.IsReal())
    right = op.GetReal();
  else
    return Value(); // ERROR

  return Value(left / right);
}

// operator<: less-than relational operator — returns VBOOL
Value Value::operator<(const Value &op) const {
  // Numeric comparison (with implicit widening)
  if ((IsInt() || IsReal()) && (op.IsInt() || op.IsReal())) {
    double left = IsInt() ? (double)GetInt() : GetReal();
    double right = op.IsInt() ? (double)op.GetInt() : op.GetReal();
    return Value(left < right);
  }

  // String comparison
  if (IsString() && op.IsString())
    return Value(GetString() < op.GetString());

  // Char comparison
  if (IsChar() && op.IsChar())
    return Value(GetChar() < op.GetChar());

  // Bool comparison
  if (IsBool() && op.IsBool())
    return Value(GetBool() < op.GetBool());

  // All other combinations are errors
  return Value();
}

//  remove trailing blanks from a string
Value Value::Trim(void) const {
  if (!IsString())
    return Value(); // ERROR for non-string types

  string s = GetString();
  size_t end = s.find_last_not_of(' ');
  if (end == string::npos)
    return Value(string("")); // all spaces
  return Value(s.substr(0, end + 1));
}

// perators needed for linking

Value Value::operator-(const Value &op) const {
  if (IsInt() && op.IsInt())
    return Value(GetInt() - op.GetInt());
  if (IsInt() && op.IsReal())
    return Value((double)GetInt() - op.GetReal());
  if (IsReal() && op.IsInt())
    return Value(GetReal() - (double)op.GetInt());
  if (IsReal() && op.IsReal())
    return Value(GetReal() - op.GetReal());
  return Value();
}

Value Value::operator*(const Value &op) const {
  if (IsInt() && op.IsInt())
    return Value(GetInt() * op.GetInt());
  if (IsInt() && op.IsReal())
    return Value((double)GetInt() * op.GetReal());
  if (IsReal() && op.IsInt())
    return Value(GetReal() * (double)op.GetInt());
  if (IsReal() && op.IsReal())
    return Value(GetReal() * op.GetReal());
  return Value();
}

Value Value::idiv(const Value &op) const {
  if (IsInt() && op.IsInt())
    return Value(GetInt() / op.GetInt());
  return Value();
}

Value Value::operator%(const Value &op) const {
  if (IsInt() && op.IsInt())
    return Value(GetInt() % op.GetInt());
  return Value();
}

Value Value::operator==(const Value &op) const {
  if (IsInt() && op.IsInt())
    return Value(GetInt() == op.GetInt());
  if ((IsInt() || IsReal()) && (op.IsInt() || op.IsReal())) {
    double left = IsInt() ? (double)GetInt() : GetReal();
    double right = op.IsInt() ? (double)op.GetInt() : op.GetReal();
    return Value(left == right);
  }
  if (IsString() && op.IsString())
    return Value(GetString() == op.GetString());
  if (IsChar() && op.IsChar())
    return Value(GetChar() == op.GetChar());
  if (IsBool() && op.IsBool())
    return Value(GetBool() == op.GetBool());
  return Value();
}

Value Value::operator>(const Value &op) const {
  if ((IsInt() || IsReal()) && (op.IsInt() || op.IsReal())) {
    double left = IsInt() ? (double)GetInt() : GetReal();
    double right = op.IsInt() ? (double)op.GetInt() : op.GetReal();
    return Value(left > right);
  }
  if (IsString() && op.IsString())
    return Value(GetString() > op.GetString());
  if (IsChar() && op.IsChar())
    return Value(GetChar() > op.GetChar());
  if (IsBool() && op.IsBool())
    return Value(GetBool() > op.GetBool());
  return Value();
}

Value Value::operator&&(const Value &op) const {
  if (IsBool() && op.IsBool())
    return Value(GetBool() && op.GetBool());
  return Value();
}

Value Value::operator||(const Value &op) const {
  if (IsBool() && op.IsBool())
    return Value(GetBool() || op.GetBool());
  return Value();
}

Value Value::operator!(void) const {
  if (IsBool())
    return Value(!GetBool());
  return Value();
}

Value Value::operator-(void) const {
  if (IsInt())
    return Value(-GetInt());
  if (IsReal())
    return Value(-GetReal());
  if (IsString()) {
    // unary minus on string = Trim
    return Trim();
  }
  return Value();
}
