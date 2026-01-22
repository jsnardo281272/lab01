#!/usr/bin/env python3
import subprocess
import argparse
import re
from time import sleep
from math import ceil
import sys

def parse_log(log: str):
    test_results = re.findall(r"^ktest: \[(.*):(.*)\] test result: (SUCCESS|FAILURE)", log, re.M)
    results = {}
    for suite, test, result in test_results:
        if suite not in results:
            results[suite] = []

        results[suite].append((test, result))
    return results

def get_vm_log(timeout: int = 10, quiet: bool = False):
    subprocess.run(["meson", "setup", "--cross-file=meson-llvm-riscv.ini", "build"], check=True)
    subprocess.run(["meson", "compile", "-C", "build"], check=True)

    qemu_cmd = ["qemu-system-riscv64",
                "-nographic",
                "-m", "1G",
                "-machine", "virt", "-bios", "none",
                "-serial", "mon:stdio",
                "-kernel", "./build/src/ktest.elf"]
    if not quiet:
        print(f"running: {" ".join(qemu_cmd)}")

    p = subprocess.Popen(qemu_cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    sleep(timeout)
    p.kill()
    p.wait()
    serial_output = p.stdout.read().decode("utf-8")
    p.communicate()

    return serial_output

def grade_assignment(timeout: int = 10, quiet: bool = False) -> bool:
    log = get_vm_log(timeout, quiet)
    print(log)
    results = parse_log(log)
    weights = {
            "page_unit_tests": 5,
            "virt_addr_unit_tests": 5,
            "ppn_unit_tests": 5,
            "pte_unit_tests": 5,
            "alloc_tests": 30,
            "vm_map_tests": 50,
    }

    print("----------[ test results ]----------")
    grade = 0
    for suite, test_results in results.items():
        print(f"{suite}:")

        points = 0
        max_points = len(test_results)
        for test, res in test_results:
            print(f"\t{test}: {res}")
            if res == "SUCCESS":
                points += 1

        suite_grade = weights[suite] * points / max_points
        print(f"\tTOTAL: {suite_grade:.2f}/{weights[suite]} points")
        grade += suite_grade
        print("------------------------------------")
    print(f"FINAL GRADE: {ceil(grade)}/100 points")
    print("------------------------------------")

    return ceil(grade) == 100

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--timeout", help="timeout for the tests in seconds", default=10)
    parser.add_argument("--quiet", help="output only the final score", default=False)
    args = parser.parse_args()

    is_full_mark = grade_assignment(int(args.timeout), args.quiet)

    ret = 0 if is_full_mark else 1
    sys.exit(ret)

