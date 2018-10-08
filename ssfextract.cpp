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

int do_uncompress(const unsigned char *ssfbin, int size, QByteArray& unpackedarr) {
    static const unsigned char aeskey[] = {
            0x52,0x36,0x46,0x1A,0xD3,0x85,0x03,0x66,
            0x90,0x45,0x16,0x28,0x79,0x03,0x36,0x23,
            0xDD,0xBE,0x6F,0x03,0xFF,0x04,0xE3,0xCA,
            0xD5,0x7F,0xFC,0xA3,0x50,0xE4,0x9E,0xD9
    };

    AES_KEY key;
    AES_set_decrypt_key(aeskey, 256, &key);

    unsigned char iv[AES_BLOCK_SIZE] = {
            0xE0,0x7A,0xAD,0x35,0xE0,0x90,0xAA,0x03,
            0x8A,0x51,0xFD,0x05,0xDF,0x8C,0x5D,0x0F
    };

    std::vector<unsigned char> out(size);

    AES_cbc_encrypt(ssfbin + 8, out.data(), size - 8, &key, iv, AES_DECRYPT);

    // We now get the decrypted data

    fprintf(stderr, "Decrypt success, writing to stdout\n");

    // FILE *fout = stdout;
    FILE *fout = fopen("dump.out", "wb");

    // do the byte order swap
    uint32_t *ptr = (uint32_t *)out.data();
    uint32_t swapped = __builtin_bswap32 (ptr[0]);
    ptr[0] = swapped;

    // We need to do the uncompress now

    QByteArray rawdata = QByteArray((const char *)out.data(), size);
    // QByteArray unpackedarr;
    unpackedarr = qUncompress(rawdata);

    fwrite((void *)unpackedarr.data(), sizeof(unsigned char), unpackedarr.size(), fout);
    fclose(fout);


    return 0;
}

int do_extract(QByteArray& input, char *dirname) {
    QDataStream ds(&input, QIODevice::ReadOnly);

    // parse the size and header size
    uint32_t size, hdr_size;
    ds.setByteOrder(QDataStream::LittleEndian);
    ds >> size;
    ds >> hdr_size;

    std::vector<uint32_t> offsets;
    for(int i = 0; i < (hdr_size / sizeof(uint32_t)); i++) {
        uint32_t offset;
        ds >> offset;
        offsets.push_back(offset);
    }

    // Well traditional STL goes well here :(
    QString dirstr(dirname);
    // dirstr.replace(".ssf", "");
    QDir skindir;
    skindir.mkdir(dirstr);

    for(auto ii = offsets.begin(); ii != offsets.end(); ii++) {
        uint32_t offset = *ii;

        // seek the offset to certain value
        ds.device()->seek((quint64)offset);
        uint32_t name_len;
        ds >> name_len;

        char* utf16str = new char[name_len];
        ds.readRawData(utf16str, name_len);
        QString filename = QString::fromUtf16((quint16*)utf16str, name_len / 2);
        delete[] utf16str;

        std::cout << "Create file " + filename.toLower().toStdString() << std::endl;

        uint32_t content_len;
        ds >> content_len;
        // Now we can read the content

        skindir.cd(dirstr);
        std::cout << skindir.filePath(filename).toLower().toStdString() << std::endl;
        QFile f(skindir.filePath(filename).toLower());
        f.open(QFile::ReadWrite);
        QByteArray buf;
        buf.resize(content_len);
        ds.readRawData(buf.data(), content_len);
        f.write(buf);
        f.close();
    }
    return 0;
}


