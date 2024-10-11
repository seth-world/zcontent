/*
 * Gérard POLO 11/2018
 *
 *  This file should be used as main routine to test utfSprintf functions suite.
 *
 *
 *
 *
 */



#define __MAIN__


#include <QCoreApplication>

#include <cstdio>
#include <iostream>
#include <stdio.h>
//#include <stddef.h>
#include <cstddef>
#include <zio/zdir.h>

#include <zconfig.h>
#include <ztoolset/zarray.h>
#include <ztoolset/zerror.h>

#include <ztoolset/zerror.h>


#include <ztoolset/utfsprintf.h>

int main(int argc, char *argv[])
{
_MODULEINIT_ ;



/* test utfSprintf */

char            buf1[2000];
char            buf2[2000];
const char      *fp_fmt[] = {
     "%+22.9Lf",
    "%4Lf",
    "%-1.9llf",
    "%1.5Lf",
    "%10.7llf",
    "% 10.8Lf",
    "%123.10llF",
    "%+4.11llf",
    "%01.3LF",
    "%3.1Lf",
    "%3.2LF",
    "%.0Lf",
    "%.1Lf",
    "%.1llf",
    NULL};
const char      *fe_fmt[] = {
    "%+22.9lle",
    "%-1.9LE",
    "%1.5Le",
    "%10.7LE",
    "% 10.8Le",
    "%123.10Le",
    "%+4.11LE",
    "%01.3Le",
    "%4LE",
    "%3.1lle",
    "%3.2LE",
    "%.0lle",
    "%.1LE",
    "%.1Le",
    NULL};
const char      *fa_fmt[] = {
    "%4LA",
    "%123.10La",
    "%+22.9La",
    "%-1.9LA",
    "%1.5La",
    "%10.7LA",
    "% 10.8La",
    "%+4.11LA",
    "%01.3lla",
    "%3.1lla",
    "%3.2LA",
    "%.0La",
    "%.1llA",
    "%.1La",
    NULL};
const char      *fg_fmt[] = {
    "%1.5Lg",
    "%.0Lg",
    "%+22.9llg",
    "%-1.9LG",
    "%10.7LG",
    "% 10.8Lg",
    "%123.10llg",
    "%+4.11llG",
    "%01.3Lg",
    "%4LG",
    "%3.1Lg",
    "%3.2LG",
    "%.1LG",
    "%.1llg",
    NULL};

const char *int_fmt[] = {
        "%d",
        "%-1.5d",
        "%1.5d",
        "%123.9d",
        "%5.5d",
        "%10.5d",
        "% 10.5d",
        "%+22.33d",
        "%01.3d",
        "%4d",
        NULL
    };

const char *str_fmt[] = {
        "%-15s",        /* utf8 string or char string */
        "%8ls",          /* utf16 string */
        "%10ls",        /* utf16 string long notation */
        "%5c",          /* utf8 char */
        "%10cw",        /* utf32 char*/
        "%-25lls",      /* utf32 string long notation*/
        NULL
    };




const char      *fk_fmt[] = {
    "%+4.11llK",
    "%>1.5LK",    /* flag: sign follows - amount upper*/
    "%+>|y<  Lk", /* flag: sign follows - thousand separator - currency sign - preceeding with 2 spaces -amount lower*/
    "%.0|y <LK", /* flag: thousand separator - currency sign - preceeding with 1 spaces -amount upper*/

    "%+22.9Y<Lk", /* flag:  currency - preceeding no space - amount upper*/
    "%-1.9|YllK",
    "%>10.7y>llK", /* flag: sign follows */
    "% 10.8Y<LK",
    "%123.10y>llK",
    "%01.3llk",
    "%4|llk",
    "%>3.1llk",
    "%3.2LK",
    "%.1|Lk",
    "%.1LK",
    NULL};

const int cst_MaxFPTest = 15 ;
    const long double  fp_nums[] =
    {   25868.6985425145 , 0.96 , 0.9996 , 2589852568741.585 ,   0.0 , 0.000996 ,   1.23E+11 , -1.5 ,
        91340.2  , 341.12349512 ,  134.21 , 0203.9 , 1.996 , 4.136 , 1.0/0.0 , nanf("") };



#include <ztoolset/ztimer.h>

ZTimer wUtfTimer,wUtfTotalTimer,wStdTimer,wStdTotalTimer;

const int cst_MaxIntTest = 8 ;
long            int_nums[] = { -1, 134, 91340, 341, 0203, 0, 3854,-2598875 };
int             x, y;
int             wFPFail=0,wFPFail_0 = 0,wFPFail_1 = 0,wFPFail_2 = 0,wFPFail_3 = 0,wFPFail_4 = 0,wFPFail_5 = 0;
int             wFPNum=0, wFPNum_0 = 0,wFPNum_1 = 0,wFPNum_2 = 0,wFPNum_3 = 0,wFPNum_4 = 0,wFPNum_5 = 0;
int             wStrFail=0;
int             wStrTest= 0;


    GGetLocaleCodes("fr_CH.utf8");

    printf("Testing snprintf format codes against system sprintf...\n");


    fprintf (stdout," ---Float serie <f> ------\n");

    wUtfTimer.init();
    wStdTimer.init();

    for (x = 0; fp_fmt[x] != nullptr; x++)
    {
        fprintf (stdout,"%d> Format string : %s\n",x,fp_fmt[x]);
        for (y = 0; y<cst_MaxFPTest; y++)  {
            wUtfTimer.start();
            utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),(utf8_t*) fp_fmt[x], fp_nums[y]);
            wUtfTimer.end();
//            fprintf (stdout," utf>> %s\n",buf1);
            wStdTimer.start();
            sprintf(buf2, fp_fmt[x], fp_nums[y]);
            wStdTimer.end();
//            fprintf (stdout," sys>> %s\n",buf2);
            if (strcmp(buf1, buf2)) {
                printf
                    ("snprintf doesn't match Format: %s for value %.15Lf\n"
                     "\tutfSnprintf = <%s>\n"
                     "\t    sprintf = <%s>\n",
                     fp_fmt[x], fp_nums[y],
                      buf1, buf2);#endif // __COMMENT__
                wFPFail_0++;
                wFPFail++;
            }
            wFPNum++;
            wFPNum_0++;
        }
    }
    printf ("\n--------------------------------------------------------------------\n");
    if (wFPFail_0>0)
        printf("\t\t %d Float serie <f>tests failed out of %d/%d.\n", wFPFail_0, wFPNum_0,wFPNum);
        else
        printf("\t\t %d/%d Float serie <f> tests passed without error (previous %d errored). \n", wFPNum_0,wFPNum,wFPFail);
    printf ("\n--------------------------------------------------------------------\n");
    wUtfTotalTimer += wUtfTimer;
    wStdTotalTimer += wStdTimer;
    printf (".........Numeric argument test report .............\n"
            " Local>>utfSprintf %s\n "
            "        syssprintf %s\n"
            " Cumulated >> utfSprintf %s\n"
            "              syssprintf %s\n",
            (char*)wUtfTimer.reportDeltaTime().content,
            (char*)wStdTimer.reportDeltaTime().content,
            (char*)wUtfTotalTimer.reportDeltaTime().content,
            (char*)wStdTotalTimer.reportDeltaTime().content);
    printf ("...................................................\n");

    fprintf (stdout," ---Integer serie ------\n");

    wUtfTimer.init();
    wStdTimer.init();

    for (x = 0; int_fmt[x] != nullptr; x++)
    {
        fprintf (stdout,"%d> Format string : %s\n",x,int_fmt[x]);
        for (y = 0; y < cst_MaxIntTest; y++) {
            wUtfTimer.start();
            utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),(utf8_t*) int_fmt[x], int_nums[y]);
            wUtfTimer.end();
