#include <gtest/gtest.h>
#include "../../M6502Lib/src/m6502.h"

class M6502StoreRegisterTests : public testing::Test
{
public:
    m6502::Mem mem;
    m6502::CPU cpu;

    virtual void SetUp()
    {
        cpu.Reset(0xFFFC, mem);
    }

    virtual void TearDown()
    {

    }

    static void VerifyUnmodifiedFlagsFromStoreRegister(const m6502::CPU& cpu, const m6502::CPU& CPUCopy)
    {
        EXPECT_EQ(cpu.Flag.C, CPUCopy.Flag.C);
        EXPECT_EQ(cpu.Flag.Z, CPUCopy.Flag.Z);
        EXPECT_EQ(cpu.Flag.B, CPUCopy.Flag.B);
        EXPECT_EQ(cpu.Flag.D, CPUCopy.Flag.D);
        EXPECT_EQ(cpu.Flag.I, CPUCopy.Flag.I);
        EXPECT_EQ(cpu.Flag.V, CPUCopy.Flag.V);
        EXPECT_EQ(cpu.Flag.N, CPUCopy.Flag.N);
    }

    void TestStoreRegisterZeroPage(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register)
    {
        // given:
        using namespace m6502;
        cpu.*Register = 0x2F;
        mem[0xFFFC] = Opcode;
        mem[0xFFFD] = 0x80;
        mem[0x0080] = 0x00;
        constexpr s32 EXPECTED_CYCLES = 3;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute(EXPECTED_CYCLES, mem);

        // then:
        EXPECT_EQ(ActualCycles, EXPECTED_CYCLES);
        EXPECT_EQ(mem[0x0080], 0x2F);
        VerifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
    }

    void TestStoreRegisterZeroPageX(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register)
    {
        // given:
        using namespace m6502;
        cpu.X = 0x0F;
        cpu.*Register = 0x42;
        mem[0xFFFC] = Opcode;
        mem[0xFFFD] = 0x80;
        mem[0x008F] = 0x00;
        constexpr s32 EXPECTED_CYCLES = 4;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute(EXPECTED_CYCLES, mem);

        // then:
        EXPECT_EQ(ActualCycles, EXPECTED_CYCLES);
        EXPECT_EQ(mem[0x008F], 0x42);
        VerifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
    }

    void TestStoreRegisterAbsolute(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register)
    {
        // given:
        using namespace m6502;
        cpu.*Register = 0x2F;
        mem[0xFFFC] = Opcode;
        mem[0xFFFD] = 0x00;
        mem[0xFFFE] = 0x80;
        mem[0x8000] = 0x00;
        constexpr s32 EXPECTED_CYCLES = 4;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute(EXPECTED_CYCLES, mem);

        // then:
        EXPECT_EQ(ActualCycles, EXPECTED_CYCLES);
        EXPECT_EQ(mem[0x8000], 0x2F);
        VerifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
    }
};

TEST_F(M6502StoreRegisterTests, STAZeroPageCanStoreTheARegisterIntoMemory)
{
    using namespace m6502;
    TestStoreRegisterZeroPage(CPU::INS_STA_ZP, &CPU::A);
}

TEST_F(M6502StoreRegisterTests, STXZeroPageCanStoreTheARegisterIntoMemory)
{
    using namespace m6502;
    TestStoreRegisterZeroPage(CPU::INS_STX_ZP, &CPU::X);
}

TEST_F(M6502StoreRegisterTests, STYZeroPageCanStoreTheARegisterIntoMemory)
{
    using namespace m6502;
    TestStoreRegisterZeroPage(CPU::INS_STY_ZP, &CPU::Y);
}

TEST_F(M6502StoreRegisterTests, STAAZeroPageXCanStoreTheARegisterIntoMemory)
{
    using namespace m6502;
    TestStoreRegisterZeroPageX(CPU::INS_STA_ZPX, &CPU::A);
}

TEST_F(M6502StoreRegisterTests, STYZeroPageXCanStoreTheARegisterIntoMemory)
{
    using namespace m6502;
    TestStoreRegisterZeroPageX(CPU::INS_STY_ZPX, &CPU::Y);
}

TEST_F(M6502StoreRegisterTests, STAAbsoluteCanStoreTheARegisterIntoMemory)
{
    using namespace m6502;
    TestStoreRegisterAbsolute(CPU::INS_STA_ABS, &CPU::A);
}

TEST_F(M6502StoreRegisterTests, STXAbsoluteCanStoreTheARegisterIntoMemory)
{
    using namespace m6502;
    TestStoreRegisterAbsolute(CPU::INS_STX_ABS, &CPU::X);
}

TEST_F(M6502StoreRegisterTests, STYAbsoluteCanStoreTheARegisterIntoMemory)
{
    using namespace m6502;
    TestStoreRegisterAbsolute(CPU::INS_STY_ABS, &CPU::Y);
}

TEST_F(M6502StoreRegisterTests, STAAbsoluteXCanStoreTheARegisterIntoMemory)
{
    // given:
    using namespace m6502;
    cpu.A = 0x42;
    cpu.X = 0x0F;
    mem[0xFFFC] = CPU::INS_STA_ABSX;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x80;
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    // when:
    const s32 ActualCycles = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ(ActualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(mem[0x800F], 0x42);
    VerifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
}

TEST_F(M6502StoreRegisterTests, STAAbsoluteYCanStoreTheARegisterIntoMemory)
{
    // given:
    using namespace m6502;
    cpu.A = 0x42;
    cpu.Y = 0x0F;
    mem[0xFFFC] = CPU::INS_STA_ABSY;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x80;
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    // when:
    const s32 ActualCycles = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ(ActualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(mem[0x800F], 0x42);
    VerifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
}

TEST_F(M6502StoreRegisterTests, STAIndirectXCanStoreTheARegisterIntoMemory)
{
    // given:
    using namespace m6502;
    cpu.A = 0x42;
    cpu.X = 0x0F;
    mem[0xFFFC] = CPU::INS_STA_INDX;
    mem[0xFFFD] = 0x20;
    mem[0x002F] = 0x00;
    mem[0x0030] = 0x80;
    mem[0x8000] = 0x00;
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU CPUCopy = cpu;

    // when:
    const s32 ActualCycles = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ(ActualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(mem[0x8000], 0x42);
    VerifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
}

TEST_F(M6502StoreRegisterTests, STAIndirectYCanStoreTheARegisterIntoMemory)
{
    // given:
    using namespace m6502;
    cpu.A = 0x42;
    cpu.Y = 0x0F;
    mem[0xFFFC] = CPU::INS_STA_INDY;
    mem[0xFFFD] = 0x20;
    mem[0x0020] = 0x00;
    mem[0x0021] = 0x80;
    mem[0x8000 + 0x0F] = 0x00;
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU CPUCopy = cpu;

    // when:
    const s32 ActualCycles = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ(ActualCycles, EXPECTED_CYCLES);
    EXPECT_EQ(mem[0x8000 + 0x0F], 0x42);
    VerifyUnmodifiedFlagsFromStoreRegister(cpu, CPUCopy);
}