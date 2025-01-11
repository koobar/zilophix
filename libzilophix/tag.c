#include "./include/file_access.h"
#include "./include/tag.h"
#include <stdlib.h>

/*!
 * @brief           Write picture binary data.
 * @param *file     File
 * @param *tag      Pointer of tag informations.
 */
static void tag_write_picture(FILE* file, tag* tag) {
    uint32_t offset;

    if (tag == NULL || tag->picture_size == 0 || tag->picture == NULL) {
        return;
    }

    write_uint8(file, TAG_ID_PICTURE);
    write_uint32(file, tag->picture_size);
    write_uint8(file, tag->picture_format);
    for (offset = 0; offset < tag->picture_size; ++offset) {
        write_uint8(file, tag->picture[offset]);
    }
}

/*!
 * @brief           Read image data from file.
 * @param *file     File
 * @param **tag     Pointer of pointer of tag informations.
 * @param size      Size of image data.
 */
static void tag_read_picture(FILE* file, tag** tag, uint32_t size) {
    uint32_t offset;

    /* Allocate memory */
    (*tag)->picture_size = (uint32_t)size;
    (*tag)->picture = (uint8_t*)malloc(sizeof(uint8_t) * size);
    if ((*tag)->picture == NULL) {
        return;
    }

    /* Read image data */
    (*tag)->picture_format = read_uint8(file);
    for (offset = 0; offset < size; ++offset) {
        (*tag)->picture[offset] = read_uint8(file);
    }
}

/*!
 * @brief       Checks whether the string specified in the first argument ends with the string specified in the second argument, but is not case sensitive.
 * @param *str  string
 * @param *end  ends with
 * @return      Returns true if argument-1 ends with the string in argument-2, false otherwise.
 */
static int str_ends_with(const char* str, const char* end) {
    size_t len = strlen(str);
    size_t endlen = strlen(end);

    if (len < endlen) {
        return 0;
    }

    return _stricmp(str + len - endlen, end) == 0;
}

/*!
 * @brief                   Initialize tag information.
 * @param *tag              Pointer of tag information.
 */
void tag_init(tag* tag) {
    const char* unknown = "\0";
    const uint16_t default_integer = 0;
    tag->title = unknown;
    tag->album = unknown;
    tag->artist = unknown;
    tag->subtitle = unknown;
    tag->publisher = unknown;
    tag->composer = unknown;
    tag->songwriter = unknown;
    tag->conductor = unknown;
    tag->copyright = unknown;
    tag->comment = unknown;
    tag->album_artist = unknown;
    tag->genre = unknown;
    tag->year = default_integer;
    tag->track_number = default_integer;
    tag->track_count = default_integer;
    tag->disc = default_integer;
    tag->picture_size = 0;
    tag->picture_format = TAG_PICTURE_FORMAT_NONE;
    tag->picture = NULL;
}

/*!
 * @brief                   Release tag information.
 * @param *tag              Pointer of tag information.
 */
void tag_free(tag* tag) {
    if (tag == NULL) {
        return;
    }

    if (tag->picture != NULL) {
        free(tag->picture);
    }
}

/*!
 * @brief                   Set image file to tag informations.
 * @param *tag              Pointer of tag information.
 * @param *picture_file     Picture file.
 */
void tag_set_picture(tag* tag, FILE* picture_file) {
    fpos_t size, offset;
    if (picture_file == NULL) {
        return;
    }

    /* Get size of picture file. */
    fseek(picture_file, 0, SEEK_END);
    fgetpos(picture_file, &size);

    /* Rewind */
    rewind(picture_file);

    /* Allocate memory */
    tag->picture_size = (uint32_t)size;
    tag->picture = (uint8_t*)malloc(sizeof(uint8_t) * (uint32_t)size);
    if (tag->picture == NULL) {
        return;
    }

    /* Read picture file */
    for (offset = 0; offset < size; ++offset) {
        tag->picture[offset] = read_uint8(picture_file);
    }

    fclose(picture_file);
}