//            fprintf (stdout," utf>> %s\n",buf1);
            wStdTimer.start();
            sprintf(buf2, int_fmt[x], int_nums[y]);
            wStdTimer.end();
//            fprintf (stdout," sys>> %s\n",buf2); //               str_Utf8,
            //               fp_nums[2], // 0.9996
            //               str_utf16,
            //               fp_nums[3], // 2589852568741.585
                           str_utf16_Small,
            if (strcmp(buf1, buf2)) {
                printf
                    ("snprintf doesn't match Format: %s for value %d\n"
                     "\tutfSnprintf = <%s>\n"
                     "\t    sprintf = <%s>\n",
                     int_fmt[x], int_nums[y],
                     buf1, buf2);
                wFPFail_1++;
                wFPFail++;
            }
            wFPNum++;
            wFPNum_1++;
        }
    }
     printf ("\n--------------------------------------------------------------------\n");
    if (wFPFail_1>0)
        printf("\t\t %d Integer serie tests failed out of %d/%d.\n", wFPFail_1, wFPNum_1,wFPNum);
        else
        printf("\t\t %d/%d Integer serie tests passed without error (previous %d errored). \n", wFPNum_1,wFPNum,wFPFail);
     printf ("\n--------------------------------------------------------------------\n");
     wUtfTotalTimer += wUtfTimer;
     wStdTotalTimer += wStdTimer;
     printf (".........Numeric argument test report .............\n"
             " Local>>utfSprintf %s\n "
             "        syssprintf %s\n"
             " Cumulated >> utfSprintf %s\n"
             "              syssprintf %s\n",
             (char*)wUtfTimer.reportDeltaTime().content,
             (char*)wStdTimer.reportDeltaTime().content,
             (char*)wUtfTotalTimer.reportDeltaTime().content,
             (char*)wStdTotalTimer.reportDeltaTime().content);
     printf ("...................................................\n");


    fprintf (stdout," ---Float serie <e> ------\n");

    wUtfTimer.init();
    wStdTimer.init();

    for (x = 0; fe_fmt[x] != nullptr; x++)
    {
        fprintf (stdout,"%d> Format string : %s\n",x,fe_fmt[x]);
        for (y = 0; y < cst_MaxFPTest ; y++) {
//             fprintf (stdout," Raw value %f   format <%s> \n",fp_nums[y],fe_fmt[x]);
            wUtfTimer.start();
            utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),(utf8_t*) fe_fmt[x], fp_nums[y]);
            wUtfTimer.end();
