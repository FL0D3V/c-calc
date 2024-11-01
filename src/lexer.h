/*
 * The lexer is used for converting the input text into usable tokens.
 */

#ifndef _LEXER_H_
#define _LEXER_H_

// TODO: Rethink if needed!
#include <stdint.h>

typedef enum {
  TOKENTYPE_NUMBER,
  TOKENTYPE_COMMA,
  TOKENTYPE_OPERATOR,
  TOKENTYPE_L_PARAM,
  TOKENTYPE_R_PARAM,

  TOKENTYPE_COUNT
} eTokenType;

typedef enum {
  OPERATION_ADD,
  OPERATION_SUB,
  OPERATION_MUL,
  OPERATION_DIV,

  OPERATION_COUNT
} eOperation;

typedef struct {
  eTokenType type;
  union {
    size_t literal;
    eOperation operation;
  };
} sToken;

typedef enum {
  NODETYPE_LITERAL,
  NODETYPE_BIN_OP,

  NODETYPE_COUNT
} eNodeType;

typedef struct sSyntaxNode sSyntaxNode;
struct sSyntaxNode {
  eNodeType type;
  union {
    size_t literal;
    struct {
      eOperation op;
      sSyntaxNode* left;
      sSyntaxNode* right;
    } sBinOp;
  };
};

static const char* nameFromOperation[OPERATION_COUNT] = {
	[OPERATION_ADD] = "Add",
	[OPERATION_SUB] = "Subtract",
	[OPERATION_MUL] = "Multiply",
	[OPERATION_DIV] = "Divide"
};

#endif // _LEXER_H_
