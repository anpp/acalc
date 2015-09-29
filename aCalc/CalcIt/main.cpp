#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "calcparser.h"
#include "def.h"

#ifdef _UNIX
#include </usr/local/include/libintl.h>
#else
#include "../../gettext/gettext-runtime/include/libintl.h"
#endif



#define PACKAGE "CalcIt"

int main(int argc, char *argv[])
{
    String s;
    CalcParser cp;
    int c;

    setlocale(LC_ALL, "");

#ifndef _QT4
    #ifndef _UNIX
        bind_textdomain_codeset(PACKAGE, "866");
    #endif
        bindtextdomain(PACKAGE, "./locale");
        textdomain(PACKAGE);
#endif

    if (argc > 1)
    {
        s = argv[1];

        if(s == "-i")
        {
            s = "";
            while((c = getc(stdin)) != EOF)
            {
                if(s == "\n")
                    s = "";
                s += c;

                if((c == '\n') && (s != "\n"))
                {
                    cp.SetParams(&s);
                    cp.Run();
                    printf(cp.GetExpression("=").toAscii());
                    s = "";
                    if(cp.HasErr())
                    {
                        printf("\n");
                        printf(cp.listErrors().toAscii());
                    }
                    else
                    {
                        if(cp.TypeRes() == FLOAT)
                            printf("%g\n", cp.GetResult());
                        else
                        {
                            printf(cp.GetResultStr().toAscii());
                            printf("\n");
                        }
                    }

                }
            }
        }
        if(s == "-t")
        {
            s = "";
            while((c = getc(stdin)) != EOF)
            {
                if(s == "\n")
                    s = "";
                s += c;

                if((c == '\n'))
                {
                    cp.AddToken(&s);
                    s = "";
                    printf(cp.GetExpression().toAscii());
                    //printf(cp.LastTokenValue().toAscii());
                    printf("\n");
                    if(cp.LastTokenValue() == "=")
                    {
                        cp.ToBack();
                        cp.Run();
                        printf(cp.GetExpression("=").toAscii());
                        s = "";
                        if(cp.HasErr())
                        {
                            printf("\n");
                            printf(cp.listErrors().toAscii());
                        }
                        else
                            printf("%g\n", cp.GetResult());
                    }

                }
            }
        }
        else
        {
            cp.SetParams(&s);
            if(!cp.Run())
                printf(cp.listErrors().toAscii());
            else
                printf("%f", cp.GetResult());
        }
    }

    return 0;
}
