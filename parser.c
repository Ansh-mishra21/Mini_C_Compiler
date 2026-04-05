#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"
#include "symbol.h"
#include "icg.h"

// ================= FUNCTION DECLARATIONS =================
char* expr();
char* term();
char* factor();
char* relExpr();
char* boolExpr();     // 🔥 NEW
void stmt();
void ifStatement();
void whileStatement();
void forStatement();  // 🔥 NEW

int pos = 0;

// ================= ERROR HANDLER =================
void error(char *msg) {
    printf("Syntax Error: %s\n", msg);
    exit(1);
}

// ================= FACTOR =================
char* factor() {
    static char buffer[100][20];
    static int idx = 0;

    if(pos < tokenCount && strcmp(tokens[pos].value, "(") == 0) {
        pos++;
        char *val = expr();

        if(pos >= tokenCount || strcmp(tokens[pos].value, ")") != 0)
            error("Expected ')'");

        pos++;
        return val;
    }

    else if(pos < tokenCount && strcmp(tokens[pos].type, "ID") == 0) {
        addSymbol(tokens[pos].value);
        strcpy(buffer[idx], tokens[pos].value);
        pos++;
        return buffer[idx++];
    }

    else if(pos < tokenCount && strcmp(tokens[pos].type, "NUM") == 0) {
        strcpy(buffer[idx], tokens[pos].value);
        pos++;
        return buffer[idx++];
    }

    else {
        error("Invalid factor");
        return NULL;
    }
}

// ================= TERM =================
char* term() {
    char *left = factor();

    while(pos < tokenCount &&
         (strcmp(tokens[pos].value, "*") == 0 ||
          strcmp(tokens[pos].value, "/") == 0)) {

        char op[2];
        strcpy(op, tokens[pos].value);
        pos++;

        char *right = factor();
        left = generateTemp(left, op, right);
    }

    return left;
}

// ================= EXPRESSION =================
char* expr() {
    char *left = term();

    while(pos < tokenCount &&
         (strcmp(tokens[pos].value, "+") == 0 ||
          strcmp(tokens[pos].value, "-") == 0)) {

        char op[2];
        strcpy(op, tokens[pos].value);
        pos++;

        char *right = term();
        left = generateTemp(left, op, right);
    }

    return left;
}

// ================= RELATIONAL =================
char* relExpr() {
    char *left = expr();

    if(pos < tokenCount &&
       (strcmp(tokens[pos].value, "<") == 0 ||
        strcmp(tokens[pos].value, ">") == 0 ||
        strcmp(tokens[pos].value, "<=") == 0 ||
        strcmp(tokens[pos].value, ">=") == 0 ||
        strcmp(tokens[pos].value, "==") == 0 ||
        strcmp(tokens[pos].value, "!=") == 0)) {

        char op[3];
        strcpy(op, tokens[pos].value);
        pos++;

        char *right = expr();
        return generateTemp(left, op, right);
    }

    return left;
}

// ================= LOGICAL =================
char* boolExpr() {
    char *left = relExpr();

    while(pos < tokenCount &&
         (strcmp(tokens[pos].value, "&&") == 0 ||
          strcmp(tokens[pos].value, "||") == 0)) {

        char op[3];
        strcpy(op, tokens[pos].value);
        pos++;

        char *right = relExpr();

        left = generateTemp(left, op, right);
    }

    return left;
}

// ================= IF-ELSE =================
void ifStatement() {

    pos++; // skip if

    if(pos >= tokenCount || strcmp(tokens[pos].value, "(") != 0)
        error("Expected '('");

    pos++;

    char *cond = boolExpr();  // 🔥 UPDATED

    if(pos >= tokenCount || strcmp(tokens[pos].value, ")") != 0)
        error("Expected ')'");

    pos++;

    char *L1 = newLabel();
    char *L2 = newLabel();

    char buffer[50];

    sprintf(buffer, "ifFalse %s goto %s", cond, L1);
    emit(buffer);

    stmt();

    sprintf(buffer, "goto %s", L2);
    emit(buffer);

    sprintf(buffer, "%s:", L1);
    emit(buffer);

    if(pos < tokenCount && strcmp(tokens[pos].value, "else") == 0) {
        pos++;
        stmt();
    }

    sprintf(buffer, "%s:", L2);
    emit(buffer);
}