/*!
 * @brief                   Set image file to tag informations.
 * @param *tag              Pointer of tag information.
 * @param *picture_file     Path of image file.
 */
void tag_set_picture_from_path(tag* tag, const char* path) {
    FILE* file = NULL;
    errno_t err;

    err = fopen_s(&file, path, "rb");
    if (err != 0) {
        return;
    }

    if (str_ends_with(path, ".png")) {
        tag->picture_format = TAG_PICTURE_FORMAT_PNG;
    }
    else if (str_ends_with(path, ".jpg") || str_ends_with(path, ".jpeg")) {
        tag->picture_format = TAG_PICTURE_FORMAT_JPG;
    }
    else if (str_ends_with(path, ".bmp")) {
        tag->picture_format = TAG_PICTURE_FORMAT_BMP;
    }

    tag_set_picture(tag, file);
}

/*!
 * @brief                   Read tag informations.
 * @param *file             file
 * @param **tag             Pointer of pointer of tag information.
 */
void tag_read(FILE* file, tag** ptag) {
    uint8_t num_tag_info, i, id;
    uint32_t size;

    if (read_bool(file)) {
        *ptag = (tag*)malloc(sizeof(tag));

        if (*ptag == NULL) {
            return;
        }

        /* Initialize */
        tag_init(*ptag);

        /* Get the number of tag informations */
        num_tag_info = read_uint8(file);

        /* Ç∑Ç◊ÇƒÇÃÉ^ÉOÇì«Ç›çûÇﬁ */
        for (i = 0; i < num_tag_info; ++i) {
            id = read_uint8(file);              /* Read ID */
            size = read_uint32(file);           /* Read bytes */

            switch (id) {
            case TAG_ID_TITLE:
                (*ptag)->title = read_string(file, size);
                break;
            case TAG_ID_ALBUM:
                (*ptag)->album = read_string(file, size);
                break;
            case TAG_ID_ARTIST:
                (*ptag)->artist = read_string(file, size);
                break;
            case TAG_ID_ALBUM_ARTIST:
                (*ptag)->album_artist = read_string(file, size);
                break;
            case TAG_ID_SUBTITLE:
                (*ptag)->subtitle = read_string(file, size);
                break;
            case TAG_ID_PUBLISHER:
                (*ptag)->publisher = read_string(file, size);
                break;
            case TAG_ID_COMPOSER:
                (*ptag)->composer = read_string(file, size);
                break;
            case TAG_ID_SONGWRITER:
                (*ptag)->songwriter = read_string(file, size);
                break;
            case TAG_ID_CONDUCTOR:
                (*ptag)->conductor = read_string(file, size);
                break;
            case TAG_ID_COPYRIGHT:
                (*ptag)->copyright = read_string(file, size);
                break;
            case TAG_ID_GENRE:
                (*ptag)->genre = read_string(file, size);
                break;
            case TAG_ID_YEAR:
                (*ptag)->year = read_uint16(file);
                break;
            case TAG_ID_TRACK_NUM:
                (*ptag)->track_number = read_uint16(file);
                break;
            case TAG_ID_TRACK_CNT:
                (*ptag)->track_count = read_uint16(file);
                break;
            case TAG_ID_DISC:
                (*ptag)->disc = read_uint16(file);
                break;
            case TAG_ID_COMMENT:
                (*ptag)->comment = read_string(file, size);
                break;
            case TAG_ID_RATE:
                (*ptag)->rate = read_uint16(file);
                break;
            case TAG_ID_PICTURE:
                tag_read_picture(file, ptag, size);
                break;
            }
        }
    }
    else {
        (*ptag) = NULL;
    }
}

/*!
 * @brief                   Write tag informations to file.
 * @param *file             file
 * @param *tag              Pointer of tag information.
 */
