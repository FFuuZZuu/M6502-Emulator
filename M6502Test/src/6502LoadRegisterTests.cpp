#include <gtest/gtest.h>
#include "../../M6502Lib/src/m6502.h"

class M6502LoadRegisterTests : public testing::Test
{
public:
    m6502::Mem mem;
    m6502::CPU cpu;

    virtual void SetUp()
    {
        cpu.Reset(mem);
    }

    virtual void TearDown()
    {

    }

    void TestLoadRegisterImmediate(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register);
    void TestLoadRegisterZeroPage(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register);
    void TestLoadRegisterZeroPageX(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register);
    void TestLoadRegisterZeroPageY(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register);
    void TestLoadRegisterAbsolute(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register);
    void TestLoadRegisterAbsoluteX(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register);
    void TestLoadRegisterAbsoluteY(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register);
    void TestLoadRegisterAbsoluteXWhenItCrossesPageBoundary(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register);
    void TestLoadRegisterAbsoluteYWhenItCrossesPageBoundary(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register);

};

static void VerifyUnmodifiedFlagsFromLoadRegister(const m6502::CPU& cpu, const m6502::CPU& CPUCopy)
{
    EXPECT_EQ(cpu.C, CPUCopy.C);
    EXPECT_EQ(cpu.B, CPUCopy.B);
    EXPECT_EQ(cpu.D, CPUCopy.D);
    EXPECT_EQ(cpu.I, CPUCopy.I);
    EXPECT_EQ(cpu.V, CPUCopy.V);
}

TEST_F(M6502LoadRegisterTests, TheCPUDoesNothingWhenWeExecuteZeroCycles)
{
    // given:
    using namespace m6502;
    constexpr s32 NUM_CYCLES = 0;

    // when:
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    // then:
    EXPECT_EQ(CyclesUsed, 0);
}

TEST_F(M6502LoadRegisterTests, CPUCanExecuteMoreCyclesThanRequestedIfRequiredByTheInstruction)
{
    // given:
    using namespace m6502;
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x84;
    constexpr s32 NUM_CYCLES = 1;

    // when:
    s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

    // then:
    EXPECT_EQ(CyclesUsed, 2);
}

