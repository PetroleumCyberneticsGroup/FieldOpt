# FieldOpt
Petroleum Field Development Optimization Framework.

## Library Path
To run the compiled program, the LD_LIBRARY_PATH variable must be extended
to include the location of the compiled library project. Using the BASH-shell,
we can do this by executing

```
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/home/einar/Documents/GitHub/FieldOpt/build-FieldOpt-Desktop-Debug/Library:/home/einar/Documents/GitHub/FieldOpt/build-FieldOpt-Desktop-Debug/ERTWrapper:/home/einar/Documents/GitHub/FieldOpt/build-FieldOpt-Desktop-Debug/Model:/usr/local/lib/x86_64-linux-gnu
export LD_LIBRARY_PATH
```

To make this persistent, we can add this code to the `~/.bashrc` file. This will add both the main library, the ERTWrapper and the ECL library from ERT to the library path.
