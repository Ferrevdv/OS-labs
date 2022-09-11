#!/usr/bin/env python3

import pathlib
import re
import random

import json
from os.path import exists

from xv6_test import Xv6Runner, TestContext, CaptureType


class TestTraps(TestContext):
    def run_tests(self):
        self._test_fs()

    def _test_fs(self):
        print("=== Testing file systems exercise ===")


        stdout = self.xv6_runner.run_xv6_exe(
            'testfs testread.txt testwrite.txt testexecute',
            extra_uprogs=['testfs'],
            capture_type=CaptureType.FULL,
            rebuild_fs_beginning=True,
            rebuild_fs_end=False
        )

        self._check_fs_output(stdout)
        if not self.failed:
            print("Initial tests suceeded. Let's see if the file modes persist over reboots...")
            stdout = self.xv6_runner.run_xv6_exe(
                'testfs testread.txt testwrite.txt testexecute',
                extra_uprogs=['testfs'],
                capture_type=CaptureType.FULL,
                rebuild_fs_beginning=False,
                compile_xv6=False,
                rebuild_fs_end=True
            )
            
            self._check_fs_output(stdout)


    def _check_fs_output(self, stdout):
        print(f'---stdout---\n{stdout}\n---end stdout---')


        # Test output of fs
        for line in stdout.splitlines():
            # Check if we even have output or just error
            if "Error:" in line:
                msg = "There seems to be an error with the output (at least one line contains 'Error')."
                
                self._test_failure(msg)
                return

            if "fail:" in line:
                msg = "At least one test failed. See above to find out what happened."
                
                self._test_failure(msg)
                return   
            
            if line.startswith(".."):
                msg = "There seems to be an error with the output (ls/directory output was printed). See above to understand why ls was printed."
                self._test_failure(msg)
                return
            

if __name__ == '__main__':
    runner = Xv6Runner(pathlib.Path(__file__).parent)
    test = TestTraps(runner)
    test.run()
