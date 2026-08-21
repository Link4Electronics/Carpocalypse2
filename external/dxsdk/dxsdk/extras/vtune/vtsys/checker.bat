@ECHO OFF
ECHO . Directions for using Checker.  An Installation Verifier.

ECHO . checker.* files must be in the same location as the setup.exe
ECHO . file.  If testing a CD installation, copy the CD contents
ECHO . to a hard drive, and then run this script with setup.exe

ECHO . Once you have run Checker, mail the log file (c:\vtunecheck.txt)
ECHO . to your support personnel.

@ECHO ON
pause

setup -ICHECKONLY
