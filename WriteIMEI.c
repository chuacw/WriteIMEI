/*
   An application to create the IMEI file for MediaTek / MTK phones when
   the IMEI is lost after a factory reset, or a format of the phone.

   Questions to chuacw+imei@gmail.com

   -- chuacw, Singapore, 10 Jun 2014
*/

#pragma hdrstop
#pragma argsused

#include <stdio.h>

#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

// TODO -ochuacw: Move the newly created MP0B_001 IMEI file to the directory /data/nvram/md/NVRAM/NVD_IMEI/ on the
// Android phone, requires a rooted phone to do so.
char *GetFileName(const char *path)
{
    char *filename = strrchr(path, '\\');
    if (filename == NULL)
        filename = path;
    else
        filename++;
    return filename;
}

int calc_imei(char inp_imei[16], char out_imei[12])
{
    char out_mask[12] = {0xAB, 0xA0, 0x6F, 0x2F, 0x1F, 0x1E, 0x9A, 0x45, 0x0, 0x0, 0x0, 0x0};
    int i=0, j=0;

    for (i=0, j=0; i < 15; i++, j++)
    {

        if (inp_imei[i] < '0' || inp_imei[i] > '9')
        {
            return 1;
        }
        out_imei[j] = (inp_imei[i] - '0');

        if (i >= 14)
            break;

        if (inp_imei[i+1] < '0' || inp_imei[i+1] > '9')
        {
             return 1;
        }
        out_imei[j] += ((inp_imei[i+1] - '0') << 4);

        out_imei[j] = out_imei[j] ^ out_mask[j];
        i++;
    }

    out_imei[j] = out_imei[j] ^ out_mask[j];

    out_imei[8] = 0x57;
    out_imei[9] = 0xDB;

    out_imei[10] = out_imei[11] = 0;

    for (i = 0; i < 10; i++)
    {
        if (i & 0x1)
        {
            out_imei[11] += out_imei[i];
        }
        else
        {
            out_imei[10] += out_imei[i];
        }
    }

    return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
    char *str = "357369035621901";


    FILE* output;
    char inp_imei[16];
    char out_imei[12] = {0};
    int i, ret;
    char *mode, *OUT_FILE;

    if (argc < 2 || strlen(argv[1])!=15 || (argc==3 && strlen(argv[2])!=15))
    {
        char* appname = GetFileName(argv[0]);
        fprintf(stdout, "Usage: %s <FirstIMEI 15 digits> [SecondIMEI 15 digits]\n", appname);
        fprintf(stdout, " eg 1: %s 012345678912345\n", appname);
        fprintf(stdout, " eg 2: %s 012345678912345 0123456789012345\n", appname);
        return 1;
    }

    OUT_FILE = "MP0B_001";

    for (i=1; i < argc; i++)
    {
        memset(inp_imei, 0, sizeof(inp_imei));
        memset(out_imei, 0, sizeof(out_imei));

        strncpy(inp_imei, argv[i], sizeof(inp_imei));

        fprintf(stdout, "New IMEI %d (15 digits) - %s\n", i, inp_imei);

        ret = calc_imei(inp_imei, out_imei);
        if ( ret )
        {
            fprintf(stderr, "Invalid IMEI %d format!\n", i);
            return 1;
        }

        mode="w";
        if (i==2)
        {
            mode="a";
        }
        output = fopen(OUT_FILE, mode);
        fwrite(out_imei, 1, sizeof(out_imei), output);
        fclose(output);

    }


    fprintf(stdout, "New IMEI in file = %s\n", OUT_FILE);

    return 0;
}

