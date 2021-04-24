/*
    (c) 2021 8_BIT-DEV Under MIT License
    p_ : meaning private function though theres nothing stopping you from using p_ functions
    _e : for enums
    _u : for unions
    _t : for structs
*/

#ifndef GJAPI_H
#define GJAPI_H

#include <http-get.h>
#include <stdbool.h>

#define free_hash(hash) free(hash)
#define free_url(url) free(url)

enum gj_type {STRING, INT, BOOL, FLOAT};\
typedef enum gj_type gj_type_e;

union gj_data
{
    char* s;
    int i;
    bool b;
    float f;
};
typedef union gj_data gj_data_u;

void set_id(char* id);
void set_pkey(char* pkey);

// General functions
void gj_api();
int gj_login(const char* username, const char* token);

// trophies
int gj_give_trophie(int tid);
int gj_revoke_trophie(int tid);

// scores
int gj_add_score(int tid, int score);
int gj_add_guest_score(const char* name, int tid, int score);
// shouldnt need to implement more of the score functions
// theses should do

// sessions
int gj_open_session();
int gj_close_session();
int gj_ping_session();
int gj_check_session(); // not implemented yet

// data
// pass everything as a pointer to the set data function
int gj_set_data(const char* k, gj_type_e type, ...);
int gj_remove_data(const char* k);
gj_data_u gj_get_data(const char* k);
// i'll implement these later

// time
long gj_get_time();
// i'll implement this later

// private functions
char* p_gethash(char* in);
char* p_makeurl(char* endpoint, int argcount, ...);
int p_checkgood(http_get_response_t* got);

#endif // GJAPI_H
