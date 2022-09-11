#!/usr/bin/env python3

import pathlib
import re

from xv6_test import Xv6Runner, TestContext, CaptureType


class TestSystemCalls(TestContext):
    def run_tests(self):
        self._test_traceme()
        self._test_trace()

    def _test_traceme(self):
        print("=== Testing traceme syscall ===")

        stdout = self.xv6_runner.run_xv6_exe(
            'testtraceme',
            extra_uprogs=['testtraceme'],
            capture_type=CaptureType.FULL
        )

        self._check_output(stdout, expected_syscalls=[12, 11, 25])

    def _test_trace(self):
        print("=== Testing trace executable ===")

        stdout = self.xv6_runner.run_xv6_exe(
            'trace testtrace',
            extra_uprogs=['testtrace'],
            capture_type=CaptureType.FULL
        )

        self._check_output(stdout, expected_syscalls=[7, 12, 11, 25])

    def _check_output(self, stdout, expected_syscalls):
        print(f'---stdout---\n{stdout}\n---end stdout---')

        actual_syscalls = []
        expected_pid = None

        for line in stdout.splitlines():
            match = re.match(r'\[(\d+)] syscall (\d+)', line)

            if match:
                expected_pid = int(match.group(1))
                syscall = int(match.group(2))
                actual_syscalls.append((expected_pid, syscall))
                continue

            match = re.match(r'pid=(\d+)', line)

            if match:
                expected_pid = int(match.group(1))
                continue

        if len(actual_syscalls) == 0:
            msg = 'No syscalls found, are you using the correct output format?'
            self._test_failure(msg)
            return

        if expected_pid is None:
            msg = 'No pid found in output, please don\'t change test files'
            self._test_failure(msg)
            return

        for pid, syscall in actual_syscalls:
            if pid != expected_pid:
                msg = f'Found syscall {syscall} for pid {pid} ' \
                      f'while expecting pid {expected_pid}'
                self._test_failure(msg)
                return

        actual_syscalls = [s[1] for s in actual_syscalls]

        if actual_syscalls != expected_syscalls:
            msg = f'Expected syscalls {expected_syscalls}, ' \
                  f'found {actual_syscalls}'
            self._test_failure(msg)
            return


if __name__ == '__main__':
    runner = Xv6Runner(pathlib.Path(__file__).parent)
    test = TestSystemCalls(runner)
    test.run()
