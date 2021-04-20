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
static jmp_buf tryCatchExcJmp[TryCatchMaxExcLvl];

// Index of the next TryCatch block in the stack of jmp_buf
// To avoid exposing this variable to the user, implement any code using
// it as functions here instead of in the #define-s of trycatch.h
static int tryCatchExcLvl = 0;

// ID of the last raised exception
// To avoid exposing this variable to the user, implement any code using
// it as functions here instead of in the #define-s of trycatch.h
// Do not use the type enum TryCatchException to allow the user to extend
// the list of exceptions with user-defined exceptions outside of enum
// TryCatchException.
static int tryCatchExc = 0;

// Flag to memorise if we are inside a catch block at a given exception level
static bool flagInCatchBlock[TryCatchMaxExcLvl] = {false};

// Label for the TryCatchExceptions
static char* exceptionStr[TryCatchExc_LastID] = {
  "",
  "TryCatchExc_Segv",
  "TryCatchExc_MallocFailed",
  "TryCatchExc_IOError",
  "TryCatchExc_TooManyExcToStrFun",
  "TryCatchException_NaN",
};

// Buffer to build default label for user defined exceptions
// Size of the buffer is calculated as length of "User-defined exception
//  ()" plus enough space to hold the representation of an int
static char userDefinedExceptionDefaultLabel[50];

// Max number of user-defined functions used to convert user-defined
// exception ID to strings
#define nbMaxUserDefinedExcToStr 256

// Current number of user-defined functions used to convert user-defined
// exception ID to strings
static int nbUserDefinedExcToStr = 0;

// Pointers to user-defined functions used to convert user-defined
// exception ID to strings
static char const* (*userDefinedExcToStr[nbMaxUserDefinedExcToStr])(int);

// Label of exceptions, must match the declaration of enum TryCatchException
// Take care of index 0 which is unused in the enum
char* TryCatchExceptionStr[TryCatchExc_LastID] = {

  "",
  "TryCatchExc_Segv",
  "TryCatchExc_MallocFailed",
  "TryCatchExc_IOError",
  "TryCatchExc_TooManyExcToStrFun",
  "TryCatchExc_NaN",

};

// Stream to print out a message each time Raise is called
static FILE* streamRaise = NULL;

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
void Raise_(
  // The TryCatchException to raise. Do not use the type enum
  // TryCatchException to allow the user to extend the list of exceptions
  // with user-defined exception outside of enum TryCatchException.
                int exc,
  // File where the exception has been raised
  char const* const filename,
  // Line where the exception has been raised
          int const line) {

  // If the stream to record exception raising is set, print the exception
  // on the stream
  if (streamRaise != NULL)
    fprintf(
      streamRaise,
      "Exception (%s) raised in %s, line %d.\n",
      TryCatchExcToStr(exc),
      filename,
      line);

  // If we are in a TryCatch block
  if (tryCatchExcLvl > 0) {

    // Memorise the last raised exception to be able to handle it if
    // it reaches the default case in the swith statement of the TryCatch
    // block
    tryCatchExc = exc;

    // Get the level in the stack where to jump back
    int jumpTo = (tryCatchExcLvl > 0 ? tryCatchExcLvl - 1 : 0);

    // If we are in a catch block
    if (flagInCatchBlock[tryCatchExcLvl] == true) {

      // The flag won't be reset by TryCatchEnd() because it's skipped by
      // the longjmp, do it here 
      flagInCatchBlock[tryCatchExcLvl] = false;

      // tryCatchExcLvl won't be updated by TryCatchEnd() because it's skipped
      // by the longjmp, do it here
      if (tryCatchExcLvl > 0) tryCatchExcLvl--;

    }

    // Call longjmp with the appropriate jmp_buf in the stack and the
    // raised TryCatchException.
    longjmp(
      tryCatchExcJmp[jumpTo],
      exc);

  }

}

// Function called when a raised TryCatchException has not been caught
// by a Catch segment
void TryCatchDefault(
  // No arguments
  void) {

  // If we are in a TryCatch block, the exception has not been caught
  // in the current block but may be catchable at lower level
  if (tryCatchExcLvl > 0) {

    // Move to the lower level in the stack of jmp_buf and raise the
    // exception again
    tryCatchExcLvl--;
    Raise(tryCatchExc);

  }

}

