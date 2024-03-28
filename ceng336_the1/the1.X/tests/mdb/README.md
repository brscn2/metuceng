# Python MDB Wrapper

MDB (Microchip Debugger) wrapper written in Python 3 using [subprocess](https://docs.python.org/3/library/subprocess.html) module in standard library. No external dependencies.

## Motivation

Although you can feed command files to `mdb`, it's not enough for some use cases:

- Write tests in a Turing-complete language.
  - Most importantly, MDB will be stuck in a `wait` command if the program doesn't hit a breakpoint. Adding a timeout to wait does not solve the issue becuase the commands are executed one by one no matter what (i.e., you cannot tell if you have stopped at a breakpoint or not).
  - Some test cases may require the tester to make decisions based on the program's output.
- MDB takes time to start (about 10 seconds on i7-9750H running Ubuntu 22.04). With this wrapper, you can reset the program and run multiple tests with the same MDB instance.

Example use case: This wrapper allows you to write a grader script more easily.

## Usage

TODO: More documentation will be added later.

## License

MIT License. See [LICENSE](LICENSE) for more details.

# Relevant Resources

- [Microchip Debugger (MDB) User's Guide](https://ww1.microchip.com/downloads/en/DeviceDoc/50002102G.pdf)
- [CI/CD Wizard in MPLAB X IDE User's Guide](https://ww1.microchip.com/downloads/en/DeviceDoc/CICD_Wizard_in_MPLAB_X_IDE_Users_Guide_50003243.pdf)
  - Includes guidelines for testing C programs. This might be a better choice if your project is in C.
