/*
 * File:   ini.h
 * Author: alex
 *
 * Created on 22 May 2017, 21:11
 */

#ifndef INI_H
#define INI_H

#include <iniparser.h>

using namespace std;

const char* INI_FILE = "beerobot.ini";

struct params {
    int src_wd = 1280;
    int src_ht = 720;
    int dst_wd = 1280;
    int dst_ht = 300;
    double cent_x = 0.5;
    double cent_y = 0.5;
    double r_inner = 0.1;
    double r_outer = 0.5;
};

void get(const dictionary *ini, int &val, const char *str) {
    val = iniparser_getint(ini, str, val);
}

void get(const dictionary *ini, double &val, const char *str) {
    val = iniparser_getdouble(ini, str, val);
}

void read_params(params &p) {
    dictionary *ini = iniparser_load(INI_FILE);
    if (ini) {
        get(ini, p.src_wd, "camera:width");
        get(ini, p.src_ht, "camera:height");

        get(ini, p.dst_wd, "unwrap:width");
        get(ini, p.dst_ht, "unwrap:height");
        get(ini, p.cent_x, "unwrap:cent-x");
        get(ini, p.cent_y, "unwrap:cent-y");
        get(ini, p.r_inner, "unwrap:r-inner");
        get(ini, p.r_outer, "unwrap:r-outer");

        // free memory
        iniparser_freedict(ini);
    }
}

void write_params(params &p) {
    FILE *ini = fopen(INI_FILE, "w");
    if (!ini) {
        cerr << "Error: Could not create file " << INI_FILE << endl;
        return;
    }

    fprintf(ini,
            "[camera]\n"
            "width  = %d\n"
            "height = %d\n\n"
            "[unwrap]\n"
            "width   = %d\n"
            "height  = %d\n"
            "cent-x  = %g\n"
            "cent-y  = %g\n"
            "r-inner = %g\n"
            "r-outer = %g\n",
            p.src_wd,
            p.src_ht,
            p.dst_wd,
            p.dst_ht,
            p.cent_x,
            p.cent_y,
            p.r_inner,
            p.r_outer
            );

    fclose(ini);
}

#endif /* INI_H */

