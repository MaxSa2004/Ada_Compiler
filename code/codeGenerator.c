// função transExpr : (Exp, Table, Temp) -> [Instr] recebe o destino onde colocar o resultado
// função transStm : (Stm, Table) -> [Instr]
// função transCond para compilar condições: transCond : (Cond, Table, Label(t), Label(f)) -> [Instr]
// função transArgs : (Exps, table) -> ...s

// gerar temporários e etiquetas 
int temp_count = 0, label_count = 0;
int newTemp() {
    return temp_count++;
}
int newLabel() {
    return label_count++;
}

void transExpr(Exp exp, dest){
    switch(...){
        case BINOP:
            t1 = newTemp();
            t2 = newTemp();
            transExpr(exp->binop.left, t1);
            transExpr(exp->binop.right, t2);
            emit3(opcode(exp->binop.op), dest, t1, t2);
            break;
    }
}