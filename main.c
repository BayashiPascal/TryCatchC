// ------------------ main.c ------------------

// Include external modules header
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Include TryCatchC module header
// #include <TryCatchC/trycatchc.h>
// Here, use the local header file for dev/test purpose
#include "trycatchc.h"

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
// Input:
//   exc: the exception ID
// Output:
//   Return the conversion  of the exception id to a string, or NULL
//   if the exception id is unknown
char const* ExcToStr(
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
int main() {

  // Set the stream to print exception raising to stdout
  TryCatchSetRaiseStream(stdout);

  // --------------
  // Simple example, raise an exception in a TryCatch block and catch it.

  Try {

    if (isnan(0. / 0.)) Raise(TryCatchExc_NaN);

  } Catch(TryCatchExc_NaN) {

    printf("Caught exception NaN\n");

  } EndCatch;

  // Output:
  //
  // Exception (TryCatchException_NaN) raised in main.c, line 86.
  // Caught exception NaN
  //

  // --------------
  // Example of TryCatch block inside another TryCatch block and exception
  // forwarded from the inner block to the outer block after being ignored
  // by the inner block.

  Try {

    Try {

      if (isnan(0. / 0.)) Raise(TryCatchExc_NaN);

    } EndCatch;

  } Catch (TryCatchExc_NaN) {

    printf("Caught exception NaN at sublevel\n");

  } EndCatch;

  // Output:
  //
  // Exception (TryCatchException_NaN) raised in main.c, line 109.
  // Caught exception NaN at sublevel
  //

  // --------------
  // Example of user-defined exception without setting the conversion
  // function.

  Try {

    Raise(myUserExceptionA);

  } EndCatch;

  // Output:
  //
  // Exception (User-defined exception (6)) raised in main.c, line 131.
  //

  // --------------
  // Example of user-defined exception aftr setting the conversion
  // function.

  TryCatchAddExcToStrFun(ExcToStr);

  Try {

    Raise(myUserExceptionA);

  } EndCatch;

  // Output:
  //
  // Exception (myUserExceptionA) raised in main.c, line 148.
  //

  // --------------
  // Example of user-defined exception with conflicting ID and without
  // conversion function.

  Try {

    Raise(conflictException);

  } EndCatch;

  // Output:
  //
  // Exception (myUserExceptionA) raised in main.c, line 163.
  // !!! TryCatch: Exception ID conflict, between conflicting exception
  // and myUserExceptionA !!!
  //

  // --------------
  // Example of user-defined exception with conflicting ID and with
  // conversion function.

  TryCatchAddExcToStrFun(ConflictExcToStr);
  Try {

    Raise(conflictException);

  } EndCatch;

  // Output:
  //
  // Exception (conflicting exception) raised in main.c, line 181.
  // !!! TryCatch: Exception ID conflict, between conflicting exception
  // and myUserExceptionA !!!
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

  } EndCatch;

  // Output:
  //
  // Exception (conflicting exception) raised in main.c, line 197.
  // Caught user-defined exception A
  //

  // --------------
  // Example of exception raised in called function and caught in calling
  // function.

  Try {

    fun();

  } Catch (TryCatchExc_NaN) {

    printf("Caught exception NaN raised in called function\n");

  } EndCatch;

  // Output:
  //
  // Exception (TryCatchException_NaN) raised in main.c, line 16.
  // Caught exception NaN raised in called function
  //

  // --------------
  // Example of exception raised in called function and uncaught in calling
  // function.

  Try {

    fun();

  } EndCatch;

  // Output:
  //
  // Exception (TryCatchException_NaN) raised in main.c, line 16.
  //

  // --------------
  // Example of exception raised outside a TryCatch block.

  Raise(TryCatchExc_NaN);

  // Output:
  //
  // Exception (TryCatchException_NaN) raised in main.c, line 257.
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
      "Caught exception %s\n",
      TryCatchExcToStr(idExc));

  } EndCatch;

  // Output:
  //
  // Exception (TryCatchException_NaN) raised in main.c, line 269.
  // Caught exception TryCatchException_NaN
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

  } EndCatchDefault;

  // Output:
  //
  // Exception (TryCatchException_NaN) raised in main.c, line 293.
  // Caught exception TryCatchException_NaN with CatchDefault
  //

  // --------------
  // Example of manually delayed exception

  Try {

    volatile int e = 0;

    Try {

      Raise(TryCatchExc_IOError);

    } CatchDefault {

      e = TryCatchGetLastExc();

    } EndCatchDefault;

    if (e != 0) Raise(e);

  } CatchDefault {

    printf(
      "Caught manually delayed exception %s.\n",
      TryCatchExcToStr(TryCatchGetLastExc()));

  } EndCatchDefault;

  // Output:
  //
  // Exception (TryCatchExc_IOError) raised in main.c, line 319.
  // Exception (TryCatchExc_IOError) raised in main.c, line 327.
  // Caught manually delayed exception TryCatchExc_IOError.
  //

  // --------------
  // Example of raised exception from user default catch block

  Try {

    Try {

      Raise(TryCatchExc_IOError);

    } CatchDefault {

      Raise(TryCatchGetLastExc());

    } EndCatchDefault;

  } CatchDefault {

    printf(
      "Caught exception from user default catch block %s.\n",
      TryCatchExcToStr(TryCatchGetLastExc()));

  } EndCatchDefault;

  // Output:
  //
  // Exception (TryCatchExc_IOError) raised in main.c, line 351.
  // Exception (TryCatchExc_IOError) raised in main.c, line 355.
  // Caught exception from user default catch block TryCatchExc_IOError.
  //

  // --------------
  // Example of raised exception from catch block

  Try {

    Try {

      Raise(TryCatchExc_IOError);

    } Catch(TryCatchExc_IOError) {

      Raise(TryCatchExc_MallocFailed);

    } EndCatch;

  } CatchDefault {

    printf(
      "Caught exception raised from catch block %s.\n",
      TryCatchExcToStr(TryCatchGetLastExc()));

  } EndCatchDefault;

  // Output:
  //
  // Exception (TryCatchExc_IOError) raised in main.c, line 381.
  // Exception (TryCatchExc_MallocFailed) raised in main.c, line 385.
  // Caught exception raised from catch block TryCatchExc_IOError.
  //

// The struct siginfo_t used to handle the SIGSEV is not defined in
// ANSI C, guard against this.
#ifndef __STRICT_ANSI__

  // --------------
  // Example of handling exception raised by SIGSEV and ReCatch-ing to
  // correctly trace the source of the exception.

  // Init the SIGSEV signal handling by TryCatch.
  TryCatchInitHandlerSigSegv();

  Try {

    int* p = NULL;
    Recatch(*p = 1);

  } Catch (TryCatchExc_Segv) {

    printf("Caught exception Segv\n");

  } EndCatch;

  // Output:
  //
  //  Exception (TryCatchExc_Segv) raised in main.c, line 418.
  //  Exception (TryCatchExc_Segv) raised in main.c, line 418.
  // Caught exception Segv
  //
#endif

  // --------------
  // Example of overflow of recursive inclusion of TryCatch blocks.

  Try {

    Try {

      Try {

        Try {

          fun();

        } EndCatch;

      } EndCatch;

    } EndCatch;

  } EndCatch;

  // Output:
  //
  // TryCatch blocks recursive incursion overflow, exiting. (You can try
  // to raise the value of TryCatchMaxExcLvl in trycatch.c, it was: 3)
  //

  return 0;

}

// ------------------ main.c ------------------
