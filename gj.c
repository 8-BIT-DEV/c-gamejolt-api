#include <gj.h>

#include <openssl/sha.h>
#include <string.h>
#include <http-get.h>

#include <stdarg.h>

char* id;
char* pkey;

char username[96];
char token[96];

int errcode = 0;
char errorinfo[128];

char* gj_opstrings[] = {"append", "prepend", "add", "subtract", "multiply", "divide"};

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
    strcat(url, "format=keypair&");
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

gj_keydata_t p_checkgood(http_get_response_t* got)
{
    gj_keydata_t data = p_parse_reponse(got->data);
    int sid = p_data_get_index(data, "success");

    if(sid == -1) {printf("BAD REQUEST : %s\n", got->data); data.status = 0;}
    else if(sid != -1)
    {
        if(strcmp(data.values[sid], "false") == 0)
        {
            data.status = 0;
            printf("ERROR IN API\n");
        }
    }

    errcode = data.status;
    return data;
}

gj_keydata_t p_parse_reponse(char* repo)
{
    gj_keydata_t data;
    data.count = 0;
    data.status = 1;
    int state = 0;
    int off = 0;

    memset(data.names, 0, 64 * 64);
    memset(data.values, 0, 64 * 64);

    for(int i = 0; i < strlen(repo); i++)
    {
        if(state == 0)
        {
            if(repo[i] != ':')
            {
                data.names[data.count][off] = repo[i];
                off++;
            } else if(repo[i] == ':')
            {
                data.names[data.count][off] = '\0';
                state = 1;
                off = 0;
            }
        } else if(state == 1)
        {
            if(repo[i] != '\n' && repo[i] != '\0')
            {
                if(repo[i] != '\"')
                {
                    data.values[data.count][off] = repo[i];
                    off++;
                }
            } else if(repo[i] == '\n' || repo[i] == '\0')
            {
                data.values[data.count][off-1] = '\0';
                state = 2;
                off = 0;
            }
        }

        if(state == 2)
        {
            off = 0;
            state = 0;
            data.count++;
        }
    }

    data.count++;
    return data;
}

int p_data_get_index(gj_keydata_t data, const char* name)
{
    for(int i = 0; i < data.count; i++)
    {
        if(strcmp(data.names[i], name) == 0) return i;
    }
    return -1;
}

void gj_api()
{
    // added this and never used it :)
}

int gj_error()
{
    int perrcode = errcode;
    errcode = 0;
    return perrcode;
}

int gj_login(const char* ply_name, const char* ply_token)
{
    strcpy(username, ply_name);
    strcpy(token, ply_token);
    char* url = p_makeurl("users/auth", 3, "game_id", id, "username", username, "user_token", token);
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code.status;
}

gj_userdata_t gj_get_user(const char* username, int uid)
{
    char uids[32];
    sprintf(uids, "%d", uid);
    char* url = p_makeurl("users/fetch", 3, "game_id", id, "username", username, "user_id", uids);
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);

    gj_userdata_t user;
    user.status[0] = NULL;

    if(code.status == 0) return user;

    user.id = atoi(code.values[1]);
    strcpy(user.type, code.values[2]);
    strcpy(user.username, code.values[3]);
    strcpy(user.avatar, code.values[4]);
    strcpy(user.signed_up, code.values[5]);
    user.signed_up_timestamp = atoi(code.values[6]);
    strcpy(user.last_logged_in, code.values[7]);
    user.last_logged_in_timestamp = atoi(code.values[8]);
    strcpy(user.status, code.values[9]);
    strcpy(user.dev_name, code.values[10]);
    strcpy(user.dev_website, code.values[11]);
    strcpy(user.dev_desc, code.values[12]);

    return user;
}

void gj_free_user(gj_userdata_t user)
{
    free(user.type);
    free(user.username);
    free(user.avatar);
    free(user.signed_up);
    free(user.last_logged_in);
    free(user.dev_name);
    free(user.dev_website);
    free(user.dev_desc);
}

gj_trophy_t gj_get_trophy(int tid)
{
    char stid[32];
    sprintf(stid, "%d", tid);
    char* url = p_makeurl("trophies", 4, "game_id", id, "username", username, "user_token", token, "trophy_id", stid);
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);

    gj_trophy_t tdata;

    if(code.status == 0) return tdata;

    tdata.id = atoi(code.values[1]);
    strcpy(tdata.title, code.values[2]);
    strcpy(tdata.diff, code.values[3]);
    strcpy(tdata.desc, code.values[4]);
    strcpy(tdata.image, code.values[5]);
    strcpy(tdata.given, code.values[6]);

    return tdata;
}

int gj_give_trophy(int tid)
{
    char stid[32];
    sprintf(stid, "%d", tid);
    char* url = p_makeurl("trophies/add-achieved", 4, "game_id", id, "username", username, "user_token", token, "trophy_id", stid);
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code.status;
}

int gj_revoke_trophy(int tid)
{
    char stid[32];
    sprintf(stid, "%d", tid);
    char* url = p_makeurl("trophies/remove-achieved", 4, "game_id", id, "username", username, "user_token", token, "trophy_id", stid);
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code.status;
}

