//
// Created by void001 on 9/29/18.
// reference: https://github.com/KDE/kimtoy/blob/master/kssf.cpp

#include <cstdio>
#include <iostream>
#include <openssl/aes.h>
#include <vector>
#include <unitypes.h>
#include <QByteArray>
#include <QDataStream>
#include <QFile>
#include <QDir>

#define MAX_BYTE (4096 * 1024)

int do_uncompress(const unsigned char *ssfbin, int size, QByteArray &unpackedarr) {
    static const unsigned char aeskey[] = {
            0x52, 0x36, 0x46, 0x1A, 0xD3, 0x85, 0x03, 0x66,
            0x90, 0x45, 0x16, 0x28, 0x79, 0x03, 0x36, 0x23,
            0xDD, 0xBE, 0x6F, 0x03, 0xFF, 0x04, 0xE3, 0xCA,
            0xD5, 0x7F, 0xFC, 0xA3, 0x50, 0xE4, 0x9E, 0xD9
    };

    AES_KEY key;
    AES_set_decrypt_key(aeskey, 256, &key);

    unsigned char iv[AES_BLOCK_SIZE] = {
            0xE0, 0x7A, 0xAD, 0x35, 0xE0, 0x90, 0xAA, 0x03,
            0x8A, 0x51, 0xFD, 0x05, 0xDF, 0x8C, 0x5D, 0x0F
    };

    unsigned char *out;
    out = new unsigned char[MAX_BYTE];  // max 4 MB ssf data

    // TODO: if the pack is not encrypted, just decompress it
    bool encrypted_archive = false;
    if (ssfbin[0] == 'S' && ssfbin[1] == 'k' && ssfbin[2] == 'i' && ssfbin[3] == 'n') {
        encrypted_archive = true;
    }

    if (encrypted_archive) {
        AES_cbc_encrypt(ssfbin + 8, out, static_cast<size_t>(size - 8), &key, iv, AES_DECRYPT);
        auto *ptr = (uint32_t *) out;
        uint32_t swapped = __builtin_bswap32(ptr[0]);
        ptr[0] = swapped;

        // do the uncompress now
        QByteArray rawdata = QByteArray((const char *) out, size);
        unpackedarr = qUncompress(rawdata);

    } else {
        std::cout << "Sorry, but we don't directly support unencrypted archive now" << std::endl;
        std::cout
                << "Please unzip the ssf file and use option -c (convert only) and set the input path to the unzipped dir."
                << std::endl;
        exit(-1);
    }

    delete (out);
    return 0;
}

int do_extract(QByteArray &input, const char *dirname) {
    QDataStream ds(&input, QIODevice::ReadOnly);

    // parse the size and header size
    uint32_t size, hdr_size;
    ds.setByteOrder(QDataStream::LittleEndian);
    ds >> size;
    ds >> hdr_size;

    std::vector<uint32_t> offsets;
    for (int i = 0; i < (hdr_size / sizeof(uint32_t)); i++) {
        uint32_t offset;
        ds >> offset;
        offsets.push_back(offset);
    }

    // Well traditional STL goes well here :(
    QString dirstr(dirname);
    // dirstr.replace(".ssf", "");
    QDir skindir;
    skindir.mkdir(dirstr);

    for (unsigned int offset : offsets) {
        // seek the offset to certain value
        ds.device()->seek((quint64) offset);
        uint32_t name_len;
        ds >> name_len;

        auto utf16str = new char[name_len];
        ds.readRawData(utf16str, name_len);
        QString filename = QString::fromUtf16((quint16 *) utf16str, name_len / 2);
        delete[] utf16str;

        // std::cout << "Create file " + filename.toLower().toStdString() << std::endl;

        uint32_t content_len;
        ds >> content_len;
        // Now we can read the content

        skindir.cd(dirstr);
        // std::cout << skindir.filePath(filename).toLower().toStdString() << std::endl;
        QFile f(skindir.absoluteFilePath(filename.toLower()));
        f.open(QFile::ReadWrite);
        QByteArray buf;
        buf.resize(content_len);
        ds.readRawData(buf.data(), content_len);
        f.write(buf);
        f.close();
    }
    return 0;
}


