from .wrapper import Mdb
from .preprocess import *
from time import time

class MdbTester:
    def __init__(self, prelude: str, breakpoints: list):
        self.prelude = prelude.strip() + "\nprogram " + ELF_FILE
        self.breakpoints = breakpoints
        self.bp2addr, self.addr2bp = None, None
        self.m = None

    def run(self, tests: list):
        print("INITIALIZING...")
        start_time = time()
        try:
            check_files()
            self.bp2addr, self.addr2bp = load_breakpoints(ASM_FILE, CMF_FILE, self.breakpoints)
            self.m = Mdb()
            self.m.prelude(self.prelude)
        except TestFailed as e:
            print("INITIALIZATION FAILED:", e)
            return
        else:
            print("INITIALIZATION SUCCESSFUL")

        for test in tests:
            print("\n\nRUNNING TEST:", test.__name__)
            if test.__doc__ is not None:
                print("\t" + test.__doc__.strip())
            try:
                test(self.m, self.bp2addr, self.addr2bp)
            except TestFailed as e:
                print("TEST FAILED:", e)
            finally:
                self.m.reset()
        print("\nFinished executing tests in %.2f seconds." % (time() - start_time))
        self.m.quit()

