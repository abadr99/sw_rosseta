import os
import lit.formats

# Basic configuration
config.name = 'sw_rosetta_lit'
config.test_format = lit.formats.ShTest(execute_external=True)

# lit will only look for test files that end in .txt
config.suffixes = ['.txt']

# Create the '%rosetta' alias to point to your compiled binary
# We check for Windows (.exe) vs Linux to ensure cross-platform compatibility
rosetta_bin = os.path.join(os.getcwd(), 'dev', 'rosetta')
if os.name == 'nt':
    rosetta_bin += '.exe'

config.substitutions.append(('%rosetta', rosetta_bin))