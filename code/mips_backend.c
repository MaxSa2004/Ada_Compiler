#include <stdio.h>
#include "mips_backend.h"

void generateMIPS(Instr *head, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp)
    {
        fprintf(stderr, "Error opening file for MIPS output\n");
        return;
    }

    Instr *curr = head;
    while (curr)
    {
        switch (curr->opcode)
        {
        case OP_ADD:
            fprintf(fp, "add $t%d, $t%d, $t%d\n",
                    curr->arg3.contents.temp_id,
                    curr->arg1.contents.temp_id,
                    curr->arg2.contents.temp_id);
            break;
        case OP_SUB:
            fprintf(fp, "sub $t%d, $t%d, $t%d\n",
                    curr->arg3.contents.temp_id,
                    curr->arg1.contents.temp_id,
                    curr->arg2.contents.temp_id);
            break;
        case OP_MOVE:
            fprintf(fp, "move $t%d, $t%d\n",
                    curr->arg3.contents.temp_id,
                    curr->arg1.contents.temp_id);
            break;
        case OP_PRINT:
            fprintf(fp, "# syscall print placeholder\n");
            break;
        case OP_LABEL:
            fprintf(fp, "%s:\n", curr->arg3.contents.name);
            break;
        case OP_JUMP:
            fprintf(fp, "j %s\n", curr->arg3.contents.name);
            break;
        case OP_JUMP_FALSE:
            fprintf(fp, "beq $t%d, $zero, %s\n",
                    curr->arg1.contents.temp_id,
                    curr->arg3.contents.name);
            break;
        default:
            fprintf(fp, "# unhandled opcode\n");
        }
        curr = curr->next;
    }

    fclose(fp);
}
