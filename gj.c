#include <gj.h>

#include <openssl/sha.h>
#include <string.h>
#include <http-get.h>
#include <jsmn.h>

#include <stdarg.h>

char* id;
char* pkey;

char username[96];
char token[96];

char* p_gethash(char* in)
{
    char finalhash[41];
    strcpy(finalhash, "");
    char holding[3];

    unsigned char digest[SHA_DIGEST_LENGTH];
    SHA1(in, strlen(in), digest);
    for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        sprintf(holding, "%02x", digest[i]);
        strcat(finalhash, holding);
    }
    return strdup(finalhash);
}

char* p_makeurl(char* endpoint, int argcount, ...)
{
    va_list args;
    va_start(args, argcount);
    char url[2048];
    strcpy(url, "https://api.gamejolt.com/api/game/v1_2/");
    strcat(url, endpoint);
    strcat(url, "/?");
    for(int i = 0; i < argcount; i++)
    {
        char* name = va_arg(args, char*);
        char* value = va_arg(args, char*);
        if(i != 0) strcat(url, "&");
        strcat(url, name);
        strcat(url, "=");
        strcat(url, value);
    }
    va_end(args);
    char* signing = malloc(strlen(url) + 256);
    strcpy(signing, url);
    strcat(signing, pkey);
    char* sig = p_gethash(signing);
    strcat(url, "&signature=");
    strcat(url, sig);
    free_hash(sig);
    free(signing);
    return strdup(url);
}

int p_checkgood(http_get_response_t* got)
{
    jsmn_parser p;
    jsmntok_t tokens[128];
    jsmn_init(&p);
    jsmn_parse(&p, got->data, got->size, tokens, 128);
    //printf("TOKN:%u\n", p.toknext);
    //printf("DATA:%s\n", got->data);
    for(int i = 0; i < p.toknext; i++)
    {
        if(strncmp(&got->data[tokens[i].start], "success", tokens[i].end - tokens[i].start) == 0)
        {
            if(strncmp(&got->data[tokens[i+1].start], "true", tokens[i+1].end - tokens[i+1].start) == 0) return 1;
            else
            {
                printf("INFO:%s\n", got->data);
                return 0;
            }
        }
    }
    return 0;
}

void gj_api()
{
    // added this and never used it :)
}

int gj_login(const char* ply_name, const char* ply_token)
{
    strcpy(username, ply_name);
    strcpy(token, ply_token);
    char* url = p_makeurl("users/auth", 3, "game_id", id, "username", username, "user_token", token);
    http_get_response_t* got = http_get(url);
    int code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code;
}

int gj_give_trophie(int tid)
{
    char stid[32];
    sprintf(stid, "%d", tid);
    char* url = p_makeurl("trophies/add-achieved", 4, "game_id", id, "username", username, "user_token", token, "trophy_id", stid);
    http_get_response_t* got = http_get(url);
    int code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code;
}

int gj_revoke_trophie(int tid)
{
    char stid[32];
    sprintf(stid, "%d", tid);
    char* url = p_makeurl("trophies/remove-achieved", 4, "game_id", id, "username", username, "user_token", token, "trophy_id", stid);
    http_get_response_t* got = http_get(url);
    int code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code;
}

int gj_add_score(int tid, int score)
{
    char stid[32];
    char sscore[32];
    sprintf(stid, "%d", tid);
    sprintf(sscore, "%d", score);
    char* url = p_makeurl("scores/add", 6, "game_id", id, "username", username, "user_token", token, "score", sscore, "sort", sscore, "table_id", stid);
    http_get_response_t* got = http_get(url);
    int code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code;
}

int gj_add_guest_score(const char* name, int tid, int score)
{
    char stid[32];
    char sscore[32];
    sprintf(stid, "%d", tid);
    sprintf(sscore, "%d", score);
    char* url = p_makeurl("scores/add", 5, "game_id", id, "guest", name, "score", sscore, "sort", sscore, "table_id", stid);
    http_get_response_t* got = http_get(url);
    int code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code;
}

int gj_open_session()
{
    char* url = p_makeurl("sessions/open", 3, "game_id", id, "username", username, "user_token", token);
    http_get_response_t* got = http_get(url);
    int code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code;
}

int gj_close_session()
{
    char* url = p_makeurl("sessions/close", 3, "game_id", id, "username", username, "user_token", token);
    http_get_response_t* got = http_get(url);
    int code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code;
}

int gj_ping_session()
{
    char* url = p_makeurl("sessions/ping", 3, "game_id", id, "username", username, "user_token", token, "status", "active");
    http_get_response_t* got = http_get(url);
    int code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code;
}

int gj_check_session()
{
}

void set_id(char* idy)
{
    id = idy;
}

void set_pkey(char* pkeyy)
{
    pkey = pkeyy;
}
