#include <stdio.h>
#include <stdlib.h>

typedef unsigned short uint16;

/*
    https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf
 */

#define OPCODE(x)  (((x) >> (2)) & (0x003F))
#define D_FIELD(x) (((x) >> (1)) & (0x0001))
#define W_FIELD(x) ((x) & (0x0001))
#define MOD(x)     (((x) >> (14)) & (0x0003))
#define REG(x)     (((x) >> (11)) & (0x0007))
#define RM(x)      (((x) >> (8)) & (0x0007))

const char *byteReg[] = {
    "al",
    "cl",
    "dl",
    "bl",
    "ah",
    "ch",
    "dh",
    "bh",
};

const char *wordReg[] = {
    "ax",
    "cx",
    "dx",
    "bx",
    "sp",
    "bp",
    "si",
    "di",
};



int OpenReadBin(const char *path, void **data, long *size)
{
    FILE *file = fopen(path, "r");

    if (!file) {
        return -1;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size <= 0) {
        fclose(file);
        return -1;
    }

    uint16 w;

    (*data)          = (void *)malloc(sizeof(char) * (*size));
    size_t readBytes = fread(*(void **)data, sizeof(char), *size, file);

    if (readBytes <= 0) {
        free(data);
        fclose(file);
        return -1;
    }

    return 0;
}

void HexDumpInstruction(uint16 inst)
{
    printf("0x%x\n", inst);
    printf("OPCODE  : 0x%06x\n", OPCODE(inst));
    printf("D_FIELD : 0x%02x\n", D_FIELD(inst));
    printf("W_FIELD : 0x%02x\n", W_FIELD(inst));
    printf("MOD     : 0x%02x\n", MOD(inst));
    printf("REG     : 0x%03x\n", REG(inst));
    printf("RM      : 0x%03x\n", RM(inst));
}

void Decode(uint16 inst)
{
    char mod = MOD(inst);
    switch (mod) {
        case 0x00:
            printf("NOT IMPLEMENTED: Memory mode, no displacement follows\n");
            break;
        case 0x01:
            printf("NOT IMPLEMENTED: Memory mode, 8-bit displacement follows\n");
            break;
        case 0x02:
            printf("NOT IMPLEMENTED: Memory mode, 16-bit displacement follows\n");
            break;
        case 0x03:
            {
                // check D field:
                //      D = 0: REG = src, RM = dst
                //      D = 1: REG = dst, RM = src

                char DField = D_FIELD(inst);
                char WField = W_FIELD(inst);

                const char *reg = (WField ? wordReg[REG(inst)] : byteReg[REG(inst)]);
                const char *rm  = (WField ? wordReg[RM(inst)] : byteReg[RM(inst)]);

                if (DField == 0x0)
                    printf("mov %s, %s\n", rm, reg);
                if (DField == 0x1)
                    printf("mov %s, %s\n", reg, rm);
            }
            break;

        default:
            printf("Unknown MOD field: 0x%x\n", mod);
            break;
    }
}

int main(int argc, char const *argv[])
{
    void *data = NULL;
    long  size = {};

    if (argc > 1) {
        if (OpenReadBin(argv[1], &data, &size) < 0)
            return 0;
    } else {
        printf("No input given\n");
        return 0;
    }


    printf("bits 16\n\n");

    // Since we are only dealing with 2-bytes instruction
    // for now uint16 is fine
    // AND assuming there are 'mov' instructions for now
    size_t instruction_count = size / 2;

    for (size_t i = 0; i < instruction_count; i++) {
        uint16 inst = *((uint16 *)data + 1 * i);
        Decode(inst);
    }

    free(data);
    return 0;
}
