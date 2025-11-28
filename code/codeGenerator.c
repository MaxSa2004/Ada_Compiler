// função transExpr : (Exp, Table, Temp) -> [Instr] recebe o destino onde colocar o resultado
// função transStm : (Stm, Table) -> [Instr]
// função transCond para compilar condições: transCond : (Cond, Table, Label(t), Label(f)) -> [Instr]
// função transArgs : (Exps, table) -> ...s
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGenerator.h"
// gerar temporários e etiquetas 
Instr *instr_head = NULL, *instr_tail = NULL;
int temp_count = 0, label_count = 0;

// criar operador temporário
Op newTemp() {
    Op op;
    op.kind = OP_TEMP;
    op.contents.temp_id = temp_count++;
    return op;
}

// criar operador label 
Op newLabel() {
    Op op;
    op.kind = OP_VAR;
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "L%d", label_count++);
    op.contents.name = strdup(buffer);
    return op;
}

Op newVar(char*name){
    Op op;
    op.kind = OP_VAR;
    op.contents.name = strdup(name);
    return op;
}

// criar operador constante
Op newConst(int val){
    Op op;
    op.kind = OP_CONST;
    op.contents.value = val;
    return op;
}

void init_code_generator(){
    instr_head = NULL;
    instr_tail = NULL;
    temp_count = 0;
    label_count = 0;
}

void emit(Opcode op, Op dest, Op arg1, Op arg2){
    Instr *new_instr = (Instr*)malloc(sizeof(Instr));
    new_instr->opcode = op;
    new_instr->arg3 = dest;
    new_instr->arg1 = arg1;
    new_instr->arg2 = arg2;
    new_instr->next = NULL;
    if(instr_head == NULL){
        instr_head = new_instr;
        instr_tail = new_instr;
    } else {
        instr_tail->next = new_instr;
        instr_tail = new_instr;
    }
    fprintf(stderr, "Emitted instruction: %d\n", op);
}

Op transExpr(Exp exp){
    Op dest, t1, t2;
    Op empty = {0};
    if(!exp) return empty;
    
    switch(exp->exp_t){
        case NUMEXP:
            return newConst(exp->fields.num);
        case IDEXP:
            return newVar(exp->fields.ident);
        case FLOATEXP:
            return newConst((int)exp->fields.fnum); // simplificação: tratar float como int
        case STREXP:
            // para simplificação, tratar string como variável
            return newVar(exp->fields.string);
        case BOOLEXP:
            return newConst(exp->fields.boolVal);
        case UNOEXP:
            t1 = transExpr(exp->fields.unoexp.child);
            dest = newTemp();
            if(exp->fields.unoexp.op == NOTexp){
                emit(OP_NOT, dest, t1, empty);
            } else {
                emit(OP_NEG, dest, t1, empty);
            }
            return dest;
        case OPEXP:
            t1 = transExpr(exp->fields.opexp.left);
            t2 = transExpr(exp->fields.opexp.right);
            dest = newTemp();
            Opcode op;
            switch(exp->fields.opexp.op){
                case SUM: 
                    op = OP_ADD; 
                    break;
                case SUB: 
                    op = OP_SUB; 
                    break;
                case TIMES: 
                    op = OP_MUL; 
                    break;
                case DIVISION: 
                    op = OP_DIV; 
                    break;
                case MODULUS:
                    op = OP_MOD;
                    break;
                case REMAINDER:
                    op = OP_REM;
                    break;
                case POW:
                    op = OP_POW;
                    break;
                case EQUAL:
                    op = OP_EQ; 
                    break;
                case INEQUAL:
                    op = OP_NEQ;
                    break;
                case LESSexp:
                    op = OP_LT;
                    break;
                case GREATERexp:
                    op = OP_GT;
                    break;
                case LEQexp:
                    op = OP_LEQ;
                    break;
                case GEQexp:
                    op = OP_GEQ;
                    break;
                case ORexp: 
                    op = OP_OR; 
                    break;
                case ANDexp:
                    op = OP_AND;
                    break;
                case XORexp:
                    op = OP_XOR;
                    break;
                
                default: 
                    op = OP_ADD; 
                    break; // default case
            }
            emit(op, dest, t1, t2);
            return dest;
        case PAREXP:
            return transExpr(exp->fields.parexp.inner);
        default:
            fprintf(stderr, "Unknown expression type\n");
            return empty;
    }
}

// void transCond(Exp exp, Op labelF){
//     Op t1,t2;
//     Op empty = {0};
//     if(exp->exp_t==OPEXP){
//         t1 = transExpr(exp->fields.opexp.left);
//         t2 = transExpr(exp->fields.opexp.right);
//         Opcode op;
//         switch(exp->fields.opexp.op){
//             case EQUAL:
//                 op = OP_NEQ; 
//                 break;
//             case INEQUAL:
//                 op = OP_EQ;
//                 break;
//             case LESSexp:
//                 op = OP_GEQ;
//                 break;
//             case GREATERexp:
//                 op = OP_LEQ;
//                 break;
//             case LEQexp:
//                 op = OP_GT;
//                 break;
//             case GEQexp:
//                 op = OP_LT;
//                 break;
//             default:
//                 op = OP_EQ;
//         }
//         emit(op, labelF, t1, t2);
//     }

