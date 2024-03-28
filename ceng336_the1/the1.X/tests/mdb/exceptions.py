
class MdbException(Exception):
    """
    Denotes errors in the testing suite itself.
    """
    pass

class TestFailed(Exception):
    """
    Raised when a test fails.
    """
    pass