int gj_add_score(int tid, int score)
{
    char stid[32];
    char sscore[32];
    sprintf(stid, "%d", tid);
    sprintf(sscore, "%d", score);
    char* url = p_makeurl("scores/add", 6, "game_id", id, "username", username, "user_token", token, "score", sscore, "sort", sscore, "table_id", stid);
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code.status;
}

int gj_add_score_extra(int tid, int score, const char* data)
{
    char stid[32];
    char sscore[32];
    sprintf(stid, "%d", tid);
    sprintf(sscore, "%d", score);
    char* url = p_makeurl("scores/add", 7, "game_id", id, "username", username, "user_token", token, "score", sscore, "sort", sscore, "table_id", stid, "extra_data", data);
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code.status;
}

int gj_add_guest_score(const char* name, int tid, int score)
{
    char stid[32];
    char sscore[32];
    sprintf(stid, "%d", tid);
    sprintf(sscore, "%d", score);
    char* url = p_makeurl("scores/add", 5, "game_id", id, "guest", name, "score", sscore, "sort", sscore, "table_id", stid);
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code.status;
}

gj_rank_t gj_get_rank(int tid, int score)
{
    char stid[32];
    char sscore[32];
    sprintf(stid, "%d", tid);
    sprintf(sscore, "%d", score);
    char* url = p_makeurl("scores/get-rank", 3, "game_id", id, "table_id", stid, "sort", sscore);
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);


    gj_rank_t rank;
    if(code.status == 0) return rank;

    rank = atoi(code.values[1]);

    return rank;
}

int gj_open_session()
{
    char* url = p_makeurl("sessions/open", 3, "game_id", id, "username", username, "user_token", token);
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code.status;
}

int gj_close_session()
{
    char* url = p_makeurl("sessions/close", 3, "game_id", id, "username", username, "user_token", token);
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code.status;
}

int gj_ping_session()
{
    char* url = p_makeurl("sessions/ping", 3, "game_id", id, "username", username, "user_token", token, "status", "active");
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);
    return code.status;
}

int gj_check_session()
{
    return 1;
}

int gj_set_data(const char* k, gj_type_e type, ...)
{
    char datas[128];

    va_list args;
    va_start(args, type);

    int tint;
    bool tbool;
    double tfloat;

    switch(type)
    {
    case STRING:
        strcpy(datas, va_arg(args, char*));
        break;
    case INT:
        sprintf(datas, "%d", va_arg(args, int));
        break;
    case BOOL:
        strcpy(datas, (va_arg(args, int) == 1) ? "true" : "false");
        break;
    case FLOAT:
        sprintf(datas, "%f", va_arg(args, double));
        break;
    }

    char* url = p_makeurl("data-store/set", 3, "game_id", id, "key", k, "data", datas);
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);

    va_end(args);

    return code.status;
}

int gj_update_data(const char* k, gj_operation_e op, gj_type_e type, ...)
{
    char datas[128];

    va_list args;
    va_start(args, type);

    switch(type)
    {
    case STRING:
        strcpy(datas, va_arg(args, char*));
        break;
    case INT:
        sprintf(datas, "%d", va_arg(args, int));
        break;
    case BOOL:
        printf("BOOLEAN IS NOT A VALID UPDATE TYPE\n");
        return 0;
        break;
    case FLOAT:
        sprintf(datas, "%f", va_arg(args, double));
        break;
    }

    char* url = p_makeurl("data-store/update", 4, "game_id", id, "key", k, "operation", gj_opstrings[op], "value", datas);
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);

    va_end(args);

    return code.status;
}

int gj_remove_data(const char* k)
{
    char* url = p_makeurl("data-store/remove", 2, "game_id", id, "key", k);
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);

    return code.status;
}

gj_data_u gj_get_data(const char* k, gj_type_e type)
{
    char* url = p_makeurl("data-store/fetch", 2, "game_id", id, "key", k);
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);

    gj_data_u data;

    if(code.status == 0) return data;

    int iddata = p_data_get_index(code, "data");

    switch(type)
    {
    case STRING:
        data.s = malloc(strlen(code.values[iddata]));
        strcpy(data.s, code.values[iddata]);
        break;
    case INT:
        data.i = atoi(code.values[iddata]);
        break;
    case BOOL:
        data.b = (strcmp(code.values[iddata], "true") == 0) ? true : false;
        break;
    case FLOAT:
        data.f = atof(code.values[iddata]);
        break;
    }

    return data;
}

gj_time_t gj_get_time()
{
    char* url = p_makeurl("time", 1, "game_id", id);
    http_get_response_t* got = http_get(url);
    gj_keydata_t code = p_checkgood(got);
    http_get_free(got);
    free_url(url);

    gj_time_t timedata;

    if(!code.status) return timedata;

    strcpy(timedata.timezone, code.values[2]);

    timedata.timestamp = atoi(code.values[1]);
    timedata.year = atoi(code.values[3]);
    timedata.month = atoi(code.values[4]);
    timedata.day = atoi(code.values[5]);
    timedata.hour = atoi(code.values[6]);
    timedata.minute = atoi(code.values[7]);
    timedata.second = atoi(code.values[8]);

    return timedata;
}

void set_id(char* idy)
{
    id = idy;
}

void set_pkey(char* pkeyy)
{
    pkey = pkeyy;
}
