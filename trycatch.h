// ------------------ trycatch.h ------------------

// Guard against multiple inclusions
#ifndef _TRYCATCH_
#define _TRYCATCH_

// Include external modules header
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <string.h>

// List of exceptions ID, must starts at 1 (0 is reserved for the setjmp at
// the beginning of the TryCatch blocks). One can extend the list at will
// here, or user-defined exceptions can be added directly in the user code
// as follows:
// enum UserDefinedExceptions {
//
//   myUserExceptionA = TryCatchException_LastID,
//   myUserExceptionB,
//   myUserExceptionC
//
// };
// TryCatchException_LastID is not an exception but a convenience to
// create new exceptions (as in the example above) while ensuring
// their ID doesn't collide with the ID of exceptions in TryCatchException.
// Exception defined here are only examples, one should create a list of
// default exceptions according to the planned use of this trycatch module.
enum TryCatchException {

  TryCatchException_test = 1,
  TryCatchException_NaN,
  TryCatchException_Segv,
  TryCatchException_LastID

};

// Function called at the beginning of a TryCatch block to guard against
// overflow of the stack of jump_buf
void TryCatchGuardOverflow(
  // No parameters
  void);

// Function called to get the jmp_buf on the top of the stack when
// starting a new TryCatch block
jmp_buf* TryCatchGetJmpBufOnStackTop(
  // No parameters
  void);

// Function called when a raised TryCatchException has not been caught
// by a Catch segment
void TryCatchDefault(
  // File where the exception occured
  char const* const filename,
  // Line where the exception occured
          int const line);

// Function called at the end of a TryCatch block
void TryCatchEnd(
  // No parameters
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
//      // End of the previous case
//      break;
//    // case of the raised exception
//    case e:
#define Catch(e) \
      break;\
    case e:

// Macro to assign several exceptions to one Catch segment in the TryCatch
// block, to be used as
//
// Catch (/*... one of TryCatchException or user-defined exception ...*/)
// CatchAlso (/*... another one ...*/) {
// /*... as many CatchAlso statement as your need ...*/
//   /*... code executed if one of the exception has been raised in the
//     TryCatch block ...
//     (Use TryCatchGetLastExc() if you need to know which exception as
//     been raised) */
//
// Comments on the macro:
//    // case of the raised exception
//    case e:
#define CatchAlso(e) \
    case e:

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
//    // default case
//    default:
#define CatchDefault \
    default:

// Tail of the TryCatch block if it doesn't contain CatchDefault, 
// to be used as
//
// } EndTry;
//
// Comments on the macro:
//      // End of the previous case
//      break;
//    // default case, i.e. any raised exception which hasn't been caught
//    // by a previous Catch is caught here
//    default:
//      // Processing of uncaught exception
//      TryCatchDefault();
//  // End of the switch statement at the head of the TryCatch block
//  }
//  // Post processing of the TryCatchBlock
//  TryCatchEnd()
#define EndTry \
      break; \
    default: \
      TryCatchDefault(__FILE__, __LINE__); \
  } \
  TryCatchEnd()

// Tail of the TryCatch block if it contains CatchDefault, 
// to be used as
//
// } EndTryWithDefault;
//
// Comments on the macro:
//  // End of the switch statement at the head of the TryCatch block
//  }
//  // Post processing of the TryCatchBlock
//  TryCatchEnd()
#define EndTryWithDefault \
  } \
  TryCatchEnd()

// Function called to raise the TryCatchException 'exc'
void Raise(
  // The TryCatchException to raise. Do not use the type enum
  // TryCatchException to allow the user to extend the list of exceptions
  // with user-defined exception outside of enum TryCatchException.
  int exc);

// The struct siginfo_t used to handle the SIGSEV is not defined in
// ANSI C, guard against this.
#ifndef __STRICT_ANSI__

// Function to set the handler function of the signal SIGSEV and raise
// TryCatchException_Segv upon reception of this signal. Must have been
// called before using Catch(TryCatchException_Segv)
void TryCatchInitHandlerSigSegv(
  // No parameters
  void);

#endif

// Function to get the ID of the last raised exception
int TryCatchGetLastExc(
  // No parameters
  void);

// End of the guard against multiple inclusion
#endif

// ------------------ trycatch.h ------------------