//            fprintf (stdout," utf>> %s\n",buf1);
            wStdTimer.start();
            sprintf(buf2, fe_fmt[x], fp_nums[y]);
            wStdTimer.end();
//            fprintf (stdout," sys>> %s\n",buf2);
            if (strcmp(buf1, buf2)) {
                printf
                    ("snprintf doesn't match Format: %s for value %.15Lf\n"
                     "\tutfSnprintf = <%s>\n"
                     "\t    sprintf = <%s>\n",
                     fe_fmt[x], fp_nums[y],
                     buf1, buf2);
                wFPFail_2++;
                wFPFail++;
            }
            wFPNum++;
            wFPNum_2++;
        }
    }
    printf ("\n--------------------------------------------------------------------\n");
    if (wFPFail_2>0)
        printf("\t\t %d Float serie <e> tests failed out of %d/%d.\n", wFPFail_2, wFPNum_2,wFPNum);
        else
        printf("\t\t %d/%d Float serie <e> tests passed without error (previous %d errored). \n", wFPNum_2,wFPNum,wFPFail);
    printf ("\n--------------------------------------------------------------------\n");
    wUtfTotalTimer += wUtfTimer;
    wStdTotalTimer += wStdTimer;
    printf (".........Numeric argument test report .............\n"
            " Local>>utfSprintf %s\n "
            "        syssprintf %s\n"
            " Cumulated >> utfSprintf %s\n"
            "              syssprintf %s\n",
            (char*)wUtfTimer.reportDeltaTime().content,
            (char*)wStdTimer.reportDeltaTime().content,
            (char*)wUtfTotalTimer.reportDeltaTime().content,
            (char*)wStdTotalTimer.reportDeltaTime().content);
    printf ("...................................................\n");

    fprintf (stdout," ---Float serie <a> ------\n");

    wUtfTimer.init();
    wStdTimer.init();

    for (x = 0; fa_fmt[x] != nullptr; x++)
    {
        fprintf (stdout,"%d> Format string : %s\n",x,fa_fmt[x]);
        for (y = 0; y < cst_MaxFPTest ; y++)
            {
//             fprintf (stdout," Raw value %Lf   format <%s> \n",fp_nums[y],fe_fmt[x]);
            wUtfTimer.start();
            utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),(utf8_t*) fa_fmt[x], fp_nums[y]);
            wUtfTimer.end();
//            fprintf (stdout," utf>> %s\n",buf1);
            wStdTimer.start();
            sprintf(buf2, fa_fmt[x], fp_nums[y]);
            wStdTimer.end();
