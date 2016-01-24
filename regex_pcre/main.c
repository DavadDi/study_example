#include <pcre.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char ** argv)
{
    if (argc != 3)
    {
        printf("Usage: %s pattern text\n", argv[0]);
        return 1;
    }

    const char * pPattern = argv[1];
    const char * pText = argv[2];
    const char * pErrMsg = NULL;
    pcre * pPcre = NULL;
    int nOffset = -1;

    if (NULL == (pPcre = pcre_compile(pPattern, 0, &pErrMsg, &nOffset, NULL)))
    {
        printf("ErrMsg=%s, Offset=%d\n", pErrMsg, nOffset);
        return 1;
    }
    else
    {
        if (pcre_exec(pPcre, NULL, pText, strlen(pText), 0, 0, NULL, 0) < 0)
        {
            printf("%s doesn't match %s\n", pText, pPattern);
        }
        else
        {
            printf("%s matches %s\n", pText, pPattern);
        }
    }

}
