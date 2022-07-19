import os

# Build statically - may throw a warning
when defined(Windows) or defined(Linux):
    switch("passL", "-static")

# Strip symbols
switch("passL", "-s")
# Change to release mode
switch("d", "release")
