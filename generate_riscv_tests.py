import os


def generate_expected_registers(values):
    result = []
    for i in range(32):
        value = values.get(i, 0x0)
        result.append(f"r[{i}] = 0x{value:x}\n")
    return "".join(result)


tests = [
    # Test Case 1: Basic Operations
    {
        "name": "test_case_1",
        "input": """## start[1] = 10
## start[2] = 20
add x3, x1, x2
sub x4, x1, x2
andi x5, x1, 0xFF
ori x6, x2, 0x10
xor x7, x1, x2
""",
        "expected": generate_expected_registers({
            1: 0xa,
            2: 0x14,
            3: 0x1e,
            4: 0xfffffff6,
            5: 0xa,
            6: 0x14,
            7: 0x1e
        }),
    },
    # Test Case 2: Whitespace Handling
    {
        "name": "test_case_2",
        "input": """## start[1] = 5
addi    x2 ,   x1   ,    0x10
sw x2 , 4(x1 )
lw    x3, 4 ( x1 )
""",
        "expected": generate_expected_registers({
            1: 0x5,
            2: 0x15,
            3: 0x15
        }),
    },
    # Test Case 3: Decimal and Hexadecimal Equivalence
    {
        "name": "test_case_3",
        "input": """## start[1] = 16
addi x2, x1, 17
addi x3, x1, 0x11
""",
        "expected": generate_expected_registers({
            1: 0x10,
            2: 0x21,
            3: 0x21
        }),
    },
    # Test Case 4: Invalid Instructions
    {
        "name": "test_case_4",
        "input": """invalid_instruction x1, x2, x3
""",
        "expected": generate_expected_registers({}),
    },
    # Test Case 5: Edge Case - No Operations
    {
        "name": "test_case_5",
        "input": """## start[1] = 0
## start[2] = 0
""",
        "expected": generate_expected_registers({
        }),
    },
    # Test Case 6: R-type Operations
    {
        "name": "test_case_6",
        "input": """## start[1] = 5
## start[2] = 3
nor x3, x1, x2
slt x4, x1, x2
sll x5, x1, x2
sra x6, x1, x2
""",
        "expected": generate_expected_registers({
            1: 0x5,
            2: 0x3,
            3: 0xfffffff8,
            4: 0x0,
            5: 0x28,
            6: 0x0
        }),
    },
    # Test Case 7: I-type Operations
    {
        "name": "test_case_7",
        "input": """## start[1] = 10
addi x2, x1, -5
slti x3, x1, 15
xori x4, x1, 0xFF
""",
        "expected": generate_expected_registers({
            1: 0xa,
            2: 0x5,
            3: 0x1,
            4: 0xf5
        }),
    },
    # Test Case 8: Memory Operations with LB and SB
    {
        "name": "test_case_8",
        "input": """## start[1] = 0x100
sb x2, 0(x1)
lb x3, 0(x1)
""",
        "expected": generate_expected_registers({
            1: 0x100
        }),
    },
    # Test Case 9: U-type Operations
    {
        "name": "test_case_9",
        "input": """lui x1, 0x12345
""",
        "expected": generate_expected_registers({
            1: 0x12345000
        }),
    },
]

os.makedirs("tests", exist_ok=True)

for test in tests:
    input_file = os.path.join("tests", f"{test['name']}_input.txt")
    expected_file = os.path.join("tests", f"{test['name']}_expected.txt")

    with open(input_file, "w") as f:
        f.write(test["input"])

    with open(expected_file, "w") as f:
        f.write(test["expected"])

print("Test cases generated successfully!")
