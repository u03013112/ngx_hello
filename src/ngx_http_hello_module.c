// #include <nginx.h>
// #include <ngx_config.h>
// #include <ngx_core.h>
// #include <ngx_http.h>

// char *ngx_http_hello(ngx_conf_t *cf, ngx_command_t *cmd,void *conf);
// static ngx_int_t ngx_http_hello_handler(ngx_http_request_t *r);

// static ngx_command_t ngx_http_hello_commands[] = {

//     { ngx_string("hello"),
//       NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
//       ngx_http_hello,
//       NGX_HTTP_LOC_CONF_OFFSET,
//       0,//这个参数暂时不理解，p85
//       NULL },
//     ngx_null_command
// };

// static ngx_http_module_t ngx_http_hello_module_ctx = {
//     NULL,   /* preconfiguration */
//     NULL,   /* postconfiguration */

//     NULL,   /* create main configuration */
//     NULL,   /* init main configuration */

//     NULL,   /* create server configuration */
//     NULL,   /* merge server configuration */

//     NULL,  /* create location configuration */
//     NULL,   /* merge location configuration */
// };

// ngx_module_t ngx_http_hello_module = {
//     NGX_MODULE_V1,
//     &ngx_http_hello_module_ctx,   /* module context */
//     ngx_http_hello_commands,      /* module directives */
//     NGX_HTTP_MODULE,                             /* module type */
//     NULL,                                        /* init master */
//     NULL,                                        /* init module */
//     NULL,   /* init process */
//     NULL,                                        /* init thread */
//     NULL,                                        /* exit thread */
//     NULL,   /* exit process */
//     NULL,                                        /* exit master */
//     NGX_MODULE_V1_PADDING
// };

// char *ngx_http_hello(ngx_conf_t *cf, ngx_command_t *cmd,void *conf){
//     ngx_http_core_loc_conf_t *clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
//     clcf->handler = ngx_http_hello_handler;
//     return NGX_CONF_OK;
// }

// static ngx_int_t ngx_http_hello_handler(ngx_http_request_t *r){
//     ngx_int_t rc = ngx_http_discard_request_body(r);
//     if (rc!= NGX_OK) {
//         return rc;
//     }
//     ngx_str_t type = ngx_string("text/plain");
//     ngx_str_t response = ngx_string("hello world");
//     r->headers_out.status = NGX_HTTP_OK;
//     r->headers_out.content_length_n = response.len;
//     r->headers_out.content_type =type;

//     rc = ngx_http_send_header(r);
//     if (rc==NGX_ERROR || rc>NGX_OK || r->header_only) {
//         return rc;
//     }
//     ngx_buf_t *b;
//     b = ngx_create_temp_buf(r->pool,response.len);
//     if (b == NULL) {
//         return NGX_HTTP_INTERNAL_SERVER_ERROR;
//     }
//     ngx_memcpy(b->pos,response.data,response.len);
//     b->last = b->pos + response.len;
//     b->last_buf = 1;

//     ngx_chain_t out;
//     out.buf = b;
//     out.next=NULL;

//     return ngx_http_output_filter(r, &out);
// }

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
 
 
typedef struct
{
        ngx_str_t hello_string;
        ngx_int_t hello_counter;
}ngx_http_hello_loc_conf_t;
 
static ngx_int_t ngx_http_hello_init(ngx_conf_t *cf);
 
static void *ngx_http_hello_create_loc_conf(ngx_conf_t *cf);
 
static char *ngx_http_hello_string(ngx_conf_t *cf, ngx_command_t *cmd,
        void *conf);
static char *ngx_http_hello_counter(ngx_conf_t *cf, ngx_command_t *cmd,
        void *conf);
 
//模块配置指令
static ngx_command_t ngx_http_hello_commands[] = {
   {
                ngx_string("hello_string"),
                NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS|NGX_CONF_TAKE1,
                ngx_http_hello_string,
                NGX_HTTP_LOC_CONF_OFFSET,
                offsetof(ngx_http_hello_loc_conf_t, hello_string),
                NULL },
 
        {
                ngx_string("hello_counter"),
                NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
                ngx_http_hello_counter,
                NGX_HTTP_LOC_CONF_OFFSET,
                offsetof(ngx_http_hello_loc_conf_t, hello_counter),
                NULL },
 
        ngx_null_command
};
 
 
/*
static u_char ngx_hello_default_string[] = "Default String: Hello, world!";
*/
static int ngx_hello_visited_times = 0;
 
//模块上下文结构
static ngx_http_module_t ngx_http_hello_module_ctx = {
        NULL,                          /* preconfiguration */
        ngx_http_hello_init,           /* postconfiguration *///handler挂载
 
        NULL,                          /* create main configuration */
        NULL,                          /* init main configuration */
 
        NULL,                          /* create server configuration */
        NULL,                          /* merge server configuration */
 
        ngx_http_hello_create_loc_conf, /* create location configuration */
        NULL                            /* merge location configuration */
};
 
//模块定义
ngx_module_t ngx_http_hello_module = {
        NGX_MODULE_V1,
        &ngx_http_hello_module_ctx,    /* module context */
        ngx_http_hello_commands,       /* module directives */
        NGX_HTTP_MODULE,               /* module type */
        NULL,                          /* init master */
        NULL,                          /* init module */
        NULL,                          /* init process */
        NULL,                          /* init thread */
        NULL,                          /* exit thread */
        NULL,                          /* exit process */
        NULL,                          /* exit master */
        NGX_MODULE_V1_PADDING
};
 