void M6502LoadRegisterTests::TestLoadRegisterImmediate(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register)
{
    // given:
    using namespace m6502;

    cpu.Z = cpu.N = true;

    mem[0xFFFC] = Opcode;
    mem[0xFFFD] = 0x84;

    // when:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute(2, mem);

    // then:
    EXPECT_EQ(cpu.*Register, 0x84);
    EXPECT_EQ(CyclesUsed, 2);
    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

void M6502LoadRegisterTests::TestLoadRegisterZeroPage(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register)
{
    // given:
    using namespace m6502;

    cpu.Z = cpu.N = true;

    mem[0xFFFC] = Opcode;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x37;

    // when:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute(3, mem);

    // then:
    EXPECT_EQ(cpu.*Register, 0x37);
    EXPECT_EQ(CyclesUsed, 3);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

void M6502LoadRegisterTests::TestLoadRegisterZeroPageX(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register)
{
    // given:
    using namespace m6502;

    cpu.Z = cpu.N = true;

    cpu.X = 5;

    mem[0xFFFC] = Opcode;
    mem[0xFFFD] = 0x42;
    mem[0x0047] = 0x37;

    // when:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute(4, mem);

    // then:
    EXPECT_EQ(cpu.*Register, 0x37);
    EXPECT_EQ(CyclesUsed, 4);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

void M6502LoadRegisterTests::TestLoadRegisterZeroPageY(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register)
{
    // given:
    using namespace m6502;

    cpu.Z = cpu.N = true;

    cpu.Y = 5;

    mem[0xFFFC] = Opcode;
    mem[0xFFFD] = 0x42;
    mem[0x0047] = 0x37;

    // when:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute(4, mem);

    // then:
    EXPECT_EQ(cpu.*Register, 0x37);
    EXPECT_EQ(CyclesUsed, 4);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

void M6502LoadRegisterTests::TestLoadRegisterAbsolute(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register)
{
    // given:
    using namespace m6502;

    cpu.Z = cpu.N = true;

    mem[0xFFFC] = Opcode;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; //0x4480
    mem[0x4480] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ(cpu.*Register, 0x37);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

void M6502LoadRegisterTests::TestLoadRegisterAbsoluteX(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register)
{
    // given:
    using namespace m6502;

    cpu.Z = cpu.N = true;

    cpu.X = 1;

    mem[0xFFFC] = Opcode;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; //0x4480
    mem[0x4481] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ(cpu.*Register, 0x37);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

void M6502LoadRegisterTests::TestLoadRegisterAbsoluteY(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register)
{
    // given:
    using namespace m6502;

    cpu.Z = cpu.N = true;

    cpu.Y = 1;

    mem[0xFFFC] = Opcode;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; //0x4480
    mem[0x4481] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ(cpu.*Register, 0x37);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

void M6502LoadRegisterTests::TestLoadRegisterAbsoluteXWhenItCrossesPageBoundary(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register)
{
    // given:
    using namespace m6502;

    cpu.X = 0xFF;

    mem[0xFFFC] = Opcode;
    mem[0xFFFD] = 0x02;
    mem[0xFFFE] = 0x44; // 0x4402
    mem[0x4501] = 0x37; // 0x4402 + 0xFF (crosses page boundary)
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    // when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ(cpu.*Register, 0x37);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

void M6502LoadRegisterTests::TestLoadRegisterAbsoluteYWhenItCrossesPageBoundary(m6502::Byte Opcode, m6502::Byte m6502::CPU::*Register)
{
    // given:
    using namespace m6502;

    cpu.Y = 0xFF;

    mem[0xFFFC] = Opcode;
    mem[0xFFFD] = 0x02;
    mem[0xFFFE] = 0x44; // 0x4402
    mem[0x4501] = 0x37; // 0x4402 + 0xFF (crosses page boundary)
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    // when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ(cpu.*Register, 0x37);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(M6502LoadRegisterTests, LDAImmediateCanLoadAValueIntoTheARegister)
{
    using namespace m6502;
    TestLoadRegisterImmediate(CPU::INS_LDA_IM, &CPU::A);
}

TEST_F(M6502LoadRegisterTests, LDXImmediateCanLoadAValueIntoTheXRegister)
{
    using namespace m6502;
    TestLoadRegisterImmediate(CPU::INS_LDX_IM, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, LDYImmediateCanLoadAValueIntoTheYRegister)
{
    using namespace m6502;
    TestLoadRegisterImmediate(CPU::INS_LDY_IM, &CPU::Y);
}

TEST_F(M6502LoadRegisterTests, LDAImmediateCanAffectTheZeroFlag)
{
    // given:
    using namespace m6502;
    cpu.A = 0x44;
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x0;
    CPU CPUCopy = cpu;

    // when:
    cpu.Execute(2, mem);

    // then:
    EXPECT_TRUE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(M6502LoadRegisterTests, LDAZeroPageCanLoadAValueIntoTheARegister)
{
    using namespace m6502;
    TestLoadRegisterZeroPage(CPU::INS_LDA_ZP, &CPU::A);
}

TEST_F(M6502LoadRegisterTests, LDXZeroPageCanLoadAValueIntoTheXRegister)
{
    using namespace m6502;
    TestLoadRegisterZeroPage(CPU::INS_LDX_ZP, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, LDYZeroPageCanLoadAValueIntoTheYRegister)
{
    using namespace m6502;
    TestLoadRegisterZeroPage(CPU::INS_LDY_ZP, &CPU::Y);
}

TEST_F(M6502LoadRegisterTests, LDAZeroPageXCanLoadAValueIntoTheXRegister)
{
    using namespace m6502;
    TestLoadRegisterZeroPageX(CPU::INS_LDA_ZPX, &CPU::A);
}

TEST_F(M6502LoadRegisterTests, LDYZeroPageXCanLoadAValueIntoTheYRegister)
{
    using namespace m6502;
    TestLoadRegisterZeroPageX(CPU::INS_LDY_ZPX, &CPU::Y);
}

TEST_F(M6502LoadRegisterTests, LDXZeroPageYCanLoadAValueIntoTheXRegister)
{
    using namespace m6502;
    TestLoadRegisterZeroPageY(CPU::INS_LDX_ZPY, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, LDAZeroPageXCanLoadAValueIntoTheARegisterWhenItWraps)
{
    // given:
    using namespace m6502;
    cpu.X = 0xFF;

    mem[0xFFFC] = CPU::INS_LDA_ZPX;
    mem[0xFFFD] = 0x80;
    mem[0x007F] = 0x37;

    // when:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute(4, mem);

    // then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(CyclesUsed, 4);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(M6502LoadRegisterTests, LDAAbsoluteCanLoadAValueIntoTheARegister)
{
    using namespace m6502;
    TestLoadRegisterAbsolute(CPU::INS_LDA_ABS, &CPU::A);
}

TEST_F(M6502LoadRegisterTests, LDXAbsoluteCanLoadAValueIntoTheXRegister)
{
    using namespace m6502;
    TestLoadRegisterAbsolute(CPU::INS_LDX_ABS, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, LDYAbsoluteCanLoadAValueIntoTheYRegister)
{
    using namespace m6502;
    TestLoadRegisterAbsolute(CPU::INS_LDY_ABS, &CPU::Y);
}

TEST_F(M6502LoadRegisterTests, LDAAbsoluteXCanLoadAValueIntoTheARegister)
{
    using namespace m6502;
    TestLoadRegisterAbsoluteX(CPU::INS_LDA_ABSX, &CPU::A);
}

TEST_F(M6502LoadRegisterTests, LDYAbsoluteXCanLoadAValueIntoTheYRegister)
{
    using namespace m6502;
    TestLoadRegisterAbsoluteX(CPU::INS_LDY_ABSX, &CPU::Y);
}

TEST_F(M6502LoadRegisterTests, LDXAbsoluteYCanLoadAValueIntoTheXRegister)
{
    using namespace m6502;
    TestLoadRegisterAbsoluteY(CPU::INS_LDX_ABSY, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, LDAAbsoluteXCanLoadAValueIntoTheARegisterWhenItCrossesPageBoundary)
{
    using namespace m6502;
    TestLoadRegisterAbsoluteXWhenItCrossesPageBoundary(CPU::INS_LDA_ABSX, &CPU::A);
}

TEST_F(M6502LoadRegisterTests, LDYAbsoluteXCanLoadAValueIntoTheYRegisterWhenItCrossesPageBoundary)
{
    using namespace m6502;
    TestLoadRegisterAbsoluteXWhenItCrossesPageBoundary(CPU::INS_LDY_ABSX, &CPU::Y);
}

TEST_F(M6502LoadRegisterTests, LDAAbsoluteYCanLoadAValueIntoTheARegister)
{
    using namespace m6502;
    TestLoadRegisterAbsoluteY(CPU::INS_LDA_ABSY, &CPU::A);
}

TEST_F(M6502LoadRegisterTests, LDAAbsoluteYCanLoadAValueIntoTheARegisterWhenItCrossesPageBoundary)
{
    using namespace m6502;
    TestLoadRegisterAbsoluteYWhenItCrossesPageBoundary(CPU::INS_LDA_ABSY, &CPU::A);
}

TEST_F(M6502LoadRegisterTests, LDXAbsoluteYCanLoadAValueIntoTheXRegisterWhenItCrossesPageBoundary)
{
    using namespace m6502;
    TestLoadRegisterAbsoluteYWhenItCrossesPageBoundary(CPU::INS_LDX_ABSY, &CPU::X);
}

TEST_F(M6502LoadRegisterTests, LDAIndirectXCanLoadAValueIntoTheARegister)
{
    // given:
    using namespace m6502;

    cpu.Z = cpu.N = true;

    cpu.X = 0x04;

    mem[0xFFFC] = CPU::INS_LDA_INDX;
    mem[0xFFFD] = 0x02;
    mem[0x0006] = 0x00; // 0x2 + 0x4
    mem[0x0007] = 0x80;
    mem[0x8000] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU CPUCopy = cpu;

    // when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(M6502LoadRegisterTests, LDAIndirectYCanLoadAValueIntoTheARegister)
{
    // given:
    using namespace m6502;

    cpu.Z = cpu.N = true;

    cpu.Y = 0x04;

    mem[0xFFFC] = CPU::INS_LDA_INDY;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x00;
    mem[0x0003] = 0x80;
    mem[0x8004] = 0x37; // 0x8000 + 0x4
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    // when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}

TEST_F(M6502LoadRegisterTests, LDAIndirectYCanLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary)
{
    // given:
    using namespace m6502;

    cpu.Y = 0xFF;

    mem[0xFFFC] = CPU::INS_LDA_INDY;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x02;
    mem[0x0003] = 0x80;
    mem[0x8101] = 0x37; // 0x8002 + 0xFF
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU CPUCopy = cpu;

    // when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyUnmodifiedFlagsFromLoadRegister(cpu, CPUCopy);
}