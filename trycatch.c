// ------------------ trycatch.c ------------------

// Include the header
#include "trycatch.h"

// Size of the stack of TryCatch blocks, define how many recursive incursion
// of TryCatch blocks can be done, overflow is checked at the beginning of
// each TryCatch blocks with TryCatchGuardOverflow()
// (Set delibarately low here to be able to test it in the example main.c)
#define TryCatchMaxExcLvl 3

// Stack of jmp_buf to memorise the TryCatch blocks
// To avoid exposing this variable to the user, implement any code using
// it as functions here instead of in the #define-s of trycatch.h
jmp_buf tryCatchExcJmp[TryCatchMaxExcLvl];

// Index of the next TryCatch block in the stack of jmp_buf
// To avoid exposing this variable to the user, implement any code using
// it as functions here instead of in the #define-s of trycatch.h
int tryCatchExcLvl = 0;

// ID of the last raised exception
// To avoid exposing this variable to the user, implement any code using
// it as functions here instead of in the #define-s of trycatch.h
// Do not use the type enum TryCatchException to allow the user to extend
// the list of exceptions with user-defined exceptions outside of enum
// TryCatchException.
int tryCatchExc = 0;

// Function called at the beginning of a TryCatch block to guard against
// overflow of the stack of jump_buf
void TryCatchGuardOverflow(
  // No arguments
  void) {

  // If the max level of incursion is reached
  if (tryCatchExcLvl == TryCatchMaxExcLvl) {

    // Print a message on the standard error output and exit
    fprintf(
      stderr,
      "TryCatch blocks recursive incursion overflow, exiting. "
      "(You can try to raise the value of TryCatchMaxExcLvl in trycatch.c, "
      "it was: %d)\n",
      TryCatchMaxExcLvl);
    exit(EXIT_FAILURE);

  }

}

// Function called to get the jmp_buf on the top of the stack when
// starting a new TryCatch block
jmp_buf* TryCatchGetJmpBufOnStackTop(
  // No arguments
  void) {

  // Reset the last raised exception
  tryCatchExc = 0;

  // Memorise the current jmp_buf at the top of the stack
  jmp_buf* topStack = tryCatchExcJmp + tryCatchExcLvl;

  // Move the index of the top of the stack of jmp_buf to the upper level
  tryCatchExcLvl++;

  // Return the jmp_buf previously at the top of the stack
  return topStack;

}

// Function called to raise the TryCatchException 'exc'
void Raise(
  // The TryCatchException to raise. Do not use the type enum
  // TryCatchException to allow the user to extend the list of exceptions
  // with user-defined exception outside of enum TryCatchException.
  int exc) {

  // If we are in a TryCatch block
  if (tryCatchExcLvl > 0) {

    // Memorise the last raised exception to be able to handle it if
    // it reaches the default case in the swith statement of the TryCatch
    // block
    tryCatchExc = exc;

    // Move to the top of the stack of jmp_buf to the lower level
    tryCatchExcLvl--;

    // Call longjmp with the appropriate jmp_buf in the stack and the
    // raised TryCatchException.
    longjmp(
      tryCatchExcJmp[tryCatchExcLvl],
      exc);

  // Else we are not in a TryCatch block
  } else {

    // Print a message on the standard error stream and ignore the
    // exception
    fprintf(
      stderr,
      "Unhandled exception (%d).\n",
      exc);

  }

}

// Function called when a raised TryCatchException has not been caught
// by a Catch segment
void TryCatchDefault(
  // No arguments
  void) {

  // If we are outside of a TryCatch block
  if (tryCatchExcLvl == 0) {

    // The exception has not been caught by a Catch segment,
    // print a message on the standard error stream and ignore it
    fprintf(
      stderr,
      "Unhandled exception (%d).\n",
      tryCatchExc);

  // Else, the exception has not been caught in the current
  // TryCatch block but may be catchable at lower level
  } else {

    // Move to the lower level in the stack of jmp_buf and raise the
    // exception again
    Raise(tryCatchExc);

  }

}

// Function called at the end of a TryCatch block
void TryCatchEnd(
  // No arguments
  void) {

  // The execution has reached the end of the current TryCatch block,
  // move back to the lower level in the stack of jmp_buf
  if (tryCatchExcLvl > 0) tryCatchExcLvl--;

}

// The struct siginfo_t used to handle the SIGSEV is not defined in
// ANSI C, guard against this.
#ifndef __STRICT_ANSI__

// Handler function to raise the exception TryCatchException_Segv when
// receiving the signal SIGSEV.
void TryCatchSigSegvHandler(
  // Received signal, will always be SIGSEV, unused
  int signal,
  // Info about the signal, unused
  siginfo_t *si,
  // Optional arguments, unused
  void *arg) {

  // Raise the exception
  Raise(TryCatchException_Segv);

}

// Function to set the handler function of the signal SIGSEV and raise
// TryCatchException_Segv upon reception of this signal. Must have been
// called before using Catch(TryCatchException_Segv)
void TryCatchInitHandlerSigSegv(
  // No arugments
  void) {

  // Create a struct sigaction to set the handler
  struct sigaction sigActionSegv;
  memset(
    &sigActionSegv,
    0,
    sizeof(struct sigaction));
  sigemptyset(&(sigActionSegv.sa_mask));
  sigActionSegv.sa_sigaction = TryCatchSigSegvHandler;
  sigActionSegv.sa_flags = SA_SIGINFO;

  // Set the handler
  sigaction(
    SIGSEGV,
    &sigActionSegv,
    NULL);

}

#endif

// ------------------ trycatch.c ------------------
