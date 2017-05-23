/*
 * File:   ini.h
 * Author: alex
 *
 * Created on 22 May 2017, 21:11
 */

#ifndef INI_H
#define INI_H

#include <iniparser.h>

using namespace cv;
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
    Size ssrc, sdst;
    Point cent;
    int r_inner, r_outer;

    void read() {
        ssrc = Size(1280, 720);
        sdst = Size(1280, 400);
        double dcent_x = 0.5;
        double dcent_y = 0.5;
        double dr_inner = 0.1;
        double dr_outer = 0.5;

        dictionary *ini = iniparser_load(INI_FILE);
        if (!ini)
            cout << "Could not find " << INI_FILE;
        else {
            cout << "Reading settings from " << INI_FILE << endl;

            get(ini, this->ssrc.width, "camera:width");
            get(ini, this->ssrc.height, "camera:height");
            get(ini, this->sdst.width, "unwrap:width");
            get(ini, this->sdst.height, "unwrap:height");

            get(ini, dcent_x, "unwrap:cent-x");
            get(ini, dcent_y, "unwrap:cent-y");
            get(ini, dr_inner, "unwrap:r-inner");
            get(ini, dr_outer, "unwrap:r-outer");

            // free memory
            iniparser_freedict(ini);
        }

        this->cent.x = round((double) this->ssrc.width * dcent_x);
        this->cent.y = round((double) this->ssrc.height * dcent_y);
        this->r_inner = round((double) this->ssrc.height * dr_inner);
        this->r_outer = round((double) this->ssrc.height * dr_outer);
    }

    void write() {
        cout << "Writing settings to " << INI_FILE << endl;

        FILE *ini = fopen(INI_FILE, "w");
        if (!ini) {
            cerr << "Error: Could not create file " << INI_FILE << endl;
            return;
        }

        double dcent_x = (double) cent.x / (double) this->ssrc.width;
        double dcent_y = (double) cent.y / (double) this->ssrc.height;
        double dr_inner = (double) this->r_inner / (double) this->ssrc.height;
        double dr_outer = (double) this->r_outer / (double) this->ssrc.height;

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
                this->ssrc.width,
                this->ssrc.height,
                this->sdst.width,
                this->sdst.height,
                dcent_x,
                dcent_y,
                dr_inner,
                dr_outer
                );

        fclose(ini);
    }
};

#endif /* INI_H */