// }

void transStm(Stm s){
    Op empty = {0};
    if(!s) return;
    fprintf(stderr, "Translating statement of type %d\n", s->stm_t);
    switch(s->stm_t){
        case ASSIGNSTM:  {
            Op right = transExpr(s->fields.assign.exp);
            Op left = newVar(s->fields.assign.ident);
            emit(OP_MOVE, left, right, empty);
            break;
        }
        case COMPOUNDSTM: 
            transStm(s->fields.compound.fst);
            transStm(s->fields.compound.snd);
            break;
        case IFSTM: {
            Op elseLabel = newLabel();
            Op endLabel = newLabel();
            Op condVal = transExpr(s->fields.ifstm.cond);
            emit(OP_JUMP_FALSE, elseLabel, condVal, empty);
            transStm(s->fields.ifstm.then_branch);
            emit(OP_LABEL, endLabel, empty, empty);
            emit(OP_LABEL, elseLabel, empty, empty);
            if(s->fields.ifstm.else_branch){
                transStm(s->fields.ifstm.else_branch);
            }
            emit(OP_LABEL, endLabel, empty, empty);
            break;
        }
        case WHILESTM: {
            Op startLabel = newLabel();
            Op endLabel = newLabel();
            emit(OP_LABEL, startLabel, empty, empty);
            Op condVal = transExpr(s->fields.whilestm.cond);
            emit(OP_JUMP_FALSE, endLabel, condVal, empty);
            transStm(s->fields.whilestm.branch);
            emit(OP_JUMP, startLabel, empty, empty);
            emit(OP_LABEL, endLabel, empty, empty);
            break;
        }
        case PUTSTM: {
            Op val = transExpr(s->fields.putstm.output);
            emit(OP_PRINT, val, empty, empty);
            break;
        }
        case PROCSTM:
            transStm(s->fields.proc.statements);
            break;
        default:
            break;
    }
    
}

void printOp(Op op){
    switch(op.kind){
        case OP_TEMP:
            printf("t%d", op.contents.temp_id);
            break;
        case OP_VAR:
            printf("%s", op.contents.name);
            break;
        case OP_CONST:
            printf("%d", op.contents.value);
            break;
        default:
            printf("unknown_op");
    }
}

char* getOpCodeString(Opcode op){
    switch(op){
        case OP_ADD: return "ADD";
        case OP_SUB: return "SUB";
        case OP_MUL: return "MUL";
        case OP_DIV: return "DIV";
        case OP_MOD: return "MOD";
        case OP_REM: return "REM";
        case OP_POW: return "POW";
        case OP_AND: return "AND";
        case OP_OR: return "OR";
        case OP_NOT: return "NOT";
        case OP_XOR: return "XOR";
        case OP_NEG: return "NEG";
        case OP_MOVE: return "MOVE";
        case OP_LOAD: return "LOAD";
        case OP_LABEL: return "LABEL";
        case OP_JUMP: return "JUMP";
        case OP_JUMP_FALSE: return "JUMP_FALSE";
        case OP_PRINT: return "PRINT";
        case OP_READ: return "READ";
        case OP_EQ: return "EQ";
        case OP_NEQ: return "NEQ";
        case OP_LT: return "LT";
        case OP_GT: return "GT";
        case OP_LEQ: return "LEQ";
        case OP_GEQ: return "GEQ";
        default: return "UNKNOWN_OPCODE";
    }
}

void printTAC(Instr *head){
    Instr *curr = head;
    printf("\n---Three Address Code:---\n");
    while(curr){
        if(curr->opcode  == OP_LABEL){
            printOp(curr->arg3);
            printf(" :\n ");
        } else {
            printf("\t%s ", getOpCodeString(curr->opcode));
            if(curr->opcode==OP_MOVE){
                printOp(curr->arg3);
                printf(" := ");
                printOp(curr->arg1);
            } else if(curr->opcode==OP_PRINT){
                printOp(curr->arg3);
            } else if(curr->opcode == OP_JUMP || curr->opcode == OP_JUMP_FALSE){
                printOp(curr->arg3);
                if(curr->opcode == OP_JUMP_FALSE){
                    printf(" , ");
                    printOp(curr->arg1);
                }
            }  else if(curr->opcode == OP_NOT || curr->opcode == OP_NEG){
                printOp(curr->arg3);
                printf(" := ");
                printOp(curr->arg1);
            }
            else {
                printOp(curr->arg3);
                printf(" := ");
                printOp(curr->arg1);
                printf(" , ");
                printOp(curr->arg2);
            }
            printf("\n");
        }
        curr = curr->next;
    }
    printf("-----------------------\n");
}