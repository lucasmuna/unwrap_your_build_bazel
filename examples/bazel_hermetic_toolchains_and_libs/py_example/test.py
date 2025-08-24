import sys

import pytest

from py_example import example


@pytest.mark.parametrize("expected", [None])
def test(expected):
    assert example.main() == expected


if __name__ == "__main__":
    sys.exit(pytest.main([__file__]))
