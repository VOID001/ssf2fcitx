//
// Created by void001 on 9/29/18.
//

#include <cstdio>
#include <openssl/aes.h>
#include <QByteArray>
#include <iostream>

#define MAX_BYTE 1024 * 1024

int do_uncompress(const unsigned char *ssfbin, int size, QByteArray&);
int do_extract(QByteArray& input, const char *dirname);
int do_convert(const char *skindir);

// usage: take the input file name from command line arg, and produce dump.out in the current directory
int main(int argc, char **argv) {
    // FILE *fin = stdin;
    FILE *fin = fopen(argv[1], "rb");
    unsigned char buf[MAX_BYTE];

    fread((void *)buf, sizeof(unsigned char), MAX_BYTE, fin);
    fseek(fin, 0, SEEK_END);
    size_t file_size = ftell(fin);
    rewind(fin);
    fclose(fin);

    // First get the raw data pack
    QByteArray unpacked;
    do_uncompress(buf, file_size, unpacked);

    // std::cout << unpacked.data() << std::endl;

    // Then we get each file and write the config
    if(argv[2] == NULL) {
        do_extract(unpacked, "skin_dir");
        do_convert("skin_dir");
        printf("Skin saved to %s", "skin_dir");
    } else {
        do_extract(unpacked, argv[2]);
        do_convert(argv[2]);
        printf("Skin saved to %s\n", argv[2]);
    }


    // TODO: trans_config();

    return 0;
}
