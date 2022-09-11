#!/usr/bin/env python3

import pathlib
import re
import random

import json
from os.path import exists

from xv6_test import Xv6Runner, TestContext, CaptureType


class TestTraps(TestContext):
    def run_tests(self):
        self._test_traps()

        # Also test bonus exercise if defined in settings
        settings_file = "exercises.json"
        if exists(settings_file):
            with open(settings_file, 'r') as f:
                config = json.load(f)
                if "bonus_exercise" in config and config["bonus_exercise"]:
                    self._test_bonus()

    def _test_traps(self):
        rand_time = random.randint(0,40)
        print("=== Testing traps exercise ===")


        stdout = self.xv6_runner.run_xv6_exe(
            'testtraps ' + str(rand_time),
            extra_uprogs=['testtraps'],
            capture_type=CaptureType.FULL
        )

        self._check_traps_output(stdout, expected_time=rand_time+2)

    def _test_bonus(self):
        print("=== Testing bonus ===")

        stdout = self.xv6_runner.run_xv6_exe(
            'testtrapsbonus',
            extra_uprogs=['testtrapsbonus'],
            capture_type=CaptureType.FULL
        )

        print(f'---stdout---\n{stdout}\n---end stdout---')
        # First line of the readme we expect to read
        expected_readme = "xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix" 
        test_success = False
        for line in stdout.splitlines():
            if "test-fail" in line:
                msg = "Test failed (see above)."
                self._test_failure(msg)
                return
            if expected_readme in line:
                test_success = True

        if not test_success:
            msg = "Test failed. Expected Readme output not read."
            self._test_failure(msg)
            return
        

    def _check_traps_output(self, stdout, expected_time):
        print(f'---stdout---\n{stdout}\n---end stdout---')


        # Test output of ticks
        stage = 0
        pages = [[], []]
        array_pts = [] # Stores first, mid, and last pt in the end
        for line in stdout.splitlines():
            # Check if we even have output or just error
            if "Error:" in line:
                msg = "There seems to be an error with the output (at least one line contains 'Error')."
                
                self._test_failure(msg)
                return
            
            # Remember array addresses
            match = re.match(r'--- Accessing array\[\d+\] @ 0x(.{16})... ---', line)
            if match:
                addr = match.group(1).lower()
                addr = addr[:-3] + '000' # Get page table of address
                array_pts.append(addr)

            # Check whether we are in the final page block (otherwise it's the first)
            if "Final page tables:" in line:
                stage = 1
                continue
            # Then see if there is a properly mapped page
            match = re.match(r'0x(.{16}) -> 0x(.{16}), mode=(.), perms=(.{3})', line)
            if match:
                va = int(match.group(1), 16)
                if va < 0x80000000 and va > 0x4000:
                    pages[stage].append(match.group(1))

        print(f'---relevant initial pages--- {str(pages[0])} ---end initial pages---')
        print(f'---relevant final pages  --- {str(pages[1])} ---end final pages---')
        print(f'---accessed array pages  --- {str(array_pts)} ---end array pages---')


        # First check whether the initial state mapped any useless pages
        if len([pt for pt in pages[0] if pt not in [array_pts[0]]] ) > 1:
            msg = "Initial mapping after malloc contains more pages than necessary. "\
                  "(The start address of the array may be accessed by the OS but all other pages should not be allocated yet)."
            self._test_failure(msg)
            return
        else:
            print("Good, initial mapping after malloc does not contain useless pages.")

        # Then check the added pages
        added_pages = list(set(pages[1]) - set(pages[0]))
        if len(added_pages) == 0:
            msg = "No additional pages were mapped after accessing the array!\n"
            self._test_failure(msg)
            return
        else:
            print("Good, accessed pages are properly mapped.")
            
        added_pages = list(set(added_pages) - set(array_pts))
        if len(added_pages) != 0:
            msg = 'Some useless pages that had nothing to do with the array were mapped: ' + str(added_pages)
            self._test_failure(msg)
            return

        # if time is None:
        #     msg = 'No timing found in output, please don\'t change test files'
        #     self._test_failure(msg)
        #     return

        # acceptable_range = range(expected_time-2, expected_time+3)
        # if "uptime" not in timings or timings["uptime"] not in acceptable_range: # Give some leeway for slight variations in implementations 
        #     msg = "System call uptime is incorrect. You either broke something in the system call or changed something you shouldn't have."
        #     self._test_failure(msg)
        #     return

        # if "fastuptime" not in timings or timings["fastuptime"] not in acceptable_range:
        #     msg = f'Fastuptime is not in the expected range! Got {timings["fastuptime"]} but expected {expected_time}'
        #     self._test_failure(msg)
        #     return

        # if timings["fastuptime"] != timings["uptime"]:
        #     msg = f'Fastuptime is not equal to system uptime! Got {timings["fastuptime"]} but expected {timings["uptime"]}'
        #     self._test_failure(msg)
        #     return

if __name__ == '__main__':
    runner = Xv6Runner(pathlib.Path(__file__).parent)
    test = TestTraps(runner)
    test.run()
