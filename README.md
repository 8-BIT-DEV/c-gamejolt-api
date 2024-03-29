# Gamejolt API library for C

# Depends On
* [HTTP GET LIBRARY](https://github.com/clibs/http-get.c)
* [OPENSSL](https://github.com/openssl/openssl)

# Helpful
* [API DOCS](https://gamejolt.com/game-api/doc/libraries)

# How to install / use in A project
* Download and put gj.h and gj.c in your project
* Download and put http-get.h and http-get.c in your project
* Link the crypto file from OPENSSL

# Usage Guide
```C
  #include <gj.h> // api header
  #include <stdio.h>
  #include <stdlib.h>
  
  int main()
  {
      set_id("id here");
      set_pkey("private key here");
      
      // this checks if any of the 4 following api calls fails
      int check = 4;
      
      gj_login("username", "token"); // to auth a user account
      check -= gj_error();
      gj_give_trophie(12321); // gives the trophie of that id
      check -= gj_error();
      gj_add_score(42321, 10000); // table id then score
      check -= gj_error();
      gj_add_guest_score("cool_name", 342342, 900000); // guest name, table id, score
      check -= gj_error();
      // for more look at the api docs and header file for rest of functions
      if(check != 0) return check;
      return 0;
  }
```