//            fprintf (stdout," sys>> %s\n",buf2);
            if (strcmp(buf1, buf2)) {
                printf
                    ("snprintf doesn't match Format: %s for value %.15Lf\n"
                     "\tutfSnprintf = <%s>\n"
                     "\t    sprintf = <%s>\n",
                     fa_fmt[x], fp_nums[y],
                     buf1, buf2);
                wFPFail_3++;
                wFPFail++;
            }
            wFPNum++;
            wFPNum_3++;
        }
    }
    printf ("\n--------------------------------------------------------------------\n");
    wUtfTotalTimer += wUtfTimer;
    wStdTotalTimer += wStdTimer;
    if (wFPFail_3>0)
        printf("\t\t %d Float serie <a> tests failed out of %d/%d.\n", wFPFail_3, wFPNum_3,wFPNum);
        else
        printf("\t\t %d/%d Float serie <a> tests passed without error (previous %d errored). \n", wFPNum_3,wFPNum,wFPFail);
    printf ("\n--------------------------------------------------------------------\n");
    printf (".........Numeric argument test report .............\n"
            " Local>>utfSprintf %s\n "
            "        syssprintf %s\n"
            " Cumulated >> utfSprintf %s\n"
            "              syssprintf %s\n",
            (char*)wUtfTimer.reportDeltaTime().content,
            (char*)wStdTimer.reportDeltaTime().content,
            (char*)wUtfTotalTimer.reportDeltaTime().content,
            (char*)wStdTotalTimer.reportDeltaTime().content);
    printf ("...................................................\n");


    fprintf (stdout,"\n\n                                ---Float serie <g> ------\n");

    wUtfTimer.init();
    wStdTimer.init();

    for (x = 0; fa_fmt[x] != nullptr; x++)
    {
        fprintf (stdout,"%d> Format string : %s\n",x,fg_fmt[x]);
        for (y = 0; y<cst_MaxFPTest; y++) {
            wUtfTimer.start();
            utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),(utf8_t*) fg_fmt[x], fp_nums[y]);
            wUtfTimer.end();
/*            printf("value %.15Lf\n", fp_nums[y]);
            fprintf (stdout," utf>> %s\n",buf1);*/
            wStdTimer.start();
            sprintf(buf2, fg_fmt[x], fp_nums[y]);
            wStdTimer.end();
