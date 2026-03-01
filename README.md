# YN Programming Language

YN is a fun, lightweight, dynamically parsed programming language built on top of standard C. It brings Python-style interpolations, strict typing, and loops into a unique new syntax format.

## Installation

An automated installer is provided for Windows systems to easily configure `yn` so it runs natively exactly like Python.

### Quick Install (Windows)
1. Double click on `install.exe` inside this folder (or run it from your terminal).
2. It will automatically create the `C:\yn` core directory, install the compiler engine there, and permanently register the `C:\yn` alias into your Windows System PATH Environment Variables.
3. Restart your terminal exactly once!
4. **Done!**

### For Bash/Git Bash
Alternatively, if you only strictly use Git Bash on Windows or Mac, you can bypass the installer and set up a terminal alias by running this command in the folder containing `ync.exe`:
```bash
echo alias yn=\'\"$(pwd)/ync.exe\"\' >> ~/.bash_profile && source ~/.bash_profile
```

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
