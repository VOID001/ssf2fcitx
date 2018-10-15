//
// Created by void001 on 9/29/18.
//

#include <cstdio>
#include <openssl/aes.h>
#include <QByteArray>
#include <iostream>
#include <getopt.h>
#include <cerrno>

#define MAX_BYTE (1024 * 1024)

int do_uncompress(const unsigned char *ssfbin, int size, QByteArray &);

int do_extract(QByteArray &input, const char *dirname);

int do_convert(const char *skindir);

void help(char *progname);

const struct option opts[] = {
        {"input",  required_argument, nullptr, 0},
        {"output", required_argument, nullptr, 0},
        {nullptr,  0,                 nullptr, 0}
};

const char default_dest[] = "skin_converted";

QMessageLogger LOG;

// usage: take the input file name from command line arg, and produce dump.out in the current directory
int main(int argc, char **argv) {
    int optindex = 0;
    char *src_file = nullptr;
    char *dest_file = nullptr;
    int ret;
    while ((ret = getopt(argc, argv, "i:o:h")) != -1) {
        switch (ret) {
            case 0:
                help(argv[0]);
                break;
            case 1:
                break;
            case 'i':
                src_file = optarg;
                break;
            case 'o':
                dest_file = optarg;
                break;
            case 'h':
                help(argv[0]);
                return 1;
            default:
                break;
        }
    }
    if (src_file == nullptr) {
        fprintf(stderr, "you must specify the ssf file to convert (see -h for options).\n");
        return -1;
    }
    if (dest_file == nullptr) {
        // use the default option
        dest_file = (char *) default_dest;
    }

    FILE *fin = fopen(src_file, "rb");
    if (!fin) {
        if (errno == ENOENT) {
            fprintf(stderr, "error opening file %s, %s\n", src_file, strerror(errno));
            return -1;
        } else {
            fprintf(stderr, "unknown error: %s\n", strerror(errno));
            return -1;
        }
    }

    unsigned char buf[MAX_BYTE];

    fread((void *) buf, sizeof(unsigned char), MAX_BYTE, fin);
    fseek(fin, 0, SEEK_END);
    auto file_size = ftell(fin);
    rewind(fin);
    fclose(fin);

    // First get the raw data pack
    QByteArray unpacked;
    do_uncompress(buf, static_cast<int>(file_size), unpacked);

    do_extract(unpacked, dest_file);
    do_convert(dest_file);
    printf("Skin saved to %s.\n", dest_file);

    return 0;
}

void help(char *progname) {
    printf("Usage: %s -i input.ssf -o output_dir\n", progname);
    printf("-i path to input ssf file\n");
    printf("-o path to output skin dir (default: skin_converted)\n");
}
