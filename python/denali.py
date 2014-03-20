import numpy as np
import itertools
from StringIO import StringIO

def read_selection_file(fileob):
    """Read the selection information from a file-like object."""

    def _process_filename(data):
        return list(data)[0].strip()

    def _process_array(lines):
        data_string = "".join(lines)
        return np.loadtxt(StringIO(data_string))

    process_map = {
            "file": _process_filename,
            "component": _process_array,
            "members": _process_array,
            "subtree": _process_array
            }

    # group the lines of the selection file, breaking on #
    grouping = itertools.groupby(fileob, lambda x: x.startswith('#'))

    # zip together the keys with the data groups
    flags = (list(g)[0].lstrip('# ').rstrip('\n') for k,g in grouping if k)
    data = (g for k,g in grouping if not k)

    selection_information = {}

    for flag, data in itertools.izip(flags, data):
        if flag in process_map:
            selection_information[flag] = process_map[flag](data)

    return selection_information