//handler模块处理函数
static ngx_int_t
ngx_http_hello_handler(ngx_http_request_t *r){
        ngx_int_t    rc;
 
        //声明缓冲区 buffer 和 chain link：
        ngx_buf_t   *b;
        ngx_chain_t  out;
 
        ngx_http_hello_loc_conf_t* my_conf;
        u_char ngx_hello_string[1024] = {0};
        ngx_uint_t content_length = 0;
 
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "ngx_http_hello_handler is called!");
 
        my_conf = ngx_http_get_module_loc_conf(r, ngx_http_hello_module);//获取 location 配置,指向调用 ngx_http_get_module_loc_conf 函数即可，该函数传入的参数是 request 结构和自定义的 module 模块
        if (my_conf->hello_string.len == 0 )
        {
                ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "hello_string is empty!");
                return NGX_DECLINED;
        }
 
 
        if (my_conf->hello_counter == NGX_CONF_UNSET
                || my_conf->hello_counter == 0)
        {
                ngx_sprintf(ngx_hello_string, "%s", my_conf->hello_string.data);
        }
        else
        {
                ngx_sprintf(ngx_hello_string, "%s Visited Times:%d", my_conf->hello_string.data,
                        ++ngx_hello_visited_times);
        }
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "hello_string:%s", ngx_hello_string);
        content_length = ngx_strlen(ngx_hello_string);
 
        /* we response to 'GET' and 'HEAD' requests only */
        if (!(r->method & (NGX_HTTP_GET|NGX_HTTP_HEAD))) {
                return NGX_HTTP_NOT_ALLOWED;
        }
 
        /* discard request body, since we don't need it here */
        rc = ngx_http_discard_request_body(r);
 
        if (rc != NGX_OK) {
                return rc;
        }
 
        /* set the 'Content-type' header */
        /*
         *r->headers_out.content_type.len = sizeof("text/html") - 1;
         *r->headers_out.content_type.data = (u_char *)"text/html";
         */
        ngx_str_set(&r->headers_out.content_type, "text/html");
 
 
        /* send the header only, if the request type is http 'HEAD' 发送响应的header头部*/
        if (r->method == NGX_HTTP_HEAD) {
                r->headers_out.status = NGX_HTTP_OK;
                r->headers_out.content_length_n = content_length;
 
                return ngx_http_send_header(r);
        }
        //发送响应的body包体
        /* allocate a buffer for your response body 分配缓冲区 buffer，使响应数据指向它*/
        b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
        if (b == NULL) {
                return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }
 
        /* attach this buffer to the buffer chain 模块挂载到 chain link 上*/
        out.buf = b;
        out.next = NULL;
 
        /* adjust the pointers of the buffer */
        b->pos = ngx_hello_string;
        b->last = ngx_hello_string + content_length;
        b->memory = 1;    /* this buffer is in memory */
        b->last_buf = 1;  /* this is the last buffer in the buffer chain */
 
        /* set the status line */
        r->headers_out.status = NGX_HTTP_OK;
        r->headers_out.content_length_n = content_length;
 
        /* send the headers of your response */
        rc = ngx_http_send_header(r);
 
        if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
                return rc;
        }
 
        /* send the buffer chain of your response发送包体 */
        return ngx_http_output_filter(r, &out);
}
 
static void *ngx_http_hello_create_loc_conf(ngx_conf_t *cf){
        ngx_http_hello_loc_conf_t* local_conf = NULL;
        local_conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_hello_loc_conf_t));
        if (local_conf == NULL){
                return NULL;
        }
 
        ngx_str_null(&local_conf->hello_string);
        local_conf->hello_counter = NGX_CONF_UNSET;
 
        return local_conf;
}
 
static char *ngx_http_hello_string(ngx_conf_t *cf, ngx_command_t *cmd, void *conf){
 
        ngx_http_hello_loc_conf_t* local_conf;
 
 
        local_conf = conf;
        char* rv = ngx_conf_set_str_slot(cf, cmd, conf);//读取字符串类型的参数
 
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "hello_string:%s", local_conf->hello_string.data);//打印出错信息
 
        return rv;
}
 
static char *ngx_http_hello_counter(ngx_conf_t *cf, ngx_command_t *cmd,void *conf){
        ngx_http_hello_loc_conf_t* local_conf;
 
        local_conf = conf;
 
        char* rv = NULL;
 
        rv = ngx_conf_set_flag_slot(cf, cmd, conf);
 
 
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "hello_counter:%d", local_conf->hello_counter);
        return rv;
}
 
//handler挂载函数
static ngx_int_t ngx_http_hello_init(ngx_conf_t *cf){
        ngx_http_handler_pt        *h;
        ngx_http_core_main_conf_t  *cmcf;
 
        cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);
 
        h = ngx_array_push(&cmcf->phases[NGX_HTTP_CONTENT_PHASE].handlers);
        if (h == NULL) {
                return NGX_ERROR;
        }
 
        *h = ngx_http_hello_handler;
 
        return NGX_OK;
}