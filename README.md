# Gamejolt API library for C

# Depends On
* [JSMN](https://github.com/zserge/jsmn)
* [HTTP GET LIBRARY](https://github.com/clibs/http-get.c)
* [OPENSSL](https://github.com/openssl/openssl)

# Helpfull
* [API DOCS](https://gamejolt.com/game-api/doc/libraries)

# How to install / use in A project
* Download and put gj.h and gj.c in your project
* Download and put http-get.h and http-get.c in your project
* Download JSMN and put it in your project
* Link the crypto file from OPENSSL

# Usage Guide
```C
  #include <gj.h> // api header
  #include <stdio.h>
  #include <stdlib.h>
  
  int main()
  {
      int ret = gj_login("username", "token"); // to auth a user accoun
      if(!ret) return -1;
      ret = gj_give_trophie(12321); // gives the trophie of that id
      if(!ret) return -1;
      ret = gj_add_score(42321, 10000); // table id then score
      if(!ret) return -1;
      ret = gj_add_guest_score("cool_name", 342342, 900000); // guest name, table id, score
      // for more look at the api docs and header file for rest of functions
      if(!ret) return -1;
      return ret;
  }
```