/*            fprintf (stdout," sys>> %s\n",buf2);*/
            if (strcmp(buf1, buf2)) {
                printf
                    ("snprintf doesn't match Format: %s for value %.15Lf\n"
                     "\tutfSnprintf = <%s>\n"
                     "\t    sprintf = <%s>\n",
                     fg_fmt[x], fp_nums[y],
                     buf1, buf2);
                wFPFail_4++;
                wFPFail++;
            }
            wFPNum++;
            wFPNum_4++;
        }
    }
    printf ("\n--------------------------------------------------------------------\n");
    if (wFPFail_4>0)
        printf("\t\t %d Float serie <g> tests failed out of %d/%d.\n", wFPFail_4, wFPNum_4,wFPNum);
        else
        printf("\t\t %d/%d Float serie <g> tests passed without error (previous %d errored). \n", wFPNum_4,wFPNum,wFPFail);
    printf ("\n--------------------------------------------------------------------\n");
    wUtfTotalTimer += wUtfTimer;
    wStdTotalTimer += wStdTimer;
    printf (".........Numeric argument test report .............\n"
            " Local>>utfSprintf %s\n "
            "        syssprintf %s\n"
            " Cumulated >> utfSprintf %s\n"
            "              syssprintf %s\n",
            (char*)wUtfTimer.reportDeltaTime().content,
            (char*)wStdTimer.reportDeltaTime().content,
            (char*)wUtfTotalTimer.reportDeltaTime().content,
            (char*)wStdTotalTimer.reportDeltaTime().content);
    printf ("...................................................\n");

    wUtfTimer.init();
    wStdTimer.init();

    fprintf (stdout," ---Float serie <k> ------\n");
    for (x = 0; fk_fmt[x] != nullptr; x++)
    {
        fprintf (stdout,"%d> Format string : %s\n",x,fk_fmt[x]);
        for (y = 0; y<cst_MaxFPTest; y++) {
//            utfCheckFullFormat((utf8_t*) fk_fmt[x], fp_nums[y]);

            utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),(utf8_t*) fk_fmt[x], fp_nums[y]);
            fprintf (stdout,"\t <%s> value %.15Lf\n",
                     fk_fmt[x], fp_nums[y]);
            fprintf (stdout," utf>> <%s>\n",
                     buf1);
            wFPNum++;
            wFPNum_5++;
        }
    }
    printf ("\n--------------------------------------------------------------------\n");
    if (wFPFail_5>0)
        printf("%d string Float serie <k> failed out of %d/%d.\n", wFPFail_5, wFPNum_5,wFPNum);
        else
        printf("%d/%d string Float serie <k> passed without error (previous %d errored). \n", wFPNum_5,wFPNum,wFPFail);
    printf ("\n--------------------------------------------------------------------\n");
    wUtfTotalTimer += wUtfTimer;
    wStdTotalTimer += wStdTimer;
    printf (".........Numeric argument test report .............\n"
            " Local>> utfSprintf %s\n syssprintf %s\n"
            " Cumulated >> utfSprintf %s\n syssprintf %s\n",
            (char*)wUtfTimer.reportDeltaTime().content,
            (char*)wStdTimer.reportDeltaTime().content,
            (char*)wUtfTotalTimer.reportDeltaTime().content,
            (char*)wStdTotalTimer.reportDeltaTime().content);
    printf ("...................................................\n");



    printf (".........Cumulated argument test report .............\n"
            " utfSprintf %s\n syssprintf %s\n",
            (char*)wUtfTotalTimer.reportDeltaTime().content,
            (char*)wStdTotalTimer.reportDeltaTime().content);
    printf ("...................................................\n");


    fprintf (stdout,
             " ---string and single char serie <s> <c> etc. ------\n"
             " Do not forget to set console to unicode utf8 output\n");

    utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
                (utf8_t*) "%-15s",
                str_Utf8_1);
    sprintf(buf2, "%-15s", (char*)str_Utf8_1);

    fprintf (stdout," utf>> ");
    fputs((const char*)buf1,stdout);
    fprintf (stdout," \n");
    fprintf (stdout," sys>> %s\n",buf2);

    if (strcmp(buf1, buf2)) {
        fprintf (stdout," value %s\n",(char*)str_Utf8_1);
        printf
            ("snprintf doesn't match Format: %s\n"
             "\tutfSnprintf = <%s>\n"
             "\t    sprintf = <%s>\n",
             "%s-15s",
             buf1, buf2);
        wStrFail++;}
    wStrTest++;

    utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
                (utf8_t*) "%8ls",
                str_utf16_2);
    sprintf(buf2, "%8ls", (wchar_t*)str_utf16_2);

    fprintf (stdout," utf>> ");
    fputs((const char*)buf1,stdout);
    fprintf (stdout," \n");
    fprintf (stdout," sys>> %ls\n",(wchar_t*)buf2);

    if (strcmp(buf1, buf2)) {
        fprintf (stdout," value %ls\n",(wchar_t*)str_utf16_2);
        printf
            ("snprintf doesn't match Format: %s\n"
             "\tutfSnprintf = <%s>\n"
             "\t    sprintf = <%s>\n",
             "%8ls",
             buf1, buf2);
        wStrFail++;}
    wStrTest++;


    if (wStrFail>0)
        printf("%d/%d string tests failed out of %d.\n", wStrFail, wStrTest);
        else
        printf("%d/%d string tests passed without error (previous %d errored). \n", wStrTest);

    utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
                (utf8_t*) "%5.7Ls",
                str_utf32_3);
    sprintf(buf2, "%5.7Ls", str_utf32_3);
/*    fprintf (stdout," utf>> ");
    fputs((const char*)buf1,stdout);
    fprintf (stdout," \n");*/
    fprintf (stdout," utf>> %s\n",buf1);
    fprintf (stdout," utf>> ");
    fputs((const char*)buf2,stdout);
    fprintf (stdout," \n");
    fprintf (stdout," sys>> %s\n",buf2);

    if (strcmp(buf1, buf2)) {
        fprintf (stdout," value %Ls\n",str_utf32_3);
        printf
            ("snprintf doesn't match Format: %s\n"
             "\tutfSnprintf = <%s>\n"
             "\t    sprintf = <%s>\n",
             "%5.7Ls",
             buf1, buf2);
        wStrFail++;}
    wStrTest++;


/*    for (long wi=0;str_utf16[wi]!=0;wi++)
            printf("<%ld> str_utf16 <%d>\n",
                   wi,str_utf16[wi]);
    for (long wi=0;str_utf32[wi]!=0;wi++)
            printf("<%ld> str_utf32 <%d>\n",
                   wi,str_utf32[wi]);
*/
    const char* str_Utf8=u8"Всё смеша́лось в до́ме Облонских. Жена́ узна́ла, что";
    const utf16_t* str_utf16=(const utf16_t*)u"Всё смеша́лось в до́ме Облонских. Жена́ узна́ла, что";
    const utf32_t* str_utf32=(const utf32_t*)U"Всё смеша́лось в до́ме Облонских. Жена́ узна́ла, что";

    const char* str_utf8_Small=u8"Всё смеша́лось";   /* size is 13 */
    const utf16_t* str_utf16_Small=(const utf16_t*)u"Всё смеша́лось";
    const utf32_t* str_utf32_Small=(const utf32_t*)U"Всё смеша́лось";

    const utf32_t char_utf32=U'д';
    const utf16_t char_utf16=u'д';

    const char* wFormat99 = "%6.5|LK ";
