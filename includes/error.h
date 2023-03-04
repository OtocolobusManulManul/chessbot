// error code definitions
#pragma once

// ALPHA/BETA SEARCH ERRORS
#define STATUS_RERUN_SEARCH -1
#define STATUS_EXPAND_SEARCH -2
#define STATUS_MINIMIZE_SEARCH -3

int sMsg = 0; // if search returns error corrsponding value will be set here

// CACHE ERRORS 

// the likelihood of an error hit
// does not concern me... I will
// take my chances with 64 bit
// integers

#define CACHE_OK NULL          // context wants to modify already existing cache (ie further evaluate)
#define CACHE_MISSMATCH -1
#define CACHE_NO_ACCESS -2
#define CACHE_BAD_HANDSHAKE -3 
#define CACHE_NOT_FOUND -4
#define CACHE_CLAIMED -5       // context is evaluating an entry
                               // that does not yet exist

#define CACHE_RELOC -6         // cache being relocated from prior
                               // hit

#define CACHE_BUSY -7

int cMsg = 0; // displays the most recent critical error
              // access faults are not a "critical" error
              // since they are meant to be happen.

              // CACHE_BUSY and CACHE_CLAIMED