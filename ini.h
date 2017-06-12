/*
 * File:   ini.h
 * Author: alex
 *
 * Created on 22 May 2017, 21:11
 */

#ifndef INI_H
#define INI_H

#include "iniparser/src/iniparser.h"

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
    // source and dest image sizes
    Size ssrc, sdst;

    // calibration values
    Point cent;
    int r_inner, r_outer;

    // x and y pixel maps
    Mat map_x;
    Mat map_y;

    void read() {
        ssrc = Size(VID_WIDTH, VID_HEIGHT);
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

            get(ini, this->sdst.width, "unwrap:width");
            get(ini, this->sdst.height, "unwrap:height");

            get(ini, dcent_x, "unwrap:cent-x");
            get(ini, dcent_y, "unwrap:cent-y");
            get(ini, dr_inner, "unwrap:r-inner");
            get(ini, dr_outer, "unwrap:r-outer");

            // free memory
            iniparser_freedict(ini);
        }

        // convert relative to absolute pixel values
        this->cent.x = round((double) this->ssrc.width * dcent_x);
        this->cent.y = round((double) this->ssrc.height * dcent_y);
        this->r_inner = round((double) this->ssrc.height * dr_inner);
        this->r_outer = round((double) this->ssrc.height * dr_outer);

        // define our x and y pixel maps
        this->map_x = Mat(this->sdst, CV_32FC1);
        this->map_y = Mat(this->sdst, CV_32FC1);
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
                "[unwrap]\n"
                "width   = %d\n"
                "height  = %d\n"
                "cent-x  = %g\n"
                "cent-y  = %g\n"
                "r-inner = %g\n"
                "r-outer = %g\n",
                this->sdst.width,
                this->sdst.height,
                dcent_x,
                dcent_y,
                dr_inner,
                dr_outer
                );

        fclose(ini);
    }

    void generate_map() {
        for (int i = 0; i < this->sdst.height; i++) {
            for (int j = 0; j < this->sdst.width; j++) {
                float r = ((float) i / (float) this->sdst.height) * (this->r_outer - this->r_inner) + this->r_inner;
                float th = ((float) j / (float) this->sdst.width) * 2 * M_PI;
                float x = this->cent.x + r * sin(th);
                float y = this->cent.y + r * cos(th);
                this->map_x.at<float>(i, j) = x;
                this->map_y.at<float>(i, j) = y;
            }
        }
    }
};

#endif /* INI_H */