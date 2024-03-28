import pickle
from dataclasses import dataclass

def bin8(value: int):
    """
    Convert 8-bit register value to binary representation.
    """
    return ("0"*8 + bin(value)[2:])[-8:]

def metronome_pattern(bar_length):
    return [3, 0] + [1, 0] * (bar_length - 1)

def timing_grade(time, target, margin):
    error = abs(target-time)
    if error <= margin:
        return 1.0
    else:
        return 0

@dataclass
class RegRecording:
    data: int
    time: int
    tainted: bool = False

    def timing_grade(self, target, margin):
        return timing_grade(self.time, target, margin)

def bitmask_history(history, bitmask: int):
    first = history[0]
    newhist = [RegRecording(first.data & bitmask, first.time, first.data & bitmask != first.data)]
    for n in history[1:]:
        if n.data & bitmask == newhist[-1].data:
            newhist[-1].time += n.time
            if n.data & bitmask != n.data:
                newhist[-1].tainted = True
        else:
            newhist.append(RegRecording(n.data & bitmask, n.time, n.data & bitmask != n.data))
    return newhist

def prefix_match(sequence, target):
    out = []
    for i, expected in enumerate(target):
        if i >= len(sequence):
            break
        if sequence[i].data != expected:
            break
        out.append(sequence[i])
    return out

def get_longest_match(sequence, target):
    prefixes = [prefix_match(sequence[i:], target) for i in range(len(sequence))]
    if not prefixes:
        return -1, []
    # Reversed because we prefer the latest longest match.
    maxi = max(reversed(range(len(prefixes))), key=lambda i: len(prefixes[i]))
    return maxi, prefixes[maxi]

class Report:
    def __init__(self, rubric: list):
        self.grades = {}
        self.rubric = rubric

    def set_grade(self, name: str, grade: float):
        self.grades[name] = grade
        matches = [r for r in self.rubric if r[0] == name]
        if len(matches) != 1:
            raise ValueError("Rubric error: " + str(matches))
        r = matches[0]
        scaled = round(grade * r[1], 2)
        print(f'Grade for "{r[0]}": {scaled:.2f} / {r[1]:.1f}')

    def save_grades(self, filename):
        with open(filename, 'wb') as f:
            pickle.dump(self.grades, f)

    def load_grades(self, filename):
        with open(filename, 'rb') as f:
            self.grades = pickle.load(f)

    def report_items(self):
        def process_rubric_item(r):
            grade = self.grades[r[0]] if r[0] in self.grades else 0.0
            grade = round(grade * r[1], 2)
            return [r[0], grade, r[1]]
        grade_items = [process_rubric_item(g) for g in self.rubric]
        f = "{0:>5} / {1:>4}  {2}"
        total_grade = round(sum([g[1] for g in grade_items]), 2)
        max_grade = round(sum([g[1] for g in self.rubric]), 2)
        return [g + [f.format(g[1], g[2], g[0])] for g in grade_items], total_grade, max_grade

    def report_str(self):
        grade_items, total_grade, max_grade = self.report_items()
        grade_report = "\n".join([g[3] for g in grade_items])
        return f"GRADE\n{grade_report}\nTOTAL GRADE: {total_grade:.2f}/{max_grade:.2f}"

def record_output(m, max_history: int, max_cycles: int, max_changes: int = 500, flicker_duration: int = 1000):
    # Watch writes to output registers
    # Note that we need to read the output ports from LATCH.
    # But both PORT and LAT registers must be watched.
    ports = ["B", "C", "D"]
    for port in ports:
        m.watch(f"PORT{port} W")
        m.watch(f"LAT{port} W")
    registers = [f"LAT{port}" for port in ports]
    history = {reg: [] for reg in registers}
    current = {reg: RegRecording(m.get(reg), 0) for reg in registers}
    cycles = 0
    changes = 0
    while max([len(v) for v in history.values()]) < max_history:
        if not m.run_timeout():
            print("Timed out while waiting for a change in the output ports!")
            m.clear_breakpoints()
            for k, v in current.items():
                v.time += m.stopwatch()
                history[k].append(v)
            return history
        time = m.stopwatch()
        for k, v in current.items():
            v.time += time
            value = m.get(k)
            if value != v.data:
                print(k, "changed from", str(v.data), "to", str(value) + " (" + bin8(value) + ") after", str(v.time/1e6) + "s", end = "")
                if v.time > flicker_duration:  # Don't record if too short
                    history[k].append(v)
                    print()
                else:
                    print(" (FLICKER, ignored)")
                current[k] = RegRecording(value, 0)
        cycles += time
        if cycles > max_cycles:
            print(f"Exceeded the maximum duration of {max_cycles} cycles")
            break
        changes += 1
        if changes > max_changes:
            print(f"Exceeded the maximum number of changes ({max_changes})")
            break
    for k, v in current.items():
        if v.time > flicker_duration or not history[k]:
            history[k].append(v)
    m.clear_breakpoints()
    return history
