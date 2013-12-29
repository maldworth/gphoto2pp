Development and Testing
=======================

Table Of Contents
-----------------
* todo - Naming Scheme with libgphoto2
* todo - Running unit tests
  * todo - Explain each test and what it does
* todo - Building doxygen documentation and committing to gh-pages
* [Debugging gphoto2pp](#debugging-gphoto2pp)
* [Debugging libgphoto2](#debugging-libgphoto2)

Debugging gphoto2pp
-------------------
This will show the various debugging mechanisms in place in order to help triage any bugs that may be happening with the library (and gphoto2).

### Logging
The log mechanism we have implemented was taken from a dr.dobbs article which is completely contained in a header file. The default log level is Error and will log to stdout. You can optionally set it to log to a file instead.
Log Levels:
* logEMERGENCY
* logALERT
* logCRITICAL
* logERROR
* logWARN
* logWARN1
* logWARN2
* logWARN3
* logINFO
* logDEBUG

### Setting Log Level
To set the logging level you can input this line anywhere in your code ``gphoto2pp::FILELog::ReportingLevel() = gphoto2pp::logDEBUG`` and you may also need to include log.h ``#include <gphoto2pp/log.h>``.

### Setting Log Output To File
Put the following two lines in your program (typically the entry point).
```cpp
...
FILE* log_fd = fopen( "mygphpto2pplog.txt", "w" );
Output2FILE::Stream() = log_fd;
...
```
If you wish to stop logging, you can do nullptr??

### Using The Logger for your Application
Using this header only logger for your application is also quite acceptable, simply include the log file with ``#include <gphoto2pp/log/.h>`` and then use this line with the appropriate log level ``FILE_LOG(logDEBUG) << "Some debug message";``

Debugging libgphoto2
-----------------
Libgphoto2 also has included some callbacks for debugging. These are separate from gphoto2pp logging, but you can log them to the same output (as seen in Example 11).

If the camera is having unexpected behavior, it most likely lies in libgphoto2. Help for debugging these issues can be found on (gphoto nabble)[http://gphoto-software.10949.n7.nabble.com/].
What I recommend is:
* Try to reproduce the error with the command line tool gphoto2 and the --debug flag. This way you can post that log output right to nabble and get some assistance.
* If you don't want to use or don't have the command line tool, the output from that is identical to what we output in the debugging callback. You will just need to capture it to a log file and post it.
