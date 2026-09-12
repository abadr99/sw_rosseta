#! /usr/bin/env python3

from pathlib import Path
import sys

error_count = 0

# Function to report the error
def report_error(file_path, error_message, error_type):
    global error_count
    error_count += 1
    # Removed undefined 'line_number' variable from the print statement
    print(f"{file_path}: {error_message}  [{error_type}]")

"""
Function to check if all source and header files are following the correct naming convention
Files Naming Convention:
    1) Start with uppercase letter
    2) Use camel case for the file name
"""
def check_file_naming_convention(file_posix):
    file_name = file_posix.name
    file_path = str(file_posix)
    excludes = ['main.cpp']
    if file_name in excludes:
        return
    # Check if the file name starts with an uppercase letter
    if not file_name[0].isupper():
        report_error(file_path, "File name must start with an uppercase letter", "file_naming_convention")
    # Check if the file contains underscores or dashes
    if '_' in file_name or '-' in file_name:
        report_error(file_path, "File name must not contain underscores or dashes", "file_naming_convention")
    # Check if the file name is too long
    if len(file_name) > 100:
        report_error(file_path, "File name must not be too long", "file_naming_convention")

"""
Directory Naming Convention:
    1) all letters are lowercase
    2) use underscore to separate words
"""
def check_dir_naming_convention(dir_posix):
    dir_name = dir_posix.name
    dir_path = str(dir_posix)
    excludes = ['Output']
    if dir_name in excludes:
        return
    # Check if all letters are lowercase
    if not dir_name.islower():
        report_error(dir_path, "Directory name must be all lowercase", "dir_naming_convention")

"""
Clean after build in tests 
    - If your regression test is generating a files (even elf files) then you should clean them
"""
def check_clean_after_build(file_posix):
    file_path = str(file_posix)
    has_build = False
    has_clean = False
    with open(file_path, 'r') as file:
        for line in file:
            text = line.strip()
            if "make" in text and "make clean" not in text:
                has_build = True
                has_clean = False
            if "make clean" in text and has_build:
                has_clean = True
    if has_build and not has_clean:
        report_error(file_path, "Make clean is not called after build", "clean_after_build")


def main():
    global error_count # Added global keyword to prevent shadowing
    
    file_list = []  # list of PosixPath objects
    dir_list = []  # list of PosixPath objects
    
    for i in range(1, len(sys.argv)):
        root_path = sys.argv[i]
        # Get all files
        file_list.extend([f for f in Path(root_path).rglob('*') if f.is_file()])
        # Get all directories
        dir_list.extend([d for d in Path(root_path).rglob('*') if d.is_dir()])
    
    # check if files are following the correct naming convention
    for posix_path in file_list:
        if posix_path.suffix not in ['.cpp', '.hpp', '.h', '.cc']:
            continue
        # Pass the Path object itself, not a string
        check_file_naming_convention(posix_path)
    
    # check if the directory follows the correct naming convention
    for posix_path in dir_list:
        # Pass the Path object itself, not a string
        check_dir_naming_convention(posix_path)
    
    # check if the clean after build is called
    for posix_path in file_list:
        if posix_path.suffix not in ['.ll']:
            continue
        # Pass the Path object itself, not a string
        check_clean_after_build(posix_path)
    return error_count

if __name__ == "__main__":
    # Wrap in sys.exit() to return the error count as the exit code (useful for CI/CD)
    sys.exit(main())