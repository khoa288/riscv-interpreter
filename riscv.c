#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "linkedlist.h"
#include "hashtable.h"
#include "riscv.h"

const int R_TYPE = 0;
const int I_TYPE = 1;
const int MEM_TYPE = 2;
const int U_TYPE = 3;
const int UNKNOWN_TYPE = 4;

/**
 * Return the type of instruction for the given operation
 * Available options are R_TYPE, I_TYPE, MEM_TYPE, UNKNOWN_TYPE
 */
static int get_op_type(char *op)
{
    const char *r_type_op[] = {"add", "sub", "and", "or", "xor", "slt", "sll", "sra"};
    const char *i_type_op[] = {"addi", "andi", "ori", "xori", "slti"};
    const char *mem_type_op[] = {"lw", "lb", "sw", "sb"};
    const char *u_type_op[] = {"lui"};
    for (int i = 0; i < (int)(sizeof(r_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(r_type_op[i], op) == 0)
        {
            return R_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(i_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(i_type_op[i], op) == 0)
        {
            return I_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(mem_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(mem_type_op[i], op) == 0)
        {
            return MEM_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(u_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(u_type_op[i], op) == 0)
        {
            return U_TYPE;
        }
    }
    return UNKNOWN_TYPE;
}
/*************** END HELPER FUNCTIONS PROVIDED FOR CONVENIENCE ****************/

// TODO: create any additional variables to store the state of the interpreter
registers_t *registers;
hashtable_t *memory;

void handle_r_type(char *op, char *instruction, registers_t *registers);
void handle_i_type(char *op, char *instruction, registers_t *registers);
void handle_mem_type(char *op, char *instruction, registers_t *registers);
void handle_u_type(char *op, char *instruction, registers_t *registers);

void preprocess_r_type(char *instruction, char **rd, char **rs1, char **rs2, int *rd_num, int *rs1_num, int *rs2_num);
void preprocess_i_type(char *instruction, char **rd, char **rs1, char **imm, int *rd_num, int *rs1_num, int *imm_num);
void preprocess_mem_type(char *instruction, char **rd, char **offset, char **rs1, int *rd_num, int *offset_num, int *rs1_num);
void preprocess_u_type(char *instruction, char **rd, char **imm, int *rd_num, int *imm_num);

// TODO: create any necessary helper functions
static void remove_special_char(char *s){
    if(s == NULL) return;
    // remove all spaces first
    int i = 0, j = 0;
    while (s[i] != '\0') {
        // If the current character is not a space, keep it
        if (s[i] != ' ') {
            s[j++] = s[i];
        }
        i++;
    }
    s[j] = '\0';

    // convert all , and () to space
    for(int i = 0; s[i]; i++){
        if(s[i] == ',' || s[i] == '(' || s[i] == ')'){
            s[i] = ' ';
        }
    }
}

static int get_value(char *s){
    // decimal parsing: start with x: x23 -> 23
    if (s[0] == 'x') {
        return atoi(s + 1);
    }
    // hex parsing: start with 0x: 0xFF -> 255 or -0xFF -> -255
    if ((s[0] == '0' && s[1] == 'x') || (s[0] == '-' && s[1] == '0' && s[2] == 'x')) {
        return (int)strtol(s, NULL, 16);
    }
    return atoi(s);
}

static int sign_extend_imm(int num){
    if(num & 0x800){
        return num | 0xFFFFF000;
    }
    return num;
}

static int sign_extend_byte(int num){
    if(num & 0x80){
        return num | 0xFFFFFF00;
    }
    return num;
}

static int get_byte(int num){
    return num & 0xFF;
}

static int get_word(int num){
    return num & 0xFFFFFFFF;
}

void init(registers_t *starting_registers){
    registers = starting_registers;
    // TODO: initialize any additional variables needed for state
    registers->r[0] = 0;
    memory = ht_init(4096);
}

void step(char *instruction)
{
    // Extracts and returns the substring before the first space character,
    // by replacing the space character with a null-terminator.
    // `instruction` now points to the next character after the space
    // See `man strsep` for how this library function works
    char *op = strsep(&instruction, " ");
    // Uses the provided helper function to determine the type of instruction
    int op_type = get_op_type(op);
    // Skip this instruction if it is not in our supported set of instructions
    if (op_type == UNKNOWN_TYPE)
    {
        return;
    }

    // TODO: write logic for evaluating instruction on current interpreter state
    remove_special_char(instruction); // remove all , and () from the instruction
    if(op_type == 0){
        handle_r_type(op, instruction, registers);
    } else if(op_type == 1){
        handle_i_type(op, instruction, registers);
    } else if (op_type == 2){
        handle_mem_type(op, instruction, registers);
    } else if (op_type == 3){
        handle_u_type(op, instruction, registers);
    }
}

void preprocess_r_type(char *instruction, char **rd, char **rs1, char **rs2, int *rd_num, int *rs1_num, int *rs2_num)
{
    *rd = strsep(&instruction, " ");
    *rs1 = strsep(&instruction, " ");
    *rs2 = strsep(&instruction, " ");
    *rd_num = get_value(*rd);
    *rs1_num = get_value(*rs1);
    *rs2_num = get_value(*rs2);
}

void handle_r_type(char *op, char *instruction, registers_t *registers)
{
    char *rd = NULL, *rs1 = NULL, *rs2 = NULL;
    int rd_num = 0, rs1_num = 0, rs2_num = 0;
    preprocess_r_type(instruction, &rd, &rs1, &rs2, &rd_num, &rs1_num, &rs2_num);
    if(strcmp(op, "add") == 0){
        registers->r[rd_num] = registers->r[rs1_num] + registers->r[rs2_num];
    }
    if(strcmp(op, "sub") == 0){
        registers->r[rd_num] = registers->r[rs1_num] - registers->r[rs2_num];
    }
    if(strcmp(op, "and") == 0){
        registers->r[rd_num] = registers->r[rs1_num] & registers->r[rs2_num];
    }
    if(strcmp(op, "or") == 0){
        registers->r[rd_num] = registers->r[rs1_num] | registers->r[rs2_num];
    }
    if(strcmp(op, "xor") == 0){
        registers->r[rd_num] = registers->r[rs1_num] ^ registers->r[rs2_num];
    }
    if(strcmp(op, "slt") == 0){
        if(registers->r[rs1_num] < registers->r[rs2_num]){
            registers->r[rd_num] = 1;
        } else {
            registers->r[rd_num] = 0;
        }
    }
    if(strcmp(op, "sll") == 0){
        registers->r[rd_num] = registers->r[rs1_num] << registers->r[rs2_num];
    }
    if(strcmp(op, "sra") == 0){
        registers->r[rd_num] = registers->r[rs1_num] >> registers->r[rs2_num];
    }
}

void preprocess_i_type(char *instruction, char **rd, char **rs1, char **imm, int *rd_num, int *rs1_num, int *imm_num){
    *rd = strsep(&instruction, " ");
    *rs1 = strsep(&instruction, " ");
    *imm = strsep(&instruction, " ");
    *rd_num = get_value(*rd);
    *rs1_num = get_value(*rs1);
    *imm_num = get_value(*imm);
    *imm_num = sign_extend_imm(*imm_num);
}

void handle_i_type(char *op, char *instruction, registers_t *registers){
    char *rd = NULL, *rs1 = NULL, *imm = NULL;
    int rd_num = 0, rs1_num = 0, imm_num = 0;
    preprocess_i_type(instruction, &rd, &rs1, &imm, &rd_num, &rs1_num, &imm_num);
    if(strcmp(op, "addi") == 0){
        registers->r[rd_num] = registers->r[rs1_num] + imm_num;
    }
    if(strcmp(op, "andi") == 0){
        registers->r[rd_num] = registers->r[rs1_num] & imm_num;
    }
    if(strcmp(op, "ori") == 0){
        registers->r[rd_num] = registers->r[rs1_num] | imm_num;
    }
    if(strcmp(op, "xori") == 0){
        registers->r[rd_num] = registers->r[rs1_num] ^ imm_num;
    }
    if(strcmp(op, "slti") == 0){
        if(registers->r[rs1_num] < imm_num){
            registers->r[rd_num] = 1;
        } else {
            registers->r[rd_num] = 0;
        }
    }
}

void preprocess_mem_type(char *instruction, char **rd, char **offset, char **rs1, int *rd_num, int *offset_num, int *rs1_num){
    *rd = strsep(&instruction, " ");
    *offset = strsep(&instruction, " ");
    *rs1 = strsep(&instruction, " ");
    *rd_num = get_value(*rd);
    *offset_num = get_value(*offset);
    *rs1_num = get_value(*rs1);
}

void handle_mem_type(char *op, char *instruction, registers_t *registers){
    char *rd = NULL, *offset = NULL, *rs1 = NULL;
    int rd_num = 0, offset_num = 0, rs1_num = 0;
    preprocess_mem_type(instruction, &rd, &offset, &rs1, &rd_num, &offset_num, &rs1_num);
    if(strcmp(op, "lw") == 0){
        int temp = 0;
        // little endian
        for(int i = 3; i > 0; i--){
            temp = temp | ht_get(memory, registers->r[rs1_num] + offset_num + i);
            temp = temp << 8;
        }
        temp = temp | ht_get(memory, registers->r[rs1_num] + offset_num);
        registers->r[rd_num] = temp;
    }
    if(strcmp(op, "lb") == 0){
        registers->r[rd_num] = sign_extend_byte(ht_get(memory, registers->r[rs1_num] + offset_num));
    }
    if(strcmp(op, "sw") == 0){
        // little endian
        int temp = get_word(registers->r[rd_num]);
        for(int i = 0; i < 4; i++){
            ht_add(memory, registers->r[rs1_num] + offset_num + i, get_byte(temp));
            temp = temp >> 8;
        }
    }
    if(strcmp(op, "sb") == 0){
        int temp = get_byte(registers->r[rd_num]);
        ht_add(memory, registers->r[rs1_num] + offset_num, temp);
    }
}

void preprocess_u_type(char *instruction, char **rd, char **imm, int *rd_num, int *imm_num){
    *rd = strsep(&instruction, " ");
    *imm = strsep(&instruction, " ");
    *rd_num = get_value(*rd);
    *imm_num = get_value(*imm);
}

void handle_u_type(char *op, char *instruction, registers_t *registers){
    char *rd = NULL, *imm = NULL;
    int rd_num = 0, imm_num = 0;
    preprocess_u_type(instruction, &rd, &imm, &rd_num, &imm_num);
    if(strcmp(op, "lui") == 0){
        registers->r[rd_num] = imm_num << 12;
    }
}