#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define OPCODE(x)  (((x) >> (2)) & (0x3F))
#define D_FIELD(x) (((x) >> (1)) & (0x01))
#define W_FIELD(x) ((x) & (0x01))
#define MOD(x)     (((x) >> (14)) & (0x03))
#define REG(x)     (((x) >> (11)) & (0x07))
#define RM(x)      (((x) >> (8)) & (0x07))

// [REG()]
static char *byte_regs[8] = {
    "al",
    "cl",
    "dl",
    "bl",
    "ah",
    "ch",
    "dh",
    "bh",
};

// [REG()]
static char *word_regs[8] = {
    "ax",
    "cx",
    "dx",
    "bx",
    "sp",
    "bp",
    "si",
    "di",
};

// [RM()]
static char *displacements[8] = {
    "bx + si",
    "bx + di",
    "bp + si",
    "bp + di",
    "si",
    "di",
    "bp",
    "bx",
};


int OpenReadBin(char *path, u8 **data, long *size)
{
    FILE *handle = fopen(path, "r");

    if (!handle) {
        return -1;
    }

    fseek(handle, 0, SEEK_END);
    *size = ftell(handle);
    fseek(handle, 0, SEEK_SET);

    if (size <= 0) {
        fclose(handle);
        return -1;
    }

    *data            = (u8 *)malloc(sizeof(u8) * (*size));
    size_t readBytes = fread(*(void **)data, sizeof(u8), *size, handle);

    if (readBytes <= 0) {
        free(data);
        fclose(handle);
        return -1;
    }

    return 0;
}

size_t Decode_RegMem_toFrom_Reg(u32 inst)
{
    size_t encoding_bytes = 0;

    u8 D_field = (u8)D_FIELD((u8)inst);
    u8 W_field = (u8)W_FIELD((u8)inst);

    u8 RM_field  = (u8)RM((u16)inst);
    u8 REG_field = (u8)REG((u16)inst);

    char *reg_str = (W_field ? word_regs[REG_field] : byte_regs[REG_field]);


    u8 MOD_field = MOD((u16)inst);
    switch (MOD_field) {
        case 0x00:
            {
                if (RM_field == (u8)0x0006u) { // Direct Access
                    printf("mov %s, [%d]\n", reg_str, (u16)(inst >> 16));
                    encoding_bytes = 4;
                } else {
                    if (D_field == 0)
                        printf("mov [%s], %s\n", displacements[RM_field], reg_str);
                    else
                        printf("mov %s, [%s]\n", reg_str, displacements[RM_field]);
                    encoding_bytes = 2;
                }
            }
            break;
        case 0x01: // MOD 01 | 8-bit displacement
            { // note: for some reason 'mov [bp], ch' & `mov dx, [bp]` fall here
                if (D_field == 0)
                    printf("mov [%s + %d], %s\n", displacements[RM_field], (u8)((inst >> 16)), reg_str);
                else
                    printf("mov %s, [%s + %d]\n", reg_str, displacements[RM_field], (u8)((inst >> 16)));
                encoding_bytes = 3;
            }
            break;
        case 0x02: // MOD 10 | 16 bit displacement
            {
                if (D_field == 0)
                    printf("mov [%s + %d], %s\n", displacements[RM_field], (u16)((inst >> 16)), reg_str);
                else
                    printf("mov %s, [%s + %d]\n", reg_str, displacements[RM_field], (u16)((inst >> 16)));
                encoding_bytes = 4;
            }
            break;
        case 0x03: // Register mode
            {
                char *rm_str = (W_field ? word_regs[RM_field] : byte_regs[RM_field]);

                if (D_field == 0)
                    printf("mov %s, %s\n\0", rm_str, reg_str);
                else
                    printf("mov %s, %s\n\0", reg_str, rm_str);

                encoding_bytes = 2;
            }
            break;
        default:
            printf("ERROR: Unknown MOD field: 0x%x\n", MOD_field);
            break;
    }

    return encoding_bytes;
}

int main(int argc, char *argv[])
{
    u8  *data = NULL;
    long size = {};

    if (argc > 1) {
        char *filename = argv[1];
        if (OpenReadBin(filename, &data, &size) < 0)
            return 0;
    } else {
        printf("No input given\n");
        return 0;
    }

    assert(data);

    printf("bits 16\n\n");

    size_t current_offset = 0;
    for (size_t i = 0; current_offset < (size_t)size; i++) {

        u64 inst = *(u64 *)(data + current_offset);
        if ((inst >> 2 & 0b00111111) == 0b00100010) {
            current_offset += Decode_RegMem_toFrom_Reg((u32)inst);
        } else if ((inst >> 4 & 0b00001111) == 0b00001011) {

            u8 w_field   = (u8)((inst >> 3) & 0x01);
            u8 reg_field = (u8)(inst & 0b00000111);

            if (w_field == 0) {
                u8 data_8bit = (u8)((inst >> 8));
                printf("mov %s, %d\n", byte_regs[reg_field], data_8bit);
                current_offset += 2;
            }
            if (w_field == 1) {
                u16 data_16bit = (u16)((inst >> 8));
                printf("mov %s, %d\n", word_regs[reg_field], data_16bit);
                current_offset += 3;
            }
        }
    }

    free(data);
    return 0;
}
