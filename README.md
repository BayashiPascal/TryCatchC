# TryCatchC

TryCatchC is a C module implementing the try/catch mechanism available in other programming languages but missing in C. It is based on the setjmp/longjmp functions. It supports recursive incursion of try/catch blocks, forward of uncaught exception to the upper level try/catch block if any, exception raised by signals like SIGSEV (non ANSI C feature), and user-defined exceptions.

## Usage

* Download and add the files `trycatch.c` and `trycatch.h` to your project.
* Modify the list of predefined exceptions in `enum TryCatchException` in `trycatch.h` according to your needs, or create your own user-defined exceptions as explained in `trycatch.h`.
* Modify the value of `TryCatchMaxExcLvl` in `trycatch.c` with a more appropriate value (maybe 256?)
* Optionally add support for signals other than SIGSEV by referring to `TryCatchInitHandlerSigSegv()` and `TryCatchSigSegvHandler()`.
* Use it in your project, for example as follows:
```
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "trycatch.h"
int main() {

  Try {

    if (isnan(0./0.)) Raise(TryCatchException_NaN);

  } Catch (TryCatchException_NaN) {

    printf("Caught exception NaN\n");

  } EndTry;

  return 0;
}
```
* Compile as follows:
```
	gcc -c main.c
	gcc -c trycatch.c
	gcc main.o trycatch.o -lm -o main
```
* Run with `./main`. Output:
```
Caught exception NaN
```

More examples can be found in `main.c` of this repository.

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

