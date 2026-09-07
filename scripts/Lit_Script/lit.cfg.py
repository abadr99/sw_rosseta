import os
import lit.formats

config.name = 'sw_rosetta_lit'
config.test_format = lit.formats.ShTest(force_execute_external=True)
config.suffixes = ['.ll']

project_root = os.path.dirname(
    os.path.dirname(
        os.path.dirname(os.path.abspath(__file__))
    )
)

config.test_source_root = os.path.join(project_root, "tests", "Regression_Tests")
config.test_exec_root   = config.test_source_root

rosetta_bin = os.path.join(
    project_root, 'build', 'dev', 'rosetta'
)
if os.name == 'nt':
    rosetta_bin += '.exe'

config.substitutions.append(('%rosetta', rosetta_bin))