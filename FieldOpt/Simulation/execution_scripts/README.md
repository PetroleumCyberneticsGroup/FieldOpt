# Execution Scripts

This directory contains scripts used to execute simulations. They have been tested in an Ubuntu environment.

## `csh_exlrun.sh`
* Executes ECL100 through the eclrun macro in CSH (C Shell).
* First parameter is the path to the work directory; second parameter is the path the ECL100 input file (.DATA).

## About execution scripts
Execution scripts are used to launch simulation. These are located in the `FieldOpt/execution_scripts` folder. In the driver file, a script is selected by specifying `"ExecutionScript": "name"`, where name is the name of the execution script without the suffix, e.g. `"csh_eclrun"`.

If you create a new execution script, it should have the same form as the existing scripts, i.e. it should take the work directory (i.e. the directory in which the output should be written) as the first argument, and the path to the `.DATA` file as the second argument. The contents of the `csh_eclrun.sh` script is shown below for reference.

```
#!/bin/csh
# Parameter 1: Work direcory.
# Parameter 2: Path to .DATA file

# Switch to work directory
cd $1

# Execute eclipse with the file path as parameter
eval eclrun eclipse $2 >& /dev/null

```

## `bash_adgprs.sh`
* Executes /usr/bin/adgprs
* First parameter is the path to the workdir, second parameter is the path to the driver file.

Note that if ADGPRS can't be started by executing `/usr/bin/adgprs driverfilenam`, you must
create a link to it, e.g.
```
sudo ln -s /opt/ADGPRS/bin/ADGPRSLinuxK3 /usr/bin/adgprs
```