// ================= WHILE =================
void whileStatement() {

    pos++; // skip while

    char *L1 = newLabel();
    char *L2 = newLabel();

    char buffer[50];

    sprintf(buffer, "%s:", L1);
    emit(buffer);

    if(pos >= tokenCount || strcmp(tokens[pos].value, "(") != 0)
        error("Expected '(' after while");

    pos++;

    char *cond = boolExpr();  // 🔥 UPDATED

    if(pos >= tokenCount || strcmp(tokens[pos].value, ")") != 0)
        error("Expected ')'");

    pos++;

    sprintf(buffer, "ifFalse %s goto %s", cond, L2);
    emit(buffer);

    stmt();

    sprintf(buffer, "goto %s", L1);
    emit(buffer);

    sprintf(buffer, "%s:", L2);
    emit(buffer);
}

// ================= FOR LOOP =================
void forStatement() {

    pos++; // skip for

    if(pos >= tokenCount || strcmp(tokens[pos].value, "(") != 0)
        error("Expected '(' after for");

    pos++;

    char buffer[100];

    // 🔹 INIT
    char var[20];
    strcpy(var, tokens[pos].value);
    pos++;

    if(strcmp(tokens[pos].value, "=") != 0)
        error("Expected '=' in for init");
    pos++;

    char *val = expr();

    sprintf(buffer, "%s = %s", var, val);
    emit(buffer);

    if(strcmp(tokens[pos].value, ";") != 0)
        error("Expected ';'");
    pos++;

    // 🔹 LABELS
    char *L1 = newLabel();
    char *L2 = newLabel();

    sprintf(buffer, "%s:", L1);
    emit(buffer);

    // 🔹 CONDITION
    char *cond = boolExpr();

    if(strcmp(tokens[pos].value, ";") != 0)
        error("Expected ';'");
    pos++;

    sprintf(buffer, "ifFalse %s goto %s", cond, L2);
    emit(buffer);

    // 🔥 SAVE increment range properly
    int incStart = pos;

    while(pos < tokenCount && strcmp(tokens[pos].value, ")") != 0) {
        pos++;
    }

    int incEnd = pos;   // 🔥 important

    if(strcmp(tokens[pos].value, ")") != 0)
        error("Expected ')'");
    pos++;

    // 🔹 BODY
    stmt();

    // 🔥 PARSE INCREMENT (SAFE WAY)
    int tempPos = incStart;
    pos = tempPos;

    char incVar[20];
    strcpy(incVar, tokens[pos].value);
    pos++;

    if(strcmp(tokens[pos].value, "=") != 0)
        error("Expected '=' in increment");
    pos++;

    char *incExpr = expr();

    char incBuffer[50];
    sprintf(incBuffer, "%s = %s", incVar, incExpr);
    emit(incBuffer);

    // 🔥 RESTORE POSITION CORRECTLY
    pos = incEnd + 1;

    // 🔹 LOOP BACK
    sprintf(buffer, "goto %s", L1);
    emit(buffer);

    // 🔹 EXIT
    sprintf(buffer, "%s:", L2);
    emit(buffer);
}

    
// ================= STATEMENT =================
void stmt() {

    // 🔹 IF
    if(pos < tokenCount && strcmp(tokens[pos].value, "if") == 0) {
        ifStatement();
        return;
    }

    // 🔹 WHILE
    if(pos < tokenCount && strcmp(tokens[pos].value, "while") == 0) {
        whileStatement();
        return;
    }

    // 🔹 FOR 🔥
    if(pos < tokenCount && strcmp(tokens[pos].value, "for") == 0) {
        forStatement();
        return;
    }

    // 🔹 ASSIGNMENT
    if(pos < tokenCount && strcmp(tokens[pos].type, "ID") == 0) {

        char var[20];
        strcpy(var, tokens[pos].value);
        addSymbol(var);
        pos++;

        if(pos >= tokenCount || strcmp(tokens[pos].value, "=") != 0)
            error("Expected '='");

        pos++;

        char *res = expr();

        if(pos >= tokenCount || strcmp(tokens[pos].value, ";") != 0)
            error("Expected ';'");

        pos++;

        char buffer[50];
        sprintf(buffer, "%s = %s", var, res);
        emit(buffer);
    }
    else {
        error("Invalid statement");
    }
}

// ================= PARSE =================
void parse() {
    pos = 0;

    while(pos < tokenCount) {
        stmt();
    }

    printf("\nSyntax Valid\n");
}