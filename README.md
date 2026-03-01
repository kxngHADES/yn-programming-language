# YN Programming Language

YN is a fun, lightweight, dynamically parsed programming language built on top of standard C. It brings Python-style interpolations, strict typing, and loops into a unique new syntax format.

## Installation

An automated installer is provided for Windows systems to easily configure `yn` so it runs natively exactly like Python.

### Quick Install (Windows Standalone GUI)
Unlike traditional C transpilers, the YN Compiler includes everything you need natively packed into a 1-click Graphical UI.
1. Download or share the `yn_install.exe` file anywhere on a Windows system.
2. Double-click `yn_install.exe` to launch the **YN Language GUI Installer**.
3. Choose to either install the native compiler only, or select the option to have the installer securely download and link GCC MinGW for you automatically in the background.
4. Once the GUI says Complete, it has cleanly deployed everything to `C:\yn` and registered your System PATH.
5. Restart your terminal exactly once!
6. **Done!**

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
