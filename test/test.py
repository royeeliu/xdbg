import os, xdbg

print("Current Directory:", os.getcwd())

for item in dir(xdbg.DebugSymbols):
    print(item)
