#ifndef SEEK_TABLE_HEADER_INCLUDED
#define SEEK_TABLE_HEADER_INCLUDED

#include "bit_stream.h"
#include "polynomial_predictor.h"
#include "sslms.h"
#include "file_access.h"
#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint32_t stream_offset;
    int32_t* poly_history;
    int32_t* sslms_weights;
    int32_t* sslms_history;
} seek_info;

typedef struct {
    uint8_t num_ch;
    seek_info* info_a;
    seek_info* info_b;
    seek_info* info_c;
    seek_info* info_d;
    seek_info* info_e;
    seek_info* info_f;
    seek_info* info_g;
    seek_info* info_h;
} seek_table;

void seek_info_save(seek_info* info, const bit_stream* stream, const sslms** sslms_filters, const polynomial_predictor** poly_predictors, uint8_t num_ch);
void seek_info_load(const seek_info* info, bit_stream* stream, sslms** sslms_filters, polynomial_predictor** poly_predictors, uint8_t num_ch);
void seek_table_write(const seek_table* table, FILE* file);
void seek_table_read(seek_table* table, FILE* file);

void seek_info_save(seek_info* info, const bit_stream* stream, const sslms** sslms_filters, const polynomial_predictor** poly_predictors, uint8_t num_ch) {

}

#endif
