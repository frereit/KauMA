#!/usr/bin/env python3
from subprocess import Popen, PIPE
from glob import glob
import json
import sys


def main():
    assert Popen(["make", "clean"]).wait() == 0, "Failed to clean project"
    assert Popen(["make", "release"]).wait() == 0, "Failed to build project"

    failures = []
    for input_filename in glob("examples/**/*.in.json"):
        expected_filename = input_filename.removesuffix(".in.json") + ".out.json"
        proc = Popen(["out/apps/kauma", input_filename], stdout=PIPE, stderr=PIPE)
        stdout, stderr = proc.communicate()
        if proc.returncode != 0:
            failures.append(
                (
                    input_filename,
                    (stdout, stderr),
                )
            )
            continue
        with open(expected_filename) as f:
            expected = json.load(f)
        actual = json.loads(stdout)
        if expected != actual:
            failures.append(
                (
                    input_filename,
                    (stdout, stderr),
                )
            )
            continue
    if len(failures) != 0:
        print("\033[91mNOT ALL TESTCASES PASSED!\033[0m")
        for failure in failures:
            print(failure[0])
            print("STDOUT:", failure[1][0], "-" * 80, sep="\n")
            print("STDERR:", failure[1][1], "-" * 80, sep="\n")
        sys.exit(1)
    else:
        print("\033[92mALL TESTCASES PASSED!\033[0m")
 
if __name__ == "__main__":
    main()
