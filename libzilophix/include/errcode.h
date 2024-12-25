#ifndef ERROR_HEADER_INCLUDED
#define ERROR_HEADER_INCLUDED

#include <stdbool.h>
#include <stdint.h>

#define ERROR_OK                                            0x0000      /* �G���[�ł͂Ȃ����Ƃ����� */
#define ERROR_UNKNOWN                                       0xffff      /* �����s���̃G���[�ł��邱�Ƃ����� */

/* �t�@�C���ǂݍ��݃G���[ */
#define ERROR_FILE_ACCESS_FAILED_TO_READ_UINT8              0x0010      /* 8�r�b�g�����̓ǂݍ��݂Ɏ��s���� */
#define ERROR_FILE_ACCESS_FAILED_TO_READ_UINT16             0x0011      /* 16�r�b�g�����̓ǂݍ��݂Ɏ��s���� */
#define ERROR_FILE_ACCESS_FAILED_TO_READ_UINT32             0x0012      /* 32�r�b�g�����̓ǂݍ��݂Ɏ��s���� */
#define ERROR_FILE_ACCESS_FAILED_TO_READ_INT16              0x0013      /* �����t��16�r�b�g�����̓ǂݍ��݂Ɏ��s���� */
#define ERROR_FILE_ACCESS_FAILED_TO_READ_INT32              0x0014      /* �����t��32�r�b�g�����̓ǂݍ��݂Ɏ��s���� */
#define ERROR_FILE_ACCESS_FAILED_TO_READ_CHAR               0x0015      /* ASCII�����̓ǂݍ��݂Ɏ��s���� */
#define ERROR_FILE_ACCESS_FAILED_TO_READ_BOOL               0x0016      /* �^�U�l�̓ǂݍ��݂Ɏ��s���� */

/* �t�@�C���������݃G���[ */
#define ERROR_FILE_ACCESS_FAILED_TO_WRITE_UINT8             0x0017      /* 8�r�b�g�����̏������݂Ɏ��s���� */
#define ERROR_FILE_ACCESS_FAILED_TO_WRITE_UINT16            0x0018      /* 16�r�b�g�����̏������݂Ɏ��s���� */
#define ERROR_FILE_ACCESS_FAILED_TO_WRITE_UINT32            0x0019      /* 32�r�b�g�����̏������݂Ɏ��s���� */
#define ERROR_FILE_ACCESS_FAILED_TO_WRITE_INT16             0x001a      /* �����t��16�r�b�g�����̏������݂Ɏ��s���� */
#define ERROR_FILE_ACCESS_FAILED_TO_WRITE_INT32             0x001b      /* �����t��32�r�b�g�����̏������݂Ɏ��s���� */
#define ERROR_FILE_ACCESS_FAILED_TO_WRITE_CHAR              0x001c      /* ASCII�����̏������݂Ɏ��s���� */
#define ERROR_FILE_ACCESS_FAILED_TO_WRITE_BOOL              0x001d      /* �^�U�l�̏������݂Ɏ��s���� */

/* �r�b�g�X�g���[���̃G���[ */
#define ERROR_BIT_STREAM_CANNOT_ALLOCATE_MEMORY             0x0020      /* �r�b�g�X�g���[���ŕK�v�ȗ̈�̃������A���P�[�V�����Ɏ��s���� */
#define ERROR_BIT_STREAM_NOT_READ_MODE                      0x0021      /* �r�b�g�X�g���[�����ǂݍ��݃��[�h�łȂ��ɂ�������炸�A�ǂݍ��݃��[�h��p�̊֐����Ăяo���ꂽ */
#define ERROR_BIT_STREAM_NOT_WRITE_MODE                     0x0022      /* �r�b�g�X�g���[�����������݃��[�h�łȂ��ɂ�������炸�A�������݃��[�h��p�̊֐����Ăяo���ꂽ */

