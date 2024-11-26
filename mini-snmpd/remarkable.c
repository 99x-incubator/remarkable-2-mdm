#include <dirent.h>
#include <errno.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "mini-snmpd.h"

#define XOCHITL_DIRECTORY "/home/root/.local/share/remarkable/xochitl"

/**
 * Counts the number of PDF or other documents within the Xochitl documents directory. 
 * This function treat thumbnail directory as a document because it seems Xochitl creates
 * one thumbnail directory for each document.
 *
 * @return The count of ".thumbnails" directories.
 */
int count_documents_in_xochitl()
{
    struct dirent *direntp;
    int directory_count = 0;
    int dir_name_length;

    DIR *doc_path = opendir(XOCHITL_DIRECTORY);
    if (doc_path == NULL) {
        perror("Failed to open Xochitl documents directory");
        return 0;
    }

    while ((direntp = readdir(doc_path)) != NULL) {
        // Skip the current and parent directory entries
        if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0) {
            continue;
        }

        // Check if the entry is a directory and has a valid name
        if (direntp->d_type == DT_DIR && direntp->d_name != NULL) {
            dir_name_length = strlen(direntp->d_name);

            // Ensure the directory name is long enough to contain ".thumbnails"
            if (dir_name_length > 11) {
                // Calculate the starting position for ".thumbnails"
                dir_name_length -= 11;

                // Compare the ending of the directory name with ".thumbnails"
                if (strcmp(direntp->d_name + dir_name_length, ".thumbnails") == 0) {
                    directory_count++;
                }
            }
        }
    }

    closedir(doc_path);
    return directory_count;
}

/**
 * Retrieves the current battery level from the system.
 *
 * @return The battery level as an unsigned char. Returns 0xFF if an error occurs.
 */
unsigned char get_battery_level()
{
    int temp_data = -1;

    FILE *max77818_info = fopen("/sys/class/power_supply/max77818_battery/capacity", "r");
    if (max77818_info == NULL) {
        perror("Failed to open battery capacity file");
        return 0xFF;
    }

    if (fscanf(max77818_info, "%d", &temp_data) != 1) {
        perror("Failed to read battery capacity");
        fclose(max77818_info);
        return 0xFF;
    }

    fclose(max77818_info);

    if (temp_data >= 0 && temp_data <= 100) {
        return (unsigned char)temp_data;
    } else {
        return 0xFF;
    }
}

/**
 * Populates the ereaderinfo structure with the current document count and battery level.
 *
 * @param ereaderinfo Pointer to the ereaderinfo_t structure to populate.
 */
void get_ereaderinfo(ereaderinfo_t *ereaderinfo)
{
    if (ereaderinfo == NULL) {
        fprintf(stderr, "Invalid ereaderinfo pointer provided.\n");
        return;
    }

    memset(ereaderinfo, 0, sizeof(*ereaderinfo));

    ereaderinfo->documentCount = count_documents_in_xochitl();
    ereaderinfo->batteryLevel = get_battery_level();
}
