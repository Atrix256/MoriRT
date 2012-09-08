/*======================================================

FloatingPointExceptions.h

Classes for turning on and off floating point exceptions

Taken from an article by Bruce Dawson

http://randomascii.wordpress.com/2012/04/21/exceptional-floating-point/

======================================================*/

#include <Windows.h>

// Declare an object of this type in a scope in order to suppress
// all floating-point exceptions temporarily. The old exception
// state will be reset at the end.
// From http://randomascii.wordpress.com/2012/04/21/exceptional-floating-point/
class FPExceptionDisabler
{
public:
    FPExceptionDisabler()
    {
        // Retrieve the current state of the exception flags. This
        // must be done before changing them. _MCW_EM is a bit
        // mask representing all available exception masks.
        _controlfp_s(&mOldValues, _MCW_EM, _MCW_EM);
        // Set all of the exception flags, which suppresses FP
        // exceptions on the x87 and SSE units.
        _controlfp_s(0, _MCW_EM, _MCW_EM);
    }
    ~FPExceptionDisabler()
    {
        // Clear any pending FP exceptions. This must be done
        // prior to enabling FP exceptions since otherwise there
        // may be a 'deferred crash' as soon the exceptions are
        // enabled.
        _clearfp();
 
        // Reset (possibly enabling) the exception status.
        _controlfp_s(0, mOldValues, _MCW_EM);
    }
 
private:
    unsigned int mOldValues;
 
    // Make the copy constructor and assignment operator private
    // and unimplemented to prohibit copying.
    FPExceptionDisabler(const FPExceptionDisabler&);
    FPExceptionDisabler& operator=(const FPExceptionDisabler&);
};
 
// Declare an object of this type in a scope in order to enable a
// specified set of floating-point exceptions temporarily. The old
// exception state will be reset at the end.
// This class can be nested.
// From http://randomascii.wordpress.com/2012/04/21/exceptional-floating-point/
class FPExceptionEnabler
{
public:
    // Overflow, divide-by-zero, and invalid-operation are the FP
    // exceptions most frequently associated with bugs.
    FPExceptionEnabler(unsigned int enableBits = _EM_OVERFLOW |
                _EM_ZERODIVIDE | _EM_INVALID)
    {
        // Retrieve the current state of the exception flags. This
        // must be done before changing them. _MCW_EM is a bit
        // mask representing all available exception masks.
        _controlfp_s(&mOldValues, _MCW_EM, _MCW_EM);
 
        // Make sure no non-exception flags have been specified,
        // to avoid accidental changing of rounding modes, etc.
        enableBits &= _MCW_EM;
 
        // Clear any pending FP exceptions. This must be done
        // prior to enabling FP exceptions since otherwise there
        // may be a 'deferred crash' as soon the exceptions are
        // enabled.
        _clearfp();
 
        // Zero out the specified bits, leaving other bits alone.
        _controlfp_s(0, ~enableBits, enableBits);
    }
    ~FPExceptionEnabler()
    {
        // Reset the exception state.
        _controlfp_s(0, mOldValues, _MCW_EM);
    }
 
private:
    unsigned int mOldValues;
 
    // Make the copy constructor and assignment operator private
    // and unimplemented to prohibit copying.
    FPExceptionEnabler(const FPExceptionEnabler&);
    FPExceptionEnabler& operator=(const FPExceptionEnabler&);
};
