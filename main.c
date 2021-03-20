// ------------------ main.c ------------------

// Include external modules header
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Include TryCatchC module header
#include <TryCatchC/trycatchc.h>

// Dummy function to test exception raised from a called function
void fun() {

  if (isnan(0. / 0.)) Raise(TryCatchExc_NaN);

}

// Example of user-defined exceptions
enum UserDefinedExceptions {

  myUserExceptionA = TryCatchExc_LastID,
  myUserExceptionB,
  myUserExceptionC,

};

// Label for the UserDefinedExceptions
char* exceptionStr[3] = {

  "myUserExceptionA",
  "myUserExceptionB",
  "myUserExceptionC"

};

// Function to convert UserDefinedExceptions to char*
char const* ExcToStr(
  // The exception ID
  int exc) {

  // If the exception ID is one of UserDefinedExceptions
  if (
    exc >= myUserExceptionA &&
    exc <= myUserExceptionC) {

    // Return the conversion
    return exceptionStr[exc - myUserExceptionA];

  // Else, the exception ID is not one of UserDefinedExceptions
  } else {

    // Return NULL to indicate there is no conversion
    return NULL;

  }

}

// Example of conflicting user-defined exceptions
#define conflictException TryCatchExc_LastID
char* conflictExceptionStr = "conflicting exception";
char const* ConflictExcToStr(int exc) {

  if (exc == conflictException) return conflictExceptionStr;
  return NULL;

}

// Main function
int main(
  // Unused command line arguments
  void) {

  // --------------
  // Simple example, raise an exception in a TryCatch block and catch it.

  Try {

    if (isnan(0. / 0.)) Raise(TryCatchExc_NaN);

  } Catch(TryCatchExc_NaN) {

    printf("Caught exception NaN\n");

  } EndTry;

  // Output:
  //
  // Caught exception NaN
  //

  // --------------
  // Example of TryCatch block inside another TryCatch block and exception
  // forwarded from the inner block to the outer block after being ignored
  // by the inner block.

  Try {

    Try {

      if (isnan(0. / 0.)) Raise(TryCatchExc_NaN);

    } EndTry;

  } Catch (TryCatchExc_NaN) {

    printf("Caught exception NaN at sublevel\n");

  } EndTry;

  // Output:
  //
  // Caught exception NaN at sublevel
  //

  // --------------
  // Example of user-defined exception without of setting the conversion
  // function.

  Try {

    Raise(myUserExceptionA);

  } EndTry;

  // Output:
  //
  // Unhandled exception (User-defined exception (6)) in main.c, line 124.
  //

  // --------------
  // Example of user-defined exception with setting of the conversion
  // function.

  TryCatchAddExcToStrFun(ExcToStr);

  Try {

    Raise(myUserExceptionA);

  } EndTry;

  // Output:
  //
  // Unhandled exception (myUserExceptionA) in main.c, line 141.
  //

  // --------------
  // Example of user-defined exception with conflicting ID and without
  // conversion function.

  Try {

    Raise(conflictException);

  } EndTry;

  // Output:
  //
  // Unhandled exception (myUserExceptionA) in main.c, line 156.
  //

  // --------------
  // Example of user-defined exception with conflicting ID and with
  // conversion function.

  TryCatchAddExcToStrFun(ConflictExcToStr);
  Try {

    Raise(conflictException);

  } EndTry;

  // Output:
  //
  // !!! TryCatch: Exception ID conflict, between conflicting exception
  // and myUserExceptionA !!!
  // Unhandled exception (conflicting exception) in main.c, line 172.
  //

  // --------------
  // Example of user-defined exception and multiple catch segments.

  Try {

    Raise(myUserExceptionA);

  } Catch (myUserExceptionA) {

    printf("Caught user-defined exception A\n");

  } Catch (myUserExceptionB) {

    printf("Caught user-defined exception B\n");

  } Catch (myUserExceptionC) {

    printf("Caught user-defined exception C\n");

  } EndTry;

  // Output:
  //
  // Caught user-defined exception A
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

  } Catch (TryCatchExc_Segv) {

    printf("Caught exception Segv\n");

  } EndTry;

  // Output:
  //
  // Caught exception Segv
  //
#endif

  // --------------
  // Example of exception raised in called function and caught in calling
  // function.

  Try {

    fun();

  } Catch (TryCatchExc_NaN) {

    printf("Caught exception NaN raised in called function\n");

  } EndTry;

  // Output:
  //
  // Caught exception NaN raised in called function
  //

  // --------------
  // Example of exception raised in called function and uncaught in calling
  // function.

  Try {

    fun();

  } EndTry;

  // Output:
  //
  // Unhandled exception (TryCatchException_NaN) in main.c, line 259.
  //

  // --------------
  // Example of exception raised outside a TryCatch block.

  Raise(TryCatchExc_NaN);

  // Output:
  //
  // Unhandled exception (TryCatchException_NaN).
  //

  // --------------
  // Example of several exceptions assigned to one single Catch segment.

  Try {

    Raise(TryCatchExc_NaN);

  } Catch (TryCatchExc_Segv)
    CatchAlso (TryCatchExc_NaN)
    CatchAlso (TryCatchExc_MallocFailed) {

    int idExc = TryCatchGetLastExc();
    printf(
      "Caught exception %d\n",
      idExc);

  } EndTry;

  // Output:
  //
  // Caught exception 5
  //

  // --------------
  // Example of a CatchDefault segment.

  Try {

    Raise(TryCatchExc_NaN);

  } CatchDefault {

    int idExc = TryCatchGetLastExc();
    printf(
      "Caught exception %s with CatchDefault\n",
      TryCatchExcToStr(idExc));

  } EndTryWithDefault;

  // Output:
  //
  // Caught exception TryCatchException_NaN with CatchDefault
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
