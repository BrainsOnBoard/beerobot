#pragma once

#include "videotype.h"

// this code relies on the iniparser library, included as a git submodule
#include "iniparser.h"

using namespace cv;
using namespace std;

class CamParams
{
private:
    /* read an int from the ini file */
    static inline void get(const dictionary *ini, int &val, const char *str)
    {
        val = iniparser_getint(ini, str, val);
    }

    /* read a boolean from the ini file */
    static inline void get(const dictionary *ini, bool &val, const char *str)
    {
        val = (iniparser_getboolean(ini, str, val ? 1 : 0) != 0);
    }

    /* read a double from the ini file */
    static inline void get(const dictionary *ini, double &val, const char *str)
    {
        val = iniparser_getdouble(ini, str, val);
    }

    const char *fpath;

    // should image be flipped in camera map
    bool flipped;
    int degoffset;

public:
    // source and dest image sizes
    Size ssrc, sdst;

    // calibration values
    Point cent;
    int r_inner, r_outer;

    // x and y pixel maps
    Mat map_x;
    Mat map_y;

    /* read parameters from ini file */
    CamParams(vid_t *vid)
      : ssrc(vid->width, vid->height)
      , sdst(1280, 400)
      , flipped(false)
    {
        double dcent_x = 0.5;
        double dcent_y = 0.5;
        double dr_inner = 0.1;
        double dr_outer = 0.5;

        fpath = vid->ini_file;

        dictionary *ini = iniparser_load(vid->ini_file);
        if (!ini)
            cout << "Could not find " << fpath;
        else {
            cout << "Reading settings from " << fpath << endl;

            get(ini, this->sdst.width, "unwrap:width");
            get(ini, this->sdst.height, "unwrap:height");

            get(ini, dcent_x, "unwrap:cent-x");
            get(ini, dcent_y, "unwrap:cent-y");
            get(ini, dr_inner, "unwrap:r-inner");
            get(ini, dr_outer, "unwrap:r-outer");
            get(ini, this->flipped, "unwrap:flipped");
            get(ini, this->degoffset, "unwrap:degoffset");

            // free memory
            iniparser_freedict(ini);
        }

        // convert relative (0.0 to 1.0) to absolute pixel values
        this->cent.x = round((double) this->ssrc.width * dcent_x);
        this->cent.y = round((double) this->ssrc.height * dcent_y);
        this->r_inner = round((double) this->ssrc.height * dr_inner);
        this->r_outer = round((double) this->ssrc.height * dr_outer);

        // define our x and y pixel maps
        this->map_x = Mat(this->sdst, CV_32FC1);
        this->map_y = Mat(this->sdst, CV_32FC1);
    }

    /* write the parameters to ini file */
    void write()
    {
        cout << "Writing settings to " << fpath << endl;

        // open file for writing
        FILE *ini = fopen(fpath, "w");
        if (!ini) {
            cerr << "Error: Could not create file " << fpath << endl;
            return;
        }

        // convert absolute pixel values to relative values (0.0 to 1.0)
        double dcent_x = (double) cent.x / (double) this->ssrc.width;
        double dcent_y = (double) cent.y / (double) this->ssrc.height;
        double dr_inner = (double) this->r_inner / (double) this->ssrc.height;
        double dr_outer = (double) this->r_outer / (double) this->ssrc.height;

        // write to file
        fprintf(ini,
                "[unwrap]\n"
                "width   = %d\n"
                "height  = %d\n"
                "cent-x  = %g\n"
                "cent-y  = %g\n"
                "r-inner = %g\n"
                "r-outer = %g\n"
                "flipped = %u\n"
                "degoffset = %d\n",
                this->sdst.width,
                this->sdst.height,
                dcent_x,
                dcent_y,
                dr_inner,
                dr_outer,
                this->flipped ? 1 : 0,
                this->degoffset);

        // close file
        fclose(ini);
    }

    /* generate a new pixel map, based on the current calibration settings */
    void generate_map()
    {
        for (int i = 0; i < this->sdst.height; i++) {
            for (int j = 0; j < this->sdst.width; j++) {
                // Get i as a fraction of unwrapped height, flipping if desired
                float frac =
                        this->flipped
                                ? 1.0 - ((float) i / (float) this->sdst.height)
                                : ((float) i / (float) this->sdst.height);

                // Convert i and j to polar
                float r =
                        frac * (this->r_outer - this->r_inner) + this->r_inner;

                float th = 2 * M_PI *
                           (((float) j / (float) this->sdst.width) -
                            ((float) degoffset / 360.0));
                float x = this->cent.x - r * sin(th);
                float y = this->cent.y + r * cos(th);
                this->map_x.at<float>(i, j) = x;
                this->map_y.at<float>(i, j) = y;
            }
        }
    }
};
