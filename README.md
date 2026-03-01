# YN Programming Language

YN is a fun, lightweight, dynamically parsed programming language built on top of standard C. It brings Python-style interpolations, strict typing, and loops into a unique new syntax format.

## Installation

To run `.yn` files natively using the `yn` command in your terminal anywhere on your system, you need to add the compiler to your system's Environment Variables `PATH`.

### For Windows Users
1. Copy the `ync.exe` compiler executable into a permanent folder on your system (e.g., `C:\YN_Compiler`).
2. Rename the file from `ync.exe` to `yn.exe`.
3. Press your Windows key and search for **"Edit the system environment variables"**, then press enter.
4. In the System Properties window, click the **"Environment Variables..."** button at the bottom.
5. In the bottom "System variables" section, find the variable named **`Path`** and double-click it.
6. Click **"New"** and type in the path to the folder where you saved the compiler (e.g., `C:\YN_Compiler`).
7. Click **OK** on all the windows to save your changes.
8. Restart your terminal completely.

### For Bash/Git Bash
Alternatively, if you strictly use Git Bash on Windows or Mac, you can just set up a terminal alias by running this command in the folder containing `ync.exe`:
```bash
echo alias yn=\'\"$(pwd)/ync.exe\"\' >> ~/.bash_profile && source ~/.bash_profile
```

---

## Running YN Programs

Once installed via the System PATH or alias instructions above, you can compile and run any `.yn` file simply by typing:

```bash
yn <filename>.yn
```

For example:
```bash
yn script.yn
```
