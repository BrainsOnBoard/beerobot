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

class CamParams {
private:

    static inline void get(const dictionary *ini, int &val, const char *str) {
        val = iniparser_getint(ini, str, val);
    }

    static inline void get(const dictionary *ini, double &val, const char *str) {
        val = iniparser_getdouble(ini, str, val);
    }

public:
    int src_wd = 1280;
    int src_ht = 720;
    int dst_wd = 1280;
    int dst_ht = 300;
    double cent_x = 0.5;
    double cent_y = 0.5;
    double r_inner = 0.1;
    double r_outer = 0.5;

    void read() {
        dictionary *ini = iniparser_load(INI_FILE);
        if (!ini) {
            cout << "Could not find " << INI_FILE;
            return;
        }

        cout << "Reading settings from " << INI_FILE << endl;

        get(ini, this->src_wd, "camera:width");
        get(ini, this->src_ht, "camera:height");
        get(ini, this->dst_wd, "unwrap:width");
        get(ini, this->dst_ht, "unwrap:height");
        get(ini, this->cent_x, "unwrap:cent-x");
        get(ini, this->cent_y, "unwrap:cent-y");
        get(ini, this->r_inner, "unwrap:r-inner");
        get(ini, this->r_outer, "unwrap:r-outer");

        // free memory
        iniparser_freedict(ini);
    }

    void write() {
        cout << "Writing settings to " << INI_FILE << endl;

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
                this->src_wd,
                this->src_ht,
                this->dst_wd,
                this->dst_ht,
                this->cent_x,
                this->cent_y,
                this->r_inner,
                this->r_outer
                );

        fclose(ini);
    }
};

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

#endif /* INI_H */

