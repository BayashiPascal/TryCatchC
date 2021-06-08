// ------------------ trycatchc.h ------------------

// Guard against multiple inclusions
#ifndef _TRYCATCHC_
#define _TRYCATCHC_

// Include external modules header
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <setjmp.h>
#include <signal.h>
#include <string.h>

// List of exceptions ID, must starts at 1 (0 is reserved for the setjmp at
// the beginning of the TryCatch blocks). One can extend the list at will
// here, or user-defined exceptions can be added directly in the user code
// as follows:
// enum UserDefinedExceptions {
//
//   myUserExceptionA = TryCatchExc_LastID,
//   myUserExceptionB,
//   myUserExceptionC
//
// };
// TryCatchExc_LastID is not an exception but a convenience to
// create new exceptions (as in the example above) while ensuring
// their ID doesn't collide with the ID of exceptions in TryCatchException.
// Exception defined here are only examples, one should create a list of
// default exceptions according to the planned use of this trycatch module.
enum TryCatchException {

  TryCatchExc_Segv = 1,
  TryCatchExc_MallocFailed,
  TryCatchExc_IOError,
  TryCatchExc_TooManyExcToStrFun,
  TryCatchExc_NaN,
  TryCatchExc_IntOverflow,
  TryCatchExc_OutOfRange,
  TryCatchExc_NotYetImplemented,
  TryCatchExc_UnitTestFailed,
  TryCatchExc_LastID

};

// Function called at the beginning of a TryCatch block to guard against
// overflow of the stack of jump_buf
void TryCatchGuardOverflow(
  void);

// Function called to get the jmp_buf on the top of the stack when
// starting a new TryCatch block
// Output:
//   Remove the jmp_buf on the top of the stack and return it
jmp_buf* TryCatchGetJmpBufOnStackTop(
  void);

// Function called when a raised TryCatchException has not been caught
// by a Catch segment
void TryCatchDefault(
  void);

// Function called when entering a catch block
void TryCatchEnterCatchBlock(
  void);

// Function called when exiting a catch block
void TryCatchExitCatchBlock(
  void);

// Function called at the end of a TryCatch block
void TryCatchEnd(
  void);

// Head of the TryCatch block, to be used as
//
// Try {
//   /*... code of the TryCatch block here ...*/
//
// Comments on the macro:
//   // Guard against recursive incursion overflow
//   TryCatchGuardOverflow();
//   // Memorise the jmp_buf on the top of the stack, setjmp returns 0
//   switch (setjmp(*TryCatchGetJmpBufOnStackTop())) {
//     // Entry point for the code of the TryCatch block
//     case 0:
#define Try \
  TryCatchGuardOverflow(); \
  switch (setjmp(*TryCatchGetJmpBufOnStackTop())) { \
    case 0:

// Catch segment in the TryCatch block, to be used as
//
// Catch (/*... one of TryCatchException or user-defined exception ...*/) {
//   /*... code executed if the exception has been raised in the
//     TryCatch block ...*/
//
// Comments on the macro:
//      // Exit the previous Catch block
//      TryCatchExitCatchBlock();
//      // End of the previous case
//      break;
//    // case of the raised exception
//    case e:
//      // Flag the entrance into the Catch block
//      TryCatchEnterCatchBlock();
#define Catch(e) \
      TryCatchExitCatchBlock();\
      break;\
    case e:\
      TryCatchEnterCatchBlock();

// Macro to assign several exceptions to one Catch segment in the TryCatch
// block, to be used as
//
// Catch (/*... one of TryCatchException or user-defined exception ...*/)
// CatchAlso (/*... another one ...*/) {
// /*... as many CatchAlso statement as your need ...*/
//   /*... code executed if one of the exception has been raised in the
//     TryCatch block ...
//     (Use TryCatchGetLastExc() if you need to know which excption as
//     been raised) */
//
// Comments on the macro:
//      // Avoid the fall through warning due to the TryCatchEnterCatchBlock()
//      // at the entrance of the Catch case
//      /* fall through */
//    // case of the raised exception
//    case e:
//      // Flag the entrance into the Catch block
//      TryCatchEnterCatchBlock();
#define CatchAlso(e) \
      /* fall through */ \
    case e:\
      TryCatchEnterCatchBlock();

// Macro to declare the default Catch segment in the TryCatch
// block, must be the last Catch segment in the TryCatch block,
// to be used as
//
// CatchDefault {
//   /*... code executed if an exception has been raised in the
//     TryCatch block and hasn't been catched by a previous Catch segment...
//     (Use TryCatchGetLastExc() if you need to know which exception as
//     been raised) */
//
// Comments on the macro:
//      // Exit the previous Catch block
//      TryCatchExitCatchBlock();
//      // End of the previous case
//      break;
//    // default case
//    default:
//      // Flag the entrance into the Catch block
//      TryCatchEnterCatchBlock();
#define CatchDefault \
      TryCatchExitCatchBlock();\
      break; \
    default:\
        TryCatchEnterCatchBlock();

// Tail of the TryCatch block, to be used as
//
// } EndCatch;
//
// Comments on the macro:
//      // Exit the previous Catch block
//      TryCatchExitCatchBlock();
//      // End of the previous case
//      break;
//    // default case, i.e. any raised exception which hasn't been catched
//    // by a previous Catch is catched here
//    default:
//      // Processing of uncatched exception
//      TryCatchDefault();
//  // End of the switch statement at the head of the TryCatch block
//  }
//  // Post processing of the TryCatchBlock
//  TryCatchEnd()
#define EndCatch \
      TryCatchExitCatchBlock();\
      break; \
    default: \
      TryCatchDefault(); \
  } \
  TryCatchEnd()

