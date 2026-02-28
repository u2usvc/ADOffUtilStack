import sys

def djb2_hash(string_to_hash):
    dwHash = 0x6623662366236623

    mask64 = 0xFFFFFFFFFFFFFFFF

    for char in string_to_hash:
        c = ord(char)
        dwHash = ((dwHash << 5) + dwHash) + c
        dwHash &= mask64

    return dwHash

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 djb2.py \"stringToHash\"")
        sys.exit(1)

    input_str = sys.argv[1]
    result = djb2_hash(input_str)

    print(result)
