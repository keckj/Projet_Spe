#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <X11/Xlib.h>


#if _N_MAIN == 5

int main(void) {
    DIR* d = opendir("/tmp/.X11-unix");

    if (d != NULL) {
        struct dirent *dr;
        while ((dr = readdir(d)) != NULL) {
            if (dr->d_name[0] != 'X')
                continue;

            char display_name[64] = ":";
            strcat(display_name, dr->d_name + 1);

            Display *disp = XOpenDisplay(display_name);
            if (disp != NULL) {
                int count = XScreenCount(disp);
                printf("Display %s has %d screens\n",
                    display_name, count);

                int i;
                for (i=0; i<count; i++)
                    printf(" %d: %dx%d\n",
                        i, XDisplayWidth(disp, i), XDisplayHeight(disp, i));

                XCloseDisplay(disp);
            }
        }
        closedir(d);
    }

    return 0;
}

#endif