/* LMS�t�B���^�̃G���[ */
#define ERROR_LMS_CANNOT_ALLOCATE_MEMORY                    0x0030      /* LMS�t�B���^�ŕK�v�ȗ̈�̃������A���P�[�V�����Ɏ��s���� */
#define ERROR_LMS_INVALID_FILTER_TAPS                       0x0031      /* LMS�t�B���^�̃^�b�v�����L���͈͊O�ł����� */
#define ERROR_LMS_UNSUPPORTED_PCM_BITS                      0x0032      /* LMS�t�B���^�ŏ������悤�Ƃ����M�����Ή����Ă��Ȃ��ʎq���r�b�g���ł����� */

/* �������\����̃G���[ */
#define ERROR_POLYNOMIAL_PREDICTOR_CANNOT_ALLOCATE_MEMORY   0x0040  /* �������\����ŕK�v�ȗ̈�̃������A���P�[�V�����Ɏ��s���� */

/* �u���b�N�̃G���[ */
#define ERROR_BLOCK_CANNOT_ALLOCATE_MEMORY                  0x0050      /* �u���b�N�ŕK�v�ȗ̈�̃������A���P�[�V�����Ɏ��s���� */

/* �T�u�u���b�N�̃G���[ */
#define ERROR_SUB_BLOCK_CANNOT_ALLOCATE_MEMORY              0x0060      /* �T�u�u���b�N�ŕK�v�ȗ̈�̃������A���P�[�V�����Ɏ��s���� */

/* �G���g���s�[�������̃G���[ */
#define ERROR_RICE_CODING_INVALID_PARAMETER                 0x0070      /* �G���g���s�[�������ɗp����ꂽ�p�����[�^���s���ȃp�����[�^�ł����� */

/* �f�R�[�h�G���[ */
#define ERROR_DECODER_CANNOT_ALLOCATE_MEMORY                0x0080      /* �f�R�[�_�ŕK�v�ȗ̈�̃������A���P�[�V�����Ɏ��s���� */
#define ERROR_DECODER_FAILED_TO_OPEN_FILE                   0x0081      /* �f�R�[�h���悤�Ƃ����t�@�C�����J���Ȃ����� */
#define ERROR_DECODER_INVALID_MAGIC_NUMBER                  0x0082      /* �f�R�[�h���悤�Ƃ����t�@�C���̃}�W�b�N�i���o�[��ZilophiX�̂��̂ł͂Ȃ����� */
#define ERROR_DECODER_UNSUPPORTED_FORMAT_VERSION            0x0083      /* �f�R�[�h���悤�Ƃ����t�@�C���Ɋ܂܂�Ă���ZilophiX�f�[�^�̃o�[�W�������T�|�[�g�ΏۊO�̃o�[�W�����ł����� */

/* �G���R�[�h�G���[ */
#define ERROR_ENCODER_CANNOT_ALLOCATE_MEMORY                0x0090      /* �G���R�[�_�[�ŕK�v�ȗ̈�̃������A���P�[�V�����Ɏ��s���� */
#define ERROR_ENCODER_FAILED_TO_OPEN_FILE                   0x0091      /* �G���R�[�h��t�@�C�����o�C�i����������(wb)���[�h�ŊJ���Ȃ����� */

typedef uint16_t error_code;

/*!
 * @brief           �w�肳�ꂽ�G���[�R�[�h�ŃG���[�����|�[�g���܂��B
 * @param error     �G���[�R�[�h
 */
void report_error(error_code error);

/*!
 * @brief           �Ō�ɔ��������G���[�̃G���[�R�[�h���擾���܂��B
 * @return          �G���[�R�[�h
 */
error_code get_last_error_code();

/*!
 * @brief           �Ō�ɔ��������G���[�̃G���[�R�[�h���擾���܂��B
 * @param value     true�Ȃ�G���[�������ɑ����Ƀv���O�������I�����܂��Bfalse�Ȃ�A�v���O�������I�����܂���B
 */
void set_on_error_exit(bool value);

#endif