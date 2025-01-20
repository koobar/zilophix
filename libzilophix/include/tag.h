#ifndef TAG_HEADER_INCLUDED
#define TAG_HEADER_INCLUDED

#define TAG_ID_TITLE               0x01
#define TAG_ID_ALBUM               0x02
#define TAG_ID_ARTIST              0x03
#define TAG_ID_ALBUM_ARTIST        0x04
#define TAG_ID_SUBTITLE            0x05
#define TAG_ID_PUBLISHER           0x06
#define TAG_ID_COMPOSER            0x07
#define TAG_ID_SONGWRITER          0x08
#define TAG_ID_CONDUCTOR           0x09
#define TAG_ID_COPYRIGHT           0x0a
#define TAG_ID_GENRE               0x0b
#define TAG_ID_YEAR                0x0d
#define TAG_ID_TRACK_NUM           0x0e
#define TAG_ID_TRACK_CNT           0x0f
#define TAG_ID_DISC                0x10
#define TAG_ID_RATE                0x11
#define TAG_ID_COMMENT             0x12
#define TAG_ID_PICTURE             0x13

#define TAG_PICTURE_FORMAT_NONE    0x00
#define TAG_PICTURE_FORMAT_PNG     0x01
#define TAG_PICTURE_FORMAT_JPG     0x01
#define TAG_PICTURE_FORMAT_BMP     0x02

#define TAG_INFO_COUNT             18

#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    const char* title;
    const char* album;
    const char* artist;
    const char* album_artist;
    const char* subtitle;
    const char* publisher;
    const char* composer;
    const char* songwriter;
    const char* conductor;
    const char* copyright;
    const char* genre;
    const char* comment; 
    uint16_t year;
    uint16_t track_number;
    uint16_t track_count;
    uint16_t disc;
    uint16_t rate;
    uint32_t picture_size;
    uint8_t picture_format;
    uint8_t* picture;
    
} tag;

/*!
 * @brief                   Initialize tag information.
 * @param *tag              Pointer to tag information.
 */
void tag_init(tag* tag);

/*!
 * @brief                   Release tag information.
 * @param *tag              Pointer to tag information.
 */
void tag_free(tag* tag);

/*!
 * @brief                   Set image file to tag informations.
 * @param *tag              Pointer to tag information.
 * @param *picture_file     Picture file.
 */
void tag_set_picture(tag* tag, FILE* picture_file);

/*!
 * @brief                   Set image file to tag informations.
 * @param *tag              Pointer to tag information.
 * @param *picture_file     Path of image file.
 */
void tag_set_picture_from_path(tag* tag, const char* path);

/*!
 * @brief                   Read tag informations.
 * @param *file             file
 * @param **tag             Pointer to pointer of tag information.
 */
void tag_read(FILE* file, tag** tag);

/*!
 * @brief                   Write tag informations to file.
 * @param *file             file
 * @param *tag              Pointer to tag information.
 */
void tag_write(FILE* file, tag* tag);

#endif
