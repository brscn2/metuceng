from mdb import *
from grading import *

prelude =  """
device PIC18F8722
set warningmessagebreakoptions.W0223_ADC_UNDERFLOW ignore
set warningmessagebreakoptions.W0222_ADC_OVERFLOW ignore
set oscillator.frequency 1
set oscillator.frequencyunit Mega
hwtool sim
"""

breakpoints = []

STEP_PERIOD = 500e3
STEP_MARGIN = 50e3

rubric = [
    ["Init Period - PORTB", 2.0],
    ["Init Period - PORTC", 2.0],
    ["Init Period - PORTD", 2.0],
    ["No Input - PORTB", 2.0],
    ["No Input - PORTC", 2.0],
    ["No Input - PORTD", 2.0],

    ["RE1 Start - PORTB", 5.0],
    ["RE1 Start - PORTC", 1.0],
    ["RE1 Start - PORTD", 1.0],
    ["RE1 Stop - PORTB", 5.0],
    ["RE1 Stop - PORTC", 1.0],
    ["RE1 Stop - PORTD", 1.0],
    ["RE1 Restart - PORTB", 5.0],
    ["RE1 Restart - PORTC", 1.0],
    ["RE1 Restart - PORTD", 1.0],

    ["RE0 Start - PORTB", 1.0],
    ["RE0 Start - PORTC", 5.0],
    ["RE0 Start - PORTD", 1.0],
    ["RE0 Stop - PORTB", 1.0],
    ["RE0 Stop - PORTC", 5.0],
    ["RE0 Stop - PORTD", 1.0],
    ["RE0 Restart - PORTB", 1.0],
    ["RE0 Restart - PORTC", 5.0],
    ["RE0 Restart - PORTD", 1.0],

    ["Sync Start - PORTB", 5.0],
    ["Sync Start - PORTC", 5.0],
    ["Sync Start - PORTD", 1.0],
    ["Sync Stop - PORTB", 5.0],
    ["Sync Stop - PORTC", 5.0],
    ["Sync Stop - PORTD", 1.0],

    ["Async Start - PORTB", 5.0],
    ["Async Start - PORTC", 6.0],
    ["Async Start - PORTD", 1.0],
    ["Async Stop - PORTB", 6.0],
    ["Async Stop - PORTC", 5.0],
    ["Async Stop - PORTD", 1.0],
]

PROGRESS_BAR = [
    0b00000001,
    0b00000011,
    0b00000111,
    0b00001111,
    0b00011111,
    0b00111111,
    0b01111111,
    0b11111111,
    0b00000000,
]
REVERSE_PROGRESS_BAR = [
    0b10000000,
    0b11000000,
    0b11100000,
    0b11110000,
    0b11111000,
    0b11111100,
    0b11111110,
    0b11111111,
    0b00000000,
]

def progress_bar(reverse=False):
    if reverse:
        return 
    return

def initialization_check(history):
    print("INITIALIZATION CHECK")
    initial = {k: v[0] for k, v in history.items()}
    for k, v in initial.items():
        incorrect = bin(v.data ^ 255).count('1')
        led_correct = 8 - incorrect
        led_grade = led_correct / 8.0
        print(f"  PORT{k[-1]}")
        print(f"    Initialization bits: " + bin8(v.data), f"({led_correct}/8 bits correct)")
        timing_grade = v.timing_grade(1e6, STEP_MARGIN)
        print("    Initialization took " + str(v.time / 1e6) + "s", "(within" if timing_grade > 0 else "(beyond", "margin)")
        print(end="    ")
        report.set_grade(f"Init Period - PORT{k[-1]}", (led_grade + timing_grade) * 0.5)
    print()

def report_grade(name: str, correct, max_correct):
    global report
    print("Correct/Max:", f"{correct} / {max_correct}")
    report.set_grade(name, correct / max_correct)

