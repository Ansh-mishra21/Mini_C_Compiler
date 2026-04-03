#ifndef ICG_H
#define ICG_H

char* generateTemp(char *arg1, char *op, char *arg2);
void printICG();

char* newLabel();
void emit(char *instruction);

#endif