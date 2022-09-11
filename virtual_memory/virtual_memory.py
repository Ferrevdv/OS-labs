#!/usr/bin/env python3

import pathlib
import re
import random

import json
from os.path import exists

from xv6_test import Xv6Runner, TestContext, CaptureType


class TestVirtualMemory(TestContext):
    def run_tests(self):
        self._test_fastuptime()

        # Also test bonus exercise if defined in settings
        settings_file = "exercises.json"
        if exists(settings_file):
            with open(settings_file, 'r') as f:
                config = json.load(f)
                if "bonus_exercise" in config and config["bonus_exercise"]:
                    self._test_nullptr()

    def _test_fastuptime(self):
        rand_time = random.randint(0,40)
        print("=== Testing fastuptime with random offset of {0} ===".format(rand_time))


        stdout = self.xv6_runner.run_xv6_exe(
            'testfastuptime ' + str(rand_time),
            extra_uprogs=['testfastuptime'],
            capture_type=CaptureType.FULL
        )

        self._check_fastuptime_output(stdout, expected_time=rand_time+2)

    def _test_nullptr(self):
        print("=== Testing null pointer ===")

        stdout = self.xv6_runner.run_xv6_exe(
            'testnullptr',
            extra_uprogs=['testnullptr'],
            capture_type=CaptureType.FULL
        )

        print(f'---stdout---\n{stdout}\n---end stdout---')
        for line in stdout.splitlines():
            if "Test fail:" in line:
                msg = 'Null pointer could be accessed.'
                self._test_failure(msg)
                return

    def _check_fastuptime_output(self, stdout, expected_time):
        print(f'---stdout---\n{stdout}\n---end stdout---')


        # Test output of ticks
        has_page = False
        time = None
        timings = {}
        for line in stdout.splitlines():
            # Check if we even have output or just error
            if "Error:" in line:
                if "input" in line:
                    msg = "Do not modify the tests. Error: The test did not receive an input."
                if "fastuptime" in line:
                    msg = "Test can only be run once you implemented fastuptime."

                self._test_failure(msg)
                return
            # Then see if there is a properly mapped page
            match = re.match(r'0x(.{16}) -> 0x(.{16}), mode=(.), perms=(.{3})', line)
            if match:
                va = match.group(1)
                if int(va, 16) >= 0x88000000:
                    mode = match.group(3)
                    perms= match.group(4)
                    if mode == "U" and perms == "r--":
                        has_page = True

            # Then look at the lower part of the output
            match = re.match(r'Time \((uptime|fastuptime)\):(\d+)', line)

            if match:
                time_type = match.group(1)
                time = int(match.group(2))
                timings[time_type] = time
                continue

        if not has_page:
            msg = "There seems to be no suitable page for fastuptime with the right permissions!\n"\
                "Maybe reconsider where there is unused address space and what permissions the page needs."
            self._test_failure(msg)
            
        if len(timings) < 2:
            msg = 'Fastuptime did not work, did you implement it already?'
            self._test_failure(msg)
            return

        if time is None:
            msg = 'No timing found in output, please don\'t change test files'
            self._test_failure(msg)
            return

        acceptable_range = range(expected_time-2, expected_time+3)
        if "uptime" not in timings or timings["uptime"] not in acceptable_range: # Give some leeway for slight variations in implementations 
            msg = "System call uptime is incorrect. You either broke something in the system call or changed something you shouldn't have."
            self._test_failure(msg)
            return

        if "fastuptime" not in timings or timings["fastuptime"] not in acceptable_range:
            msg = f'Fastuptime is not in the expected range! Got {timings["fastuptime"]} but expected {expected_time}'
            self._test_failure(msg)
            return

        if timings["fastuptime"] != timings["uptime"]:
            msg = f'Fastuptime is not equal to system uptime! Got {timings["fastuptime"]} but expected {timings["uptime"]}'
            self._test_failure(msg)
            return

if __name__ == '__main__':
    runner = Xv6Runner(pathlib.Path(__file__).parent)
    test = TestVirtualMemory(runner)
    test.run()
