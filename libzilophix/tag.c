#include "./include/file_access.h"
#include "./include/tag.h"
#include <stdlib.h>

/*!
 * @brief           �^�O�\���̂̃����o�ϐ�picture�Ɋi�[���ꂽ�摜�̃o�C�i���f�[�^���A�^�O�Ƃ��ăt�@�C���ɏo�͂��܂��B
 * @param *file     �t�@�C���n���h��
 * @param *tag      �^�O�\���̂̃n���h��
 */
static void tag_write_picture(FILE* file, tag* tag) {
    uint32_t offset;

    if (tag == NULL || tag->picture_size == 0 || tag->picture == NULL) {
        return;
    }

    /* �摜�f�[�^���������� */
    write_uint8(file, TAG_ID_PICTURE);
    write_uint32(file, tag->picture_size);
    write_uint8(file, tag->picture_format);
    for (offset = 0; offset < tag->picture_size; ++offset) {
        write_uint8(file, tag->picture[offset]);
    }
}

/*!
 * @brief           �t�@�C������摜�f�[�^��ǂݍ��݂܂��B
 * @param *file     �t�@�C���n���h��
 * @param **tag     �^�O�\���̂̃|�C���^�̃|�C���^
 * @param size      �摜�f�[�^�̃T�C�Y
 */
static void tag_read_picture(FILE* file, tag** tag, uint32_t size) {
    uint32_t offset;

    /* �摜�f�[�^���i�[����̈���m�� */
    (*tag)->picture_size = (uint32_t)size;
    (*tag)->picture = (uint8_t*)malloc(sizeof(uint8_t) * size);
    if ((*tag)->picture == NULL) {
        return;
    }

    /* �摜�f�[�^��ǂݍ��� */
    (*tag)->picture_format = read_uint8(file);
    for (offset = 0; offset < size; ++offset) {
        (*tag)->picture[offset] = read_uint8(file);
    }
}

/*!
 * @brief       1�ڂ̈����Ɏw�肳�ꂽ�����񂪁A2�ڂ̈����Ɏw�肳�ꂽ������ŏI�����Ă��邩�ǂ����𔻒肵�܂��B�A���A�啶���Ə���������ʂ��܂���B
 * @param *str  ������
 * @param *end  ����1���A���̕�����ŏI�����Ă��邩�ǂ����𔻒肵�܂��B
 * @return      ����1������2�̕�����ŏI�����Ă����true���A�����łȂ����false��Ԃ��܂��B
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
 * @brief                   �^�O�\���̂����������܂��B
 * @param *tag              �^�O�\���̂̃n���h��
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
 * @brief                   �^�O�\���̂�������܂��B
 * @param *tag              �^�O�\���̂̃n���h��
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
 * @brief                   �^�O�\���̂ɉ摜�t�@�C����ݒ肵�܂��B
 * @param *tag              �^�O�\���̂̃n���h��
 * @param *picture_file     �摜�t�@�C���̃n���h��
 */
void tag_set_picture(tag* tag, FILE* picture_file) {
    fpos_t size, offset;
    if (picture_file == NULL) {
        return;
    }

    /* �摜�t�@�C���̃T�C�Y���擾 */
    fseek(picture_file, 0, SEEK_END);
    fgetpos(picture_file, &size);

    /* �摜�t�@�C���̐擪�ɖ߂� */
    rewind(picture_file);

    /* �摜�t�@�C���̃f�[�^���i�[����̈���m�� */
    tag->picture_size = (uint32_t)size;
    tag->picture = (uint8_t*)malloc(sizeof(uint8_t) * (uint32_t)size);
    if (tag->picture == NULL) {
        return;
    }

    /* �摜�t�@�C���̃f�[�^��ǂݍ��� */
    for (offset = 0; offset < size; ++offset) {
        tag->picture[offset] = read_uint8(picture_file);
    }

    fclose(picture_file);
}

/*!
 * @brief                   �^�O�\���̂ɉ摜�t�@�C����ݒ肵�܂��B
 * @param *tag              �^�O�\���̂̃n���h��
 * @param *picture_file     �摜�t�@�C���̃p�X
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
 * @brief                   �^�O��ǂݍ��݁A�^�O�\���̂Ɋi�[���܂��B
 * @param *file             �t�@�C���̃n���h��
 * @param **tag             �^�O�\���̂̃|�C���^�̃|�C���^
 */