def pattern_check(name: str, history, expected, target, margin):
    print(f'CHECKING "{name}"')
    print("(Half point for every LED match, full point for every LED match with correct duration.)")
    prefix_index, longest_sequence = get_longest_match(history, expected)
    if prefix_index == -1:
        print("Pattern check failed: No recorded changes in port.")
        report_grade(name, 0.0, len(expected))
        return
    # 0.5  for each correct LED pattern with incorrect timing
    # 0.75 if timing error is within 2*margin
    # 1.0  if timing error is within margin
    grades = [a.timing_grade(target, margin) * 0.5 + 0.5 for a in longest_sequence]
    expected_padded = [""] * prefix_index + [hex(e) for e in expected]
    print(" Expected:", " ".join(["{:>4}".format(e) for e in expected_padded]))
    print("   Output:", " ".join(["{:>4}".format(hex(e.data)) for e in history]))
    print("Time (ms):", " ".join(["{:>4}".format(str(e.time // 1000)) for e in history]))
    print("   Points:", " ".join(["{:>4}".format(grade) for grade in [""] * prefix_index + grades]))
    report_grade(name, sum(grades), len(expected))
    print()

def advanced_check(name: str, history, expected):
    print(f'CHECKING "{name}"')
    print("(Half point for every LED match, full point for every LED match with correct duration.)")
    def check_one(a, e):
        if a.data == e[0]:
            if e[1] is None:
                return 1.0
            return 0.5 + 0.5 * a.timing_grade(e[1], e[2])
        return 0.0
    print("Time (ms):", " ".join(["{:>4}".format("-" if e[1] is None else str(int(e[1] / 1000))) for e in expected]))
    print(" Expected:", " ".join(["{:>4}".format(hex(e[0])) for e in expected]))
    print("   Output:", " ".join(["{:>4}".format(hex(e.data)) for e in history]))
    print("Time (ms):", " ".join(["{:>4}".format(str(e.time // 1000)) for e in history]))
    if len(history) > len(expected):
        print("Expected length and recorded length don't match!")
        report_grade(name, 0.0, len(expected))
    else:
        grades = [check_one(a, e) for a, e in zip(history, expected)]
        print("   Points:", " ".join(["{:>4}".format(grade) for grade in grades]))
        report_grade(name, sum(grades), len(expected))
    print()

def blinking_check(name: str, history, target, margin, start=None, offset=1):
    print(f'CHECKING "{name}" (Blinking)')
    if len(history) < 1 + offset:
        print("Blinking check failed: Recorded pattern is not long enough.")
        report_grade(name, 0.0, 1)
        return
    if start is None:
        start = 0 if history[offset].data == 0 else 1
    l = len(history) - offset
    expected = ([start, start^1] * l)[:l]
    last = None
    grades = []
    for a, e in zip(history[offset:], expected):
        timing_grade = a.timing_grade(target, margin) * 0.5
        if e == a.data:
            led_grade = 0.5
        elif last is not None and a.data == last ^ 1:
            # LED is incorrect, but the first bit is flipped
            led_grade = 0.25
        else:
            led_grade = 0.0
        grades.append(timing_grade + led_grade)
        last = a.data
    expected_padded = [""] * offset + [hex(e) for e in expected]
    print(" Expected:", " ".join(["{:>4}".format(e) for e in expected_padded]))
    print("   Output:", " ".join(["{:>4}".format(hex(e.data)) for e in history]))
    print("Time (ms):", " ".join(["{:>4}".format(str(e.time // 1000)) for e in history]))
    print("   Points:", " ".join(["{:>4}".format(grade) for grade in [""] * offset + grades]))
    report_grade(name, sum(grades), len(expected))
    print()

def is_off_after_init(name: str, history):
    print(f'CHECKING "{name}" (Must be turned off after initialization)')
    expected_padded = ["", "0x0"]
    print(" Expected:", " ".join(["{:>4}".format(e) for e in expected_padded]))
    print("   Output:", " ".join(["{:>4}".format(hex(e.data)) for e in history]))
    print("Time (ms):", " ".join(["{:>4}".format(str(e.time // 1000)) for e in history]))
    criteria = [
        len(history) >= 2 and history[1].data == 0,
        len(history) == 2,  # Make sure that it doesn't turn on
    ]
    report_grade(name, sum(criteria), len(criteria))
    print()

def is_turned_off(name: str, history, lenient=True):
    print(f'CHECKING "{name}" (Must be turned off)')
    print("   Output:", " ".join(["{:>4}".format(hex(e.data)) for e in history]))
    print("Time (ms):", " ".join(["{:>4}".format(str(e.time // 1000)) for e in history]))
    if len(history) == 1 and history[0].data == 0:
        report.set_grade(name, 1.0)
    elif lenient:
        if len(history) == 2 and history[1].data == 0:
            report.set_grade(name, 1.0)
        else:
            print("Should have been turned off at the start")
            report.set_grade(name, 0.0)
    else:
        report.set_grade(name, 0.0)
    print()

#
# TESTS
#

def no_input_test(m, bp2addr, addr2bp):
    """
    Tests whether the device works as intended when no input is given.
    """
    global report
    history = record_output(m, 9, 6 * 1000 * 1000)
    print()
    initialization_check(history)
    is_off_after_init("No Input - PORTB", history["LATB"])
    is_off_after_init("No Input - PORTC", history["LATC"])
    blinking_check("No Input - PORTD", history["LATD"], STEP_PERIOD, STEP_MARGIN, start=0)


def portb_test(m, bp2addr, addr2bp):
    """
    Tests the PORTB progress bar (RE1) in isolation.
    """
    global report

    # Skip the initialization period.
    history = record_output(m, 1, 1200 * 1000)
    print("Initialization ended")

    m.stim("re1.scl")
    history = record_output(m, 12, 560*12 * 1000)
    expected_bar = [0] + (PROGRESS_BAR * 2)[:11]
    print()
    pattern_check("RE1 Start - PORTB", history["LATB"], expected_bar, STEP_PERIOD, STEP_MARGIN)
    is_turned_off("RE1 Start - PORTC", history["LATC"], lenient=False)
    blinking_check("RE1 Start - PORTD", history["LATD"], STEP_PERIOD, STEP_MARGIN, 0, 0)
    print()

    m.stim("re1.scl")
    history = record_output(m, 5, 560*5 * 1000)
    print()
    is_turned_off("RE1 Stop - PORTB", history["LATB"])
    is_turned_off("RE1 Stop - PORTC", history["LATC"], lenient=False)
    blinking_check("RE1 Stop - PORTD", history["LATD"], STEP_PERIOD, STEP_MARGIN, 0, 0)
    print()

    m.stim("re1.scl")
    history = record_output(m, 12, 560*12 * 1000)
    expected_bar = [0] + (PROGRESS_BAR * 2)[:11]
    print()
    pattern_check("RE1 Restart - PORTB", history["LATB"], expected_bar, STEP_PERIOD, STEP_MARGIN)
    is_turned_off("RE1 Restart - PORTC", history["LATC"], lenient=False)
    blinking_check("RE1 Restart - PORTD", history["LATD"], STEP_PERIOD, STEP_MARGIN, 1, 0)
    print()


def portc_test(m, bp2addr, addr2bp):
    """
    Tests the PORTC progress bar (RE0) in isolation.
    """
    global report

    # Skip the initialization period.
    history = record_output(m, 1, 1200 * 1000)
    print("Initialization ended")

    m.stim("re0.scl")
    history = record_output(m, 12, 560*12 * 1000)
    expected_bar = [0] + (REVERSE_PROGRESS_BAR * 2)[:11]
    print()
    is_turned_off("RE0 Start - PORTB", history["LATB"], lenient=False)
    pattern_check("RE0 Start - PORTC", history["LATC"], expected_bar, STEP_PERIOD, STEP_MARGIN)
    blinking_check("RE0 Start - PORTD", history["LATD"], STEP_PERIOD, STEP_MARGIN, 0, 0)
    print()

    m.stim("re0.scl")
    history = record_output(m, 5, 560*5 * 1000)
    print()
    is_turned_off("RE0 Stop - PORTB", history["LATB"], lenient=False)
    is_turned_off("RE0 Stop - PORTC", history["LATC"])
    blinking_check("RE0 Stop - PORTD", history["LATD"], STEP_PERIOD, STEP_MARGIN, 0, 0)
    print()

    m.stim("re0.scl")
    history = record_output(m, 12, 560*12 * 1000)
    expected_bar = [0] + (REVERSE_PROGRESS_BAR * 2)[:11]
    print()
    is_turned_off("RE0 Restart - PORTB", history["LATB"], lenient=False)
    pattern_check("RE0 Restart - PORTC", history["LATC"], expected_bar, STEP_PERIOD, STEP_MARGIN)
    blinking_check("RE0 Restart - PORTD", history["LATD"], STEP_PERIOD, STEP_MARGIN, 1, 0)
    print()


def sync_test(m, bp2addr, addr2bp):
    """
    Starts and stops both progress bars at the same time.
    """
    global report

    # Skip the initialization period.
    history = record_output(m, 1, 1200 * 1000)
    print("Initialization ended")

    m.stim("sync.scl")
    history = record_output(m, 12, 560*12 * 1000)
    print()
    pattern_check("Sync Start - PORTB", history["LATB"], [0] + (PROGRESS_BAR * 2)[:11], STEP_PERIOD, STEP_MARGIN)
    pattern_check("Sync Start - PORTC", history["LATC"], [0] + (REVERSE_PROGRESS_BAR * 2)[:11], STEP_PERIOD, STEP_MARGIN)
    blinking_check("Sync Start - PORTD", history["LATD"], STEP_PERIOD, STEP_MARGIN, 0, 0)
    print()

    m.stim("sync.scl")
    history = record_output(m, 5, 560*5 * 1000)
    print()
    is_turned_off("Sync Stop - PORTC", history["LATC"])
    is_turned_off("Sync Stop - PORTB", history["LATB"])
    blinking_check("Sync Stop - PORTD", history["LATD"], STEP_PERIOD, STEP_MARGIN, 0, 0)
    print()


def async_test(m, bp2addr, addr2bp):
    """
    Starts and stops progress bars asynchronously.
    """
    global report

    # Skip the initialization period.
    history = record_output(m, 1, 1200 * 1000)

    print("Loading async0.scl stimulus file")
    m.stim("async0.scl")

    history = record_output(m, 6, 560*6 * 1000)
    print()
    pattern_check("Async Start - PORTB", history["LATB"], [0] + PROGRESS_BAR[:5], STEP_PERIOD, STEP_MARGIN)
    advanced_check("Async Start - PORTC", history["LATC"], [
        (0, STEP_PERIOD*2, STEP_MARGIN*2),
        (REVERSE_PROGRESS_BAR[0], STEP_PERIOD, STEP_MARGIN),
        (REVERSE_PROGRESS_BAR[1], STEP_PERIOD, STEP_MARGIN),
        (REVERSE_PROGRESS_BAR[2], STEP_PERIOD, STEP_MARGIN),
        (REVERSE_PROGRESS_BAR[3], STEP_PERIOD, STEP_MARGIN),
    ])
    blinking_check("Async Start - PORTD", history["LATD"], STEP_PERIOD, STEP_MARGIN, 0, 0)
    print()

    print("Loading async1.scl stimulus file")
    m.stim("async1.scl")

    history = record_output(m, 4, 560*5 * 1000)
    print()
    advanced_check("Async Stop - PORTB", history["LATB"], [
        (PROGRESS_BAR[5], STEP_PERIOD, STEP_MARGIN),
        (PROGRESS_BAR[6], STEP_PERIOD, STEP_MARGIN),
        (0, None, None),
    ])
    advanced_check("Async Stop - PORTC", history["LATC"], [
        (REVERSE_PROGRESS_BAR[4], STEP_PERIOD, STEP_MARGIN),
        (0, None, None),
    ])
    blinking_check("Async Stop - PORTD", history["LATD"], STEP_PERIOD, STEP_MARGIN, 0, 0)
    print()


if __name__ == "__main__":
    report = Report(rubric)

    tester = MdbTester(prelude, breakpoints)
    tester.run([
        no_input_test,
        portb_test,
        portc_test,
        sync_test,
        async_test,
    ])

    print()
    print(report.report_str())
    report.save_grades("grades.pkl")
