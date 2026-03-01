# YN Programming Language

YN is a fun, lightweight, dynamically parsed programming language built on top of standard C. It brings Python-style interpolations, strict typing, and loops into a unique new syntax format.

## Installation

An automated installer is provided for Windows systems to easily configure `yn` so it runs natively exactly like Python.

### Quick Install (Windows)
1. Double click on `install.exe` inside this folder (or run it from your terminal).
2. It will automatically create the `C:\yn` core directory, install the compiler engine there, and permanently register the `C:\yn` alias into your Windows System PATH Environment Variables.
3. Restart your terminal exactly once!
4. **Done!**

### For Developers (Compiling from Source)
If you wish to modify the underlying compiler code (e.g., `parser.c`, `compiler.c`, `symbol_table.c`, or `ync.c`), you can easily recompile the entire engine by running the included build script:

```console
compile.bat
```

*(Or manually run: `gcc ync.c compiler.c parser.c symbol_table.c -o ync.exe`)*
Then follow the **Quick Install** instructions above to globally register your customized build!

---

## Running YN Programs

Once installed via the Auto-Installer or alias, you can compile and immediately interpret any `.yn` file simply by typing:

```bash
yn <filename>.yn
```

For example:
```bash
yn main.yn
```
