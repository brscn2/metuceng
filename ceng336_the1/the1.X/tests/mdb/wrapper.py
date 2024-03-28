# Wrapper for MDB
import subprocess
from .exceptions import *

def get_breakpoint(output):
    for line in output:
        if "\taddress:" in line:
            return line[line.rindex(':')+1:-1].upper()
    return None

class Mdb:
    """
    Python Wrapper around MDB, Microchip debugger.
    """

    def __init__(self):
        self.p = subprocess.Popen(
                "mdb", stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        while True:
            line = self.p.stderr.readline().decode()
            if line.startswith("WARNING: Unable to create a system terminal, creating a dumb terminal"):
                break
        # Output of last command
        self.last = []

    def exec(self, command):
        """
        Execute a given command, return output.
        """
        if (exitcode := self.p.poll()) != None:
            raise MdbException("MDP process is dead with exit code " + str(exitcode))
        command += "\necho DONE\n"
        self.p.stdin.write(command.encode())
        self.p.stdin.flush()
        lines = []
        while True:
            line = self.p.stdout.readline().decode()
            if line.endswith("/*DONE*/\n"):
                break
            # else:  # Uncomment for quick debugging
            #     print("[MDB]", line)
            lines.append(line)
        return lines

    def prelude(self, prelude):
        """
        Prelude commands: adjust settings and load a program.
        """
        lines = self.exec(prelude)
        for line in lines:
            if line.startswith("Program succeeded."):
                return
        raise MdbException("Couldn't load program. Output:\n" + "".join(lines))

    def quit(self):
        """
        Send quit command and wait until exit.
        Returns the exit code.
        """
        if (exitcode := self.p.poll()) != None:
            raise MdbException("MDP process is dead with exit code " + str(exitcode))
        # We cannot receive confirmation like other commands
        command = "quit\n"
        self.p.stdin.write(command.encode())
        self.p.stdin.flush()
        return self.p.wait()

    def run_timeout(self, timeout = 30000):
        """
        Run until a breakpoint is reached or timeout (30 seconds).
        Returns the address of breakpoint, None if timeout.
        """
        lines = self.exec("continue\nwait " + str(timeout))
        # Usual output:
        # ['>Running\n', '\n', '>\n', 'Single breakpoint: @0xF89\n', 'Simulator halted\n', 'Stop at\n', '\taddress:0x1a6\n', '\tfile:/home/.../main.s\n', '\tsource line:317\n', '\n', '>\n']
        # Worst case:
        # ['>Running\n', '\n', '>\n', 'Single breakpoint: @0xF89\n', 'Simulator halted\n', '\n']
        # Address in the next command's output
        # Timeout:
        # ['>Running\n', '\n', '>\n'] or ['>\n', '>\n']
        bp = get_breakpoint(lines)
        if bp is not None:
            return bp
        if not [i for i in lines if i.startswith("Simulator halted")]:
            # Genuine timeout
            return bp
        print("TESTER: Run didn't timeout but we don't have any address information.")
        lines = self.exec("halt")
        bp = get_breakpoint(lines)
        if bp is not None:
            print("TESTER: Determined the address from the next command.")
            return bp
        raise MdbException("Failed to determine the address of breakpoint.")

    def run(self, timeout = 30000):
        """
        Run until a breakpoint is reached and raise an exception if timeout (30 seconds).
        Returns the address of breakpoint.
        """
        bp = self.run_timeout(timeout)
        if not bp:
            raise TestFailed("Timeout is reached while waiting for a breakpoint")
        return bp

    def stopwatch(self):
        """
        Returns stopwatch cycle count.
        """
        for line in self.exec("stopwatch"):
            # Output looks like:
            # ['>Stopwatch cycle count = 4440176 (444.0176 ms)\n']
            if "Stopwatch cycle count" in line:
                return int(line[line.index('=')+2:line.index('(')-1])

    def breakpoint(self, bp):
        """
        Add a new breakpoint.
        """
        return self.exec("break *" + bp)

    def only_breakpoint(self, bp):
        """
        Clears all breakpoints and enables the given breakpoint.
        """
        return self.exec("delete\nbreak *" + bp)

    def clear_breakpoints(self):
        """
        Clear all Breakpoints.
        """
        return self.exec("delete")

    def watch(self, watchpoint):
        """
        Set watch point.
        Argument must be of form: "address breakonType[:value]"
        address can be a variable name.
        breakonType: W or RW
        """
        out = self.exec("watch " + watchpoint)
        if out[0].startswith(">Error: "):
            raise MdbException("Set watchpoint error:\n" + "".join(out))
        return out

    def get(self, name: str, datasize: int = 1):
        """
        Get a variable by name or address.
        """
        command = "print /datasize:" + str(datasize) + " " + name
        out = self.exec(command)
        # The output is not always on the first line because it sometimes
        # reports the breakpoint first.
        for line in out[::-1]:
            if '=' in line:
                line = line.strip()
                value = line[line.rindex('=')+1:]
                break
        else:
            print("TESTER ERROR: VARIABLE GET FAILED\nLINES:")
            print(out)
            print("Trying again...")
            out = self.exec(command)
            print("This time:")
            print(out)
            line = out[0].strip()
            value = line[line.rindex('=')+1:]
            # Throws variable error if it still fails
        if value.startswith("Symbol does not exist"):
            raise TestFailed("Symbol does not exist: " + str(name))
        elif value:
            return int(value)
        else:
            # Sometimes it prints to the newline
            return int(out[1].strip())

    def reset(self):
        """
        Halt the execution and reset the simulator (including hardware reset).
        Also clears breakpoints and the stimulus file.
        """
        return self.exec("halt\ndelete\nstim\nreset MCLR")

    def step(self):
        """
        Step program until it reaches a different source line.
        Only enters a function if there is line number information for the function.
        """
        return self.exec("step")

    def next(self):
        """
        Equivalent to the step command but treats subroutine calls as one instruction.
        """
        return self.exec("next")

    def stepi(self, n: int = 1):
        """
        Execute the given number of machine instructions.
        """
        return self.exec("stepi " + str(n))

    def stim(self, stimfile = None):
        """
        Set the stimulus .scl file or clear it if no path is given.
        """
        if stimfile:
            out = self.exec('stim "%s"' % stimfile.strip())
            for line in out:
                if line.startswith("Invalid stimulus file"):
                    raise MdbException("Invalid stimulus file: " + stimfile)
            return out
        else:
            return self.exec("stim")