// Function called when entering a catch block
void TryCatchEnterCatchBlock(
  // No arguments
  void) {

  // Update the flag
  flagInCatchBlock[tryCatchExcLvl] = true;

}

// Function called when exiting a catch block
void TryCatchExitCatchBlock(
  // No arguments
  void) {

  // Update the flag
  flagInCatchBlock[tryCatchExcLvl] = false;

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

// Handler function to raise the exception TryCatchExc_Segv when
// receiving the signal SIGSEV.
void TryCatchSigSegvHandler(
  // Received signal, will always be SIGSEV, unused
  int signal,
  // Info about the signal, unused
  siginfo_t *si,
  // Optional arguments, unused
  void *arg) {

  // Unused parameters
  (void)signal; (void)si; (void)arg;

  // Raise the exception
  Raise(TryCatchExc_Segv);

}

// Function to set the handler function of the signal SIGSEV and raise
// TryCatchExc_Segv upon reception of this signal. Must have been
// called before using Catch(TryCatchExc_Segv)
void TryCatchInitHandlerSigSegv(
  // No arguments
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

// Function to get the ID of the last raised exception
int TryCatchGetLastExc(
  // No parameters
  void) {

  // Return the ID
  return tryCatchExc;

}

// Function to convert an exception ID to char*
char const* TryCatchExcToStr(
  // The exception ID
  int exc) {

  // Declare the pointer to the result string
  char const* excStr = NULL;

  // If the exception ID is one of TryCatchException
  if (exc < TryCatchExc_LastID) excStr = exceptionStr[exc];

  // Loop on user-defined conversion functions
  for (
    int iFun = 0;
    iFun < nbUserDefinedExcToStr;
    ++iFun) {

    // Get the conversion using this function
    char const* str = (*userDefinedExcToStr[iFun])(exc);

    // If the exception ID could be converted
    if (str != NULL) {

      // If there was already another result of conversion,
      // it means there is a ID conflict
      if (excStr != NULL) {

        fprintf(
          stderr,
          "!!! TryCatch: Exception ID conflict, between %s and %s !!!\n",
          str,
          excStr);

      }

      // Update the pointer the result
      excStr = str;

    }

  }

  // If we haven't find a conversion yet
  if (excStr == NULL) {

    // Create a default string
    sprintf(
      userDefinedExceptionDefaultLabel,
      "User-defined exception (%d)",
      exc);
    excStr = userDefinedExceptionDefaultLabel;

  }

  // Return the converted exception to string
  return excStr;

}

// Function to add a function used by TryCatch to convert user-defined
// function to a string. The function in argument must return NULL if its
// argument is not an exception ID it is handling, else a pointer to a
// statically allocated string.
// It is highly recommended to provide conversion functions to cover
// all the user defined exceptions as it also allows TryCatch to detect
// conflict between exception IDs.
void TryCatchAddExcToStrFun(
  // The conversion function to add
  char const* (fun(int))) {

  // If the buffer of pointer to conversion function is full, raise
  // the exception TooManyExcToStrFun
  if (nbUserDefinedExcToStr >= nbMaxUserDefinedExcToStr)
    Raise(TryCatchExc_TooManyExcToStrFun);

  // Loop on the pointer to conversion functions
  for (
    int iFun = 0;
    iFun < nbUserDefinedExcToStr;
    ++iFun) {

    // If this is the function in argument
    if (userDefinedExcToStr[iFun] == fun) {

      // Avoid adding it several times
      return;

    }

  }

  // Add the pointer
  userDefinedExcToStr[nbUserDefinedExcToStr] = fun;

  // Increment the number of conversion functions
  ++nbUserDefinedExcToStr;

}

// Set the stream on which to print exception raising, set it to NULL to
// turn off messages
void TryCatchSetRaiseStream(
  // The stream to used
  FILE* const stream) {

  // Set the stream
  streamRaise = stream;

}

// ------------------ trycatch.c ------------------
