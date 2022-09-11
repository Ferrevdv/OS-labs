#!/usr/bin/env python3

import pathlib
import re
import random

import json
from os.path import exists

from xv6_test import Xv6Runner, TestContext, CaptureType


class TestLocks(TestContext):
    def run_tests(self):
        print("=== Testing locks exercise ===")

        stdout = self.xv6_runner.run_xv6_exe(
            'print_contention stressmem',
            extra_uprogs=[],
            capture_type=CaptureType.FULL
        )

        self._check_traps_output(stdout)

    def _check_traps_output(self, stdout):
        print(f'---stdout---\n{stdout}\n---end stdout---')

        locks = []
        for line in stdout.splitlines():
            # Check if we even have output or just error
            if "Error:" in line:
                msg = "There seems to be an error with the output (at least one line contains 'Error')."

                self._test_failure(msg)
                return

            match = re.search('#acquires=(\d+), contention=(\d+)', line)

            if match:
                acquires = int(match.group(1))
                contention = int(match.group(2))
                locks.append((acquires, contention))


        if len(locks) == 0 :
                msg = "No spinlocks have been registered with perf_register_spinlock"
                self._test_failure(msg)
                return
        elif len(locks) == 1 :
                msg = "Expected multiple spinlocks in your solution. Only a single spinlock was registered."
                self._test_failure(msg)
                return
        else:
            (acquires_total, contention_total) = locks[-1]

            if acquires_total < 700000:
                 msg = "Not enough total lock acquires. Are you sure stressmem is executing correctly?"
                 self._test_failure(msg)
                 return

            if contention_total > 100000:
                msg = "Lock contention too high. Have you implemented a per-processor free list?"




if __name__ == '__main__':
    runner = Xv6Runner(pathlib.Path(__file__).parent)
    test = TestLocks(runner)
    test.run()
