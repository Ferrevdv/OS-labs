import pathlib
import subprocess
import abc
import sys
import enum


class CaptureType(enum.Enum):
    FULL = 1
    STDOUT = 2


class Xv6Runner:
    def __init__(self, xv6_dir):
        self.xv6_dir = pathlib.Path(xv6_dir)
        self.shinit_file = self.xv6_dir / '.shinit'
        self.fs_file = self.xv6_dir / 'fs.img'
        self.fscat_exe = self.xv6_dir / 'mkfs/fscat'

    def run_xv6_exe(self, name, capture_type=CaptureType.STDOUT,
                    extra_uprogs=[]):
        # Remove fs.img to force a rebuild that includes .shinit
        self.fs_file.unlink(missing_ok=True)

        if capture_type == CaptureType.STDOUT:
            self.shinit_file.write_text(f'{name} > stdout.txt\nhalt')
        else:
            self.shinit_file.write_text(f'{name}\nhalt')

        shinit_relative_path = self.shinit_file.relative_to(self.xv6_dir)
        extra_uprogs = ' '.join(f'user/_{name}' for name in extra_uprogs)

        try:
            self.make(
                target='all',
                EXTRA_FS_FILES=shinit_relative_path,
                EXTRA_UPROGS=extra_uprogs
            )

            proc = self.make(target='qemu', capture_output=True)

            if capture_type == CaptureType.STDOUT:
                stdout = self.fscat('stdout.txt')
            else:
                stdout = proc.stdout.decode()

            return stdout
        finally:
            # Remove fs files to force a rebuild the next time xv6 is booted
            self.fs_file.unlink(missing_ok=True)
            self.shinit_file.unlink(missing_ok=True)


    def fscat(self, file_name):
        self.make(self.fscat_exe.relative_to(self.xv6_dir))

        proc = self.run(
            [self.fscat_exe, self.fs_file, file_name],
            stdout=subprocess.PIPE,
            text=True
        )

        return proc.stdout

    def make(self, target=None, capture_output=False, **kwargs):
        cmd = ['make']
        cmd += [f'{k}={v}' for k, v in kwargs.items()]

        if target is not None:
            cmd.append(target)

        return self.run(cmd, capture_output=capture_output)

    def run(self, cmd, **kwargs):
        return subprocess.run(cmd, check=True, cwd=self.xv6_dir, **kwargs)


class TestContext(abc.ABC):
    def __init__(self, xv6_runner):
        self.xv6_runner = xv6_runner
        self.failed = False

    def _test_failure(self, msg):
        print(f'Failure: {msg}')
        self.failed = True

    @abc.abstractmethod
    def run_tests(self):
        pass

    def run(self):
        self.run_tests()

        if self.failed:
            print('Some tests failed')
            sys.exit(1)
        else:
            print('All tests passed')
