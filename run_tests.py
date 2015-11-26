import subprocess
import sys

proc = subprocess.Popen(['./run_tests.sh'],stdout=subprocess.PIPE)
if len(sys.argv) == 1: # Full output
    while True:
      line = proc.stdout.readline()
      if line != '':
        print line.rstrip()
      else:
        break
elif len(sys.argv) == 2 and sys.argv[1] == "--shortsum": # Short summary
    while True:
      line = proc.stdout.readline()
      if line != '':
        if line.startswith('[==='):
            print line.rstrip()
        elif "FAILED" in line:
            print line.rstrip()
      else:
        break

elif len(sys.argv) == 2 and sys.argv[1] == "--longsum": # Long summary
    while True:
      line = proc.stdout.readline()
      if line != '':
        if (line.startswith('[---') and line.endswith('total)\n')) or line.startswith('[==='):
            print line.rstrip()
        elif "FAILED" in line:
            print line.rstrip()
      else:
        break