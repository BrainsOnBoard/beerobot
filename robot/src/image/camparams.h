#pragma once

#include "opencv2/opencv.hpp"
#include "videotype.h"

// this code relies on the iniparser library, included as a git submodule
extern "C"
{
#include "iniparser.h"
}

namespace Image {
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

    const char *m_FilePath;

    // should image be flipped in camera map
    bool m_Flipped;
    int m_DegreeOffset;

public:
    // source and dest image sizes
    cv::Size m_SizeSource, m_SizeDest;

    // calibration values
    cv::Point m_Center;
    int m_RadiusInner, m_RadiusOuter;

    // x and y pixel maps
    cv::Mat m_MapX, m_MapY;

    /* read parameters from ini file */
    CamParams(vid_t *vid)
      : m_SizeSource(vid->width, vid->height)
      , m_SizeDest(1280, 400)
      , m_Flipped(false)
    {
        double dcent_x = 0.5;
        double dcent_y = 0.5;
        double dr_inner = 0.1;
        double dr_outer = 0.5;

        m_FilePath = vid->ini_file;

        dictionary *ini = iniparser_load(vid->ini_file);
        if (!ini) {
            std::cout << "Could not find " << m_FilePath << std::endl;
        } else {
            std::cout << "Reading settings from " << m_FilePath << std::endl;

            get(ini, this->m_SizeDest.width, "unwrap:width");
            get(ini, this->m_SizeDest.height, "unwrap:height");

            get(ini, dcent_x, "unwrap:cent-x");
            get(ini, dcent_y, "unwrap:cent-y");
            get(ini, dr_inner, "unwrap:r-inner");
            get(ini, dr_outer, "unwrap:r-outer");
            get(ini, this->m_Flipped, "unwrap:flipped");
            get(ini, this->m_DegreeOffset, "unwrap:degoffset");

            // free memory
            iniparser_freedict(ini);
        }

        // convert relative (0.0 to 1.0) to absolute pixel values
        this->m_Center.x = round((double) this->m_SizeSource.width * dcent_x);
        this->m_Center.y = round((double) this->m_SizeSource.height * dcent_y);
        this->m_RadiusInner =
                round((double) this->m_SizeSource.height * dr_inner);
        this->m_RadiusOuter =
                round((double) this->m_SizeSource.height * dr_outer);

        // define our x and y pixel maps
        this->m_MapX = cv::Mat(this->m_SizeDest, CV_32FC1);
        this->m_MapY = cv::Mat(this->m_SizeDest, CV_32FC1);
    }

    /* write the parameters to ini file */
    void write()
    {
        std::cout << "Writing settings to " << m_FilePath << std::endl;

        // open file for writing
        FILE *ini = fopen(m_FilePath, "w");
        if (!ini) {
            std::cerr << "Error: Could not create file " << m_FilePath
                      << std::endl;
            return;
        }

        // convert absolute pixel values to relative values (0.0 to 1.0)
        double dcent_x =
                (double) m_Center.x / (double) this->m_SizeSource.width;
        double dcent_y =
                (double) m_Center.y / (double) this->m_SizeSource.height;
        double dr_inner = (double) this->m_RadiusInner /
                          (double) this->m_SizeSource.height;
        double dr_outer = (double) this->m_RadiusOuter /
                          (double) this->m_SizeSource.height;

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
                this->m_SizeDest.width,
                this->m_SizeDest.height,
                dcent_x,
                dcent_y,
                dr_inner,
                dr_outer,
                this->m_Flipped ? 1 : 0,
                this->m_DegreeOffset);

        // close file
        fclose(ini);
    }

    /* generate a new pixel map, based on the current calibration settings */
    void generateMap()
    {
        for (int i = 0; i < this->m_SizeDest.height; i++) {
            for (int j = 0; j < this->m_SizeDest.width; j++) {
                // Get i as a fraction of unwrapped height, flipping if desired
                float frac =
                        this->m_Flipped
                                ? 1.0 - ((float) i /
                                         (float) this->m_SizeDest.height)
                                : ((float) i / (float) this->m_SizeDest.height);

                // Convert i and j to polar
                float r = frac * (this->m_RadiusOuter - this->m_RadiusInner) +
                          this->m_RadiusInner;

                float th = 2 * M_PI *
                           (((float) j / (float) this->m_SizeDest.width) -
                            ((float) m_DegreeOffset / 360.0));
                float x = this->m_Center.x - r * sin(th);
                float y = this->m_Center.y + r * cos(th);
                this->m_MapX.at<float>(i, j) = x;
                this->m_MapY.at<float>(i, j) = y;
            }
        }
    }
};
}
