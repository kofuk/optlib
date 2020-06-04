# optlib

Cross-platform command-line option parser.

## Platform

optlib supports natural command-line argument style for each platform.

Platform  | Long option       | Short option | Back-end
----------|-------------------|--------------|--------------------
GNU/Linux | --long-option arg | -s arg       | glibc `getopt_long`
POSIX (1) | none              | -s arg       | libc `getopt`
Windows   | -LongOption arg   | none         | none

(1): Except for GNU-based POSIX environment.

## License

optlib is Free Software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
