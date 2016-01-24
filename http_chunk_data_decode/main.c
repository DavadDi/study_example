#include "multipart_parser.h"
#include "post_data.h"
#include <stdlib.h>
#include <stdio.h>

#include <string.h>

void print_buf(const char *buf, int len)
{
    fprintf(stderr, "[len %d] ", len);
    int i = 0;
    for (i = 0; i < len; i++){
        putchar(buf[i]);
    }

    putchar('\n');
}

static int multipart_body_on_header_field(multipart_parser* p, const char *at, size_t length)
{
    fprintf(stderr, "multipart_body_on_header_field:");
    print_buf(at, length);
    return 0;
}

static int multipart_body_on_header_value(multipart_parser* p, const char *at, size_t length)
{

    fprintf(stderr, "multipart_body_on_header_value:");
    print_buf(at, length);
    return 0;

}

static int multipart_body_on_data(multipart_parser* p, const char *at, size_t length)
{

    fprintf(stderr, "multipart_body_on_data:");
    print_buf(at, length);
    return 0;

}
static int multipart_body_on_header_complete(multipart_parser* p)
{

    fprintf(stderr, "multipart_body_on_header_complete\n\n");
    return 0;

}
static int multipart_body_on_data_end(multipart_parser* p)
{

    fprintf(stderr, "multipart_body_on_data_end\n\n");
    return 0;

}
static int multipart_body_end(multipart_parser* p)
{

    fprintf(stderr, "multipart_body_end\n\n");
    return 0;

}

static const multipart_parser_settings mt_parser_settings =
{
    multipart_body_on_header_field,
    multipart_body_on_header_value,
    multipart_body_on_data,
    NULL,
    multipart_body_on_header_complete,
    multipart_body_on_data_end,
    multipart_body_end

};

int main()
{
    int i = 0;
    const char *boundary = "------WebKitFormBoundaryakSAdU8jNn0LWuxK";
    multipart_parser *mtp = multipart_parser_init(boundary, &mt_parser_settings);

    i = multipart_parser_execute(mtp, post_data, sizeof(post_data));
    fprintf(stderr, "res %d , boundary len %lu\n", i, strlen(boundary));
    return 0;
}
