//
// Created by void001 on 9/29/18.
// Converts the extracted ssf theme to fcitx theme

#include <QSettings>
#include <QDir>
#include <iostream>
#include <codecvt>
#include <locale>
#include <string>
#include <iconv.h>
#include <QProcess>
#include <wait.h>
#include <unistd.h>
#include <QVector>
#include <QTextCodec>
#include <QtGui/QPixmap>

#define MAX_CHARS 255

char convert_from_keys[][MAX_CHARS] = {
        "General/zhongwen_first_color",
};

char convert_to_keys[][MAX_CHARS] = {

};


int do_convert(char *skindir) {
    /*
     * First convert config file to UTF8 encoding
     */

    // TODO: remove the iconv call
    QDir dir(skindir);
    QProcess process;
    QStringList args;
    args.append("--from-code=UTF16");
    args.append("--to-code=UTF8");
    args.append(dir.filePath("skin.ini"));
    args.append("--output=" + dir.filePath("skin.ini.u8"));
    qint64 pid;

    process.startDetached("iconv", args,  NULL, &pid);
    usleep(10);

    QSettings ssfconf(dir.filePath("skin.ini.u8"), QSettings::IniFormat);
    ssfconf.setIniCodec("UTF-8");
    QSettings fcitxconf(dir.filePath("fcitx_skin.conf"), QSettings::NativeFormat);
    fcitxconf.setIniCodec("UTF-8");
    fcitxconf.clear();
    QStringList list = ssfconf.allKeys();

    std::cout << dir.filePath("skin.ini").toStdString() << std::endl;
    const char *fileName = dir.filePath("skin.ini").toStdString().c_str();

    if(process.exitCode() != 0) {
        std::cout << "Exit code not zero, code = " << process.exitCode() << std::endl;
    }

    for(auto x : list) {
        auto key = x.toStdWString();
        std::wcout << key << std::endl;
    }

    fcitxconf.beginGroup("SkinInfo");
    fcitxconf.setValue("Name", "[CONVERT FROM SSF]" + ssfconf.value("skin_name").toString());
    fcitxconf.setValue("Version", ssfconf.value("skin_version").toString());
    fcitxconf.setValue("Author", ssfconf.value("skin_author").toString() + "<" + ssfconf.value("skin_email").toString() + ">");
    fcitxconf.setValue("Desc", ssfconf.value("skin_info").toString());
    fcitxconf.endGroup();

    ssfconf.beginGroup("Display");
    fcitxconf.beginGroup("SkinFont");
    fcitxconf.setValue("FontSize", ssfconf.value("font_size").toInt());
    fcitxconf.setValue("MenuFontSize", ssfconf.value("font_size").toInt() - 3);
    fcitxconf.setValue("RespectDPI", true);

    // zhongwen_first_color => FirstCandColor
    auto val = ssfconf.value("zhongwen_first_color");
    uint32_t ptr = val.toString().toInt(nullptr, 16);
    // printf("%X", ptr);
    char buf[MAX_CHARS];
    sprintf(buf, "%d %d %d", (ptr & 0xFF0000) >> 16, (ptr & 0x00FF00) >> 8, ptr & 0x0000FF);
    // fcitxconf.setValue("FirstCandColor", QString(buf));

    // zhongwen_first_color => UserPhraseColor
    val = ssfconf.value("zhongwen_color");
    ptr = val.toString().toInt(nullptr, 16);
    sprintf(buf, "%d %d %d", (ptr & 0xFF0000) >> 16, (ptr & 0x00FF00) >> 8, ptr & 0x0000FF);
    fcitxconf.setValue("UserPhraseColor", QString(buf));

    val = ssfconf.value("comphint_color");
    ptr = val.toString().toInt(nullptr, 16);
    sprintf(buf, "%d %d %d", (ptr & 0xFF0000) >> 16, (ptr & 0x00FF00) >> 8, ptr & 0x0000FF);
    fcitxconf.setValue("CodeColor", QString(buf));
    fcitxconf.setValue("OtherColor", QString(buf));
    fcitxconf.setValue("FirstCandColor", QString(buf));

    fcitxconf.setValue("TipColor", "37 191 237");
    // fcitxconf.setValue("OtherColor", "255 223 231"); // OtherColor = FirstCandidColor = comphint color
    fcitxconf.setValue("ActiveMenuColor", "204 41 76");
    fcitxconf.setValue("InactiveMenuColor", "255 223 231");
    fcitxconf.setValue("IndexColor", "128 0 0");
    // TODO: Input Color is the color for the input string(那一串拼音), but I don't found it in sogou config
    fcitxconf.setValue("InputColor", "244 208 0");

    fcitxconf.endGroup();
    ssfconf.endGroup();

    ssfconf.beginGroup("Scheme_H1");
    fcitxconf.beginGroup("SkinInputBar");

    fcitxconf.setValue("BackImg", ssfconf.value("pic"));
    fcitxconf.setValue("FillHorizontal", "Copy");
    fcitxconf.setValue("CursorColor", "255 255 255");
    // TODO: 搞清楚 Margin 的关系，优先搞清楚 SkinInputBar 的
    // TODO: Read the fcitx skin code and draw the margin picture

    // we need to open the picture first
    QString input_bar_file_name = ssfconf.value("pic").toString();
    QImage input_bar_pixmap(dir.filePath(input_bar_file_name));
    if(input_bar_pixmap.isNull()) {
        fprintf(stderr, "FAIL TO OPEN file %s", input_bar_file_name.toStdString().c_str());
        exit(-1);
    }
    int pixw = input_bar_pixmap.width();
    int pixh = input_bar_pixmap.height();
    std::cout << "W=" << pixw << std::endl << "H=" << pixh << std::endl;

    int marge_left = ssfconf.value("pinyin_marge").toStringList().at(2).toInt();

    fcitxconf.setValue("MarginLeft", marge_left);
    fcitxconf.setValue("MarginRight", pixw / 2);
    // TODO: the +- 50 is magic here, we need to change it
    fcitxconf.setValue("MarginBottom", pixh / 2 - 50);
    fcitxconf.setValue("MarginTop", pixh / 2 + 50);
    fcitxconf.setValue("InputPos", 0);
    fcitxconf.setValue("OutputPos", 4);
    // TODO: generate back arrow and forward arrow for fcitx
    // TODO: STUB NOW
    fcitxconf.setValue("BackArrow", "prev.png");
    fcitxconf.setValue("ForwardArrow", "next.png");
    fcitxconf.setValue("BackArrowX", 140);
    fcitxconf.setValue("BackArrowY", 175);
    fcitxconf.setValue("ForwardArrowX", 130);
    fcitxconf.setValue("ForwardArrowY", 175);

    fcitxconf.endGroup();
    ssfconf.endGroup();

    ssfconf.beginGroup("StatusBar");
    fcitxconf.beginGroup("SkinMainBar");

    fcitxconf.setValue("BackImg", ssfconf.value("pic"));
    QStringList cn_en_list = ssfconf.value("cn_en").toStringList();
    // TODO: QSettings IS SHIT! The below line won't output anything
    std::cout << ssfconf.value("cn_en").toString().split(",").join(",").toStdString() << std::endl;
    fcitxconf.setValue("Active", cn_en_list.at(0));
    fcitxconf.setValue("Eng", cn_en_list.at(1));
    fcitxconf.setValue("Logo", "logo.png");

    fcitxconf.setValue("MarginLeft", 45);
    fcitxconf.setValue("MarginRight", 50);
    fcitxconf.setValue("MarginBottom", 25);
    fcitxconf.setValue("MarginTop", 40);

    fcitxconf.endGroup();
    ssfconf.endGroup();

    fcitxconf.beginGroup("SkinKeyboard");
    fcitxconf.setValue("KeyColor", "105 21 20");
    fcitxconf.endGroup();

    fcitxconf.beginGroup("SkinTrayIcon");
    fcitxconf.setValue("Active", "active.png");
    fcitxconf.setValue("Inactive", "inactive.png");
    fcitxconf.endGroup();

    fcitxconf.beginGroup("SkinMenu");
    fcitxconf.setValue("BackImg", "menu.png");
    fcitxconf.setValue("MarginLeft", 50);
    fcitxconf.setValue("MarginRight", 180);
    fcitxconf.setValue("MarginBottom", 162);
    fcitxconf.setValue("MarginTop", 27);
    fcitxconf.setValue("ActiveColor", "255 223 231");
    fcitxconf.setValue("LineColor", "255 223 231");
    fcitxconf.setValue("FillHorizontal", "Copy");
    fcitxconf.endGroup();

    return 0;
}

