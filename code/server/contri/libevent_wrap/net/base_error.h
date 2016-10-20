/**
 * @filedesc: 
 * 
 * @author: 
 *  bbwang
 * @date: 
 *  2016/8/4 20:02:59
 * @modify:
 *
**/
#ifndef _BASE_ERROR_H_
#define _BASE_ERROR_H_


enum err_index 
{
    ERR_OK = 0,
    ERR_TASKID_MISTAKE,
    ERR_SESSION_NOT_EXIST,
    ERR_TASK_NOT_EXIST,
    ERR_TRANSLATER_NOT_EXIST,
    ERR_CAN_NOT_TRANSLATER_MSG,
	ERR_NO_PROCESSOR,
};



struct BaseError
{
    int err_code;
    char *err_msg;
    char *detail_desc;

    char *print()
    {
        static char print_info_cstr[4096];
        int n = snprintf(print_info_cstr, sizeof(print_info_cstr), "[%d, %s, %s]", err_code, err_msg, detail_desc);
        if (n <= 0) {
            return "fatal error!";
        }
        return print_info_cstr;
    }
};


static const BaseError[] const err_list = 
{
    {ERR_OK,                   "OK",                           "all is ok"},
    {ERR_TASKID_MISTAKE,       "TASK ID ERROR",                "task id should be "}



};

#endif // _BASE_ERROR_H_

