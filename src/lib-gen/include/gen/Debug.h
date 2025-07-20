//----------------------------------------------------------------
//
// File: Debug.h
//
//----------------------------------------------------------------

#pragma once

#ifdef DEBUG
#    define D(x) x
#    include <iostream>
#    define DOUT(x) std::cout << x << "\n";
#    define DERR(x) std::cerr << x << "\n";
#else
#    define D(x)    // empty
#    define DOUT(x) // empty
#    define DERR(x) // empty
#endif

// Example for DOUT()
//
//     DOUT("Value of pi: " << pi)
//
// For multi-line handling with the D() macro, do this
// 
// D({
//     std::cout << "Debug mode enabled\n";
//     someDebugOnlyFunction();
// });
//

//----------------------------------------------------------------
    