void tag_write(FILE* file, tag* tag) {
    uint32_t size;

    /* Write tag check code */
    write_bool(file, tag != NULL);

    if (tag != NULL) {
        /* Write the number of tags. */
        write_uint8(file, TAG_INFO_COUNT);

        /*  TAG Structure
         *  -------------------------------------
         *  | ID (1Byte) | Size (4Bytes) | Data |
         *  -------------------------------------
         */

        /* Title */
        size = (uint32_t)(strlen(tag->title) + 1);
        write_uint8(file, TAG_ID_TITLE);
        write_uint32(file, size);
        write_string(file, tag->title, size);

        /* Album name */
        size = (uint32_t)(strlen(tag->album) + 1);
        write_uint8(file, TAG_ID_ALBUM);
        write_uint32(file, size);
        write_string(file, tag->album, size);

        /* Artist name */
        size = (uint32_t)(strlen(tag->artist) + 1);
        write_uint8(file, TAG_ID_ARTIST);
        write_uint32(file, size);
        write_string(file, tag->artist, size);

        /* Album artist name */
        size = (uint32_t)(strlen(tag->album_artist) + 1);
        write_uint8(file, TAG_ID_ALBUM_ARTIST);
        write_uint32(file, size);
        write_string(file, tag->album_artist, size);

        /* Subtitle */
        size = (uint32_t)(strlen(tag->subtitle) + 1);
        write_uint8(file, TAG_ID_SUBTITLE);
        write_uint32(file, size);
        write_string(file, tag->subtitle, size);

        /* Publisher */
        size = (uint32_t)(strlen(tag->publisher) + 1);
        write_uint8(file, TAG_ID_PUBLISHER);
        write_uint32(file, size);
        write_string(file, tag->publisher, size);

        /* Composer */
        size = (uint32_t)(strlen(tag->composer) + 1);
        write_uint8(file, TAG_ID_COMPOSER);
        write_uint32(file, size);
        write_string(file, tag->composer, size);

        /* Songwriter */
        size = (uint32_t)(strlen(tag->songwriter) + 1);
        write_uint8(file, TAG_ID_SONGWRITER);
        write_uint32(file, size);
        write_string(file, tag->songwriter, size);

        /* Conductor */
        size = (uint32_t)(strlen(tag->conductor) + 1);
        write_uint8(file, TAG_ID_CONDUCTOR);
        write_uint32(file, size);
        write_string(file, tag->conductor, size);

        /* Copyright */
        size = (uint32_t)(strlen(tag->copyright) + 1);
        write_uint8(file, TAG_ID_COPYRIGHT);
        write_uint32(file, size);
        write_string(file, tag->copyright, size);

        /* Genre */
        size = (uint32_t)(strlen(tag->genre) + 1);
        write_uint8(file, TAG_ID_GENRE);
        write_uint32(file, size);
        write_string(file, tag->genre, size);

        /* Comment */
        size = (uint32_t)(strlen(tag->comment) + 1);
        write_uint8(file, TAG_ID_COMMENT);
        write_uint32(file, size);
        write_string(file, tag->comment, size);

        /* Year */
        size = 2;
        write_uint8(file, TAG_ID_YEAR);
        write_uint32(file, size);
        write_uint16(file, tag->year);

        /* Track number */
        size = 2;
        write_uint8(file, TAG_ID_TRACK_NUM);
        write_uint32(file, size);
        write_uint16(file, tag->track_number);

        /* Track count */
        size = 2;
        write_uint8(file, TAG_ID_TRACK_CNT);
        write_uint32(file, size);
        write_uint16(file, tag->track_count);

        /* Disc number */
        size = 2;
        write_uint8(file, TAG_ID_DISC);
        write_uint32(file, size);
        write_uint16(file, tag->disc);

        /* Rate */
        size = 2;
        write_uint8(file, TAG_ID_RATE);
        write_uint32(file, size);
        write_uint16(file, tag->rate);

        /* Picture */
        tag_write_picture(file, tag);
    }
}