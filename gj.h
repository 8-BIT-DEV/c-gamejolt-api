/*
    2021 8_BIT-DEV Under MIT License
    p_ : meaning private function though theres nothing stopping you from using p_ functions
    _e : for enums
    _u : for unions
    _t : for structs or typedefs
*/

#ifndef GJAPI_H
#define GJAPI_H

#include <http-get.h>
#include <stdbool.h>

#define free_hash(hash) free(hash)
#define free_url(url) free(url)

// In the future I plan to add to more extras
#if defined(GJ_NO_EXTRAS) && defined(GJ_MORE_EXTRAS)
#error CONFLICTING CONFIGURATION
#endif // defined

enum gj_type {STRING, INT, BOOL, FLOAT};\
typedef enum gj_type gj_type_e;

enum gj_operation {APPEND, PREPEND, ADD, SUBTRACT, MULTIPLY, DIVIDE};\
typedef enum gj_operation gj_operation_e;

union gj_data
{
    char* s;
    int i;
    bool b;
    float f;
};
typedef union gj_data gj_data_u;

struct gj_keydata
{
    char names[64][64];
    char values[64][64];
    int count;
    int status;
};
typedef struct gj_keydata gj_keydata_t;

struct gj_userdata
{
    int id;
    char type[12];
    char username[64];
    char avatar[96];
    char signed_up[12];
    int signed_up_timestamp;
    char last_logged_in[12];
    int last_logged_in_timestamp;
    char status[16];
    char dev_name[64];
    char dev_website[96];
    char dev_desc[184];
};
typedef struct gj_userdata gj_userdata_t;

struct gj_time
{
    int timestamp;
    char timezone[32];
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
};
typedef struct gj_time gj_time_t;

struct gj_trophie
{
    int id;
    char title[32];
    char desc[128];
    char diff[12];
    char image[96];
    char given[24];
};
typedef struct gj_trophie gj_trophy_t;

typedef int gj_rank_t;

void set_id(char* id);
void set_pkey(char* pkey);

// General functions
void gj_api();
int gj_error();
int gj_login(const char* username, const char* token);
gj_userdata_t gj_get_user(const char* username, int uid);
void gj_free_user(gj_userdata_t user);

// trophies
gj_trophy_t gj_get_trophy(int tid);
int gj_give_trophy(int tid);
int gj_revoke_trophy(int tid);

// scores
int gj_add_score(int tid, int score);
int gj_add_score_extra(int tid, int score, const char* data);
int gj_add_guest_score(const char* name, int tid, int score);
gj_rank_t gj_get_rank(int tid, int score);
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
int gj_update_data(const char* k, gj_operation_e op, gj_type_e type, ...);
int gj_remove_data(const char* k);
gj_data_u gj_get_data(const char* k, gj_type_e type);
// i'll implement these later

// time
gj_time_t gj_get_time();
// i'll implement this later

// private functions
char* p_gethash(char* in);
char* p_makeurl(char* endpoint, int argcount, ...);
gj_keydata_t p_checkgood(http_get_response_t* got);

gj_keydata_t p_parse_reponse(char* repo);
int p_data_get_index(gj_keydata_t data, const char* name);

#endif // GJAPI_H
