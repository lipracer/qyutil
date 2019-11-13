#include "global_func.h"
#include "Pingback.hpp"

void list_init(inq_list * list, void * data)
{
    *list = (inq_node*)malloc(sizeof(inq_node));
    (*list)->data = data;
    (*list)->next = nullptr;
}

void list_insert_node(inq_node *node, void * data)
{
    inq_node *new_node = (inq_node*)malloc(sizeof(inq_node));
    
    inq_node * next = node->next;
    node->next = new_node;
    new_node->next = next;
    
    new_node->data = data;
    new_node->next = nullptr;
}

void list_rem_node(inq_node * node)
{
    if(!node) return;
    inq_node * next = node->next;
    if(next)
    {
        node->data = next->data;
        node->next = next->next;
        free(next);
    }
}

void list_destroy(inq_list list)
{
    if(nullptr == list)
    {
        return;
    }
    inq_node * cur_node = nullptr;
    for(;;)
    {
        if(nullptr == list)
        {
            break;
        }
        cur_node = list;
        list = list->next;
        free(cur_node);
    }
}

void inq_list_test()
{
    inq_list list;
    int array[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    list_init(&list, array);
    
    inq_node * last_node = list;
    for (int i = 1; i < 10; i++)
    {
        list_insert_node(last_node, array + i);
        last_node = last_node->next;
    }
    list_destroy(list);
}

void init_qyutil(char * workpath, char * pingback_url, int params_count, ...)
{
    qyutil::list<qyutil::ss_pair> params;
    va_list list;
    va_start(list, params_count);
    for(int i = 0; i < params_count; ++i)
    {
        inq_str_pair pair = va_arg(list, inq_str_pair);
        params.push_back(qyutil::ss_pair(pair.key, pair.value));
    }
    va_end(list);
    
    qyutil::Pingback::shared().init_public_params(params);
}

namespace qyutil
{

void split_string(const string& str, const string target, vector<string>& result)
{
    result.clear();
    if(!str.length()) return;
    
    size_t pre_pos = 0;
    size_t cur_pos = string::npos;
    
    do
    {
        if(pre_pos >= str.length())
        {
            break;
        }
        cur_pos = str.find(target, pre_pos);
        if(cur_pos != string::npos)
        {
            result.push_back(str.substr(pre_pos, cur_pos - pre_pos));
            pre_pos = cur_pos + target.length();
        }
        else
        {
            if(pre_pos)
            {
                result.push_back(str.substr(pre_pos));
            }
            break;
        }

    }while(true);
}

}
