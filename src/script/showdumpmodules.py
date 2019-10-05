import xdbg

dc = xdbg.DebugClient.Current
sym = dc.Symbols
names = list(sym.LoadedModules.keys())
names.sort()
for item in names:
    print(item)
