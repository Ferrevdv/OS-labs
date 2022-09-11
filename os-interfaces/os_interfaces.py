#!/usr/bin/env python3

import subprocess
import pathlib
import collections

from xv6_test import Xv6Runner, TestContext


class TestOsInterfaces(TestContext):
    def run_tests(self):
        try:
            exec_log = self.xv6_runner.run_xv6_exe('evaluation')
        except subprocess.CalledProcessError:
            self._test_failure('could not build/run xv6 executable')
            return

        if exec_log == '':
            msg = 'No output from test. Did you create the evaluation ' \
                  'executable? See the terminal output above for more info.'
            self._test_failure(msg)
            return

        print(f'===stdout===\n{exec_log}\n===end stdout===')

        mem = collections.defaultdict(dict)

        def format_error():
            self._test_failure('wrong output format\n')

        try:
            for line in exec_log.splitlines():
                parts = line.split(':')

                if len(parts) == 3:
                    who, which, address = parts
                    value = None
                elif len(parts) == 4:
                    who, which, address, value = parts
                else:
                    continue

                address = int(address, 16)

                if value is not None:
                    value = int(value)

                mem[who.lower()][which] = (address, value)
        except ValueError:
            self._test_failure('wrong output format\n')
            return

        mem_items = {'stack', 'heap', 'data', 'text'}

        if mem.keys() != {'parent', 'child'} or \
                mem['parent'].keys() != mem_items or \
                mem['child'].keys() != mem_items:
            self._test_failure('wrong output format\n')
            return

        if mem['parent']['heap'][0] == mem['child']['heap'][0]:
            self._test_failure('heap addresses are the same')

        def value_differs(item):
            parent_value = mem['parent'][item][1]
            child_value = mem['child'][item][1]

            if parent_value is None:
                return child_value is None
            else:
                return parent_value != child_value

        if not all(value_differs(item) for item in mem_items):
            self._test_failure('(some) values are the same')


if __name__ == '__main__':
    runner = Xv6Runner(pathlib.Path(__file__).parent)
    test = TestOsInterfaces(runner)
    test.run()
