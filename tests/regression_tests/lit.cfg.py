import os
import shutil
import lit.formats

config.name = 'sw_rosetta_lit'
config.test_format = lit.formats.ShTest(force_execute_external=True)
config.suffixes = ['.ll']

project_root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

config.test_source_root = os.path.join(project_root, "tests", "regression_tests")
config.test_exec_root   = config.test_source_root
config.substitutions.append(('%build_dir', lit_config.params.get('build_dir', '')))
# Main Rosetta binary
rosetta_bin = os.path.join(project_root, 'build', 'dev', 'rosetta')
if os.name == 'nt':
    rosetta_bin += '.exe'

# FileCheck resolution: use pip/system executable on Windows, vendored ELF on Linux
if os.name == 'nt':
    filecheck_bin = shutil.which('filecheck') or shutil.which('FileCheck') or 'filecheck'
else:
    filecheck_bin = os.path.join(project_root, 'third_party', 'llvm', 'FileCheck')

config.substitutions.append(('%rosetta', rosetta_bin))
config.substitutions.append(('FileCheck', filecheck_bin))