void tag_read(FILE* file, tag** ptag) {
    uint8_t num_tag_info, i, id;
    uint32_t size;

    if (read_bool(file)) {
        *ptag = (tag*)malloc(sizeof(tag));

        if (*ptag == NULL) {
            return;
        }

        /* �^�O���������� */
        tag_init(*ptag);

        /* �^�O�����擾 */
        num_tag_info = read_uint8(file);

        /* ���ׂẴ^�O��ǂݍ��� */
        for (i = 0; i < num_tag_info; ++i) {
            id = read_uint8(file);              /* �^�OID��ǂݍ��� */
            size = read_uint32(file);           /* �^�O�̃f�[�^�̃o�C�g����ǂݍ��� */

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
 * @brief                   �^�O���������݂܂��B
 * @param *file             �t�@�C���̃n���h��
 * @param *tag             �^�O�\���̂̃n���h��
 */
void tag_write(FILE* file, tag* tag) {
    uint32_t size;

    /* �^�O�̑��ݔ���p�t���O���������� */
    write_bool(file, tag != NULL);

    if (tag != NULL) {
        /* �^�O���̑������������� */
        write_uint8(file, TAG_INFO_COUNT);

        /*  �^�O���̃f�[�^�\�� 
         *  -------------------------------------
         *  | ID (1Byte) | Size (4Bytes) | Data |
         *  -------------------------------------
         * 
         *  ID�̓^�O�̎�ނ������B�Ⴆ�΁A0x01�Ȃ�Ȃ̃^�C�g�����A0x02�Ȃ�A���o�����������B
         *  Size �́AData �̃o�C�g���������B������f�[�^�̏ꍇ�͂��̓��e���ςł��邪�A
         *  �g���b�N�ԍ��┭�s�N�ȂǁA�Œ蒷�̐����f�[�^���i�[�����ꍇ�ASize��ǂݍ��܂Ȃ��Ă�
         *  Data �̃T�C�Y��ϐ��̌^���琄�肷�邱�Ƃ��\�ł���B�������A�f�[�^�\����A�Œ蒷��
         *  �f�[�^�ł����Ă��A�K�� Size �͏������܂Ȃ���΂Ȃ�Ȃ��B
         */

        /* �^�C�g�� */
        size = (uint32_t)(strlen(tag->title) + 1);
        write_uint8(file, TAG_ID_TITLE);
        write_uint32(file, size);
        write_string(file, tag->title, size);

        /* �A���o���� */
        size = (uint32_t)(strlen(tag->album) + 1);
        write_uint8(file, TAG_ID_ALBUM);
        write_uint32(file, size);
        write_string(file, tag->album, size);

        /* �A�[�e�B�X�g�� */
        size = (uint32_t)(strlen(tag->artist) + 1);
        write_uint8(file, TAG_ID_ARTIST);
        write_uint32(file, size);
        write_string(file, tag->artist, size);

        /* �A���o���A�[�e�B�X�g�� */
        size = (uint32_t)(strlen(tag->album_artist) + 1);
        write_uint8(file, TAG_ID_ALBUM_ARTIST);
        write_uint32(file, size);
        write_string(file, tag->album_artist, size);

        /* �T�u�^�C�g�� */
        size = (uint32_t)(strlen(tag->subtitle) + 1);
        write_uint8(file, TAG_ID_SUBTITLE);
        write_uint32(file, size);
        write_string(file, tag->subtitle, size);

        /* ���s�� */
        size = (uint32_t)(strlen(tag->publisher) + 1);
        write_uint8(file, TAG_ID_PUBLISHER);
        write_uint32(file, size);
        write_string(file, tag->publisher, size);

        /* ��Ȏ� */
        size = (uint32_t)(strlen(tag->composer) + 1);
        write_uint8(file, TAG_ID_COMPOSER);
        write_uint32(file, size);
        write_string(file, tag->composer, size);

        /* �쎌�� */
        size = (uint32_t)(strlen(tag->songwriter) + 1);
        write_uint8(file, TAG_ID_SONGWRITER);
        write_uint32(file, size);
        write_string(file, tag->songwriter, size);

        /* �w���� */
        size = (uint32_t)(strlen(tag->conductor) + 1);
        write_uint8(file, TAG_ID_CONDUCTOR);
        write_uint32(file, size);
        write_string(file, tag->conductor, size);

        /* ���쌠�\�� */
        size = (uint32_t)(strlen(tag->copyright) + 1);
        write_uint8(file, TAG_ID_COPYRIGHT);
        write_uint32(file, size);
        write_string(file, tag->copyright, size);

        /* �W������ */
        size = (uint32_t)(strlen(tag->genre) + 1);
        write_uint8(file, TAG_ID_GENRE);
        write_uint32(file, size);
        write_string(file, tag->genre, size);

        /* �R�����g */
        size = (uint32_t)(strlen(tag->comment) + 1);
        write_uint8(file, TAG_ID_COMMENT);
        write_uint32(file, size);
        write_string(file, tag->comment, size);

        /* ���s�N */
        size = 2;
        write_uint8(file, TAG_ID_YEAR);
        write_uint32(file, size);
        write_uint16(file, tag->year);

        /* �g���b�N�ԍ� */
        size = 2;
        write_uint8(file, TAG_ID_TRACK_NUM);
        write_uint32(file, size);
        write_uint16(file, tag->track_number);

        /* �g���b�N�� */
        size = 2;
        write_uint8(file, TAG_ID_TRACK_CNT);
        write_uint32(file, size);
        write_uint16(file, tag->track_count);

        /* �f�B�X�N�ԍ� */
        size = 2;
        write_uint8(file, TAG_ID_DISC);
        write_uint32(file, size);
        write_uint16(file, tag->disc);

        /* �]�� */
        size = 2;
        write_uint8(file, TAG_ID_RATE);
        write_uint32(file, size);
        write_uint16(file, tag->rate);

        /* �摜�̏������� */
        tag_write_picture(file, tag);
    }
}