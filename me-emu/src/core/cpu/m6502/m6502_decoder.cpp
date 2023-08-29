#include "m6502_decoder.h"

m6502::Word m6502::CPU::AddrZeroPage(s32& Cycles, const Mem& memory)
{
    Byte ZeroPageAddr = FetchByte(Cycles, memory);
    return ZeroPageAddr;
}

m6502::Word m6502::CPU::AddrZeroPageX(s32& Cycles, const Mem& memory)
{
    Byte ZeroPageAddr = FetchByte(Cycles, memory);
    ZeroPageAddr += X;
    Cycles--;
    return ZeroPageAddr;
}

m6502::Word m6502::CPU::AddrZeroPageY(s32& Cycles, const Mem& memory)
{
    Byte ZeroPageAddr = FetchByte(Cycles, memory);
    ZeroPageAddr += Y;
    Cycles--;
    return ZeroPageAddr;
}

m6502::Word m6502::CPU::AddrAbsolute(s32& Cycles, const Mem& memory)
{
    Word AbsAddress = FetchWord(Cycles, memory);
    return AbsAddress;
}

m6502::Word m6502::CPU::AddrAbsoluteX(s32& Cycles, const Mem& memory)
{
    Word AbsAddress                = FetchWord(Cycles, memory);
    Word AbsAddressX               = AbsAddress + X;
    const bool CrossedPageBoundary = (AbsAddress ^ AbsAddressX) >> 8;
    if (CrossedPageBoundary) {
        Cycles--;
    }

    return AbsAddressX;
}

m6502::Word m6502::CPU::AddrAbsoluteX_5(s32& Cycles, const Mem& memory)
{
    Word AbsAddress  = FetchWord(Cycles, memory);
    Word AbsAddressX = AbsAddress + X;
    Cycles--;
    return AbsAddressX;
}

m6502::Word m6502::CPU::AddrAbsoluteY(s32& Cycles, const Mem& memory)
{
    Word AbsAddress                = FetchWord(Cycles, memory);
    Word AbsAddressY               = AbsAddress + Y;
    const bool CrossedPageBoundary = (AbsAddress ^ AbsAddressY) >> 8;
    if (CrossedPageBoundary) {
        Cycles--;
    }

    return AbsAddressY;
}

m6502::Word m6502::CPU::AddrAbsoluteY_5(s32& Cycles, const Mem& memory)
{
    Word AbsAddress  = FetchWord(Cycles, memory);
    Word AbsAddressY = AbsAddress + Y;
    Cycles--;
    return AbsAddressY;
}

m6502::Word m6502::CPU::AddrIndirectX(s32& Cycles, const Mem& memory)
{
    Byte ZPAddress = FetchByte(Cycles, memory);
    ZPAddress += X;
    Cycles--;
    Word EffectiveAddr = ReadWord(Cycles, ZPAddress, memory);
    return EffectiveAddr;
}

m6502::Word m6502::CPU::AddrIndirectY(s32& Cycles, const Mem& memory)
{
    Byte ZPAddress                 = FetchByte(Cycles, memory);
    Word EffectiveAddr             = ReadWord(Cycles, ZPAddress, memory);
    Word EffectiveAddrY            = EffectiveAddr + Y;
    const bool CrossedPageBoundary = (EffectiveAddr ^ EffectiveAddrY) >> 8;
    if (CrossedPageBoundary) {
        Cycles--;
    }
    return EffectiveAddrY;
}

m6502::Word m6502::CPU::AddrIndirectY_6(s32& Cycles, const Mem& memory)
{
    Byte ZPAddress      = FetchByte(Cycles, memory);
    Word EffectiveAddr  = ReadWord(Cycles, ZPAddress, memory);
    Word EffectiveAddrY = EffectiveAddr + Y;
    Cycles--;
    return EffectiveAddrY;
}


m6502::Word m6502::CPU::LoadPrg(const Byte* Program, u32 NumBytes, Mem& memory) const
{
    Word LoadAddress = 0;
    if (Program && NumBytes > 2) {
        u32 At        = 0;
        const Word Lo = Program[At++];
        const Word Hi = Program[At++] << 8;
        LoadAddress   = Lo | Hi;
        for (Word i = LoadAddress; i < LoadAddress + NumBytes - 2; i++) {
            // TODO: mmio_bus copy?
            memory[i] = Program[At++];
        }
    }

    return LoadAddress;
}

void m6502::CPU::PrintStatus() const
{
    printf("A: %d X: %d Y: %d\n", A, X, Y);
    printf("PC: %d SP: %d\n", PC, SP);
    printf("PS: %d\n", PS);
}
