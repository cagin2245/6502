#pragma once
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
/*

6502 64k Microprocessor Emulator


C64 Memory Map   : https://sta.c64.org/cbm64mem.html
C64 Reset PRocess: https://www.c64-wiki.com/wiki/Reset_(Process)


Registers:

Program Counter  (Instruction address register (IAR)):

-> Pointer to the adress that the CPU is executing the code for
Usually, the PC is incremented after fetching an instruction,
and holds the memory address of ("points to") the next instruction that would be executed.

Processors usually fetch instructions sequentially from memory,
but control transfer instructions change the sequence by placing a new value in the PC.
These include branches (sometimes called jumps), subroutine calls, and returns.
A transfer that is conditional on the truth of some assertion lets the computer
follow a different sequence under different conditions.

Stack Pointer:

-> Adress of the current part of the stack that program is using.
Small piece of memory that gets reused.
Purpose is to keep track of a call stack.

Processor Status:

-> Single byte that bunch of bits in it that have flags in it that gets set depending on
what certain instruction is happening.

CARRY_FLAG          -> 0 = FALSE             1 = TRUE

The carry flag is set if the last operation caused an overflow from bit 7 of the result or an underflow from bit 0.
This condition is set during arithmetic, comparison and during logical shifts.
It can be explicitly set using the 'Set Carry Flag' (SEC) instruction and cleared with 'Clear Carry Flag' (CLC).


ZERO_FLAG           -> 0 = RESULT NOT ZERO   1 = RESULT ZERO

The zero flag is set if the result of the last operation as was zero.


IRQ_DISABLE_FLAG    -> 0 = ENABLE            1 = DISABLE

The interrupt disable flag is set if the program has executed a 'Set Interrupt Disable' (SEI) instruction.
While this flag is set the processor will not respond to interrupts from devices until
it is cleared by a 'Clear Interrupt Disable' (CLI) instruction.


DECIMAL_MODE_FLAG   -> 0 = FALSE             1 = TRUE

While the decimal mode flag is set the processor will obey
the rules of Binary Coded Decimal (BCD) arithmetic during addition and subtraction.
The flag can be explicitly set using 'Set Decimal Flag' (SED) and cleared with 'Clear Decimal Flag' (CLD).


BREAK_COMMAND_FLAG  -> 0 = NO_BREAK          1 = BREAK

The break command bit is set when a BRK instruction has been executed and an interrupt has been generated to process it.


UNUSED              ->


OVERFLOW_FLAG       -> 0 = FALSE             1 = TRUE

The overflow flag is set during arithmetic operations if the result has yielded an invalid 2's complement result
(e.g. adding to positive numbers and ending up with a negative result: 64 + 64 => -128).
It is determined by looking at the carry between bits 6 and 7 and between bit 7 and the carry flag.


NEGATIVE_FLAG       -> 0 = POSITIVE          1 = NEGATIVE

The negative flag is set if the result of the last operation had bit 7 set to a one.

*/
typedef uint8_t Byte;
typedef uint16_t Word;



using u32 = unsigned int;


struct Mem // Memory
{
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];
    void Initialize()
    {
        for (u32 i = 0; i < MAX_MEM; i++)
        {
            Data[i] = 0;
        }
    }
    /* read 1 byte*/
    Byte operator[](u32 Address)const
    {

        // assert data here as < MAX_MEM
        return Data[Address];
    }
    /* write 1 byte*/
    Byte& operator[](u32 Address)
    {

        // assert data here as < MAX_MEM
        return Data[Address];
    }
    void WriteWord(Word Value, u32 Address, u32& Cycles)
    {
        Data[Address] = Value & 0xFF;
        Data[Address + 1] = (Value >> 8);
        Cycles -= 2;
    }
};
struct CPU
{

    Word PC; // Program Counter
    Word SP; // Stack Pointer

    Byte A, X, Y; // Accumulator, Index Register X, Index Register Y -> Registers


    // Processor state
    // Status flags
    Byte C : 1; // Carry Flag
    Byte Z : 1; // Zero Flag
    Byte I : 1; // Interrupt Disable
    Byte D : 1; // Decimal Mode
    Byte B : 1; // Break Command
    Byte V : 1; // Overflow Command
    Byte N : 1; // Negative Flag

    // Processor Status
    void Reset(Mem& memory) // Resetting the CPU when turning on our PC
    {
        PC = 0xFFFC;
        SP = 0x0100;
        D = 0;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
        memory.Initialize();



    }

    Byte FetchByte(u32& Cycles, Mem& memory)
    {
        Byte Data = memory[PC];
        PC++;
        Cycles--;
        return Data;

    }
    Word FetchWord(u32 Cycles, Mem& memory)
    {
        // 6502 is little endian -> First byte to read is going to be least significant byte of the data
        Word Data = memory[PC];
        PC++;


        Data |= (memory[PC] << 8);
        PC++;
        Cycles += 2;
        // To handle endiannes we have to swap bytes
        // if( PLATFORM_BIG_ENDIAN)
        // SwapBytesInWord(Data)
        return Data;
    }

    Byte ReadByte(u32& Cycles, Byte Address, Mem& memory)
    {
        Byte Data = memory[Address];
        Cycles--;
        return Data;
    }
    // opcodes
    static constexpr Byte INS_LDA_IM = 0xA9, INS_LDA_ZP = 0xA5, INS_LDA_ZPX = 0xB5,

        INS_JSR = 0x20; // LDA Immidiate Mode -> First byte is opcode, second byte is actual value to load


    void LDASetStatus()
    {
        Z = (A == 0);
        N = (A & 0b10000000) > 0;
    }

    void Execute(u32 Cycles, Mem& memory)
    {
        // LDA Load Accumulator -> Take data and put in in register,
        // zero and negative flags getting set appropriate (Set Z if A = 0, set N if bit 7 of A is set)
        // JSR - Jump to Subroutine -> Pushes the adress (minus one) of the return point on to the stack and then sets the PC to the target memory address
        while (Cycles > 0)
        {
            Byte Ins = FetchByte(Cycles, memory); // instruction
            switch (Ins)
            {
            case INS_LDA_IM:
            {
                Byte Value = FetchByte(Cycles, memory);
                A = Value;
                LDASetStatus();
            }break;
            case INS_LDA_ZP:
            {
                Byte ZeroPageAddress = FetchByte(Cycles, memory);
                A = ReadByte(Cycles, ZeroPageAddress, memory);
                LDASetStatus();
            }
            break;
            case INS_LDA_ZPX:
            {
                Byte ZeroPageXAddress = FetchByte(Cycles, memory);
                ZeroPageXAddress += X;
                Cycles--;
                A = ReadByte(Cycles, ZeroPageXAddress, memory);
                LDASetStatus();
            }break;
            case INS_JSR:
            {
                Word SubAddr = FetchWord(Cycles, memory);
                memory.WriteWord(PC - 1, SP, Cycles);
                SP++;
                PC = SubAddr;
                Cycles--;
            } break;
            default:
            {
                printf("Instruction is not handled %d", Ins);
            }break;
            }
        }

    }


};
