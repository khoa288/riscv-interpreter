import os

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
        "expected": """r[0] = 0x0
r[1] = 0xa
r[2] = 0x14
r[3] = 0x1e
r[4] = 0xfffffff6
r[5] = 0xa
r[6] = 0x14
r[7] = 0x1e
""" + "".join([f"r[{i}] = 0x0\n" for i in range(8, 32)]),
    },
    # Test Case 2: Whitespace Handling
    {
        "name": "test_case_2",
        "input": """## start[1] = 5
addi    x2 ,   x1   ,    0x10
sw x2 , 4(x1 )
lw    x3, 4 ( x1 )
""",
        "expected": """r[0] = 0x0
r[1] = 0x5
r[2] = 0x15
r[3] = 0x15
""" + "".join([f"r[{i}] = 0x0\n" for i in range(4, 32)]),
    },
    # Test Case 3: Decimal and Hexadecimal Equivalence
    {
        "name": "test_case_3",
        "input": """## start[1] = 16
addi x2, x1, 17
addi x3, x1, 0x11
""",
        "expected": """r[0] = 0x0
r[1] = 0x10
r[2] = 0x21
r[3] = 0x21
""" + "".join([f"r[{i}] = 0x0\n" for i in range(4, 32)]),
    },
    # Test Case 4: Little-Endian Memory
    {
        "name": "test_case_4",
        "input": """## start[1] = 0x100
sw x2, 4(x1)
lw x3, 4(x1)
""",
        "expected": """r[0] = 0x0
r[1] = 0x100
r[2] = 0x0
r[3] = 0x0
""" + "".join([f"r[{i}] = 0x0\n" for i in range(4, 32)]),
    },
    # Test Case 5: Invalid Instructions
    {
        "name": "test_case_5",
        "input": """invalid_instruction x1, x2, x3
""",
        "expected": """r[0] = 0x0
""" + "".join([f"r[{i}] = 0x0\n" for i in range(1, 32)]),
    },
    # Test Case 6: Edge Case - No Operations
    {
        "name": "test_case_6",
        "input": """## start[1] = 0
## start[2] = 0
""",
        "expected": """r[0] = 0x0
r[1] = 0x0
r[2] = 0x0
""" + "".join([f"r[{i}] = 0x0\n" for i in range(3, 32)]),
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
