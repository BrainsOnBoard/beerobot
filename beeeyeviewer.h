#pragma once

#include "httpclient.h"

void run_eye_viewer(HttpClient &client) {
    Mat view;

    // set opencv window to display full screen
    cvNamedWindow("bee view", CV_WINDOW_NORMAL);
    setWindowProperty("bee view", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);

    // display remote camera input on loop until user presses escape
    while (true) {
        //eye.get_eye_view(view, imorig);
        if (!client.get_image(view)) {
            cerr << "Error: Could not read from HTTP client" << endl;
            exit(1);
        }

        // show image
        imshow("bee view", view);


        // read keypress in
        if ((waitKey(1) & 0xff) == KB_ESC) {
            break;
        }
    }
}