/*
    printf(" format <%s>\n",wFormat99);
    utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
                (const utf8_t*)wFormat99,
                fp_nums[3] // 2589852568741.585
                );

    fprintf (stdout," utf>> %s\n",buf1);
*/
const char* wFormat = "<%1.10s> %+>05Y>  LK <%2.10ls>  %y  <Lk <%5.7Ls> %6.5|LK ";
size_t wSIZE_T = 999858;
intmax_t wIM= 325;
utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
            (const utf8_t*)" pointer <%p> size_t <%z> ptrdiff_t <%t> intmax_t <%j> \n",
            wFormat,
            wSIZE_T,
            wFormat+1,
            wIM
            );
    fprintf (stdout," utf>> %s\n",buf1);

    utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
                (const utf8_t*)" single character cS <%cS> \n",
                char_utf16
                );
    fprintf (stdout," utf>> %s\n",buf1);

    /* single char utf16 with padding left justified */

    utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
                (const utf8_t*)" single character -8lc <%-8lc> \n",
                char_utf16
                );
     fprintf (stdout," utf>> %s\n",buf1);

    utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
                (const utf8_t*)" single character cW <%cW> \n",
                char_utf32
                );
    fprintf (stdout," utf>> %s\n",buf1);
    /* single char utf16 with padding right justified */
    utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
                (const utf8_t*)" single character 8Lc <%8Lc> \n",
                char_utf32
                );
     fprintf (stdout," utf>> %s\n",buf1);

    printf(" format <%s>\n",wFormat);
    utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
                (const utf8_t*)wFormat,
                str_Utf8,
                fp_nums[2], // 0.9996
                str_utf16,
                fp_nums[9], // 341.12349512
                str_utf32,
                fp_nums[3] // 2589852568741.585
                );

    fprintf (stdout," utf>> %s\n",buf1);




     /* utf32 string padded to 20 - Long double Amount zero padded 5 slots thousands sep sign right currency right space separated*/
    const char* wFormat1 = "<%20ls>  <%+>020.4|Y>  LK> ";
    const char* wFormat2 = "<%-20ls>  <%+>020.4|Y>  LK> ";

    const char* wFormat13 = "<%+>010.0|Y>K> ";
    utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
                (const utf8_t*)wFormat13,
                (double)312.0  // 341.12349512
                );
    printf(" format <%s>\n",wFormat13);
    fprintf (stdout," utf>> %s\n",buf1);

    const char* wFormat12 = "<%+>09.0|Y>K> ";
    utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
                (const utf8_t*)wFormat12,
                (double)312.0  // 341.12349512
                );

    printf(" format <%s>\n",wFormat12);
    fprintf (stdout," utf>> %s\n",buf1);

    const char* wFormat14 = "<%+>011.0|Y>K> ";
    utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
                (const utf8_t*)wFormat14,
                (double)312.0  // 341.12349512
                );
    printf(" format <%s>\n",wFormat14);
    fprintf (stdout," utf>> %s\n",buf1);


    const char* wFormat10 = "<%+>07.0|Y>K> ";
    utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
                (const utf8_t*)wFormat10,
                (double)312.0  // 341.12349512
                );
    printf(" format <%s>\n",wFormat10);
    fprintf (stdout," utf>> %s\n",buf1);

    const char* wFormat11 = "<%+>08.0|Y>K> ";
    utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
                (const utf8_t*)wFormat11,
                (double)312.0  // 341.12349512
                );
    printf(" format <%s>\n",wFormat11);
    printf(" utf>> %s\n",buf1);


    printf(" format <%s>\n",wFormat1);
    utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
                (const utf8_t*)wFormat1,
                str_utf16_Small,
                fp_nums[9]  // 341.12349512
                );
    fprintf (stdout," utf>> %s\n",buf1);

    printf(" format <%s>\n",wFormat2);
    utfSnprintf(ZCHARSET_UTF8, buf1, sizeof(buf1),
                (const utf8_t*)wFormat2,
                str_utf16_Small,
                fp_nums[9]  // 341.12349512
                );


    _EXIT_(EXIT_SUCCESS);


} // main

