import xdbg

dc = xdbg.DebugClient.Current
sym = dc.Symbols
names = list(sym.LoadedModules.keys())
mod = sym.LoadedModules[names[0]]
print(mod)
print("File version:", mod.FileVersion)
print("Product version:", mod.ProductVersion)
