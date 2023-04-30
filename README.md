# 8086 Disassembler

A simple 8086 instruction decoder as part of the Computer, Enhance! performance aware programming series by Casey Muratori.

changelog:

2023-04-24: only decodes mov instruction, assuming every instruction is encoded in 2-bytes (listings 37, 38)
2023-04-29: added decoding for immediate & src/dst address calculation with displacement (listing 39)
