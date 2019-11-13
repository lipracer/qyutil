#ifndef __GLOBAL_FUNC_H__
#define __GLOBAL_FUNC_H__

#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _inq_node
{
    void * data;
    _inq_node * next;
    
} inq_node;

typedef inq_node * inq_list;

void list_init(inq_list * list, void * data);
void list_insert_node(inq_node *node, void * data);
void list_rem_node(inq_node * node);
void list_destroy(inq_list list);

void inq_list_test();

typedef struct _inq_str_pair
{
    char * key;
    char * value;
} inq_str_pair;

void init_qyutil(char * workpath, char * pingback_url, int params_count, ...);

#ifdef __cplusplus
}
#endif

namespace qyutil
{
using namespace std;
void split_string(const string& str, const string target, vector<string>& result);

}


#endif
