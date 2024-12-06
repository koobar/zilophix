# ZilophiX (ZpX)

## WHAT IS THIS?
ZilophiX: ZilophiX is a open-source lossless audio codec.

16-bit PCM compression provides approximately the same compression ratio as 
FLAC's maximum efficiency setting.

24-bit PCM compression provides compression rates similar to Monkey's Audio's
maximum efficiency setting, and in some cases even higher.

Reference implementation of ZilophiX is a released under WTFPL Version 2. 
ZilophiX algorithms are released in the public domain. 

## HOW TO BUILD
run ``build.py``.

How to run ``build.py`` on your computer:
```
python build.py
```

If the compilation completes successfully, object files and executable files will be generated in the following directory.

```
        ../bin        Executable files
        ../obj        Object files
```

gcc is required to run the build script. You can also compile with clang by editing build.py.
Additionally, the build script is intended to be run on Python 3.13.0 or later.