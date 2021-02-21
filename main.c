// ------------------ main.c ------------------

// Include external modules header
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Include trycatch module header
#include "trycatch.h"

// Dummy function to test exception raised from a called function
void fun() {

  if (isnan(0. / 0.)) Raise(TryCatchException_NaN);

}

// Main function
int main(
  // Unused command line arguments
  void) {

  // --------------
  // Simple example, raise an exception in a TryCatch block and catch it.

  Try {

    if (isnan(0. / 0.)) Raise(TryCatchException_NaN);

  } Catch(TryCatchException_NaN) {

    printf("Catched exception NaN\n");

  } EndTry;

  // Output:
  //
  // Catched exception NaN
  //

  // --------------
  // Example of TryCatch block inside another TryCatch block and exception
  // forwarded from the inner block to the outer block after being ignored
  // by the inner block.

  Try {

    Try {

      if (isnan(0. / 0.)) Raise(TryCatchException_NaN);

    } EndTry;

  } Catch (TryCatchException_NaN) {

    printf("Catched exception NaN at sublevel\n");

  } EndTry;

  // Output:
  //
  // Catched exception NaN at sublevel
  //

  // --------------
  // Example of user defined exception and multiple catch segments.

  enum UserDefinedExceptions {

    myUserExceptionA = TryCatchException_LastID,
    myUserExceptionB,
    myUserExceptionC

  };

  Try {

    Raise(myUserExceptionA);

  } Catch (myUserExceptionA) {

    printf("Catched user defined exception A\n");

  } Catch (myUserExceptionB) {

    printf("Catched user defined exception B\n");

  } Catch (myUserExceptionC) {

    printf("Catched user defined exception C\n");

  } EndTry;

  // Output:
  //
  // Catched user defined exception A
  //

// The struct siginfo_t used to handle the SIGSEV is not defined in
// ANSI C, guard against this.
#ifndef __STRICT_ANSI__

  // --------------
  // Example of handling exception raided by SIGSEV.

  // Init the SIGSEV signal handling by TryCatch.
  TryCatchInitHandlerSigSegv();

  Try {

    int *p = NULL;
    *p = 1;

  } Catch (TryCatchException_Segv) {

    printf("Catched exception Segv\n");

  } EndTry;

  // Output:
  //
  // Catched exception Segv
  //
#endif

  // --------------
  // Example of exception raised in called function and catched in calling
  // function.

  Try {

    fun();

  } Catch (TryCatchException_NaN) {

    printf("Catched exception NaN raised in called function\n");

  } EndTry;

  // Output:
  //
  // Catched exception NaN raised in called function
  //

  // --------------
  // Example of exception raised in called function and uncatched in calling
  // function.

  Try {

    fun();

  } EndTry;

  // Output:
  //
  // Unhandled exception (2).
  //

  // --------------
  // Example of exception raised outside a TryCatch block.

  Raise(TryCatchException_NaN);

  // Output:
  //
  // Unhandled exception (2).
  //

  // --------------
  // Example of overflow of recursive inclusion of TryCatch blocks.

  Try {

    Try {

      Try {

        Try {

          fun();

        } EndTry;

      } EndTry;

    } EndTry;

  } EndTry;

  // Output:
  //
  // TryCatch blocks recursive incursion overflow, exiting. (You can try
  // to raise the value of TryCatchMaxExcLvl in trycatch.c, it was: 3)
  //

  return 0;

}

// ------------------ main.c ------------------
