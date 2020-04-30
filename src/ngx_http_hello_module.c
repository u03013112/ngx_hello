#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

char *ngx_http_hello(ngx_conf_t *cf, ngx_command_t *cmd,void *conf);
static ngx_int_t ngx_http_hello_handler(ngx_http_request_t *r);
static ngx_int_t ngx_http_hello_init(ngx_conf_t *cf);

static ngx_command_t ngx_http_hello_commands[] = {

    { ngx_string("hello"),
      NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_http_hello,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,//这个参数暂时不理解，https://www.nginx.com/resources/wiki/extending/api/configuration/#c-ngx-command-t
      NULL },
    ngx_null_command
};

static ngx_http_module_t ngx_http_hello_module_ctx = {
    NULL,   /* preconfiguration */
    ngx_http_hello_init,   /* postconfiguration */

    NULL,   /* create main configuration */
    NULL,   /* init main configuration */

    NULL,   /* create server configuration */
    NULL,   /* merge server configuration */

    NULL,  /* create location configuration */
    NULL,   /* merge location configuration */
};

ngx_module_t ngx_http_hello_module = {
    NGX_MODULE_V1,
    &ngx_http_hello_module_ctx,   /* module context */
    ngx_http_hello_commands,      /* module directives */
    NGX_HTTP_MODULE,                             /* module type */
    NULL,                                        /* init master */
    NULL,                                        /* init module */
    NULL,   /* init process */
    NULL,                                        /* init thread */
    NULL,                                        /* exit thread */
    NULL,   /* exit process */
    NULL,                                        /* exit master */
    NGX_MODULE_V1_PADDING
};

char *ngx_http_hello(ngx_conf_t *cf, ngx_command_t *cmd,void *conf){
    ngx_http_core_loc_conf_t *clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_hello_handler;
    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_hello_handler(ngx_http_request_t *r){
    ngx_int_t rc = ngx_http_discard_request_body(r);
    if (rc!= NGX_OK) {
        return rc;
    }
    ngx_str_t type = ngx_string("text/plain");
    ngx_str_t response = ngx_string("hello world");
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = response.len;
    r->headers_out.content_type =type;

    rc = ngx_http_send_header(r);
    if (rc==NGX_ERROR || rc>NGX_OK || r->header_only) {
        return rc;
    }
    ngx_buf_t *b;
    b = ngx_create_temp_buf(r->pool,response.len);
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    ngx_memcpy(b->pos,response.data,response.len);
    b->last = b->pos + response.len;
    b->last_buf = 1;

    ngx_chain_t out;
    out.buf = b;
    out.next=NULL;

    return ngx_http_output_filter(r, &out);
}

static ngx_int_t ngx_http_hello_access_handler(ngx_http_request_t *r){
    ngx_log_error(NGX_LOG_ERR,r->connection->log,0,"hello access\n");
    return NGX_OK;
}

static ngx_int_t ngx_http_hello_init(ngx_conf_t *cf){
    ngx_http_handler_pt        *h;
    ngx_http_core_main_conf_t  *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_hello_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_ACCESS_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_hello_access_handler;

    return NGX_OK;
}