// Tail of the TryCatch block if it contains CatchDefault,
// to be used as
//
// } EndCatchDefault;
//
// Comments on the macro:
//  // End of the switch statement at the head of the TryCatch block
//  }
//  // Post processing of the TryCatchBlock
//  TryCatchEnd()
#define EndCatchDefault \
  } \
  TryCatchEnd()

// Function called to raise the TryCatchException 'exc'
// Inputs:
//        exc: The TryCatchException to raise. Do not use the type enum
//             TryCatchException to allow the user to extend the list of
//             exceptions with user-defined exception outside of enum
//             TryCatchException.
//   filename: File where the exception has been raised
//       line: Line where the exception has been raised
void Raise_(
                int exc,
  char const* const filename,
          int const line);

// Wrapper to call Raise_ with file name and line number
#define Raise(e) Raise_(e, __FILE__, __LINE__)

// Macro to recatch and forward an exception. This is usefull when an exception
// may be raised by a handler, in which case the trace loose track of where
// the exception has occured. By ReCatch-ing the block of code B susceptible
// of triggering the handler, one can ensure the trace will properly indicates
// this block of code as the source of the exception.
#define ReCatch(B) \
  Try { B; } CatchDefault { Raise(TryCatchGetLastExc()); } EndCatchDefault;

// The struct siginfo_t used to handle the SIGSEV is not defined in
// ANSI C, guard against this.
#ifndef __STRICT_ANSI__

// Function to set the handler function of the signal SIGSEV and raise
// TryCatchExc_Segv upon reception of this signal. Must have been
// called before using Catch(TryCatchExc_Segv)
void TryCatchInitHandlerSigSegv(
  void);

#endif

// Function to get the ID of the last raised exception
// Output:
//   Return the id of the last raised exception
int TryCatchGetLastExc(
  void);

// Function to convert an exception ID to char*
// Input:
//   exc: The exception ID
// Output:
//   Return the stringified exception
char const* TryCatchExcToStr(
  int exc);

// Function to add a function used by TryCatch to convert user-defined
// function to a string. The function in argument must return NULL if its
// argument is not an exception ID it is handling, else a pointer to a
// statically allocated string.
// It is highly recommended to provide conversion functions to cover
// all the user defined exceptions as it also allows TryCatch to detect
// conflict between exception IDs.
// Input:
//   fun: The conversion function to add
void TryCatchAddExcToStrFun(
  char const* (*fun)(int));

// Set the stream on which to print exception raising, set it to NULL to
// turn off messages
// Input:
//   stream: The stream to used
void TryCatchSetRaiseStream(
  FILE* const stream);

// End of the guard against multiple inclusion
#endif

// ------------------ trycatch.h ------------------
