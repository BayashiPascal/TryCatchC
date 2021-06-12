# TryCatchC

TryCatchC is a C module implementing the try/catch mechanism available in other programming languages but missing in C. It is based on the setjmp/longjmp functions. It supports recursive incursion of try/catch blocks, forward of uncaught exception to the upper level try/catch block if any, exception raised by signals like SIGSEV (non ANSI C feature), shared handling of several exceptions, user-defined default handler for uncaught exception, trace of raised exceptions toward a stream, and user-defined exceptions.

## Usage

* Copy this repository
```
wget https://github.com/BayashiPascal/TryCatchC/archive/main.zip
unzip main.zip
mv TryCatchC-main TryCatchC
rm main.zip
```
* Compile and install
```
cd TryCatchC
make
sudo make install
cd -
```
* Optionnally you can delete the repository 
```
rm -rf TryCatchC
```
* Then use it in your project, for example as follows:
```
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <TryCatchC/trycatchc.h>
int main() {

  TRY {

    if (isnan(0./0.)) RAISE(TryCatchException_NaN);

  } CATCH (TryCatchException_NaN) {

    printf("Caught exception NaN\n");

  } ENDCATCH;

  return 0;
}
```
* Compile as follows:
```
	gcc -c main.c
	gcc main.o -ltrycatchc -lm -o main
```
* Run with `./main`. Output:
```
Caught exception NaN
```

More examples can be found in `main.c` of this repository.

## Warning

### Clobbered warning

The dummy example below:

```
TRY {
  int e = 0;
  TRY { 
    /* something which doesn't use e */
  } CATCHDEFAULT {
    /* something which uses e */
  } ENDCATCHDEFAULT;
  /* something which uses e */
} ENDCATCH;
```

raises `warning: variable ‘e’ might be clobbered by ‘longjmp’ or ‘vfork’ [-Wclobbered]` when compiled with `gcc -Wextra -On -c main.c` where `n>0` (gcc --version is 10.1.0).

Searching on the web shows it's a known problem since forever ([link](https://gcc.gnu.org/legacy-ml/gcc/1997-11/msg00029.html)). As far as I understand, the optimizer sees `e` unused in the `Try` block and delay its declaration into the `CATCHDEFAULT` block, after the `setjmp` occurs, hence the clobbering when it's used after the Try/Catch if the longjmp had occured.

The warning can be avoided by adding `-Wno-clobbered ` to the compilation command and ignore this seemingly erroneous warning, or by declaring `e` as `volatile` to force the optimizer to leave the declaration where it is:

```
TRY {
  volatile int e = 0;
  TRY { 
    /* something which doesn't use e */
  } CATCHDEFAULT {
    /* something which uses e */
  } ENDCATCHDEFAULT;
  /* something which uses e */
} ENDCATCH;
```

### Returning from inside a Try block

If you want to return/goto from inside a Try block, you need to call `TryCatchEnd()` before returning, else the internal stack of frame of TryCatch gets inconsistent, leading to undefined behaviour. The example below shows the correct way to use `return` inside a Try block.

```
void fun(int a) {
  TRY {
    ...
    if (...) {
      TryCatchEnd();
      return;
    }
    ...
  } ENDCATCH;
}
```

If you're returning from several levels of imbrication of Try blocks, you must call `TryCatchEnd()` as many time as you skip `EndCatch` by returning.

## License

TryCatchC, a C implementation of the try/catch mechanism in C
Copyright (C) 2021  Pascal Baillehache

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

