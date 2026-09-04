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

config.test_source_root = os.path.join(
    project_root, 'tests', 'Regression_Tests'
)

regression_driver = os.path.join(
    project_root, 'build', 'tests', 'regression_driver'
)

if os.name == 'nt':
    regression_driver += '.exe'

config.substitutions.append(
    ('%regression_driver', regression_driver)
)