#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <urlmon.h>
#include <wlanapi.h>
#include <wininet.h>
#include <iphlpapi.h>
#include <psapi.h>
#include "webview2loader.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <sys/stat.h>
#include <initguid.h>
#include <unknwn.h>
#include "WebView2.h"

static const int TASKBAR_H = 36;
static const int TITLEBAR_H = 26;

static const COLORREF CLR_TASKBAR = RGB(192, 192, 192);
static const COLORREF CLR_TITLE_ACTIVE = RGB(200, 80, 0);
static const COLORREF CLR_TITLE_INACT = RGB(160, 100, 40);
static COLORREF CLR_WIN_BG = RGB(192, 192, 192);

enum AppType { APP_ABOUT, APP_SETTINGS, APP_CALC, APP_NOTEPAD, APP_REDSTEAM, APP_FILES, APP_CODE, APP_KCPP, APP_TERM, APP_DRAU, APP_KML, APP_UPD, APP_TASKMGR, APP_PAINT, APP_PPT, APP_WEB, APP_INET };
enum Lang { LANG_EN=0, LANG_RU, LANG_FR, LANG_DE, LANG_COUNT };
static int g_lang = LANG_EN;

struct Strings {
    const wchar_t* start;
    const wchar_t* calculator;
    const wchar_t* notepad;
    const wchar_t* settings;
    const wchar_t* about;
    const wchar_t* red_steam;
    const wchar_t* exit_s;
    const wchar_t* my_computer;
    const wchar_t* lang_label;
    const wchar_t* lang_en;
    const wchar_t* lang_ru;
    const wchar_t* lang_fr;
    const wchar_t* lang_de;
    const wchar_t* settings_title;
    const wchar_t* sys_info;
    const wchar_t* display;
    const wchar_t* audio;
    const wchar_t* keyboard;
    const wchar_t* cpu;
    const wchar_t* ram;
    const wchar_t* wallpaper;
    const wchar_t* sound;
    const wchar_t* on;
    const wchar_t* off;
    const wchar_t* about_os;
    const wchar_t* about_feat;
    const wchar_t* feat_windows;
    const wchar_t* feat_calc;
    const wchar_t* feat_notepad;
    const wchar_t* feat_icons;
    const wchar_t* feat_start;
    const wchar_t* feat_taskbar;
    const wchar_t* gaming_platform;
    const wchar_t* minebild;
    const wchar_t* brightness;
    const wchar_t* file_manager;
    const wchar_t* new_folder;
    const wchar_t* new_file;
    const wchar_t* back;
    const wchar_t* folder_name;
    const wchar_t* file_name;
    const wchar_t* sys_error;
    const wchar_t* sys_error_msg;
    const wchar_t* code_editor;
    const wchar_t* kcpp;
    const wchar_t* save;
    const wchar_t* saved;
    const wchar_t* delete_s;
    const wchar_t* copy_s;
    const wchar_t* paste_s;
    const wchar_t* run;
    const wchar_t* compile_run;
    const wchar_t* wclass;
    const wchar_t* version;
    const wchar_t* open_s;
    const wchar_t* rename_s;
    const wchar_t* cancel;
    const wchar_t* new_txt;
    const wchar_t* new_cpp;
    const wchar_t* new_kcpp;
    const wchar_t* new_py;
    const wchar_t* new_html;
    const wchar_t* new_json;
    const wchar_t* new_md;
    const wchar_t* new_bat;
    const wchar_t* new_asm;
    const wchar_t* search_s;
    const wchar_t* sys_readonly;
    const wchar_t* sys_layer_info;
    const wchar_t* folder;
    const wchar_t* term;
    const wchar_t* drau;
    const wchar_t* sec;
    const wchar_t* sec_trig;
    const wchar_t* alert_title;
    const wchar_t* alert_file;
    const wchar_t* alert_layer;
    const wchar_t* alert_restored;
    const wchar_t* alert_crit;
    const wchar_t* alert_high;
    const wchar_t* alert_med;
    const wchar_t* alert_languages;
    const wchar_t* last_patrol;
    const wchar_t* pass_label;
    const wchar_t* pass_btn_set;
    const wchar_t* pass_btn_clear;
    const wchar_t* pass_wrong;
    const wchar_t* pass_login_hint;
};

static const Strings g_strings[LANG_COUNT] = {
    {
        L"Start", L"Calculator", L"Notepad", L"Settings", L"About", L"Red Steam", L"Shutdown", L"My Computer",
        L"Language", L"English", L"Russian", L"French", L"German",
        L"Settings", L"System Information",
        L"Display:", L"Audio:", L"Keyboard:", L"CPU:", L"RAM:", L"Wallpaper:", L"Sound:",
        L"ON", L"OFF",
        L"About KashmaOS 7", L"Features:",
        L"Draggable Windows", L"Calculator", L"Notepad",
        L"Desktop Icons", L"Start Menu", L"Taskbar with Clock",
        L"Gaming Platform", L"Minebild", L"Brightness",
        L"Files", L"New Folder", L"New File", L"Back", L"Folder:", L"File:",
        L"Access Denied", L"Cannot modify system files!",
        L"Code Editor", L"KCPP", L"Save", L"Saved!", L"Delete", L"Copy", L"Paste", L"Run", L"Compile & Run", L"Classes", L"v4.10.2.0",
        L"Open", L"Rename", L"Cancel", L"New TXT", L"New CPP", L"New KCPP", L"New PY", L"New HTML", L"New JSON", L"New MD", L"New BAT", L"New ASM", L"Search...", L"SYSTEM FILE / READ-ONLY", L"200 defense layers armed", L"Folder", L"Terminal", L"Drau (3D Cube)", L"Protection (Rust Core)", L"System tamper detected! Files restored from secure storage.",
        L"SECURITY ALERT", L"File:", L"Layer:", L"Status: RESTORED from secure backup", L"CRITICAL: RAM emergency rebuild", L"HIGH: multi-hash breach", L"MEDIUM: honeypot triggered", L"Rust Core + ASM + C + Ada + Pascal + Fortran + Honeypot + SystemROM + Guardian + 200 LAYERS", L"Last patrol:",
        L"Password:", L"Set", L"Clear", L"Wrong password", L"Enter password to continue"
    },
    {
        L"\u041f\u0443\u0441\u043a",
        L"\u041a\u0430\u043b\u044c\u043a\u0443\u043b\u044f\u0442\u043e\u0440",
        L"\u0411\u043b\u043e\u043a\u043d\u043e\u0442",
        L"\u041d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0438",
        L"\u041e \u0441\u0435\u0431\u0435",
        L"Red Steam",
        L"Shutdown",
        L"\u041c\u043e\u0439 \u043a\u043e\u043c\u043f\u044c\u0442\u0435\u0440",
        L"\u042f\u0437\u044b\u043a",
        L"English",
        L"\u0420\u0443\u0441\u0441\u043a\u0438\u0439",
        L"\u0424\u0440\u0430\u043d\u0446\u0443\u0437\u0441\u043a\u0438\u0439",
        L"\u041d\u0435\u043c\u0435\u0446\u043a\u0438\u0439",
        L"\u041d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0438",
        L"\u0421\u0438\u0441\u0442\u0435\u043c\u043d\u0430\u044f \u0438\u043d\u0444\u043e\u0440\u043c\u0430\u0446\u0438\u044f",
        L"\u0414\u0438\u0441\u043f\u043b\u0435\u0439:",
        L"\u0410\u0443\u0434\u0438\u043e:",
        L"\u041a\u043b\u0430\u0432\u0438\u0430\u0442\u0443\u0440\u0430:",
        L"\u041f\u0440\u043e\u0446.:",
        L"\u041e\u0437\u0443:",
        L"\u0424\u043e\u043d:",
        L"\u0417\u0432\u0443\u043a:",
        L"\u0412\u043a\u043b",
        L"\u0412\u044b\u043a\u043b",
        L"\u041e \u043d\u0430\u0448\u0435\u043c \u0441\u0438\u0441\u0442\u0435\u043c\u0435",
        L"\u0425\u0430\u0440\u0430\u043a\u0442\u0435\u0440\u0438\u0441\u0442\u0438\u043a\u0430:",
        L"\u0424\u0438\u043a\u0441\u0438\u0440\u043e\u0432\u0430\u043d\u043e \u043e\u043a\u043d\u043e",
        L"\u0412\u0438\u0434\u0436\u0435\u0442",
        L"\u0411\u043b\u043e\u043a\u043d\u043e\u0442",
        L"\u0418\u043a\u043e\u043d\u043a\u0438 \u043d\u0430 \u0440\u0430\u0431\u043e\u0447\u0435\u043c \u0441\u0442\u043e\u043b\u0435",
        L"\u041c\u0435\u043d\u044e \u00ab\u041f\u0443\u0441\u043a\u00bb",
        L"\u041f\u0430\u043d\u0435\u043b\u044c \u0437\u0430\u0434\u0430\u0447",
        L"\u0418\u0433\u0440\u043e\u0432\u0430\u044f \u043f\u043b\u0430\u0442\u0444\u043e\u0440\u043c\u0430",
        L"\u041c\u0438\u043d\u0435\u0431\u0438\u043b\u0434",
        L"\u042f\u0440\u043a\u043e\u0441\u0442\u044c",
        L"\u0424\u0430\u0439\u043b\u044b", L"\u041d\u043e\u0432\u0430\u044f \u043f\u0430\u043f\u043a\u0430", L"\u041d\u043e\u0432\u044b\u0439 \u0444\u0430\u0439\u043b", L"\u041d\u0430\u0437\u0430\u0434", L"\u041f\u0430\u043f\u043a\u0430:", L"\u0424\u0430\u0439\u043b:",
        L"\u0414\u043e\u0441\u0442\u0443\u043f \u0437\u0430\u043f\u0440\u0435\u0449\u0435\u043d", L"\u041d\u0435\u043b\u044c\u0437\u044f \u0438\u0437\u043c\u0435\u043d\u044f\u0442\u044c \u0441\u0438\u0441\u0442\u0435\u043c\u043d\u044b\u0435 \u0444\u0430\u0439\u043b\u044b!",
        L"\u0420\u0435\u0434\u0430\u043a\u0442\u043e\u0440 \u043a\u043e\u0434\u0430", L"KCPP", L"\u0421\u043e\u0445\u0440\u0430\u043d\u0438\u0442\u044c", L"\u0421\u043e\u0445\u0440\u0430\u043d\u0435\u043d\u043e!", L"\u0423\u0434\u0430\u043b\u0438\u0442\u044c", L"\u041a\u043e\u043f\u0438\u0440\u043e\u0432\u0430\u0442\u044c", L"\u0412\u0441\u0442\u0430\u0432\u0438\u0442\u044c", L"\u0417\u0430\u043f\u0443\u0441\u0442\u0438\u0442\u044c", L"\u0421\u043a\u043e\u043c\u043f\u0438\u043b\u0438\u0440\u043e\u0432\u0430\u0442\u044c \u0438 \u0437\u0430\u043f\u0443\u0441\u0442\u0438\u0442\u044c", L"\u041a\u043b\u0430\u0441\u0441\u044b", L"v4.10.2.0",
        L"\u041e\u0442\u043a\u0440\u044b\u0442\u044c", L"\u041f\u0435\u0440\u0435\u0438\u043c\u0435\u043d\u043e\u0432\u0430\u0442\u044c", L"\u041e\u0442\u043c\u0435\u043d\u0430", L"\u041d\u043e\u0432\u044b\u0439 TXT", L"\u041d\u043e\u0432\u044b\u0439 CPP", L"\u041d\u043e\u0432\u044b\u0439 KCPP", L"\u041d\u043e\u0432\u044b\u0439 PY", L"\u041d\u043e\u0432\u044b\u0439 HTML", L"\u041d\u043e\u0432\u044b\u0439 JSON", L"\u041d\u043e\u0432\u044b\u0439 MD", L"\u041d\u043e\u0432\u044b\u0439 BAT", L"\u041d\u043e\u0432\u044b\u0439 ASM", L"\u041f\u043e\u0438\u0441\u043a...", L"\u0421\u0418\u0421\u0422\u0415\u041c\u041d\u042b\u0419 \u0424\u0410\u0419\u041b / \u0442\u043e\u043b\u044c\u043a\u043e \u0447\u0442\u0435\u043d\u0438\u0435", L"200 \u0440\u0443\u0431\u0435\u0436\u0435\u0439 \u0437\u0430\u0449\u0438\u0442\u044b \u0432\u043a\u043b\u044e\u0447\u0435\u043d\u043e", L"\u041f\u0430\u043f\u043a\u0430", L"\u0422\u0435\u0440\u043c\u0438\u043d\u0430\u043b", L"Drau (3D \u043a\u0443\u0431)", L"\u0417\u0430\u0449\u0438\u0442\u0430 (Rust Core)", L"\u041e\u0431\u043d\u0430\u0440\u0443\u0436\u0435\u043d\u043e \u0432\u043c\u0435\u0448\u0430\u0442\u0435\u043b\u044c\u0441\u0442\u0432\u043e \u0432 \u0441\u0438\u0441\u0442\u0435\u043c\u043d\u044b\u0435 \u0444\u0430\u0439\u043b\u044b! \u0424\u0430\u0439\u043b\u044b \u0432\u043e\u0441\u0441\u0442\u0430\u043d\u043e\u0432\u043b\u0435\u043d\u044b \u0438\u0437 \u0437\u0430\u0449\u0438\u0449\u0435\u043d\u043d\u043e\u0433\u043e \u0445\u0440\u0430\u043d\u0438\u043b\u0438\u0449\u0430.",
        L"\u0422\u0420\u0415\u0412\u041e\u0413\u0410 \u0411\u0415\u0417\u041e\u041f\u0410\u0421\u041d\u041e\u0421\u0422\u0418", L"\u0424\u0430\u0439\u043b:", L"\u0420\u0443\u0431\u0435\u0436:", L"\u0421\u0442\u0430\u0442\u0443\u0441: \u0412\u041e\u0421\u0421\u0422\u0410\u041d\u041e\u0412\u041b\u0415\u041d\u041e \u0438\u0437 \u0437\u0430\u0449\u0438\u0449\u0435\u043d\u043d\u043e\u0433\u043e \u0445\u0440\u0430\u043d\u0438\u043b\u0438\u0449\u0430", L"\u041a\u0420\u0418\u0422\u0418\u0427\u041d\u041e: \u0432\u043e\u0441\u0441\u0442\u0430\u043d\u043e\u0432\u043b\u0435\u043d\u0438\u0435 \u0438\u0437 \u043e\u043f\u0435\u0440\u0430\u0442\u0438\u0432\u043d\u043e\u0439 \u043f\u0430\u043c\u044f\u0442\u0438", L"\u0412\u042b\u0421\u041e\u041a\u0418\u0419: \u043d\u0430\u0440\u0443\u0448\u0435\u043d\u0438\u0435 \u043f\u0440\u043e\u0432\u0435\u0440\u043a\u0438 \u0445\u044d\u0448\u0435\u0439", L"\u0421\u0420\u0415\u0414\u041d\u0418\u0419: \u0441\u0440\u0430\u0431\u043e\u0442\u0430\u043b\u0430 \u043b\u043e\u0432\u0443\u0448\u043a\u0430", L"Rust Core + ASM + C + Ada + Pascal + Fortran + Honeypot + SystemROM + Guardian + 200 LAYERS", L"\u041f\u043e\u0441\u043b\u0435\u0434\u043d\u044f\u044f \u043f\u0440\u043e\u0432\u0435\u0440\u043a\u0430:",
        L"\u041f\u0430\u0440\u043e\u043b\u044c:", L"\u0423\u0441\u0442\u0430\u043d\u043e\u0432\u0438\u0442\u044c", L"\u041e\u0447\u0438\u0441\u0442\u0438\u0442\u044c", L"\u041d\u0435\u0432\u0435\u0440\u043d\u044b\u0439 \u043f\u0430\u0440\u043e\u043b\u044c", L"\u0412\u0432\u0435\u0434\u0438\u0442\u0435 \u043f\u0430\u0440\u043e\u043b\u044c \u0434\u043b\u044f \u0432\u0445\u043e\u0434\u0430"
    },
    {
        L"D\u00e9marrer", L"Calculatrice", L"Bloc-notes", L"Param\u00e8tres", L"\u00c0 propos", L"Red Steam", L"Shutdown", L"Poste de travail",
        L"Langue", L"Anglais", L"Russe", L"Fran\u00e7ais", L"Allemand",
        L"Param\u00e8tres", L"Informations syst\u00e8me",
        L"\u00c9cran:", L"Audio:", L"Clavier:", L"Processeur:", L"RAM:", L"Fond d'\u00e9cran:", L"Son:",
        L"Marche", L"\u00c9teint",
        L"\u00c0 propos de KashmaOS 7", L"Fonctionnalit\u00e9s:",
        L"Fen\u00eatres d\u00e9pla\u00e7ables", L"Calculatrice", L"Bloc-notes",
        L"Ic\u00f4nes bureau", L"Menu D\u00e9marrer", L"Barre des t\u00e2ches avec Horloge",
        L"Plateforme de jeu", L"Minebild", L"Luminosit\u00e9",
        L"Fichiers", L"Nouveau dossier", L"Nouveau fichier", L"Retour", L"Dossier:", L"Fichier:",
        L"Acc\u00e8s refus\u00e9", L"Impossible de modifier les fichiers syst\u00e8me!",
        L"\u00c9diteur de code", L"KCPP", L"Enregistrer", L"Enregistr\u00e9!", L"Supprimer", L"Copier", L"Coller", L"Ex\u00e9cuter", L"Compiler & Ex\u00e9cuter", L"Classes", L"v4.10.2.0",
        L"Ouvrir", L"Renommer", L"Annuler", L"Nouveau TXT", L"Nouveau CPP", L"Nouveau KCPP", L"Nouveau PY", L"Nouveau HTML", L"Nouveau JSON", L"Nouveau MD", L"Nouveau BAT", L"Nouveau ASM", L"Recherche...", L"FICHIER SYST\u00c8ME / lecture seule", L"200 couches de d\u00e9fense arm\u00e9es", L"Dossier", L"Terminal", L"Drau (cube 3D)", L"Protection (Rust Core)", L"Intrusion dans les fichiers syst\u00e8me d\u00e9tect\u00e9e ! Fichiers restaur\u00e9s depuis le stockage prot\u00e9g\u00e9.",
        L"ALERTE S\u00c9CURIT\u00c9", L"Fichier:", L"Couche:", L"\u00c9tat: RESTAUR\u00c9 depuis le stockage s\u00e9curis\u00e9", L"CRITIQUE: Rebuild RAM d'urgence", L"\u00c9LEV\u00c9: violation multi-hash", L"MOYEN: pi\u00e8ge d\u00e9clench\u00e9", L"Rust Core + ASM + C + Ada + Pascal + Fortran + Honeypot + SystemROM + Guardian + 200 LAYERS", L"Derni\u00e8re patrouille:",
        L"Mot de passe:", L"D\u00e9finir", L"Effacer", L"Mot de passe incorrect", L"Entrez le mot de passe pour continuer"
    },
    {
        L"Starten", L"Taschenrechner", L"Editor", L"Einstellungen", L"\u00dcber", L"Red Steam", L"Shutdown", L"Arbeitsplatz",
        L"Sprache", L"Englisch", L"Russisch", L"Franz\u00f6sisch", L"Deutsch",
        L"Einstellungen", L"Systeminformation",
        L"Bildschirm:", L"Audio:", L"Tastatur:", L"CPU:", L"RAM:", L"Hintergrund:", L"Ton:",
        L"Ein", L"Aus",
        L"\u00dcber KashmaOS 7", L"Funktionen:",
        L"Ziehbare Fenster", L"Taschenrechner", L"Editor",
        L"Desktopsymbole", L"Startmen\u00fc", L"Taskleiste mit Uhr",
        L"Gaming-Plattform", L"Minebild", L"Helligkeit",
        L"Dateien", L"Neuer Ordner", L"Neue Datei", L"Zur\u00fcck", L"Ordner:", L"Datei:",
        L"Zugriff verweigert", L"Systemdateien k\u00f6nnen nicht ge\u00e4ndert werden!",
        L"Code-Editor", L"KCPP", L"Speichern", L"Gespeichert!", L"L\u00f6schen", L"Kopieren", L"Einf\u00fcgen", L"Ausf\u00fchren", L"Kompilieren & Starten", L"Klassen", L"v4.10.2.0",
        L"Offnen", L"Umbenennen", L"Abbrechen", L"Neuer TXT", L"Neuer CPP", L"Neuer KCPP", L"Neues PY", L"Neues HTML", L"Neues JSON", L"Neues MD", L"Neues BAT", L"Neues ASM", L"Suche...", L"SYSTEMDATEI / nur lesen", L"200 Schutzschichten aktiv", L"Ordner", L"Terminal", L"Drau (3D-W\u00fcrfel)", L"Schutz (Rust Core)", L"Eingriff in Systemdateien erkannt! Dateien aus sicherem Speicher wiederhergestellt.",
        L"SICHERHEITSALARM", L"Datei:", L"Schicht:", L"Status: WIEDERHERGESTELLT aus sicherem Speicher", L"KRITISCH: RAM-Notfallwiederherstellung", L"H\u00d6CHST: Multi-Hash-Verletzung", L"MITTEL: Falle ausgel\u00f6st", L"Rust Core + ASM + C + Ada + Pascal + Fortran + Honeypot + SystemROM + Guardian + 200 LAYERS", L"Letzter Check:",
        L"Passwort:", L"Festlegen", L"Entfernen", L"Falsches Passwort", L"Passwort eingeben"
    }
};

#define S g_strings[g_lang]

struct OsWindow {
    int x, y, w, h;
    bool open, minimized;
    wchar_t title[64];
    AppType app;
    double calcVal;
    double calcMem;
    char calcDisp[64];
    char calcOp;
    bool calcNewNum;
    char notepad[4096];
    int notepadLen;
    char notepadFile[260];
    bool notepadRO;
    char filePath[260];
    char filesPath[260];
    int filesScroll;
    int filesCount;
    char filesList[256][260];
    bool filesIsDir[256];
    wchar_t filesSize[256][24];
    int filesSel;
    bool filesSearch;
    wchar_t searchQ[64];
    char sPaths[256][520];
    bool sIsDir[256];
    int sCount;
    int sScroll;
    char code[16384];
    int codeLen;
    char codeFile[260];
    bool codeModified;
    char kcppPath[260];
    char termBuf[16384];
    int termLen;
    int termScroll;
    float drauRx;
    float drauRy;
    bool drauSpin;
    bool kmlRunning;
    bool kmlHostMode;
    int kmlFocused;
    char kmlHostIp[64];
    char kmlGamePort[16];
    char kmlListenPort[16];
    char kmlHostPort[16];
    char kmlLocalPort[16];
    char kmlLog[8192];
    int kmlLogLen;
    char kmlRoom[8];
    int kmlClientCount;
    char updUrl[512];
    char updLog[8192];
    int updLogLen;
    bool updBusy;
    int updRan;
    char updOkMsg[512];
    wchar_t passText[65];
    int passLen;
    bool passFocus;
    COLORREF paintPix[192][256];
    bool paintDown;
    int paintColor;
    int paintSize;
    int paintLastX, paintLastY;
    bool paintEraser;
    int pptCur;
    int pptCount;
    wchar_t pptTitle[16][64];
    wchar_t pptBody[16][256];
    COLORREF pptBg[16];
    bool pptShow;
    int pptFocus;
    int pptScroll;
    wchar_t inetSsid[8][33];
    bool inetSec[8];
    bool inetKnown[8];
    int inetSig[8];
    bool inetConn[8];
    int inetCount;
    int inetSel;
    int inetScroll;
    bool inetBusy;
    bool inetPassOpen;
    wchar_t inetPass[65];
    int inetPassLen;
    bool inetConnected;
    wchar_t inetStatus[160];
    wchar_t inetIp[48];
    wchar_t inetNetName[64];
    wchar_t inetMsg[160];
};

static OsWindow g_windows[8];
static int g_winCount = 0;
static int g_activeWin = -1;
static bool g_dragging = false;
static int g_dragOffX, g_dragOffY;
static bool g_resizing = false;
static int g_resizeEdge = 0;
static bool g_startOpen = false;
static HWND g_hwnd = NULL;
static HHOOK g_winHook = NULL;
static void KmlAppendLog(OsWindow& w, const char* fmt, ...);
static void UpdAppendLog(OsWindow& w, const char* fmt, ...);
static void RunCmdCapture(const char* cmdline, const char* outFile, bool append, unsigned long* peakKB);
static void OpenWindow(AppType app);
static unsigned __stdcall InetScanThread(void* arg);
static unsigned __stdcall InetConnectThread(void* arg);
static void WebView2Open();
static void SyncWebView();
static bool g_soundOn = true;
static int g_brightness = 100;
static COLORREF g_wallTop = RGB(255, 140, 0);
static COLORREF g_wallBot = RGB(200, 60, 0);
static int g_wallIdx = 0;
static const COLORREF WALL_COLORS2[10][2] = {
    { RGB(255,140,0),  RGB(200,60,0)   },
    { RGB(0,128,128),  RGB(0,60,80)    },
    { RGB(0,0,128),    RGB(0,0,60)     },
    { RGB(128,0,64),   RGB(80,0,30)    },
    { RGB(0,64,0),     RGB(0,30,0)     },
    { RGB(70,90,180),  RGB(30,40,110)  },
    { RGB(140,0,0),    RGB(70,0,0)     },
    { RGB(20,100,160), RGB(10,45,80)   },
    { RGB(120,60,140), RGB(55,28,70)   },
    { RGB(30,30,30),   RGB(10,10,10)   },
};
static int g_wallStyle = 0;      /* 0 = вертикальный градиент, 1 = горизонтальный */
static int g_cursorIdx = 0;      /* стиль курсора */
static int g_settingsPage = 0;   /* раздел настроек: 0 фон, 1 мышь, 2 система, 3 звук, 4 пароль, 5 язык */
static const int SET_SBW = 132;  /* ширина боковой панели настроек */
static char G_DRIVE_ROOT[MAX_PATH];
static const char* G_DRIVE_DEV = "C:\\Users\\kashm\\OneDrive\\Desktop\\KashmaOS\\KashmaOS7_Cpp\\DriveG";

static const char* sysFiles7[] = {"version.txt","config.ini","kernel.c","boot.asm","start.asm","modules.txt","memory.txt","shield_trap.bin","shield_trap2.dat","core_guard.dat","bootlog.txt","syslog.txt","drivers.cfg","registry.dat","cmos.ini","rtc.dat","keymap.ini","swap.sys"};
static const int SYS_FILE_COUNT = 18;

#define LAYER_COUNT 200
#define LAYER_STORE_NAM 0xAA
#define LAYER_STORE_DB1 0x55

static unsigned long g_layersArmed = 0;
static char g_lastLayerInfo[96] = "L000 [Init]";
static unsigned long LayerDigest(const BYTE* d, DWORD len, int layer);

struct DeskFile { char name[260]; bool isDir; int x, y; };
static DeskFile g_deskFiles[128];
static int g_deskCount = 0;
static int g_deskSel = -1;
static int g_deskCtxSel = -1;
static bool g_deskDragging = false;
static int g_deskDragIdx = -1;
static int g_deskDragOffX = 0, g_deskDragOffY = 0;
static int g_secCount = 0;
static int g_nextCheck = 3;
static int g_lastCheckH = 0, g_lastCheckM = 0, g_lastCheckS = 0;
static volatile LONG g_checkBusy = 0;
static bool g_foundTamper = false;
static char g_exePath[MAX_PATH] = {};
static char g_romDir[MAX_PATH] = {};
static HANDLE g_mainMutex = NULL;
static HANDLE g_guardMutex = NULL;
static void RunSecurityCheck();
static void RunSecurityCheckInner();
static bool IsSystemPath(const char* path);
static bool g_alertOpen = false;
static wchar_t g_alertFile[260] = {};
static wchar_t g_alertLayer[128] = {};
static DWORD g_alertTime = 0;
static int g_alertSeverity = 0;

static SOCKET g_kmlListen = INVALID_SOCKET;
static volatile LONG g_kmlRun = 0;
static HANDLE g_kmlThread = NULL;
static volatile LONG g_kmlClientId = 0;
static char g_kmlHostIpCur[64] = "127.0.0.1";
static int g_kmlHostPortCur = 26401;
static int g_kmlGamePortCur = 25565;
static int g_kmlLocalPortCur = 25565;

static const char* OS_VERSION = "v4.10.2.0";
static char g_updBaseUrl[512] = "https://selutine9-source.github.io/kashmaMessenger/upd/";
static volatile LONG g_autoUpd = 1;
static volatile LONG g_updBusy = 0;
static volatile LONG g_updCheckDone = 0;
static volatile LONG g_updAvailable = 0;
static char g_updRemoteVer[64] = "";
static char g_updRemoteFnv[32] = "";
static char g_updRemoteDjb[32] = "";
static bool g_updateRestart = false;
static char g_updateNewExe[MAX_PATH] = "";

enum BootState { BOOT_LOGO, BOOT_LOGIN, BOOT_DESKTOP };
static int g_shutdownSeq = 0;
static int g_keySeq = 0;
static BootState g_boot = BOOT_LOGO;
static unsigned long g_passHash = 0;
static unsigned long g_passHash2 = 0;
static bool g_passSet = false;
static bool g_loginErr = false;

/* ---------- embedded WebView2 (Kashma Internet) ---------- */
static const wchar_t* WV2_URL = L"https://selutine9-source.github.io/kashmaMessenger/home.html";
typedef HRESULT (STDAPICALLTYPE *PFN_Wv2CreateEnv)(PCWSTR, PCWSTR, ICoreWebView2EnvironmentOptions*, ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*);
typedef HRESULT (STDAPICALLTYPE *PFN_Wv2GetVersion)(PCWSTR, LPWSTR*);
static HMODULE g_wvLoader = NULL;
static PFN_Wv2CreateEnv g_wvCreateEnv = NULL;
static PFN_Wv2GetVersion g_wvGetVersion = NULL;
static ICoreWebView2Environment* g_wvEnv = NULL;
static ICoreWebView2Controller* g_wvCtrl = NULL;
static ICoreWebView2* g_wvView = NULL;
static volatile LONG g_wvBusy = 0;
static bool g_wvReady = false;
static bool g_wvFailed = false;
class WebView2PermHandler;
static WebView2PermHandler* g_wvPermHandler = NULL;
static EventRegistrationToken g_wvPermToken = {};
static void WebView2SetupPerms();

class WebView2CtlHandler : public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler {
public:
    volatile LONG m_refs;
    WebView2CtlHandler() { m_refs = 1; }
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_ICoreWebView2CreateCoreWebView2ControllerCompletedHandler)) {
            *ppv = (void*)this; AddRef(); return S_OK;
        }
        *ppv = NULL; return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return (ULONG)InterlockedIncrement(&m_refs); }
    STDMETHODIMP_(ULONG) Release() override { ULONG r = (ULONG)InterlockedDecrement(&m_refs); if (!r) delete this; return r; }
    STDMETHODIMP Invoke(HRESULT result, ICoreWebView2Controller* ctrl) override {
        if (FAILED(result) || !ctrl) { g_wvFailed = true; SyncWebView(); return S_OK; }
        g_wvCtrl = ctrl; ctrl->AddRef();
        g_wvCtrl->get_CoreWebView2(&g_wvView);
        if (!g_wvView) { g_wvFailed = true; SyncWebView(); return S_OK; }
        g_wvReady = true;
        WebView2SetupPerms();
        g_wvCtrl->put_IsVisible(FALSE);
        g_wvCtrl->put_ZoomFactor(1.0);
        g_wvView->Navigate(WV2_URL);
        SyncWebView();
        return S_OK;
    }
};

class WebView2PermHandler : public ICoreWebView2PermissionRequestedEventHandler {
public:
    volatile LONG m_refs;
    WebView2PermHandler() { m_refs = 1; }
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_ICoreWebView2PermissionRequestedEventHandler)) {
            *ppv = (void*)this; AddRef(); return S_OK;
        }
        *ppv = NULL; return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return (ULONG)InterlockedIncrement(&m_refs); }
    STDMETHODIMP_(ULONG) Release() override { ULONG r = (ULONG)InterlockedDecrement(&m_refs); if (!r) delete this; return r; }
    STDMETHODIMP Invoke(ICoreWebView2* sender, ICoreWebView2PermissionRequestedEventArgs* args) override {
        if (!args) return E_POINTER;
        args->put_State(COREWEBVIEW2_PERMISSION_STATE_ALLOW);
        return S_OK;
    }
};

static void WebView2SetupPerms() {
    if (g_wvView) {
        g_wvPermHandler = new WebView2PermHandler();
        g_wvView->add_PermissionRequested(g_wvPermHandler, &g_wvPermToken);
    }
}

class WebView2EnvHandler : public ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler {
public:
    volatile LONG m_refs;
    WebView2EnvHandler() { m_refs = 1; }
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler)) {
            *ppv = (void*)this; AddRef(); return S_OK;
        }
        *ppv = NULL; return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return (ULONG)InterlockedIncrement(&m_refs); }
    STDMETHODIMP_(ULONG) Release() override { ULONG r = (ULONG)InterlockedDecrement(&m_refs); if (!r) delete this; return r; }
    STDMETHODIMP Invoke(HRESULT result, ICoreWebView2Environment* env) override {
        InterlockedExchange(&g_wvBusy, 0);
        if (FAILED(result) || !env) { g_wvFailed = true; return S_OK; }
        g_wvEnv = env; env->AddRef();
        WebView2CtlHandler* h = new WebView2CtlHandler();
        g_wvEnv->CreateCoreWebView2Controller(g_hwnd, h);
        return S_OK;
    }
};

static void ExtractEmbeddedLoaderDll(const char* outPath) {
    HANDLE hf = CreateFileA(outPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hf == INVALID_HANDLE_VALUE) return;
    DWORD wr = 0;
    if (WEBVIEW2LOADER_BIN_LEN > 0 && WEBVIEW2LOADER_BIN_LEN <= 0xFFFFFFFFu)
        WriteFile(hf, WEBVIEW2LOADER_BIN, (DWORD)WEBVIEW2LOADER_BIN_LEN, &wr, NULL);
    CloseHandle(hf);
}

static void WebView2LoadLoader() {
    char dir[MAX_PATH] = {};
    GetModuleFileNameA(NULL, dir, MAX_PATH);
    char* sl = strrchr(dir, '\\');
    if (sl) sl[1] = 0; else dir[0] = 0;
    char p[MAX_PATH];
    wsprintfA(p, "%sWebView2Loader.dll", dir);
    g_wvLoader = LoadLibraryA(p);
    if (!g_wvLoader) g_wvLoader = LoadLibraryA("WebView2Loader.dll");
    if (!g_wvLoader) {
        char tmp[MAX_PATH];
        if (GetTempPathA(MAX_PATH, tmp)) {
            wsprintfA(p, "%sKashmaWebView2Loader.dll", tmp);
            ExtractEmbeddedLoaderDll(p);
            g_wvLoader = LoadLibraryA(p);
        }
    }
    if (g_wvLoader)
        g_wvCreateEnv = (PFN_Wv2CreateEnv)GetProcAddress(g_wvLoader, "CreateCoreWebView2EnvironmentWithOptions");
    if (g_wvLoader)
        g_wvGetVersion = (PFN_Wv2GetVersion)GetProcAddress(g_wvLoader, "GetAvailableCoreWebView2BrowserVersionString");
    if (!g_wvCreateEnv) g_wvFailed = true;
}

static bool WebView2RuntimeAvailable() {
    if (!g_wvGetVersion) return false;
    LPWSTR ver = NULL;
    HRESULT hr = g_wvGetVersion(NULL, &ver);
    bool ok = SUCCEEDED(hr) && ver != NULL;
    if (ver) CoTaskMemFree(ver);
    return ok;
}

static bool WebView2InstallRuntime() {
    char tmp[MAX_PATH];
    if (!GetTempPathA(MAX_PATH, tmp)) tmp[0] = 0;
    char setup[MAX_PATH];
    wsprintfA(setup, "%sKashmaWebView2Setup.exe", tmp);
    DeleteFileA(setup);
    MessageBoxA(g_hwnd, "Браузер-компонент не найден.\r\nОС ставит его сама (первый запуск, 1-2 минуты).", "KashmaOS", MB_OK | MB_ICONINFORMATION);
    if (URLDownloadToFileA(NULL, "https://go.microsoft.com/fwlink/p/?LinkId=2124703", setup, 0, NULL) != S_OK) {
        MessageBoxA(g_hwnd, "Не удалось скачать компонент браузера.\r\nПроверь интернет.", "KashmaOS", MB_OK | MB_ICONERROR);
        return false;
    }
    char cmd[MAX_PATH + 64];
    wsprintfA(cmd, "\"%s\" /silent /install", setup);
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    if (!CreateProcessA(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        MessageBoxA(g_hwnd, "Ошибка запуска установки компонента.", "KashmaOS", MB_OK | MB_ICONERROR);
        return false;
    }
    WaitForSingleObject(pi.hProcess, 600000);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    for (int i = 0; i < 60; i++) {
        if (WebView2RuntimeAvailable()) return true;
        Sleep(500);
    }
    return WebView2RuntimeAvailable();
}

static void WebView2Open() {
    if (g_wvFailed) return;
    if (!g_wvLoader) WebView2LoadLoader();
    if (g_wvFailed) return;
    if (g_wvEnv || g_wvReady) { SyncWebView(); return; }
    if (!WebView2RuntimeAvailable()) {
        if (!WebView2InstallRuntime()) { g_wvFailed = true; return; }
    }
    if (InterlockedCompareExchange(&g_wvBusy, 1, 0) == 0)
        g_wvCreateEnv(NULL, NULL, NULL, new WebView2EnvHandler());
    SyncWebView();
}

static void SyncWebView() {
    if (!g_wvReady || !g_wvCtrl || !g_wvView) return;
    RECT cr;
    GetClientRect(g_hwnd, &cr);
    OsWindow* web = NULL;
    static int lastIdx = -2;
    static RECT lastB = {0,0,0,0};
    static bool lastVis = false;
    for (int i = g_winCount - 1; i >= 0; i--) {
        OsWindow& w = g_windows[i];
        if (w.open && !w.minimized && w.app == APP_WEB) { web = &w; break; }
    }
    if (!web || g_activeWin != (int)(web - g_windows)) {
        if (lastVis) { g_wvCtrl->put_IsVisible(FALSE); lastVis = false; }
        lastIdx = -2;
        return;
    }
    RECT b;
    b.left = web->x;
    b.top = web->y + TITLEBAR_H;
    b.right = web->x + web->w;
    b.bottom = web->y + web->h;
    if (b.left < cr.left) b.left = cr.left;
    if (b.top < cr.top) b.top = cr.top;
    if (b.right > cr.right) b.right = cr.right;
    if (b.bottom > cr.bottom) b.bottom = cr.bottom;
    if (b.right <= b.left || b.bottom <= b.top) {
        if (lastVis) { g_wvCtrl->put_IsVisible(FALSE); lastVis = false; }
        return;
    }
    if (lastIdx != (int)(web - g_windows) ||
        lastB.left != b.left || lastB.top != b.top ||
        lastB.right != b.right || lastB.bottom != b.bottom) {
        g_wvCtrl->put_Bounds(b);
        lastB = b;
        lastIdx = (int)(web - g_windows);
    }
    if (!lastVis) { g_wvCtrl->put_IsVisible(TRUE); lastVis = true; }
}

static bool TextAppActive() {
    if (g_activeWin < 0 || !g_windows[g_activeWin].open) return false;
    AppType a = g_windows[g_activeWin].app;
    return a == APP_NOTEPAD || a == APP_CODE || a == APP_KCPP || a == APP_TERM ||
           a == APP_FILES || a == APP_PPT || a == APP_SETTINGS || a == APP_KML || a == APP_INET;
}

static void ApplyCursor() {
    static const LPCTSTR curShapes[] = { IDC_ARROW, IDC_CROSS, IDC_HAND, IDC_IBEAM, IDC_WAIT };
    HCURSOR hc = LoadCursor(NULL, curShapes[g_cursorIdx % 5]);
    SetClassLongPtr(g_hwnd, GCLP_HCURSOR, (LONG_PTR)hc);
    SetCursor(hc);
}

static void ClampCursorToOs() {
    if (!g_hwnd) return;
    RECT r;
    if (GetWindowRect(g_hwnd, &r)) ClipCursor(&r);
}

static LRESULT CALLBACK WinKeyHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        PKBDLLHOOKSTRUCT kb = (PKBDLLHOOKSTRUCT)lParam;
        bool down = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
        bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        if (kb->vkCode == VK_ESCAPE && down) {
            if (ctrl && shift && !alt) {
                OpenWindow(APP_TASKMGR);
                InvalidateRect(g_hwnd, NULL, FALSE);
                return 1;
            }
            if (alt || ctrl) return 1;
        }
        if (kb->vkCode == VK_TAB && down && alt) return 1;
        if (kb->vkCode == VK_F4 && down && alt) return 1;
        if (kb->vkCode == VK_APPS) return 1;
        if (kb->vkCode == VK_LWIN || kb->vkCode == VK_RWIN) {
            if (g_hwnd && (g_boot == BOOT_DESKTOP || g_boot == BOOT_LOGIN)) {
                if (wParam == WM_KEYDOWN || wParam == WM_KEYUP) g_startOpen = !g_startOpen;
                InvalidateRect(g_hwnd, NULL, FALSE);
            }
            return 1;
        }
    }
    return CallNextHookEx(g_winHook, nCode, wParam, lParam);
}
static bool g_bootAnimDone = false;
static int g_bootFrame = 0;
static wchar_t g_loginInput[64] = L"";
static int g_loginLen = 0;
static bool g_errorOpen = false;
static wchar_t g_errorTitle[128] = L"";
static wchar_t g_errorMsg[256] = L"";

static char g_copyPath[520] = "";
static bool g_copyIsDir = false;
static bool g_iconDragging = false;
static int g_dragIcon = -1;
static int g_iconDragOffX = 0, g_iconDragOffY = 0;

enum CtxAction { CTX_OPEN, CTX_COPY, CTX_PASTE, CTX_DELETE, CTX_RENAME, CTX_NEWFOLDER, CTX_NEWTXT, CTX_NEWCPP, CTX_NEWKCPP, CTX_OPENFILES, CTX_NEWPY, CTX_NEWHTML, CTX_NEWJSON, CTX_NEWMD, CTX_NEWBAT, CTX_NEWASM };
static bool g_ctxOpen = false;
static int g_ctxX = 0, g_ctxY = 0;
static int g_ctxWin = -1;
static int g_ctxCount = 0;
static CtxAction g_ctxActions[14];
static int g_editMode = 0; /* 0=none, 1=rename, 2=kcpp path */
static int g_editWin = -1;
static int g_editIdx = -1;
static wchar_t g_editBuf[260] = L"";
static int g_editLen = 0;

static void WriteFileA_(const char* path, const char* data) {
    HANDLE hf = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (hf != INVALID_HANDLE_VALUE) { DWORD wr; WriteFile(hf, data, (DWORD)strlen(data), &wr, NULL); CloseHandle(hf); }
}

static void InitGDrive() {
char p[520];
    CreateDirectoryA(G_DRIVE_ROOT, NULL);
    wsprintfA(p, "%s\\System", G_DRIVE_ROOT); CreateDirectoryA(p, NULL);
    wsprintfA(p, "%s\\System\\RustCore", G_DRIVE_ROOT); CreateDirectoryA(p, NULL);

    for (int si = 0; si < SYS_FILE_COUNT; si++) {
        wsprintfA(p, "%s\\System\\%s", G_DRIVE_ROOT, sysFiles7[si]);
        SetFileAttributesA(p, FILE_ATTRIBUTE_NORMAL);
    }
    wsprintfA(p, "%s\\System\\version.txt", G_DRIVE_ROOT);
    WriteFileA_(p, "KashmaOS 7.0 Build 20260906\r\nKernel: C kernel v1.0\r\nBoot: ASM real->protected mode\r\nGUI: Win32 API C++ desktop");
    wsprintfA(p, "%s\\System\\config.ini", G_DRIVE_ROOT);
    WriteFileA_(p, "[system]\r\nhostname=kashma-pc\r\nlang=en\r\ntheme=orange\r\nresolution=auto\r\n\r\n[kernel]\r\ntype=c\r\nboot=asm\r\nversion=1.0");
    wsprintfA(p, "%s\\System\\kernel.c", G_DRIVE_ROOT);
    WriteFileA_(p, "// KashmaOS Kernel Source\r\n// See real kernel at KashmaOS7/kernel.c\r\n");
    wsprintfA(p, "%s\\System\\boot.asm", G_DRIVE_ROOT);
    WriteFileA_(p, "; KashmaOS Boot Sector\r\n; See real boot at KashmaOS7/boot.asm\r\n");
    wsprintfA(p, "%s\\System\\start.asm", G_DRIVE_ROOT);
    WriteFileA_(p, "; KashmaOS Protected Mode Switch\r\n; See real start at KashmaOS7/start.asm\r\n");
    wsprintfA(p, "%s\\System\\modules.txt", G_DRIVE_ROOT);
    WriteFileA_(p, "Loaded modules:\r\n- kernel (C, loaded)\r\n- boot (ASM, sector 0)\r\n- vga (text mode)\r\n- keyboard (PS/2)\r\n- fat12 (filesystem)\r\n- kcpp (compiler, added in 4.9.5)\r\n- rustcore (protection, added in 4.10.1.0_2)\r\n- layer200 (defense matrix, 200 layers, added in 4.10.1.0_2)");
    wsprintfA(p, "%s\\System\\memory.txt", G_DRIVE_ROOT);
    WriteFileA_(p, "Memory map:\r\n0x0000 - 0x03FF  IVT\r\n0x0400 - 0x04FF  BDA\r\n0x0500 - 0x7BFF  Free\r\n0x7C00 - 0x7DFF  Boot sector\r\n0x7E00 - 0xFFFF  Free");
    wsprintfA(p, "%s\\System\\bootlog.txt", G_DRIVE_ROOT);
    WriteFileA_(p, "BOOT LOG (last boot)\r\n-------------------\r\n[BIOS] Power-on self test passed\r\n[BOOT] Sector 0 loaded (512 bytes)\r\n[ASM] Real mode init OK\r\n[KERNEL] Protected mode entered\r\n[DRIVERS] GDI + keyboard mounted\r\n[GUI] Desktop ready in 1.4s");
    wsprintfA(p, "%s\\System\\syslog.txt", G_DRIVE_ROOT);
    WriteFileA_(p, "SYSTEM LOG\r\n----------\r\n[10.09.2026 08:00] kernel boot\r\n[10.09.2026 08:00] security core armed\r\n[10.09.2026 08:01] user logged in\r\n[10.09.2026 08:02] file manager started");
    wsprintfA(p, "%s\\System\\drivers.cfg", G_DRIVE_ROOT);
    WriteFileA_(p, "[drivers]\r\ndisplay=vga.asm\r\nkeyboard=ps2.asm\r\ntimer=pit.asm\r\ndisk=fat12.asm\r\naudio=sb16.asm");
    wsprintfA(p, "%s\\System\\registry.dat", G_DRIVE_ROOT);
    WriteFileA_(p, "REGISTRY (read-only)\r\n--------------------\r\nHKCU/Desktop/Wallpaper=orange\r\nHKCU/Desktop/Brightness=100\r\nHKLM/Security/RustCore=armed\r\nHKLM/Security/Layers=200");
    wsprintfA(p, "%s\\System\\cmos.ini", G_DRIVE_ROOT);
    WriteFileA_(p, "[cmos]\r\nrtc_ok=yes\r\nbattery=good\r\nchecksum=0x55AA\r\nboot_order=C:");
    wsprintfA(p, "%s\\System\\rtc.dat", G_DRIVE_ROOT);
    WriteFileA_(p, "RTC DATA\r\n--------\r\nlast_shutdown=normal\r\nuptime_seconds=0\r\nreboot_count=7");
    wsprintfA(p, "%s\\System\\keymap.ini", G_DRIVE_ROOT);
    WriteFileA_(p, "[keyboard]\r\nlayout=en_us\r\nrepeat_rate=30\r\ncaps_lock=off");
    wsprintfA(p, "%s\\System\\swap.sys", G_DRIVE_ROOT);
    WriteFileA_(p, "SWAP AREA v1.0 (locked)\r\nPage 0: free\r\nPage 1: free\r\nPage 2: free\r\n");

    wsprintfA(p, "%s\\Users", G_DRIVE_ROOT); CreateDirectoryA(p, NULL);
    wsprintfA(p, "%s\\Users\\Default", G_DRIVE_ROOT); CreateDirectoryA(p, NULL);
    wsprintfA(p, "%s\\Users\\Default\\Desktop", G_DRIVE_ROOT); CreateDirectoryA(p, NULL);
    wsprintfA(p, "%s\\Users\\Default\\Desktop\\readme.txt", G_DRIVE_ROOT);
    WriteFileA_(p, "Welcome to KashmaOS!\r\n\r\nThis is the virtual G: drive.\r\nSystem files are in G:\\System\\\r\nYou can create your own files here.");
    wsprintfA(p, "%s\\Users\\Default\\Documents", G_DRIVE_ROOT); CreateDirectoryA(p, NULL);
    wsprintfA(p, "%s\\Users\\Default\\Documents\\notes.txt", G_DRIVE_ROOT);
    WriteFileA_(p, "My Notes\r\n--------\r\nWrite anything here...");
    wsprintfA(p, "%s\\Users\\Default\\Pictures", G_DRIVE_ROOT); CreateDirectoryA(p, NULL);

    wsprintfA(p, "%s\\Program Files", G_DRIVE_ROOT); CreateDirectoryA(p, NULL);
    wsprintfA(p, "%s\\Program Files\\list.txt", G_DRIVE_ROOT);
    WriteFileA_(p, "Installed programs:\r\n- Calculator\r\n- Notepad\r\n- Red Steam");

    wsprintfA(p, "%s\\readme.txt", G_DRIVE_ROOT);
    WriteFileA_(p, "=== KashmaOS 7 Virtual Drive ===\r\n\r\nG: is the main disk of KashmaOS.\r\n\r\nFolders:\r\n  System/    - OS kernel and config\r\n  Users/     - User profiles\r\n  Program Files/ - Installed apps\r\n\r\nCreated by KashmaOS7_Cpp");
}

struct DesktopIcon {
    int x, y;
    COLORREF bg;
    AppType app;
};
static DesktopIcon g_icons[] = {
    { 20,  20,  RGB(0,128,0),   APP_SETTINGS },
    { 96,  20,  RGB(128,128,0), APP_CALC },
    { 20,  104, RGB(0,0,128),   APP_NOTEPAD },
    { 96,  104, RGB(192,0,0),   APP_REDSTEAM },
    { 20,  188, RGB(0,100,160), APP_FILES },
    { 96,  188, RGB(160,80,190), APP_DRAU },
    { 20,  272, RGB(20,140,120), APP_KML },
    { 96,  272, RGB(30,70,150),  APP_UPD },
    { 20,  356, RGB(200,60,60),  APP_PAINT },
    { 96,  356, RGB(60,80,180),  APP_PPT },
    { 172, 20,  RGB(50,120,210), APP_WEB },
    { 172, 104, RGB(0,140,160),  APP_INET },
};
static const int ICON_COUNT = sizeof(g_icons)/sizeof(g_icons[0]);

static const wchar_t* GetExtraLabel(int which) {
    if (g_lang == LANG_RU)
        return which ? L"\u041f\u0440\u0435\u0437\u0435\u043d\u0442\u0430\u0446\u0438\u0438" : L"\u041f\u044d\u0439\u043d\u0442";
    if (g_lang == LANG_FR)
        return which ? L"Pr\u00e9sentations" : L"Peinture";
    if (g_lang == LANG_DE)
        return which ? L"Pr\u00e4sentationen" : L"Malen";
    return which ? L"Presentations" : L"Paint";
}

static const wchar_t* GetInternetLabel() {
    if (g_lang == LANG_RU) return L"\u0418\u043d\u0442\u0435\u0440\u043d\u0435\u0442";
    return L"Internet";
}

static const wchar_t* GetIconLabel(int idx) {
    switch (g_icons[idx].app) {
        case APP_SETTINGS: return S.my_computer;
        case APP_CALC: return S.calculator;
        case APP_NOTEPAD: return S.notepad;
        case APP_REDSTEAM: return S.red_steam;
        case APP_FILES: return S.file_manager;
        case APP_KCPP: return S.kcpp;
        case APP_DRAU: return S.drau;
        case APP_KML: return L"KashmaLink";
        case APP_UPD: return L"Update Center";
        case APP_PAINT: return GetExtraLabel(0);
        case APP_PPT: return GetExtraLabel(1);
        case APP_WEB: return GetInternetLabel();
        case APP_INET: return L"\u0418\u043d\u0442\u0435\u0440\u043d\u0435\u0442 \u041a\u0430\u0448\u043c\u0430\u041e\u0421";
        default: return L"?";
    }
}

static const wchar_t* GetTitleForApp(AppType app) {
    switch (app) {
        case APP_CALC: return S.calculator;
        case APP_NOTEPAD: return S.notepad;
        case APP_SETTINGS: return S.settings;
        case APP_ABOUT: return S.about;
        case APP_REDSTEAM: return S.red_steam;
        case APP_FILES: return S.file_manager;
        case APP_CODE: return S.code_editor;
        case APP_KCPP: return S.kcpp;
        case APP_TERM: return S.term;
        case APP_DRAU: return S.drau;
        case APP_UPD: return L"Update Center";
        case APP_TASKMGR: return L"KashmaOS Task Manager";
        case APP_PAINT: return GetExtraLabel(0);
        case APP_PPT: return GetExtraLabel(1);
        case APP_WEB: return GetInternetLabel();
        case APP_INET: return L"\u0418\u043d\u0442\u0435\u0440\u043d\u0435\u0442 \u041a\u0430\u0448\u043c\u0430\u041e\u0421";
        default: return L"Window";
    }
}

static int HitTest(int mx, int my, OsWindow& w) {
    if (mx < w.x || mx > w.x+w.w || my < w.y || my > w.y+w.h) return -1;
    int bw = 18, bh = 18;
    int bx = w.x + w.w - bw - 4, by = w.y + 4;
    if (mx >= bx && mx <= bx+bw && my >= by && my <= by+bh) return 10;
    bx -= bw + 2;
    if (mx >= bx && mx <= bx+bw && my >= by && my <= by+bh) return 11;
    bx -= bw + 2;
    if (mx >= bx && mx <= bx+bw && my >= by && my <= by+bh) return 12;
    if (my >= w.y && my <= w.y + TITLEBAR_H) return 1;
    if (mx > w.x+w.w-12 && my > w.y+w.h-12) return 3;
    if (mx > w.x+w.w-6) return 2;
    return 0;
}

static bool PtIn(int mx, int my, int rx, int ry, int rw, int rh) {
    return mx >= rx && mx <= rx+rw && my >= ry && my <= ry+rh;
}

static void OpenWindow(AppType app) {
    for (int i = 0; i < g_winCount; i++) {
        if (g_windows[i].open && g_windows[i].app == app && app != APP_FILES) {
            g_windows[i].minimized = false;
            wcscpy(g_windows[i].title, GetTitleForApp(app));
            g_activeWin = i;
            return;
        }
    }
    if (g_winCount >= 8) return;
    OsWindow& w = g_windows[g_winCount++];
    memset(&w, 0, sizeof(w));
    w.open = true;
    w.w = 560; w.h = 400;
    w.x = 100 + g_winCount * 30;
    w.y = 50 + g_winCount * 30;
    w.app = app;
    wcscpy(w.title, GetTitleForApp(app));
    g_activeWin = g_winCount - 1;
    if (app == APP_SETTINGS) { w.w = 620; w.h = 480; }
    if (app == APP_CALC) { w.calcVal = 0; w.calcMem = 0; w.calcOp = 0; w.calcNewNum = true; strcpy(w.calcDisp, "0"); }
    if (app == APP_FILES) {
        w.w = 620; w.h = 460;
        strcpy(w.filesPath, G_DRIVE_ROOT);
        CreateDirectoryA(G_DRIVE_ROOT, NULL);
        w.filesScroll = 0;
        w.filesSel = -1;
    }
    if (app == APP_CODE) {
        strcpy(w.codeFile, "");
        w.code[0] = 0; w.codeLen = 0; w.codeModified = false;
    }
    if (app == APP_KCPP) {
        strcpy(w.kcppPath, G_DRIVE_ROOT);
    }
    if (app == APP_NOTEPAD) {
        wsprintfA(w.notepadFile, "%s\\Users\\Default\\Documents\\notes.txt", G_DRIVE_ROOT);
        HANDLE hf = CreateFileA(w.notepadFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hf != INVALID_HANDLE_VALUE) {
            DWORD s2 = GetFileSize(hf, NULL); if (s2 > 4095) s2 = 4095;
            DWORD rd; ReadFile(hf, w.notepad, s2, &rd, NULL);
            w.notepad[rd] = 0; w.notepadLen = (int)rd;
            CloseHandle(hf);
        }
    }
    if (app == APP_DRAU) {
        w.drauRx = 0.6f;
        w.drauRy = 0.4f;
        w.drauSpin = true;
    }
    if (app == APP_KML) {
        w.w = 560; w.h = 440;
        w.kmlRunning = false;
        w.kmlHostMode = true;
        w.kmlFocused = 1;
        strcpy(w.kmlHostIp, "127.0.0.1");
        strcpy(w.kmlGamePort, "25565");
        strcpy(w.kmlListenPort, "26401");
        strcpy(w.kmlHostPort, "26401");
        strcpy(w.kmlLocalPort, "25565");
        w.kmlLog[0] = 0; w.kmlLogLen = 0;
        strcpy(w.kmlRoom, "------");
        w.kmlClientCount = 0;
        KmlAppendLog(w, "[KML] KashmaLink v0.1 ready");
    }
    if (app == APP_UPD) {
        w.w = 600; w.h = 440;
        strncpy(w.updUrl, g_updBaseUrl, 511); w.updUrl[511] = 0;
        w.updLog[0] = 0; w.updLogLen = 0;
        w.updBusy = false; w.updRan = 0;
        strcpy(w.updOkMsg, "");
        UpdAppendLog(w, "[UPD] Update Center ready");
    }
    if (app == APP_TASKMGR) {
        w.w = 600; w.h = 420;
    }
    if (app == APP_INET) {
        w.w = 540; w.h = 440;
        w.inetCount = 0;
        w.inetSel = -1;
        w.inetScroll = 0;
        w.inetBusy = true;
        w.inetPassOpen = false;
        w.inetPass[0] = 0;
        w.inetPassLen = 0;
        w.inetConnected = false;
        w.inetStatus[0] = 0;
        w.inetIp[0] = 0;
        w.inetNetName[0] = 0;
        w.inetMsg[0] = 0;
        _beginthreadex(NULL, 0, InetScanThread, &w, 0, NULL);
    }
    if (app == APP_PAINT) {
        w.w = 700; w.h = 480;
        for (int y = 0; y < 192; y++)
            for (int x = 0; x < 256; x++)
                w.paintPix[y][x] = RGB(255,255,255);
        w.paintColor = 0;
        w.paintSize = 3;
        w.paintEraser = false;
        w.paintDown = false;
    }
    if (app == APP_PPT) {
        w.w = 700; w.h = 480;
        w.pptCur = 0;
        w.pptCount = 1;
        wcscpy(w.pptTitle[0], GetExtraLabel(1));
        wcscpy(w.pptBody[0], L"Welcome to KashmaOS Presentations!\n\nCreate slides and press Play to show them.");
        w.pptBg[0] = RGB(18,80,150);
        w.pptShow = false;
        w.pptFocus = 0;
        w.pptScroll = 0;
    }
    if (app == APP_WEB) {
        w.w = 470; w.h = 300;
        WebView2Open();
    }
}

static const char* MESSENGER_URL = "https://selutine9-source.github.io/kashmaMessenger/home.html";

static void OpenFilesAt(const char* path) {
    OsWindow* w = NULL;
    for (int k = 0; k < g_winCount; k++)
        if (g_windows[k].open && g_windows[k].app == APP_FILES) { w = &g_windows[k]; break; }
    if (!w) { OpenWindow(APP_FILES); w = &g_windows[g_winCount-1]; }
    if (w) {
        strcpy(w->filesPath, path);
        w->filesScroll = 0; w->filesSel = -1;
        w->minimized = false;
        g_activeWin = (int)(w - g_windows);
    }
}

static void GetDesktopPath(char* out) {
    wsprintfA(out, "%s\\Users\\Default\\Desktop", G_DRIVE_ROOT);
}

static void DeskPosPath(char* out) {
    wsprintfA(out, "%s\\System\\desk_pos.dat", G_DRIVE_ROOT);
}

static void SaveDeskPositions() {
    char pf[520];
    DeskPosPath(pf);
    HANDLE hf = CreateFileA(pf, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (hf == INVALID_HANDLE_VALUE) return;
    DWORD wr;
    int cnt = g_deskCount;
    WriteFile(hf, &cnt, 4, &wr, NULL);
    for (int i = 0; i < g_deskCount; i++) {
        int isd = g_deskFiles[i].isDir ? 1 : 0;
        WriteFile(hf, g_deskFiles[i].name, 260, &wr, NULL);
        WriteFile(hf, &isd, 4, &wr, NULL);
        WriteFile(hf, &g_deskFiles[i].x, 4, &wr, NULL);
        WriteFile(hf, &g_deskFiles[i].y, 4, &wr, NULL);
    }
    CloseHandle(hf);
}

struct DeskPos { char name[260]; bool isDir; int x, y; };

static void LoadDeskPositions(DeskPos* arr, int* n) {
    *n = 0;
    char pf[520];
    DeskPosPath(pf);
    HANDLE hf = CreateFileA(pf, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hf == INVALID_HANDLE_VALUE) return;
    DWORD rd; int cnt = 0;
    if (!ReadFile(hf, &cnt, 4, &rd, NULL)) { CloseHandle(hf); return; }
    if (cnt < 0 || cnt > 128) cnt = 0;
    for (int i = 0; i < cnt && i < 128; i++) {
        DeskPos& p = arr[*n];
        memset(p.name, 0, 260);
        int isd = 0;
        if (!ReadFile(hf, p.name, 260, &rd, NULL)) break;
        if (!ReadFile(hf, &isd, 4, &rd, NULL)) break;
        if (!ReadFile(hf, &p.x, 4, &rd, NULL)) break;
        if (!ReadFile(hf, &p.y, 4, &rd, NULL)) break;
        p.isDir = (isd == 1);
        (*n)++;
    }
    CloseHandle(hf);
}

static void SnapDeskPos(int& x, int& y) {
    int W = GetSystemMetrics(SM_CXSCREEN);
    int H = GetSystemMetrics(SM_CYSCREEN);
    int maxX = W - 70;
    int maxY = H - TASKBAR_H - 78;
    if (x < 0) x = 0; if (y < 0) y = 0;
    if (x > maxX) x = maxX; if (y > maxY) y = maxY;
    x = 20 + ((x - 20 + 38) / 76) * 76;
    y = 20 + ((y - 20 + 42) / 84) * 84;
    if (x > maxX) x = 20 + ((maxX - 20) / 76) * 76;
    if (y > maxY) y = 20 + ((maxY - 20) / 84) * 84;
}

static bool DeskRectTaken(int x, int y) {
    for (int k = 0; k < ICON_COUNT; k++) {
        DesktopIcon& ic = g_icons[k];
        if (x == ic.x && y == ic.y) return true;
    }
    for (int k = 0; k < g_deskCount; k++) {
        DeskFile& d = g_deskFiles[k];
        if (x == d.x && y == d.y) return true;
    }
    return false;
}

static void FindDeskSlot(int* ox, int* oy) {
    int W = GetSystemMetrics(SM_CXSCREEN);
    int H = GetSystemMetrics(SM_CYSCREEN);
    int maxX = W - 70;
    int maxY = H - TASKBAR_H - 78;
    for (int r = 0; ; r++) {
        int y = 20 + r * 84;
        if (y + 78 > maxY) break;
        for (int c = 0; ; c++) {
            int x = 20 + c * 76;
            if (x + 70 > maxX) break;
            if (!DeskRectTaken(x, y)) { *ox = x; *oy = y; return; }
        }
    }
    *ox = 20; *oy = 20;
}

static void ScanDesktopFiles() {
    DeskPos saved[128];
    int savedN = 0;
    LoadDeskPositions(saved, &savedN);
    g_deskCount = 0;
    char pat[520]; wsprintfA(pat, "%s\\Users\\Default\\Desktop\\*", G_DRIVE_ROOT);
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pat, &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(fd.cFileName,".")==0||strcmp(fd.cFileName,"..")==0) continue;
            if (g_deskCount >= 128) break;
            DeskFile& d = g_deskFiles[g_deskCount];
            strcpy(d.name, fd.cFileName);
            d.isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            int x = -1, y = -1;
            for (int k = 0; k < savedN; k++) {
                if (strcmp(saved[k].name, d.name) == 0 && saved[k].isDir == d.isDir) {
                    x = saved[k].x; y = saved[k].y; break;
                }
            }
            int maxX = GetSystemMetrics(SM_CXSCREEN) - 70;
            int maxY = GetSystemMetrics(SM_CYSCREEN) - TASKBAR_H - 78;
            if (x < 0 || x > maxX || y < 0 || y > maxY) {
                FindDeskSlot(&x, &y);
            } else {
                SnapDeskPos(x, y);
            }
            d.x = x; d.y = y;
            g_deskCount++;
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }
    SaveDeskPositions();
}

static void DeskPosOf(int idx, int* ox, int* oy) {
    *ox = g_deskFiles[idx].x;
    *oy = g_deskFiles[idx].y;
}

static void DeskFullPath(int idx, char* out) {
    char deskp[520];
    GetDesktopPath(deskp);
    wsprintfA(out, "%s\\%s", deskp, g_deskFiles[idx].name);
}

static bool strend(const char* str, const char* ext);
static void OpenNotepad(char* fullpath) {
    OsWindow* nw = NULL;
    for (int k = 0; k < g_winCount; k++)
        if (g_windows[k].open && g_windows[k].app == APP_NOTEPAD) { nw = &g_windows[k]; break; }
    if (!nw) { OpenWindow(APP_NOTEPAD); nw = &g_windows[g_winCount-1]; }
    if (nw) {
        strcpy(nw->notepadFile, fullpath);
        nw->notepadRO = IsSystemPath(fullpath);
        HANDLE hf = CreateFileA(fullpath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hf != INVALID_HANDLE_VALUE) {
            DWORD sz = GetFileSize(hf, NULL); if (sz > 4095) sz = 4095;
            DWORD rd; ReadFile(hf, nw->notepad, sz, &rd, NULL);
            nw->notepad[rd] = 0; nw->notepadLen = rd;
            nw->minimized = false; g_activeWin = (int)(nw - g_windows);
            CloseHandle(hf);
        }
    }
}

static void OpenDrau(char* fullpath) {
    OsWindow* dw = NULL;
    for (int k = 0; k < g_winCount; k++)
        if (g_windows[k].open && g_windows[k].app == APP_DRAU) { dw = &g_windows[k]; break; }
    if (!dw) { OpenWindow(APP_DRAU); dw = &g_windows[g_winCount-1]; }
    if (dw) {
        if (fullpath[0]) wcscpy(dw->title, S.drau);
        dw->minimized = false;
        g_activeWin = (int)(dw - g_windows);
    }
}

static void OpenDeskFile(int idx) {
    char fp[520];
    DeskFullPath(idx, fp);
    if (g_deskFiles[idx].isDir) { OpenFilesAt(fp); return; }
    if (strend(fp, ".cpp")) {
        OsWindow* nw = NULL;
        for (int k = 0; k < g_winCount; k++)
            if (g_windows[k].open && g_windows[k].app == APP_CODE) { nw = &g_windows[k]; break; }
        if (!nw) { OpenWindow(APP_CODE); nw = &g_windows[g_winCount-1]; }
        if (nw) {
            strcpy(nw->codeFile, fp);
            HANDLE hf = CreateFileA(fp, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
            if (hf != INVALID_HANDLE_VALUE) {
                DWORD s2 = GetFileSize(hf, NULL); if (s2 > 16383) s2 = 16383;
                DWORD rd; ReadFile(hf, nw->code, s2, &rd, NULL);
                nw->code[rd] = 0; nw->codeLen = rd; CloseHandle(hf);
            }
            nw->codeModified = false; nw->minimized = false;
            g_activeWin = (int)(nw - g_windows);
        }
    } else if (strend(fp, ".kcpp")) {
        OsWindow* nw = NULL;
        for (int k = 0; k < g_winCount; k++)
            if (g_windows[k].open && g_windows[k].app == APP_KCPP) { nw = &g_windows[k]; break; }
        if (!nw) { OpenWindow(APP_KCPP); nw = &g_windows[g_winCount-1]; }
        if (nw) { strcpy(nw->kcppPath, fp); nw->minimized = false; g_activeWin = (int)(nw - g_windows); }
    } else if (strend(fp, ".drau")) {
        OpenDrau(fp);
    } else {
        OpenNotepad(fp);
    }
}

static void ApplyEditDialog() {
    if (g_editMode == 2) {
        OsWindow& w = g_windows[g_editWin];
        char nb[260];
        WideCharToMultiByte(CP_ACP, 0, g_editBuf, -1, nb, 260, NULL, NULL);
        strcpy(w.kcppPath, nb);
        g_editMode = 0;
        return;
    }
    if (g_editMode == 1) {
        char nb[260];
        WideCharToMultiByte(CP_ACP, 0, g_editBuf, -1, nb, 260, NULL, NULL);
        if (g_editWin >= 0 && g_editIdx >= 0) {
            OsWindow& w = g_windows[g_editWin];
            char oldp[520], newp[520];
            wsprintfA(oldp, "%s\\%s", w.filesPath, w.filesList[g_editIdx]);
            wsprintfA(newp, "%s\\%s", w.filesPath, nb);
            if (strcmp(oldp, newp) != 0) MoveFileA(oldp, newp);
            RunSecurityCheck();
        } else if (g_deskCtxSel >= 0) {
            char oldp[520], newp[520];
            DeskFullPath(g_deskCtxSel, oldp);
            char deskp[520];
            GetDesktopPath(deskp);
            wsprintfA(newp, "%s\\%s", deskp, nb);
            if (strcmp(oldp, newp) != 0) MoveFileA(oldp, newp);
            ScanDesktopFiles();
            g_deskCtxSel = -1;
        }
        g_editMode = 0;
    }
}

static void UpdateAllTitles() {
    for (int i = 0; i < g_winCount; i++)
        if (g_windows[i].open) wcscpy(g_windows[i].title, GetTitleForApp(g_windows[i].app));
}

static HFONT MakeFont(int size, bool bold) {
    return CreateFontW(size, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL,
        FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI");
}

static void DrawButtonW(HDC hdc, int x, int y, int w, int h, const wchar_t* text, bool pressed, bool hl) {
    COLORREF bg = hl ? RGB(200,80,0) : (pressed ? RGB(160,160,160) : CLR_TASKBAR);
    HBRUSH br = CreateSolidBrush(bg);
    RECT r = {x, y, x+w, y+h};
    FillRect(hdc, &r, br); DeleteObject(br);
    HPEN pen = CreatePen(PS_SOLID, hl?2:1, hl?RGB(150,50,0):RGB(0,0,0));
    SelectObject(hdc, pen); Rectangle(hdc, x, y, x+w, y+h); DeleteObject(pen);
    if (!pressed) {
        HPEN hi = CreatePen(PS_SOLID, 1, RGB(255,255,255));
        SelectObject(hdc, hi);
        MoveToEx(hdc, x, y+h-1, NULL); LineTo(hdc, x, y); LineTo(hdc, x+w-1, y); DeleteObject(hi);
        HPEN lo = CreatePen(PS_SOLID, 1, RGB(80,80,80));
        SelectObject(hdc, lo);
        MoveToEx(hdc, x+w-1, y, NULL); LineTo(hdc, x+w-1, y+h-1); LineTo(hdc, x, y+h-1); DeleteObject(lo);
    }
    HFONT fnt = MakeFont(13, hl);
    HFONT old = (HFONT)SelectObject(hdc, fnt);
    SetTextColor(hdc, hl?RGB(255,255,255):RGB(0,0,0));
    SetBkMode(hdc, TRANSPARENT);
    DrawTextW(hdc, text, -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old); DeleteObject(fnt);
}

static void CalcPress(OsWindow& w, const char* key) {
    char buf[128];
    if (key[0] >= '0' && key[0] <= '9') {
        if (w.calcNewNum) { w.calcDisp[0] = key[0]; w.calcDisp[1] = 0; w.calcNewNum = false; }
        else { int len = strlen(w.calcDisp); if (len < 60) { w.calcDisp[len] = key[0]; w.calcDisp[len+1] = 0; } }
    } else if (key[0] == '.') {
        if (w.calcNewNum) { strcpy(w.calcDisp, "0."); w.calcNewNum = false; }
        else if (!strchr(w.calcDisp, '.')) { int len = strlen(w.calcDisp); w.calcDisp[len] = '.'; w.calcDisp[len+1] = 0; }
    } else if (key[0] == 'C') {
        w.calcVal = 0; w.calcMem = 0; w.calcOp = 0; w.calcNewNum = true; strcpy(w.calcDisp, "0");
    } else if (key[0] == '+' || key[0] == '-' || key[0] == '*' || key[0] == '/') {
        double cur = atof(w.calcDisp);
        if (w.calcOp && !w.calcNewNum) {
            switch (w.calcOp) { case '+': w.calcVal += cur; break; case '-': w.calcVal -= cur; break;
            case '*': w.calcVal *= cur; break; case '/': if (cur != 0) w.calcVal /= cur; break; }
        } else { w.calcVal = cur; }
        w.calcOp = key[0]; w.calcNewNum = true;
        sprintf(buf, "%.12g", w.calcVal); strcpy(w.calcDisp, buf);
    } else if (key[0] == '=') {
        double cur = atof(w.calcDisp);
        if (w.calcOp) {
            switch (w.calcOp) { case '+': w.calcVal += cur; break; case '-': w.calcVal -= cur; break;
            case '*': w.calcVal *= cur; break; case '/': if (cur != 0) w.calcVal /= cur; break; }
            sprintf(buf, "%.12g", w.calcVal); strcpy(w.calcDisp, buf); w.calcOp = 0;
        }
        w.calcNewNum = true;
    } else if (key[0] == 'M') { w.calcMem = atof(w.calcDisp); }
    else if (key[0] == 'R') { sprintf(buf, "%.12g", w.calcMem); strcpy(w.calcDisp, buf); w.calcNewNum = true; }
}

static bool CalcHitAll(OsWindow& w, int mx, int my, char* out) {
    int padX = w.x + 10, padY = w.y + TITLEBAR_H + 30;
    int bw = 60, bh = 36, gap = 4;
    const char* btns[4][5] = { {"MC","MR","MS","M+","C"},{"7","8","9","/",""},{"4","5","6","*",""},{"1","2","3","-",""} };
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 5; c++) {
            if (btns[r][c][0] == 0) continue;
            int bx = padX + c*(bw+gap), by = padY + r*(bh+gap);
            if (mx >= bx && mx <= bx+bw && my >= by && my <= by+bh) { strcpy(out, btns[r][c]); return true; }
        }
    int row4y = padY + 4*(bh+gap);
    if (PtIn(mx, my, padX, row4y, 2*bw+gap, bh)) { strcpy(out, "0"); return true; }
    if (PtIn(mx, my, padX+2*(bw+gap), row4y, bw, bh)) { strcpy(out, "."); return true; }
    if (PtIn(mx, my, padX+3*(bw+gap), row4y, bw, bh)) { strcpy(out, "+"); return true; }
    if (PtIn(mx, my, padX+4*(bw+gap), row4y, bw, bh)) { strcpy(out, "="); return true; }
    return false;
}

static void DrawCalcApp(HDC hdc, OsWindow& w) {
    int padX = w.x + 10, padY = w.y + TITLEBAR_H + 4;
    HBRUSH dbr = CreateSolidBrush(RGB(200,220,200));
    RECT dr = {padX, padY, padX + w.w - 20, padY + 22};
    FillRect(hdc, &dr, dbr); DeleteObject(dbr);
    HPEN dp = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, dp); Rectangle(hdc, padX, padY, padX+w.w-20, padY+22); DeleteObject(dp);
    HFONT df = MakeFont(16, TRUE);
    HFONT old = (HFONT)SelectObject(hdc, df);
    SetTextColor(hdc, RGB(0,0,0)); SetBkMode(hdc, TRANSPARENT);
    RECT dtr = {padX+4, padY, padX+w.w-24, padY+22};
    DrawTextA(hdc, w.calcDisp, -1, &dtr, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old); DeleteObject(df);
    const char* btns[4][5] = { {"MC","MR","MS","M+","C"},{"7","8","9","/",""},{"4","5","6","*",""},{"1","2","3","-",""} };
    int bw = 60, bh = 36, gap = 4;
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 5; c++) {
            if (btns[r][c][0] == 0) continue;
            wchar_t wbtn[8]; MultiByteToWideChar(CP_ACP, 0, btns[r][c], -1, wbtn, 8);
            DrawButtonW(hdc, padX+c*(bw+gap), padY+28+r*(bh+gap), bw, bh, wbtn, false, false);
        }
    DrawButtonW(hdc, padX, padY+28+4*(bh+gap), bw*2+gap, bh, L"0", false, false);
    DrawButtonW(hdc, padX+2*(bw+gap), padY+28+4*(bh+gap), bw, bh, L".", false, false);
    DrawButtonW(hdc, padX+3*(bw+gap), padY+28+4*(bh+gap), bw, bh, L"+", false, false);
    DrawButtonW(hdc, padX+4*(bw+gap), padY+28+4*(bh+gap), bw, bh, L"=", false, true);
}

static void DrawNotepadApp(HDC hdc, OsWindow& w) {
    int tbY = w.y + TITLEBAR_H + 4;
    int tbW = w.w - 8;
    HBRUSH tbg = CreateSolidBrush(RGB(220,220,220));
    RECT tb = {w.x + 4, tbY, w.x + 4 + tbW, tbY + 30};
    FillRect(hdc, &tb, tbg); DeleteObject(tbg);
    HPEN tbp = CreatePen(PS_SOLID, 1, RGB(140,140,140));
    SelectObject(hdc, tbp);
    MoveToEx(hdc, w.x + 4, tbY + 30, NULL); LineTo(hdc, w.x + 4 + tbW, tbY + 30);
    DeleteObject(tbp);
    DrawButtonW(hdc, w.x + 8, tbY + 3, 90, 24, S.save, false, false);
    if (w.notepadFile[0]) {
        HFONT nf = MakeFont(11, FALSE);
        HFONT old = (HFONT)SelectObject(hdc, nf);
        if (w.notepadRO) {
            HFONT rbf = MakeFont(11, TRUE);
            HFONT oldR = (HFONT)SelectObject(hdc, rbf);
            SetTextColor(hdc, RGB(200,80,0)); SetBkMode(hdc, TRANSPARENT);
            RECT rr = {w.x + 104, tbY, w.x + 260, tbY + 30};
            DrawTextW(hdc, S.sys_readonly, -1, &rr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, oldR);
            SetTextColor(hdc, RGB(60,60,60));
            RECT fr = {w.x + 265, tbY, w.x + 4 + tbW - 6, tbY + 30};
            DrawTextA(hdc, w.notepadFile, -1, &fr, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS);
        } else {
            SetTextColor(hdc, RGB(60,60,60));
            RECT fr = {w.x + 104, tbY, w.x + 4 + tbW - 6, tbY + 30};
            DrawTextA(hdc, w.notepadFile, -1, &fr, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS);
        }
        SelectObject(hdc, old); DeleteObject(nf);
    }

    int padX = w.x + 4, padY = tbY + 34;
    int ew = w.w - 8, eh = w.h - TITLEBAR_H - 4 - 34 - 4;
    HBRUSH wbr = CreateSolidBrush(RGB(255,255,255));
    RECT wr = {padX, padY, padX+ew, padY+eh};
    FillRect(hdc, &wr, wbr); DeleteObject(wbr);
    HPEN bp = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, bp); Rectangle(hdc, padX, padY, padX+ew, padY+eh); DeleteObject(bp);

    if (g_activeWin >= 0 && &g_windows[g_activeWin] == &w) {
        HPEN cur = CreatePen(PS_SOLID, 1, RGB(0,0,0));
        SelectObject(hdc, cur);
        HFONT nf = MakeFont(13, FALSE);
        HFONT old = (HFONT)SelectObject(hdc, nf);
        SIZE sz;
        if (w.notepadLen > 0) {
            GetTextExtentPoint32A(hdc, w.notepad, w.notepadLen, &sz);
        } else {
            GetTextExtentPoint32A(hdc, " ", 1, &sz);
        }
        int cx = padX + 6 + sz.cx;
        int cy = padY + 4;
        MoveToEx(hdc, cx, cy, NULL); LineTo(hdc, cx, cy + 16);
        DeleteObject(cur);
        SelectObject(hdc, old); DeleteObject(nf);
    }

    if (w.notepadLen > 0) {
        HFONT nf = MakeFont(13, FALSE);
        HFONT old = (HFONT)SelectObject(hdc, nf);
        SetTextColor(hdc, RGB(0,0,0)); SetBkMode(hdc, TRANSPARENT);
        RECT tr = {padX+4, padY+4, padX+ew-4, padY+eh-4};
        DrawTextA(hdc, w.notepad, -1, &tr, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EDITCONTROL);
        SelectObject(hdc, old); DeleteObject(nf);
    }
}

static void DrawTerminalApp(HDC hdc, OsWindow& w) {
    int padX = w.x + 4, padY = w.y + TITLEBAR_H + 4;
    int ew = w.w - 8, eh = w.h - TITLEBAR_H - 8;
    HBRUSH bg = CreateSolidBrush(RGB(12,12,12));
    RECT wr = {padX, padY, padX+ew, padY+eh};
    FillRect(hdc, &wr, bg); DeleteObject(bg);
    HPEN bp = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, bp); Rectangle(hdc, padX, padY, padX+ew, padY+eh); DeleteObject(bp);
    HFONT tf = MakeFont(14, FALSE);
    HFONT old = (HFONT)SelectObject(hdc, tf);
    SetTextColor(hdc, RGB(60,220,120)); SetBkMode(hdc, TRANSPARENT);
    int lineH = 20;
    char buf[16384];
    strcpy(buf, w.termBuf);
    char* lines[512];
    int ln = 0;
    lines[ln++] = buf;
    for (char* p = buf; *p; p++) {
        if (*p == '\r') *p = ' ';
        else if (*p == '\n') {
            *p = 0;
            if (ln < 512) lines[ln++] = p + 1;
        }
    }
    int visible = eh / lineH;
    int start = ln - visible;
    if (start < 0) start = 0;
    start += w.termScroll;
    if (start < 0) start = 0;
    for (int k = 0; k < visible; k++) {
        int idx = start + k;
        if (idx >= ln) break;
        char line[512];
        strncpy(line, lines[idx], 511); line[511] = 0;
        RECT lr = {padX+6, padY+4+k*lineH, padX+ew-6, padY+4+(k+1)*lineH};
        DrawTextA(hdc, line, -1, &lr, DT_LEFT | DT_TOP | DT_END_ELLIPSIS);
    }
    SelectObject(hdc, old); DeleteObject(tf);
}

static void DrawDrauApp(HDC hdc, OsWindow& w) {
    int ex = w.x + 4, ey = w.y + TITLEBAR_H + 4;
    int ew = w.w - 8, eh = w.h - TITLEBAR_H - 8;
    HBRUSH bg = CreateSolidBrush(RGB(16,18,38));
    RECT br = {ex, ey, ex+ew, ey+eh};
    FillRect(hdc, &br, bg); DeleteObject(bg);
    HBRUSH bg2 = CreateSolidBrush(RGB(24,28,60));
    RECT br2 = {ex, ey, ex+ew, ey+58};
    FillRect(hdc, &br2, bg2); DeleteObject(bg2);

    float cY = cosf(w.drauRy), sY = sinf(w.drauRy);
    float cX = cosf(w.drauRx), sX = sinf(w.drauRx);
    float v[8][3] = {{-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},{-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}};
    float px[8], py[8], pz[8];
    int cx = ex + ew/2, cy = ey + eh/2 + 30;
    float sc = (ew < eh ? ew : eh) * 0.32f;
    for (int i = 0; i < 8; i++) {
        float x = v[i][0]*cY + v[i][2]*sY;
        float z = -v[i][0]*sY + v[i][2]*cY;
        float y = v[i][1]*cX - z*sX;
        float zz = v[i][1]*sX + z*cX;
        float p = 4.6f / (4.6f + zz);
        pz[i] = zz;
        px[i] = cx + x*sc*p;
        py[i] = cy + y*sc*p;
    }
    int edges[12][2] = {{0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,4},{0,4},{1,5},{2,6},{3,7}};
    for (int e = 0; e < 12; e++) {
        int a = edges[e][0], b = edges[e][1];
        float d = (pz[a] + pz[b]) * 0.5f;
        float t = 0.5f - d*0.14f;
        if (t < 0.15f) t = 0.15f;
        if (t > 0.95f) t = 0.95f;
        int gg = (int)(30 + t*200), bb = (int)(160 + t*95);
        HPEN lp = CreatePen(PS_SOLID, e >= 8 ? 2 : 2, RGB(gg, (int)(60 + t*120), bb));
        HPEN oldp = (HPEN)SelectObject(hdc, lp);
        MoveToEx(hdc, (int)px[a], (int)py[a], NULL);
        LineTo(hdc, (int)px[b], (int)py[b]);
        SelectObject(hdc, oldp); DeleteObject(lp);
    }
    HPEN glow = CreatePen(PS_SOLID, 1, RGB(80,90,190));
    HPEN oldg = (HPEN)SelectObject(hdc, glow);
    Ellipse(hdc, cx-46, cy-46, cx+46, cy+46);
    SelectObject(hdc, oldg); DeleteObject(glow);

    HFONT ttf = MakeFont(14, TRUE);
    HFONT oldt = (HFONT)SelectObject(hdc, ttf);
    SetTextColor(hdc, RGB(220,220,255)); SetBkMode(hdc, TRANSPARENT);
    wchar_t ttl[64];
    wsprintfW(ttl, L"Drau 3D  [%hs]", w.drauSpin ? "RUN" : "PAUSE");
    RECT ttr = {ex+12, ey+12, ex+ew-12, ey+40};
    DrawTextW(hdc, ttl, -1, &ttr, DT_LEFT | DT_TOP);
    SelectObject(hdc, oldt); DeleteObject(ttf);
    HFONT htf = MakeFont(10, FALSE);
    HFONT oldh = (HFONT)SelectObject(hdc, htf);
    SetTextColor(hdc, RGB(140,150,200));
    RECT htr = {ex, ey+eh-22, ex+ew, ey+eh-4};
    DrawTextW(hdc, L"click window to pause/resume", -1, &htr, DT_CENTER | DT_TOP);
    SelectObject(hdc, oldh); DeleteObject(htf);
}

static void PaintFillWhite(OsWindow& w) {
    for (int y = 0; y < 192; y++)
        for (int x = 0; x < 256; x++)
            w.paintPix[y][x] = RGB(255,255,255);
}

static COLORREF PaintCurColor(OsWindow& w) {
    static const COLORREF pal[10] = { RGB(0,0,0), RGB(255,255,255), RGB(220,30,30), RGB(20,150,40), RGB(30,60,220), RGB(240,220,30), RGB(240,130,20), RGB(20,190,190), RGB(190,30,190), RGB(130,70,20) };
    return pal[w.paintColor % 10];
}

static void PaintDot(OsWindow& w, int cx, int cy) {
    COLORREF c = w.paintEraser ? RGB(255,255,255) : PaintCurColor(w);
    int r = w.paintSize;
    if (r < 1) r = 1;
    for (int dy = -r; dy <= r; dy++)
        for (int dx = -r; dx <= r; dx++) {
            int px = cx + dx, py = cy + dy;
            if (px >= 0 && px < 256 && py >= 0 && py < 192)
                w.paintPix[py][px] = c;
        }
}

static void PaintLine(OsWindow& w, int x0, int y0, int x1, int y1) {
    int steps = abs(x1-x0) > abs(y1-y0) ? abs(x1-x0) : abs(y1-y0);
    if (steps < 1) steps = 1;
    for (int i = 0; i <= steps; i++)
        PaintDot(w, x0 + (x1-x0)*i/steps, y0 + (y1-y0)*i/steps);
}

static bool PaintMapPoint(OsWindow& w, int mx, int my, int& px, int& py) {
    int ex = w.x + 4, ey = w.y + TITLEBAR_H + 4;
    int ew = w.w - 8, eh = w.h - TITLEBAR_H - 8;
    int cw = ew, ch = eh - 48;
    if (cw * 3 > ch * 4) cw = ch * 4 / 3; else ch = cw * 3 / 4;
    int cx = ex + (ew - cw) / 2;
    int cy = ey + 48 + ((eh - 48) - ch) / 2;
    if (mx < cx || mx > cx+cw || my < cy || my > cy+ch) return false;
    px = (mx - cx) * 256 / cw;
    py = (my - cy) * 192 / ch;
    if (px < 0) px = 0; if (px > 255) px = 255;
    if (py < 0) py = 0; if (py > 191) py = 191;
    return true;
}

static bool PaintSaveFile(OsWindow& w) {
    char path[MAX_PATH];
    wsprintfA(path, "%s\\Users\\Default\\Pictures", G_DRIVE_ROOT);
    CreateDirectoryA(path, NULL);
    strcat(path, "\\paint.bmp");
    HANDLE hf = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (hf == INVALID_HANDLE_VALUE) return false;
    DWORD rowSize = (256 * 3 + 3) & ~3;
    BITMAPFILEHEADER bf = {};
    bf.bfType = 0x4D42;
    bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bf.bfSize = bf.bfOffBits + rowSize * 192;
    BITMAPINFOHEADER bi = {};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = 256;
    bi.biHeight = 192;
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = BI_RGB;
    DWORD wr;
    WriteFile(hf, &bf, sizeof(bf), &wr, NULL);
    WriteFile(hf, &bi, sizeof(bi), &wr, NULL);
    BYTE row[256 * 3 + 4];
    for (int y = 191; y >= 0; y--) {
        for (int x = 0; x < 256; x++) {
            COLORREF c = w.paintPix[y][x];
            row[x * 3 + 0] = (BYTE)GetBValue(c);
            row[x * 3 + 1] = (BYTE)GetGValue(c);
            row[x * 3 + 2] = (BYTE)GetRValue(c);
        }
        WriteFile(hf, row, rowSize, &wr, NULL);
    }
    CloseHandle(hf);
    return true;
}

static void DrawPaintApp(HDC hdc, OsWindow& w) {
    int ex = w.x + 4, ey = w.y + TITLEBAR_H + 4;
    int ew = w.w - 8, eh = w.h - TITLEBAR_H - 8;
    HBRUSH tbBg = CreateSolidBrush(RGB(225,225,225));
    RECT tbR = {ex, ey, ex+ew, ey+48};
    FillRect(hdc, &tbR, tbBg); DeleteObject(tbBg);
    HPEN sep = CreatePen(PS_SOLID, 1, RGB(150,150,150));
    SelectObject(hdc, sep);
    MoveToEx(hdc, ex, ey+48, NULL); LineTo(hdc, ex+ew, ey+48);
    DeleteObject(sep);

    int sx = ex + 6, sy = ey + 8;
    static const COLORREF pal[10] = { RGB(0,0,0), RGB(255,255,255), RGB(220,30,30), RGB(20,150,40), RGB(30,60,220), RGB(240,220,30), RGB(240,130,20), RGB(20,190,190), RGB(190,30,190), RGB(130,70,20) };
    for (int i = 0; i < 10; i++) {
        HBRUSH sw = CreateSolidBrush(pal[i]);
        RECT sr = {sx + i*28, sy, sx + i*28 + 24, sy + 24};
        FillRect(hdc, &sr, sw); DeleteObject(sw);
        HPEN bp = CreatePen(PS_SOLID, (i == w.paintColor) ? 3 : 1, (i == w.paintColor) ? RGB(200,80,0) : RGB(90,90,90));
        SelectObject(hdc, bp);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, sr.left, sr.top, sr.right, sr.bottom);
        DeleteObject(bp);
    }
    int bx = sx + 10*28 + 8;
    const int sizes[3] = {1, 3, 6};
    for (int i = 0; i < 3; i++) {
        bool sOn = (w.paintSize == sizes[i]);
        const wchar_t* lbl = (i==0) ? L"S" : (i==1) ? L"M" : L"L";
        DrawButtonW(hdc, bx + i*38, sy, 34, 24, lbl, false, sOn);
    }
    bx += 3*38 + 4;
    DrawButtonW(hdc, bx, sy, 56, 24, L"Eraser", false, w.paintEraser);
    int curColBtn = bx + 56 + 8;
    HBRUSH cc = CreateSolidBrush(w.paintEraser ? RGB(255,255,255) : PaintCurColor(w));
    RECT ccr = {curColBtn, sy+2, curColBtn+20, sy+22};
    FillRect(hdc, &ccr, cc); DeleteObject(cc);
    HPEN cbp = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, cbp);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, ccr.left, ccr.top, ccr.right, ccr.bottom);
    DeleteObject(cbp);
    int clrX = curColBtn + 26;
    DrawButtonW(hdc, clrX, sy, 60, 24, L"Clear", false, false);
    DrawButtonW(hdc, clrX + 66, sy, 60, 24, S.save, false, false);

    int cw = ew, ch = eh - 48;
    if (cw * 3 > ch * 4) cw = ch * 4 / 3; else ch = cw * 3 / 4;
    int cx = ex + (ew - cw) / 2;
    int cy = ey + 48 + ((eh - 48) - ch) / 2;
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = 256;
    bi.bmiHeader.biHeight = -192;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    StretchDIBits(hdc, cx, cy, cw, ch, 0, 0, 256, 192, w.paintPix, &bi, DIB_RGB_COLORS, SRCCOPY);
    HPEN bp2 = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, bp2);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, cx, cy, cx+cw, cy+ch);
    DeleteObject(bp2);
}

static void PptContentRects(OsWindow& w, int& ex, int& ey, int& ew, int& eh) {
    ex = w.x + 4; ey = w.y + TITLEBAR_H + 4;
    ew = w.w - 8; eh = w.h - TITLEBAR_H - 8;
}

static void DrawPptApp(HDC hdc, OsWindow& w) {
    int ex, ey, ew, eh; PptContentRects(w, ex, ey, ew, eh);
    HBRUSH bg = CreateSolidBrush(RGB(58,58,62));
    RECT br = {ex, ey, ex+ew, ey+eh};
    FillRect(hdc, &br, bg); DeleteObject(bg);

    if (w.pptShow) {
        int sw = ew, sh = eh;
        if (sw*3 > sh*4) sw = sh*4/3; else sh = sw*3/4;
        int scx = ex + (ew-sw)/2, scy = ey + (eh-sh)/2;
        HBRUSH sl = CreateSolidBrush(w.pptBg[w.pptCur]);
        RECT sr = {scx, scy, scx+sw, scy+sh};
        FillRect(hdc, &sr, sl); DeleteObject(sl);
        HPEN sp = CreatePen(PS_SOLID, 1, RGB(255,255,255));
        SelectObject(hdc, sp);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, scx, scy, scx+sw, scy+sh);
        DeleteObject(sp);
        int tsz = sh/12; if (tsz < 18) tsz = 18; if (tsz > 42) tsz = 42;
        HFONT tf = MakeFont(tsz, TRUE);
        HFONT old = (HFONT)SelectObject(hdc, tf);
        SetTextColor(hdc, RGB(255,255,255)); SetBkMode(hdc, TRANSPARENT);
        RECT tr = {scx+24, scy+18, scx+sw-24, scy+sh/3};
        DrawTextW(hdc, w.pptTitle[w.pptCur], -1, &tr, DT_LEFT | DT_VCENTER | DT_WORDBREAK);
        int bsz = sh/18; if (bsz < 11) bsz = 11; if (bsz > 26) bsz = 26;
        HFONT bf2 = MakeFont(bsz, FALSE);
        SelectObject(hdc, bf2);
        RECT br2 = {scx+32, scy+sh/3+16, scx+sw-32, scy+sh-22};
        DrawTextW(hdc, w.pptBody[w.pptCur], -1, &br2, DT_LEFT | DT_TOP | DT_WORDBREAK);
        SelectObject(hdc, old); DeleteObject(tf); DeleteObject(bf2);
        HFONT hf = MakeFont(11, FALSE);
        old = (HFONT)SelectObject(hdc, hf);
        SetTextColor(hdc, RGB(200,200,200));
        wchar_t hbt[64]; wsprintfW(hbt, L"Slide %d / %d   -   click or Right arrow to continue", w.pptCur+1, w.pptCount);
        RECT hr = {scx, scy+sh-20, scx+sw, scy+sh-4};
        DrawTextW(hdc, hbt, -1, &hr, DT_CENTER | DT_TOP);
        SelectObject(hdc, old); DeleteObject(hf);
        return;
    }

    HBRUSH tbar = CreateSolidBrush(RGB(205,205,208));
    RECT tb = {ex, ey, ex+ew, ey+40};
    FillRect(hdc, &tb, tbar); DeleteObject(tbar);
    HPEN sep = CreatePen(PS_SOLID, 1, RGB(120,120,125));
    SelectObject(hdc, sep);
    MoveToEx(hdc, ex, ey+40, NULL); LineTo(hdc, ex+ew, ey+40);
    DeleteObject(sep);

    int by = ey + 8;
    DrawButtonW(hdc, ex+6, by, 64, 24, L"+ Slide", false, false);
    DrawButtonW(hdc, ex+76, by, 58, 24, L"- Slide", false, false);
    DrawButtonW(hdc, ex+140, by, 32, 24, L"<", false, false);
    DrawButtonW(hdc, ex+178, by, 32, 24, L">", false, false);
    DrawButtonW(hdc, ex+216, by, 56, 24, L"Play", false, false);
    HFONT hf = MakeFont(11, FALSE);
    HFONT old = (HFONT)SelectObject(hdc, hf);
    SetTextColor(hdc, RGB(60,60,60)); SetBkMode(hdc, TRANSPARENT);
    wchar_t cnt[40]; wsprintfW(cnt, L"Slide %d / %d", w.pptCur+1, w.pptCount);
    RECT cr = {ex+282, by, ex+380, by+24};
    DrawTextW(hdc, cnt, -1, &cr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old); DeleteObject(hf);

    static const COLORREF slideBgs[6] = { RGB(18,80,150), RGB(150,30,30), RGB(20,130,60), RGB(180,120,10), RGB(60,50,140), RGB(40,40,40) };
    int cxx = ex+400;
    for (int i = 0; i < 6; i++) {
        HBRUSH sw = CreateSolidBrush(slideBgs[i]);
        RECT sr = {cxx + i*26, by+2, cxx + i*26 + 20, by + 22};
        FillRect(hdc, &sr, sw); DeleteObject(sw);
        HPEN bp = CreatePen(PS_SOLID, (w.pptBg[w.pptCur]==slideBgs[i]) ? 3 : 1, (w.pptBg[w.pptCur]==slideBgs[i]) ? RGB(240,200,40) : RGB(90,90,90));
        SelectObject(hdc, bp);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, sr.left, sr.top, sr.right, sr.bottom);
        DeleteObject(bp);
    }

    int thX = ex + 8, thY = ey + 46, thW = 82;
    int vis = (eh - 50) / 74;
    if (vis < 1) vis = 1;
    int shown = w.pptCount - w.pptScroll;
    if (shown > vis) shown = vis;
    for (int i = 0; i < shown; i++) {
        int idx = w.pptScroll + i;
        int ty = thY + i*74;
        bool sel = (idx == w.pptCur);
        HBRUSH thB = CreateSolidBrush(w.pptBg[idx]);
        RECT thR = {thX, ty, thX+thW, ty+60};
        FillRect(hdc, &thR, thB); DeleteObject(thB);
        HPEN tp = CreatePen(PS_SOLID, sel ? 3 : 1, sel ? RGB(255,200,40) : RGB(120,120,120));
        SelectObject(hdc, tp);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, thR.left, thR.top, thR.right, thR.bottom);
        DeleteObject(tp);
        HFONT nf = MakeFont(9, FALSE);
        old = (HFONT)SelectObject(hdc, nf);
        SetTextColor(hdc, RGB(255,255,255));
        wchar_t nm[16]; wsprintfW(nm, L"%d", idx+1);
        RECT nr = {thX+2, ty, thX+thW, ty+14};
        DrawTextW(hdc, nm, -1, &nr, DT_RIGHT | DT_TOP);
        SelectObject(hdc, old); DeleteObject(nf);
        HFONT tf2 = MakeFont(8, FALSE);
        old = (HFONT)SelectObject(hdc, tf2);
        RECT ttr = {thX+3, ty+8, thX+thW-3, ty+56};
        DrawTextW(hdc, w.pptTitle[idx], -1, &ttr, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_END_ELLIPSIS);
        SelectObject(hdc, old); DeleteObject(tf2);
    }

    int pvx = thX + thW + 10;
    int pvw = ex + ew - pvx - 8;
    int pvy = ey + 46;
    int pvh = eh - 50;
    if (pvw * 3 > pvh * 4) pvw = pvh * 4 / 3; else pvh = pvw * 3 / 4;
    pvx += (ex + ew - pvx - 8 - pvw) / 2;
    pvy += (eh - 50 - pvh) / 2;
    HBRUSH sl = CreateSolidBrush(w.pptBg[w.pptCur]);
    RECT sr2 = {pvx, pvy, pvx+pvw, pvy+pvh};
    FillRect(hdc, &sr2, sl); DeleteObject(sl);
    HPEN sp2 = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, sp2);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, pvx, pvy, pvx+pvw, pvy+pvh);
    DeleteObject(sp2);

    RECT titleRc = {pvx+14, pvy+14, pvx+pvw-14, pvy+14 + pvh/4};
    RECT bodyRc = {pvx+16, pvy+14 + pvh/4 + 10, pvx+pvw-16, pvy+pvh-16};
    int tsz2 = pvh/10; if (tsz2 < 14) tsz2 = 14; if (tsz2 > 36) tsz2 = 36;
    HFONT tf = MakeFont(tsz2, TRUE);
    old = (HFONT)SelectObject(hdc, tf);
    SetTextColor(hdc, RGB(255,255,255));
    DrawTextW(hdc, w.pptTitle[w.pptCur], -1, &titleRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    SelectObject(hdc, old); DeleteObject(tf);
    int bsz2 = pvh/14; if (bsz2 < 10) bsz2 = 10; if (bsz2 > 24) bsz2 = 24;
    HFONT bf = MakeFont(bsz2, FALSE);
    old = (HFONT)SelectObject(hdc, bf);
    RECT brb = bodyRc;
    DrawTextW(hdc, w.pptBody[w.pptCur], -1, &brb, DT_LEFT | DT_TOP | DT_WORDBREAK);
    SelectObject(hdc, old); DeleteObject(bf);

    if (w.pptFocus) {
        HPEN fp = CreatePen(PS_SOLID, 0, RGB(255,255,255));
        SelectObject(hdc, fp);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        RECT fr = (w.pptFocus == 1) ? titleRc : bodyRc;
        Rectangle(hdc, fr.left-2, fr.top-2, fr.right+2, fr.bottom+2);
        if (w.pptFocus == 1) {
            HFONT cf = MakeFont(tsz2, TRUE);
            old = (HFONT)SelectObject(hdc, cf);
            SetTextColor(hdc, RGB(255,255,255));
            SIZE sz;
            GetTextExtentPoint32W(hdc, w.pptTitle[w.pptCur], (int)wcslen(w.pptTitle[w.pptCur]), &sz);
            int cxl = titleRc.left + sz.cx + 2;
            if (cxl < titleRc.right) {
                MoveToEx(hdc, cxl, titleRc.top + 3, NULL); LineTo(hdc, cxl, titleRc.bottom - 3);
            }
            SelectObject(hdc, old); DeleteObject(cf);
        }
        DeleteObject(fp);
    }
}

static void KmlAppendLog(OsWindow& w, const char* fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    _vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    int need = (int)strlen(buf) + 2;
    if (w.kmlLogLen + need >= (int)sizeof(w.kmlLog)) {
        int keep = 4096;
        if (w.kmlLogLen > keep) {
            memmove(w.kmlLog, w.kmlLog + w.kmlLogLen - keep, keep);
            w.kmlLogLen = keep;
        }
    }
    if (w.kmlLogLen + need < (int)sizeof(w.kmlLog)) {
        w.kmlLog[w.kmlLogLen++] = '\r';
        w.kmlLog[w.kmlLogLen++] = '\n';
        memcpy(w.kmlLog + w.kmlLogLen, buf, strlen(buf));
        w.kmlLogLen += (int)strlen(buf);
        w.kmlLog[w.kmlLogLen] = 0;
    }
    if (g_hwnd) InvalidateRect(g_hwnd, NULL, FALSE);
}

static void UpdAppendLog(OsWindow& w, const char* fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    _vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    int need = (int)strlen(buf) + 2;
    if (w.updLogLen + need >= (int)sizeof(w.updLog)) {
        int keep = 4096;
        if (w.updLogLen > keep) {
            memmove(w.updLog, w.updLog + w.updLogLen - keep, keep);
            w.updLogLen = keep;
        }
    }
    if (w.updLogLen + need < (int)sizeof(w.updLog)) {
        w.updLog[w.updLogLen++] = '\r';
        w.updLog[w.updLogLen++] = '\n';
        memcpy(w.updLog + w.updLogLen, buf, strlen(buf));
        w.updLogLen += (int)strlen(buf);
        w.updLog[w.updLogLen] = 0;
    }
    if (g_hwnd) InvalidateRect(g_hwnd, NULL, FALSE);
}

static bool UpdHttpGet(const char* url, char* outBuf, int outCap) {
    HINTERNET net = InternetOpenA("KashmaUpdater/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!net) return false;
    DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_KEEP_CONNECTION;
    if (strncmp(url, "https://", 8) == 0) flags |= INTERNET_FLAG_SECURE;
    HINTERNET req = InternetOpenUrlA(net, url, NULL, 0, flags, (DWORD_PTR)0);
    if (!req) { InternetCloseHandle(net); return false; }
    DWORD code = 0, cl = sizeof(code);
    HttpQueryInfoA(req, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &code, &cl, NULL);
    if (code != 200) { InternetCloseHandle(req); InternetCloseHandle(net); return false; }
    int n = 0; DWORD rd = 0;
    while (n < outCap - 1 && InternetReadFile(req, outBuf + n, outCap - 1 - n, &rd) && rd > 0)
        n += (int)rd;
    InternetCloseHandle(req);
    InternetCloseHandle(net);
    outBuf[n] = 0;
    return n > 0;
}

static unsigned __stdcall UpdCheckThread(void* p) {
    OsWindow* w = (OsWindow*)p;
    strncpy(g_updBaseUrl, w->updUrl, 511); g_updBaseUrl[511] = 0;
    char url[1200];
    int ul = (int)strlen(g_updBaseUrl);
    if (ul && g_updBaseUrl[ul - 1] != '/') strcat(g_updBaseUrl, "/");
    wsprintfA(url, "%supdate.txt", g_updBaseUrl);
    UpdAppendLog(*w, "[UPD] checking %s", url);
    char buf[8192];
    if (!UpdHttpGet(url, buf, sizeof(buf))) {
        UpdAppendLog(*w, "[UPD] ERROR: update server unreachable");
        InterlockedExchange(&g_updBusy, 0);
        return 0;
    }
    char ver[64] = "", fnv[32] = "", djb[32] = "";
    char* line = strtok(buf, "\r\n");
    while (line) {
        if      (!strncmp(line, "version=", 8)) { strncpy(ver, line + 8, 63); ver[63] = 0; }
        else if (!strncmp(line, "fnv=", 4))     { strncpy(fnv, line + 4, 31); fnv[31] = 0; }
        else if (!strncmp(line, "djb=", 4))     { strncpy(djb, line + 4, 31); djb[31] = 0; }
        line = strtok(NULL, "\r\n");
    }
    if (ver[0]) {
        strncpy(g_updRemoteVer, ver, 63); g_updRemoteVer[63] = 0;
        strncpy(g_updRemoteFnv, fnv, 31);
        strncpy(g_updRemoteDjb, djb, 31);
        UpdAppendLog(*w, "[UPD] remote version: %s", ver);
        if (strcmp(ver, OS_VERSION) == 0) {
            UpdAppendLog(*w, "[UPD] OS is already up to date");
            InterlockedExchange(&g_updAvailable, 0);
        } else {
            UpdAppendLog(*w, "[UPD] NEW version available!");
            InterlockedExchange(&g_updAvailable, 1);
        }
    } else {
        UpdAppendLog(*w, "[UPD] ERROR: bad manifest");
    }
    InterlockedExchange(&g_updCheckDone, 1);
    InterlockedExchange(&g_updBusy, 0);
    return 0;
}

static unsigned __stdcall UpdApplyThread(void* p) {
    OsWindow* w = (OsWindow*)p;
    int ul = (int)strlen(g_updBaseUrl);
    if (ul && g_updBaseUrl[ul - 1] != '/') strcat(g_updBaseUrl, "/");
    char url[1200];
    wsprintfA(url, "%smain.cpp", g_updBaseUrl);
    wsprintfA(w->updOkMsg, "[UPD] downloading %s", url);
    UpdAppendLog(*w, "[UPD] downloading %s", url);
    char buf[524288];
    if (!UpdHttpGet(url, buf, sizeof(buf))) {
        UpdAppendLog(*w, "[UPD] ERROR: failed to download main.cpp");
        InterlockedExchange(&g_updBusy, 0);
        return 0;
    }
    // verify hashes
    unsigned long gotF = 2166136261UL, gotJ = 5381UL;
    for (char* c = buf; *c; c++) { gotF ^= (unsigned char)*c; gotF *= 16777619UL; gotJ = ((gotJ << 5) + gotJ) + (unsigned char)*c; }
    char wantF[32] = "", wantJ[32] = "";
    wsprintfA(wantF, "%08lX", gotF);
    wsprintfA(wantJ, "%08lX", gotJ);
    if (g_updRemoteFnv[0] && _stricmp(g_updRemoteFnv, wantF) != 0) {
        UpdAppendLog(*w, "[UPD] SECURITY: fnv mismatch, update aborted");
        InterlockedExchange(&g_updBusy, 0);
        return 0;
    }
    if (g_updRemoteDjb[0] && _stricmp(g_updRemoteDjb, wantJ) != 0) {
        UpdAppendLog(*w, "[UPD] SECURITY: djb mismatch, update aborted");
        InterlockedExchange(&g_updBusy, 0);
        return 0;
    }
    UpdAppendLog(*w, "[UPD] integrity verified");

    // write source to exe dir
    char srcPath[MAX_PATH];
    strcpy(srcPath, g_exePath);
    char* slash = strrchr(srcPath, '\\');
    slash[1] = 0;
    strcat(srcPath, "kashmaos7_new_src.cpp");
    WriteFileA_(srcPath, buf);

    char newExe[MAX_PATH];
    strcpy(newExe, srcPath);
    strcpy(newExe + strlen(newExe) - 3, "exe");
    char cmd[1600];
    wsprintfA(cmd, "C:\\msys64\\mingw64\\bin\\g++.exe -o \"%s\" \"%s\" -static -lgdi32 -lmsimg32 -lpsapi -lws2_32 -lole32 -luuid -lurlmon -lwininet -liphlpapi -lwlanapi -mwindows -std=c++17",
        newExe, srcPath);
    UpdAppendLog(*w, "[UPD] compiling...");
    unsigned long peak = 0;
    char out[560];
    wsprintfA(out, "%s\\System\\term_out.txt", G_DRIVE_ROOT);
    RunCmdCapture(cmd, out, false, &peak);
    if (GetFileAttributesA(newExe) == INVALID_FILE_ATTRIBUTES) {
        UpdAppendLog(*w, "[UPD] ERROR: build failed");
        InterlockedExchange(&g_updBusy, 0);
        return 0;
    }
    UpdAppendLog(*w, "[UPD] build OK (%lu KB peak)", peak);

    strncpy(g_updateNewExe, newExe, MAX_PATH - 1);
    g_updateRestart = true;
    UpdAppendLog(*w, "[UPD] restarting into new version...");
    InterlockedExchange(&g_updBusy, 0);
    PostQuitMessage(0);
    return 0;
}

static void AutoUpdLog(const char* fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    _vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    char path[MAX_PATH];
    wsprintfA(path, "%s\\System\\auto_upd.log", G_DRIVE_ROOT);
    FILE* f = fopen(path, "a");
    if (f) {
        SYSTEMTIME st; GetLocalTime(&st);
        fprintf(f, "[%02d:%02d:%02d] %s\r\n", st.wHour, st.wMinute, st.wSecond, buf);
        fclose(f);
    }
}

static unsigned __stdcall AutoUpdThread(void* p) {
    (void)p;
    Sleep(20000);
    int failCount = 0;
    for (;;) {
        if (g_autoUpd && !g_updBusy) {
            InterlockedExchange(&g_updBusy, 1);
            int ul = (int)strlen(g_updBaseUrl);
            if (ul && g_updBaseUrl[ul - 1] != '/') strcat(g_updBaseUrl, "/");
            char url[1200];
            wsprintfA(url, "%supdate.txt", g_updBaseUrl);
            AutoUpdLog("[AUTO] checking %s", url);
            char buf[8192];
            if (!UpdHttpGet(url, buf, sizeof(buf))) {
                AutoUpdLog("[AUTO] ERROR: update server unreachable");
                InterlockedExchange(&g_updBusy, 0);
                failCount++;
                if (failCount > 5) failCount = 5;
                Sleep(30 * 60 * 1000);  // retry faster on failure
                continue;
            }
            failCount = 0;
            char ver[64] = "", fnv[32] = "", djb[32] = "";
            char* line = strtok(buf, "\r\n");
            while (line) {
                if      (!strncmp(line, "version=", 8)) { strncpy(ver, line + 8, 63); ver[63] = 0; }
                else if (!strncmp(line, "fnv=", 4))     { strncpy(fnv, line + 4, 31); fnv[31] = 0; }
                else if (!strncmp(line, "djb=", 4))     { strncpy(djb, line + 4, 31); djb[31] = 0; }
                line = strtok(NULL, "\r\n");
            }
            if (ver[0] && strcmp(ver, OS_VERSION) != 0) {
                strncpy(g_updRemoteVer, ver, 63); g_updRemoteVer[63] = 0;
                strncpy(g_updRemoteFnv, fnv, 31);
                strncpy(g_updRemoteDjb, djb, 31);
                AutoUpdLog("[AUTO] NEW version %s available (current %s), applying...", ver, OS_VERSION);
                // download main.cpp
                wsprintfA(url, "%smain.cpp", g_updBaseUrl);
                AutoUpdLog("[AUTO] downloading %s", url);
                char src[524288];
                if (!UpdHttpGet(url, src, sizeof(src))) {
                    AutoUpdLog("[AUTO] ERROR: failed to download main.cpp");
                    InterlockedExchange(&g_updBusy, 0);
                    Sleep(30 * 60 * 1000);
                    continue;
                }
                unsigned long gotF = 2166136261UL, gotJ = 5381UL;
                for (char* c = src; *c; c++) { gotF ^= (unsigned char)*c; gotF *= 16777619UL; gotJ = ((gotJ << 5) + gotJ) + (unsigned char)*c; }
                char wantF[32] = "", wantJ[32] = "";
                wsprintfA(wantF, "%08lX", gotF);
                wsprintfA(wantJ, "%08lX", gotJ);
                if (g_updRemoteFnv[0] && _stricmp(g_updRemoteFnv, wantF) != 0) {
                    AutoUpdLog("[AUTO] SECURITY: fnv mismatch, update aborted");
                    InterlockedExchange(&g_updBusy, 0);
                    Sleep(60 * 60 * 1000);
                    continue;
                }
                if (g_updRemoteDjb[0] && _stricmp(g_updRemoteDjb, wantJ) != 0) {
                    AutoUpdLog("[AUTO] SECURITY: djb mismatch, update aborted");
                    InterlockedExchange(&g_updBusy, 0);
                    Sleep(60 * 60 * 1000);
                    continue;
                }
                AutoUpdLog("[AUTO] integrity verified");
                char srcPath[MAX_PATH];
                strcpy(srcPath, g_exePath);
                char* slash = strrchr(srcPath, '\\');
                slash[1] = 0;
                strcat(srcPath, "kashmaos7_new_src.cpp");
                WriteFileA_(srcPath, src);
                char newExe[MAX_PATH];
                strcpy(newExe, srcPath);
                strcpy(newExe + strlen(newExe) - 3, "exe");
                char cmd[1600];
                wsprintfA(cmd, "C:\\msys64\\mingw64\\bin\\g++.exe -o \"%s\" \"%s\" -static -lgdi32 -lmsimg32 -lpsapi -lws2_32 -lole32 -luuid -lurlmon -lwininet -liphlpapi -lwlanapi -mwindows -std=c++17",
                    newExe, srcPath);
                AutoUpdLog("[AUTO] compiling...");
                unsigned long peak = 0;
                char out[560];
                wsprintfA(out, "%s\\System\\term_out.txt", G_DRIVE_ROOT);
                RunCmdCapture(cmd, out, false, &peak);
                if (GetFileAttributesA(newExe) == INVALID_FILE_ATTRIBUTES) {
                    AutoUpdLog("[AUTO] ERROR: build failed");
                    InterlockedExchange(&g_updBusy, 0);
                    Sleep(60 * 60 * 1000);
                    continue;
                }
                AutoUpdLog("[AUTO] build OK (%lu KB peak)", peak);
                strncpy(g_updateNewExe, newExe, MAX_PATH - 1);
                g_updateRestart = true;
                AutoUpdLog("[AUTO] restarting into new version...");
                InterlockedExchange(&g_updBusy, 0);
                PostQuitMessage(0);
                return 0;
            } else if (ver[0]) {
                AutoUpdLog("[AUTO] up to date (%s)", OS_VERSION);
            } else {
                AutoUpdLog("[AUTO] ERROR: bad manifest");
            }
            InterlockedExchange(&g_updBusy, 0);
        }
        Sleep(60 * 60 * 1000);  // check every hour
    }
    return 0;
}

static bool KmlTcpConnect(const char* ip, int port, SOCKET& out) {
    out = socket(AF_INET, SOCK_STREAM, 0);
    if (out == INVALID_SOCKET) return false;
    sockaddr_in sa = {};
    sa.sin_family = AF_INET;
    sa.sin_port = htons((u_short)port);
    sa.sin_addr.s_addr = inet_addr(ip);
    if (sa.sin_addr.s_addr == INADDR_NONE) {
        hostent* he = gethostbyname(ip);
        if (!he) { closesocket(out); out = INVALID_SOCKET; return false; }
        memcpy(&sa.sin_addr, he->h_addr_list[0], 4);
    }
    if (connect(out, (sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR) {
        closesocket(out); out = INVALID_SOCKET; return false;
    }
    return true;
}

static void KmlPump(SOCKET src, SOCKET dst) {
    char buf[65536];
    for (;;) {
        int n = recv(src, buf, (int)sizeof(buf), 0);
        if (n <= 0) return;
        int off = 0;
        while (off < n) {
            int wr = send(dst, buf + off, n - off, 0);
            if (wr <= 0) return;
            off += wr;
        }
    }
}

struct KmlPipe { SOCKET a, b; };

static unsigned __stdcall KmlPipeA(void* p) {
    KmlPipe* pp = (KmlPipe*)p;
    KmlPump(pp->a, pp->b);
    shutdown(pp->b, SD_BOTH); closesocket(pp->b);
    return 0;
}

static unsigned __stdcall KmlPipeB(void* p) {
    KmlPipe* pp = (KmlPipe*)p;
    KmlPump(pp->b, pp->a);
    shutdown(pp->a, SD_BOTH); closesocket(pp->a);
    return 0;
}

static unsigned __stdcall KmlHostThread(void* param) {
    OsWindow* w = (OsWindow*)param;
    SOCKET ls = socket(AF_INET, SOCK_STREAM, 0);
    if (ls == INVALID_SOCKET) {
        KmlAppendLog(*w, "[HOST] socket fail");
        InterlockedExchange(&g_kmlRun, 0);
        return 1;
    }
    BOOL yes = 1;
    setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes));
    sockaddr_in sa = {};
    sa.sin_family = AF_INET;
    sa.sin_port = htons((u_short)g_kmlHostPortCur);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(ls, (sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR) {
        KmlAppendLog(*w, "[HOST] bind 0.0.0.0:%d failed", g_kmlHostPortCur);
        closesocket(ls);
        InterlockedExchange(&g_kmlRun, 0);
        return 1;
    }
    listen(ls, 16);
    g_kmlListen = ls;
    KmlAppendLog(*w, "[HOST] room %s up on TCP %d", w->kmlRoom, g_kmlHostPortCur);
    while (InterlockedCompareExchange(&g_kmlRun, 1, 1) == 1) {
        SOCKET cs = accept(ls, NULL, NULL);
        if (cs == INVALID_SOCKET) break;
        long id = InterlockedIncrement(&g_kmlClientId);
        w->kmlClientCount = (int)(id + 1);
        SOCKET gs = INVALID_SOCKET;
        if (KmlTcpConnect("127.0.0.1", g_kmlGamePortCur, gs)) {
            KmlAppendLog(*w, "[HOST] client 26.0.0.%d joined", (int)(id + 1));
            KmlPipe* pp = new KmlPipe;
            pp->a = cs; pp->b = gs;
            _beginthreadex(NULL, 0, KmlPipeA, pp, 0, NULL);
            _beginthreadex(NULL, 0, KmlPipeB, pp, 0, NULL);
        } else {
            closesocket(cs);
            KmlAppendLog(*w, "[HOST] game server 127.0.0.1:%d not reachable", g_kmlGamePortCur);
        }
    }
    closesocket(ls);
    g_kmlListen = INVALID_SOCKET;
    return 0;
}

static unsigned __stdcall KmlClientThread(void* param) {
    OsWindow* w = (OsWindow*)param;
    SOCKET ls = socket(AF_INET, SOCK_STREAM, 0);
    if (ls == INVALID_SOCKET) {
        KmlAppendLog(*w, "[CLIENT] socket fail");
        InterlockedExchange(&g_kmlRun, 0);
        return 1;
    }
    BOOL yes = 1;
    setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes));
    sockaddr_in sa = {};
    sa.sin_family = AF_INET;
    sa.sin_port = htons((u_short)g_kmlLocalPortCur);
    sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(ls, (sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR) {
        KmlAppendLog(*w, "[CLIENT] bind 127.0.0.1:%d failed", g_kmlLocalPortCur);
        closesocket(ls);
        InterlockedExchange(&g_kmlRun, 0);
        return 1;
    }
    listen(ls, 16);
    g_kmlListen = ls;
    KmlAppendLog(*w, "[CLIENT] game->127.0.0.1:%d -> %s:%d", g_kmlLocalPortCur, g_kmlHostIpCur, g_kmlHostPortCur);
    while (InterlockedCompareExchange(&g_kmlRun, 1, 1) == 1) {
        SOCKET game = accept(ls, NULL, NULL);
        if (game == INVALID_SOCKET) break;
        long id = InterlockedIncrement(&g_kmlClientId);
        w->kmlClientCount = (int)(id + 1);
        SOCKET hs = INVALID_SOCKET;
        if (!KmlTcpConnect(g_kmlHostIpCur, g_kmlHostPortCur, hs)) {
            closesocket(game);
            KmlAppendLog(*w, "[CLIENT] host %s:%d not reachable", g_kmlHostIpCur, g_kmlHostPortCur);
            continue;
        }
        KmlAppendLog(*w, "[CLIENT] tunnel 26.0.0.%d up", (int)(id + 1));
        KmlPipe* pp = new KmlPipe;
        pp->a = game; pp->b = hs;
        _beginthreadex(NULL, 0, KmlPipeA, pp, 0, NULL);
        _beginthreadex(NULL, 0, KmlPipeB, pp, 0, NULL);
    }
    closesocket(ls);
    g_kmlListen = INVALID_SOCKET;
    return 0;
}

static void KmlStartHost(OsWindow& w) {
    w.kmlGamePort[15] = 0; w.kmlListenPort[15] = 0;
    g_kmlGamePortCur = atoi(w.kmlGamePort);
    g_kmlHostPortCur = atoi(w.kmlListenPort);
    if (g_kmlGamePortCur < 1 || g_kmlGamePortCur > 65535 || g_kmlHostPortCur < 1 || g_kmlHostPortCur > 65535) {
        KmlAppendLog(w, "[KML] bad port value");
        return;
    }
    for (int i = 0; i < 6; i++) w.kmlRoom[i] = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789"[rand() % 32];
    w.kmlRoom[6] = 0;
    w.kmlClientCount = 0;
    InterlockedExchange(&g_kmlClientId, 0);
    InterlockedExchange(&g_kmlRun, 1);
    w.kmlRunning = true;
    g_kmlThread = (HANDLE)_beginthreadex(NULL, 0, KmlHostThread, &w, 0, NULL);
}

static void KmlStartJoin(OsWindow& w) {
    w.kmlHostIp[63] = 0; w.kmlHostPort[15] = 0; w.kmlLocalPort[15] = 0;
    strncpy(g_kmlHostIpCur, w.kmlHostIp, 63); g_kmlHostIpCur[63] = 0;
    g_kmlHostPortCur = atoi(w.kmlHostPort);
    g_kmlLocalPortCur = atoi(w.kmlLocalPort);
    if (!g_kmlHostIpCur[0] || g_kmlHostPortCur < 1 || g_kmlHostPortCur > 65535 ||
        g_kmlLocalPortCur < 1 || g_kmlLocalPortCur > 65535) {
        KmlAppendLog(w, "[KML] bad parameters");
        return;
    }
    w.kmlClientCount = 0;
    InterlockedExchange(&g_kmlClientId, 0);
    InterlockedExchange(&g_kmlRun, 1);
    w.kmlRunning = true;
    g_kmlThread = (HANDLE)_beginthreadex(NULL, 0, KmlClientThread, &w, 0, NULL);
}

static void KmlStop(OsWindow& w) {
    InterlockedExchange(&g_kmlRun, 0);
    if (g_kmlListen != INVALID_SOCKET) {
        closesocket(g_kmlListen);
        g_kmlListen = INVALID_SOCKET;
    }
    if (g_kmlThread) {
        WaitForSingleObject(g_kmlThread, 2000);
        g_kmlThread = NULL;
    }
    w.kmlRunning = false;
    w.kmlClientCount = 0;
    KmlAppendLog(w, "[KML] tunnel closed");
}

static void DrawKmlApp(HDC hdc, OsWindow& w) {
    int ex = w.x + 4, ey = w.y + TITLEBAR_H + 4;
    int ew = w.w - 8, eh = w.h - TITLEBAR_H - 8;
    HBRUSH bg = CreateSolidBrush(RGB(230,245,240));
    RECT br = {ex, ey, ex+ew, ey+eh};
    FillRect(hdc, &br, bg); DeleteObject(bg);
    HBRUSH hdr = CreateSolidBrush(RGB(20,140,120));
    RECT hr = {ex, ey, ex+ew, ey+40};
    FillRect(hdc, &hr, hdr); DeleteObject(hdr);
    HFONT hf = MakeFont(15, TRUE);
    HFONT old = (HFONT)SelectObject(hdc, hf);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255,255,255));
    RECT tr = {ex+12, ey+8, ex+ew-12, ey+32};
    DrawTextW(hdc, L"KashmaLink  -  virtual LAN", -1, &tr, DT_LEFT | DT_TOP);
    SelectObject(hdc, old); DeleteObject(hf);

    int tabW = 110, tabH = 26, taby = ey + 44;
    int tabX1 = ex + 12, tabX2 = ex + 12 + tabW + 6;
    HBRUSH active = CreateSolidBrush(RGB(20,140,120));
    HBRUSH inactive = CreateSolidBrush(RGB(190,220,210));
    SelectObject(hdc, active);
    if (w.kmlHostMode) {
        RECT t1 = {tabX1, taby, tabX1+tabW, taby+tabH};
        FillRect(hdc, &t1, active);
        SelectObject(hdc, inactive);
        RECT t2 = {tabX2, taby, tabX2+tabW, taby+tabH};
        FillRect(hdc, &t2, inactive);
    } else {
        RECT t1 = {tabX1, taby, tabX1+tabW, taby+tabH};
        FillRect(hdc, &t1, inactive);
        SelectObject(hdc, active);
        RECT t2 = {tabX2, taby, tabX2+tabW, taby+tabH};
        FillRect(hdc, &t2, active);
    }
    HFONT tf = MakeFont(13, TRUE);
    old = (HFONT)SelectObject(hdc, tf);
    SetTextColor(hdc, RGB(255,255,255));
    RECT tr1 = {tabX1, taby, tabX1+tabW, taby+tabH};
    DrawTextW(hdc, L"HOST", -1, &tr1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    RECT tr2 = {tabX2, taby, tabX2+tabW, taby+tabH};
    DrawTextW(hdc, L"JOIN", -1, &tr2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old); DeleteObject(tf);

    HFONT sf = MakeFont(12, FALSE);
    old = (HFONT)SelectObject(hdc, sf);
    int fy = taby + tabH + 14;
    int labX = ex + 12, fldX = ex + 140, fldW = 150, fldH = 24;
    SetTextColor(hdc, RGB(0,0,0));
    int fcount = w.kmlHostMode ? 2 : 3;
    for (int i = 0; i < fcount; i++) {
        const char* label = NULL;
        char* val = NULL;
        if (w.kmlHostMode) {
            label = (i == 0) ? "Game port" : "Listen port";
            val = (i == 0) ? w.kmlGamePort : w.kmlListenPort;
        } else {
            label = (i == 0) ? "Host IP" : (i == 1) ? "Host port" : "Local port";
            val = (i == 0) ? w.kmlHostIp : (i == 1) ? w.kmlHostPort : w.kmlLocalPort;
        }
        RECT lr = {labX, fy + i*30, fldX, fy + i*30 + fldH};
        DrawTextA(hdc, label, -1, &lr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        HBRUSH wb = CreateSolidBrush(RGB(255,255,255));
        RECT fr = {fldX, fy + i*30, fldX+fldW, fy + i*30 + fldH};
        FillRect(hdc, &fr, wb); DeleteObject(wb);
        HPEN fp = CreatePen(PS_SOLID, w.kmlFocused == i+1 ? 2 : 1,
                            w.kmlFocused == i+1 ? RGB(20,140,120) : RGB(120,120,120));
        SelectObject(hdc, fp); SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, fr.left, fr.top, fr.right, fr.bottom);
        DeleteObject(fp);
        RECT vr = {fldX+4, fy + i*30, fldX+fldW, fy + i*30 + fldH};
        DrawTextA(hdc, val, -1, &vr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    }

    int by = fy + fcount*30 + 14, bW = 130, bH = 30;
    int startX = ex + 12;
    RECT btn = {startX, by, startX+bW, by+bH};
    HBRUSH sb = CreateSolidBrush(w.kmlRunning ? RGB(160,60,60) : RGB(20,140,120));
    FillRect(hdc, &btn, sb); DeleteObject(sb);
    HPEN bp = CreatePen(PS_SOLID, 1, RGB(10,90,75));
    SelectObject(hdc, bp); SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, btn.left, btn.top, btn.right, btn.bottom);
    DeleteObject(bp);
    HFONT bf = MakeFont(13, TRUE);
    old = (HFONT)SelectObject(hdc, bf);
    SetTextColor(hdc, RGB(255,255,255));
    const wchar_t* startLbl = w.kmlHostMode ? L"Start Host" : L"Start Join";
    if (w.kmlRunning) startLbl = L"Stop";
    DrawTextW(hdc, startLbl, -1, &btn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    RECT roomr = {startX + bW + 16, by, ex+ew-12, by+bH};
    if (w.kmlRunning) {
        wchar_t roomText[80];
        wsprintfW(roomText, w.kmlHostMode ? L"Room: %S   tunnels: %d" : L"Tunnels up: %d",
                  (const char*)w.kmlRoom, w.kmlClientCount);
        DrawTextW(hdc, roomText, -1, &roomr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
    SelectObject(hdc, old); DeleteObject(bf);

    int lgy = by + bH + 12;
    RECT lbr = {ex+12, lgy, ex+ew-12, ey+eh-10};
    HBRUSH lw = CreateSolidBrush(RGB(255,255,255));
    FillRect(hdc, &lbr, lw); DeleteObject(lw);
    HPEN lp = CreatePen(PS_SOLID, 1, RGB(150,150,150));
    SelectObject(hdc, lp); SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, lbr.left, lbr.top, lbr.right, lbr.bottom);
    DeleteObject(lp);
    SelectObject(hdc, old); DeleteObject(sf);

    HFONT lf2 = MakeFont(11, FALSE);
    old = (HFONT)SelectObject(hdc, lf2);
    SetTextColor(hdc, RGB(40,40,40));
    RECT cl = {lbr.left+5, lbr.top+3, lbr.right-5, lbr.bottom-3};
    DrawTextA(hdc, w.kmlLog, -1, &cl, DT_LEFT | DT_TOP | DT_NOPREFIX);
    SelectObject(hdc, old); DeleteObject(lf2);
}

static void DrawTaskMgrApp(HDC hdc, OsWindow& w) {
    int ex = w.x + 4, ey = w.y + TITLEBAR_H + 4;
    int ew = w.w - 8, eh = w.h - TITLEBAR_H - 8;
    HBRUSH bg = CreateSolidBrush(RGB(238,240,246));
    RECT br = {ex, ey, ex+ew, ey+eh};
    FillRect(hdc, &br, bg); DeleteObject(bg);
    HBRUSH hdr = CreateSolidBrush(RGB(30,70,150));
    RECT hr = {ex, ey, ex+ew, ey+40};
    FillRect(hdc, &hr, hdr); DeleteObject(hdr);
    HFONT hf = MakeFont(15, TRUE);
    HFONT old = (HFONT)SelectObject(hdc, hf);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255,255,255));
    RECT tr = {ex+12, ey+8, ex+ew-12, ey+32};
    DrawTextW(hdc, L"Task Manager  -  active processes", -1, &tr, DT_LEFT | DT_TOP);
    SelectObject(hdc, old); DeleteObject(hf);

    static const wchar_t* colNames[] = { L"Process", L"Type", L"State" };
    int cxs[] = { ex + 12, ex + 260, ex + 400 };
    int rw = 28, ry = ey + 46;
    HFONT hf2 = MakeFont(12, TRUE);
    old = (HFONT)SelectObject(hdc, hf2);
    SetTextColor(hdc, RGB(0,0,0));
    for (int c = 0; c < 3; c++) {
        RECT cr = {cxs[c], ry, cxs[c] + 140, ry + rw};
        DrawTextW(hdc, colNames[c], -1, &cr, DT_LEFT | DT_TOP);
    }
    SelectObject(hdc, old); DeleteObject(hf2);

    int idx = 0;
    HFONT rf = MakeFont(12, FALSE);
    old = (HFONT)SelectObject(hdc, rf);
    SetTextColor(hdc, RGB(30,30,30));
    int count = 0;
    for (int i = 0; i < g_winCount; i++) {
        if (!g_windows[i].open) continue;
        count++;
        int y0 = ry + rw + 2 + idx * rw;
        if (idx % 2 == 1) {
            HBRUSH zb = CreateSolidBrush(RGB(228,232,240));
            RECT zr = {ex, y0, ex+ew, y0+rw};
            FillRect(hdc, &zr, zb); DeleteObject(zb);
        }
        RECT r1 = {cxs[0], y0, ex+300, y0+rw};
        DrawTextW(hdc, g_windows[i].title, -1, &r1, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        const wchar_t* ttype = L"GUI app";
        RECT r2 = {cxs[1], y0, cxs[1]+140, y0+rw};
        DrawTextW(hdc, ttype, -1, &r2, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        RECT r3 = {cxs[2], y0, cxs[2]+140, y0+rw};
        DrawTextW(hdc, g_windows[i].minimized ? L"Minimized" : L"Running", -1, &r3, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        if (g_activeWin == i) {
            HBRUSH ab = CreateSolidBrush(RGB(200,220,250));
            RECT ar = {ex+2, y0, ex+ew-2, y0+rw};
            FillRect(hdc, &ar, ab); DeleteObject(ab);
            SetTextColor(hdc, RGB(0,60,140));
            DrawTextW(hdc, L"<< active", -1, &r3, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
            SetTextColor(hdc, RGB(30,30,30));
        }
        idx++;
    }
    SelectObject(hdc, old); DeleteObject(rf);
    if (count == 0) {
        HFONT emp = MakeFont(12, FALSE);
        old = (HFONT)SelectObject(hdc, emp);
        SetTextColor(hdc, RGB(120,120,120));
        RECT er = {ex, ry+rw+10, ex+ew, ey+eh};
        DrawTextW(hdc, L"No running processes", -1, &er, DT_CENTER | DT_TOP);
        SelectObject(hdc, old); DeleteObject(emp);
    }
}

static void InetLocalIp(wchar_t* out) {
    out[0] = 0;
    ULONG sz = 0;
    GetAdaptersAddresses(AF_INET, 0, NULL, NULL, &sz);
    if (!sz) return;
    IP_ADAPTER_ADDRESSES* aa = (IP_ADAPTER_ADDRESSES*)malloc(sz);
    if (!aa) return;
    if (GetAdaptersAddresses(AF_INET, 0, NULL, aa, &sz) == NO_ERROR) {
        for (IP_ADAPTER_ADDRESSES* p = aa; p; p = p->Next) {
            if (p->IfType == IF_TYPE_SOFTWARE_LOOPBACK) continue;
            if (p->OperStatus != IfOperStatusUp) continue;
            for (IP_ADAPTER_UNICAST_ADDRESS* u = p->FirstUnicastAddress; u; u = u->Next) {
                sockaddr_in* sa = (sockaddr_in*)u->Address.lpSockaddr;
                if (!sa) continue;
                unsigned char* b = (unsigned char*)&sa->sin_addr;
                wsprintfW(out, L"%u.%u.%u.%u", (unsigned)b[0], (unsigned)b[1], (unsigned)b[2], (unsigned)b[3]);
                goto done;
            }
        }
    }
done:
    free(aa);
}

static void EscapeXmlW(const wchar_t* in, wchar_t* out, int cap) {
    int oi = 0;
    for (const wchar_t* p = in; *p && oi < cap - 8; p++) {
        if (*p == L'&') { wcscpy(out + oi, L"&amp;"); oi += 5; }
        else if (*p == L'<') { wcscpy(out + oi, L"&lt;"); oi += 4; }
        else if (*p == L'>') { wcscpy(out + oi, L"&gt;"); oi += 4; }
        else if (*p == L'"') { wcscpy(out + oi, L"&quot;"); oi += 6; }
        else out[oi++] = *p;
    }
    out[oi] = 0;
}

static void SsidBytesToWide(const unsigned char* s, int n, wchar_t* out) {
    int o = 0, i = 0;
    while (i < n && o < 32) {
        unsigned char c = s[i];
        unsigned cp = 0; int extra = 0;
        if (c < 0x80) { cp = c; }
        else if ((c >> 5) == 0x6) { cp = c & 0x1F; extra = 1; }
        else if ((c >> 4) == 0xE) { cp = c & 0x0F; extra = 2; }
        else if ((c >> 3) == 0x1E) { cp = c & 0x07; extra = 3; }
        else { cp = L'?'; extra = 0; }
        i++;
        int ok = 1;
        for (int k = 0; k < extra && i < n; k++) {
            if ((s[i] & 0xC0) != 0x80) { ok = 0; break; }
            cp = (cp << 6) | (s[i] & 0x3F);
            i++;
        }
        if (!ok) out[o++] = L'?';
        else out[o++] = (cp <= 0xFFFF) ? (wchar_t)cp : L'?';
    }
    out[o] = 0;
}

static void InetScan(OsWindow& w) {
    w.inetCount = 0;
    w.inetSel = -1;
    for (int i = 0; i < 8; i++) { w.inetSsid[i][0] = 0; w.inetSec[i] = false; w.inetKnown[i] = false; w.inetSig[i] = 0; w.inetConn[i] = false; }
    w.inetNetName[0] = 0;
    w.inetIp[0] = 0;
    DWORD flags = 0;
    w.inetConnected = (InternetGetConnectedState(&flags, 0) != 0);
    InetLocalIp(w.inetIp);
    HANDLE hClient = NULL;
    DWORD nVer = 0;
    if (WlanOpenHandle(2, NULL, &nVer, &hClient) != ERROR_SUCCESS) {
        if (w.inetConnected) wsprintfW(w.inetStatus, L"\u0418\u043d\u0442\u0435\u0440\u043d\u0435\u0442 Windows: \u0434\u0430  |  IP: %s", w.inetIp[0] ? w.inetIp : L"-");
        else wcscpy(w.inetStatus, L"\u0418\u043d\u0442\u0435\u0440\u043d\u0435\u0442 Windows: \u043d\u0435\u0442 \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u044f");
        return;
    }
    WLAN_INTERFACE_INFO_LIST* ifl = NULL;
    if (WlanEnumInterfaces(hClient, NULL, &ifl) != ERROR_SUCCESS || !ifl) {
        WlanCloseHandle(hClient, NULL);
        if (w.inetConnected) wsprintfW(w.inetStatus, L"\u0418\u043d\u0442\u0435\u0440\u043d\u0435\u0442 Windows: \u0434\u0430  |  IP: %s", w.inetIp[0] ? w.inetIp : L"-");
        else wcscpy(w.inetStatus, L"\u0418\u043d\u0442\u0435\u0440\u043d\u0435\u0442 Windows: \u043d\u0435\u0442 \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u044f");
        wcscpy(w.inetNetName, L"Wi-Fi \u0430\u0434\u0430\u043f\u0442\u0435\u0440 \u043d\u0435 \u043d\u0430\u0439\u0434\u0435\u043d");
        return;
    }
    GUID* gw = NULL;
    for (DWORD i = 0; i < ifl->dwNumberOfItems; i++) {
        if (ifl->InterfaceInfo[i].isState == wlan_interface_state_disconnected ||
            ifl->InterfaceInfo[i].isState == wlan_interface_state_connected) {
            gw = &ifl->InterfaceInfo[i].InterfaceGuid;
            break;
        }
        if (!gw) gw = &ifl->InterfaceInfo[i].InterfaceGuid;
    }
    if (gw) {
        for (DWORD i = 0; i < ifl->dwNumberOfItems; i++) {
            WLAN_INTERFACE_INFO& ii = ifl->InterfaceInfo[i];
            if (ii.isState == wlan_interface_state_connected) {
                DWORD sz2 = 0;
                WLAN_CONNECTION_ATTRIBUTES* ca = NULL;
                if (WlanQueryInterface(hClient, &ii.InterfaceGuid, wlan_intf_opcode_current_connection, NULL, &sz2, (PVOID*)&ca, NULL) == ERROR_SUCCESS && ca) {
                    if (ca->strProfileName[0]) wcscpy(w.inetNetName, ca->strProfileName);
                    else if (ca->wlanAssociationAttributes.dot11Ssid.uSSIDLength > 0)
                        SsidBytesToWide(ca->wlanAssociationAttributes.dot11Ssid.ucSSID, (int)ca->wlanAssociationAttributes.dot11Ssid.uSSIDLength, w.inetNetName);
                    w.inetNetName[63] = 0;
                }
                WlanFreeMemory(ca);
            }
        }
        WlanScan(hClient, gw, NULL, NULL, NULL);
        WLAN_AVAILABLE_NETWORK_LIST* nl = NULL;
        if (WlanGetAvailableNetworkList(hClient, gw, 0, NULL, &nl) == ERROR_SUCCESS && nl) {
            for (DWORD k = 0; k < nl->dwNumberOfItems; k++) {
                if (w.inetCount >= 8) break;
                WLAN_AVAILABLE_NETWORK& n = nl->Network[k];
                int idx = w.inetCount++;
                w.inetSig[idx] = (int)n.wlanSignalQuality;
                w.inetSec[idx] = n.bSecurityEnabled ? true : false;
                w.inetConn[idx] = (n.dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED) ? true : false;
                DWORD sl = n.dot11Ssid.uSSIDLength;
                if (sl > 32) sl = 32;
                SsidBytesToWide(n.dot11Ssid.ucSSID, (int)sl, w.inetSsid[idx]);
            }
            WlanFreeMemory(nl);
        }
        WLAN_PROFILE_INFO_LIST* pl = NULL;
        if (WlanGetProfileList(hClient, gw, NULL, &pl) == ERROR_SUCCESS && pl) {
            for (DWORD k = 0; k < pl->dwNumberOfItems; k++) {
                const wchar_t* pn = pl->ProfileInfo[k].strProfileName;
                for (int i = 0; i < w.inetCount; i++)
                    if (w.inetSsid[i][0] && _wcsicmp(pn, w.inetSsid[i]) == 0) w.inetKnown[i] = true;
            }
            WlanFreeMemory(pl);
        }
    }
    WlanFreeMemory(ifl);
    WlanCloseHandle(hClient, NULL);
    if (w.inetConnected)
        wsprintfW(w.inetStatus, L"\u0418\u043d\u0442\u0435\u0440\u043d\u0435\u0442 Windows: \u0434\u0430  |  IP: %s", w.inetIp[0] ? w.inetIp : L"-");
    else
        wcscpy(w.inetStatus, L"\u0418\u043d\u0442\u0435\u0440\u043d\u0435\u0442 Windows: \u043d\u0435\u0442 \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u044f");
    if (!w.inetNetName[0]) wcscpy(w.inetNetName, L"Wi-Fi: \u043d\u0435 \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u043e");
}

static void InetConnectTo(OsWindow& w, int sel) {
    if (sel < 0 || sel >= w.inetCount) return;
    HANDLE hClient = NULL;
    DWORD nVer = 0;
    if (WlanOpenHandle(2, NULL, &nVer, &hClient) != ERROR_SUCCESS) {
        wcscpy(w.inetMsg, L"\u041e\u0448\u0438\u0431\u043a\u0430: Wi-Fi \u043d\u0435\u0434\u043e\u0441\u0442\u0443\u043f\u0435\u043d");
        return;
    }
    WLAN_INTERFACE_INFO_LIST* ifl = NULL;
    if (WlanEnumInterfaces(hClient, NULL, &ifl) != ERROR_SUCCESS || !ifl) {
        WlanCloseHandle(hClient, NULL);
        wcscpy(w.inetMsg, L"\u041e\u0448\u0438\u0431\u043a\u0430: Wi-Fi \u043d\u0435\u0434\u043e\u0441\u0442\u0443\u043f\u0435\u043d");
        return;
    }
    GUID* gw = NULL;
    if (ifl->dwNumberOfItems > 0) gw = &ifl->InterfaceInfo[0].InterfaceGuid;
    if (!gw) {
        WlanFreeMemory(ifl);
        WlanCloseHandle(hClient, NULL);
        wcscpy(w.inetMsg, L"\u041e\u0448\u0438\u0431\u043a\u0430: Wi-Fi \u043d\u0435\u0434\u043e\u0441\u0442\u0443\u043f\u0435\u043d");
        return;
    }
    wchar_t sid[33];
    wcscpy(sid, w.inetSsid[sel]);
    wcscpy(w.inetMsg, L"\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435...");
    if (w.inetSec[sel]) {
        if (!w.inetKnown[sel]) {
            if (w.inetPass[0] == 0) {
                wcscpy(w.inetMsg, L"\u041d\u0435\u0442 \u043f\u0430\u0440\u043e\u043b\u044f");
                WlanFreeMemory(ifl);
                WlanCloseHandle(hClient, NULL);
                return;
            }
            wchar_t sidE[80];
            wchar_t passE[80];
            EscapeXmlW(sid, sidE, 80);
            EscapeXmlW(w.inetPass, passE, 80);
            wchar_t xml[2200];
            wsprintfW(xml, L"<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name>%s</name><SSIDConfig><SSID><name>%s</name></SSID></SSIDConfig><connectionType>ESS</connectionType><connectionMode>auto</connectionMode><MSM><security><authEncryption><authentication>WPA2PSK</authentication><encryption>AES</encryption><useOneX>false</useOneX></authEncryption><sharedKey><keyType>passPhrase</keyType><protected>false</protected><keyMaterial>%s</keyMaterial></sharedKey></security></MSM></WLANProfile>", sidE, sidE, passE);
            DWORD reason = 0;
            WlanSetProfile(hClient, gw, 0, xml, NULL, TRUE, NULL, &reason);
        }
        WLAN_CONNECTION_PARAMETERS cp;
        ZeroMemory(&cp, sizeof(cp));
        cp.wlanConnectionMode = wlan_connection_mode_profile;
        cp.strProfile = sid;
        cp.pDot11Ssid = NULL;
        cp.pDesiredBssidList = NULL;
        cp.dot11BssType = dot11_BSS_type_infrastructure;
        cp.dwFlags = 0;
        HRESULT hw = WlanConnect(hClient, gw, &cp, NULL);
        if (hw != ERROR_SUCCESS)
            wcscpy(w.inetMsg, w.inetKnown[sel] ? L"\u041d\u0435 \u0443\u0434\u0430\u043b\u043e\u0441\u044c \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0438\u0442\u044c\u0441\u044f" : L"\u041d\u0435 \u0443\u0434\u0430\u043b\u043e\u0441\u044c \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0438\u0442\u044c\u0441\u044f (\u043f\u0440\u043e\u0432\u0435\u0440\u044c \u043f\u0430\u0440\u043e\u043b\u044c)");
    } else {
        DOT11_SSID ds;
        ZeroMemory(&ds, sizeof(ds));
        int L = 0;
        for (int i = 0; i < 32 && sid[i]; i++) { ds.ucSSID[i] = (unsigned char)sid[i]; L = i + 1; }
        ds.uSSIDLength = (ULONG)L;
        WLAN_CONNECTION_PARAMETERS cp;
        ZeroMemory(&cp, sizeof(cp));
        cp.wlanConnectionMode = wlan_connection_mode_discovery_unsecure;
        cp.strProfile = NULL;
        cp.pDot11Ssid = &ds;
        cp.pDesiredBssidList = NULL;
        cp.dot11BssType = dot11_BSS_type_infrastructure;
        cp.dwFlags = 0;
        HRESULT hw = WlanConnect(hClient, gw, &cp, NULL);
        if (hw != ERROR_SUCCESS)
            wcscpy(w.inetMsg, L"\u041d\u0435 \u0443\u0434\u0430\u043b\u043e\u0441\u044c \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0438\u0442\u044c\u0441\u044f");
    }
    WlanFreeMemory(ifl);
    WlanCloseHandle(hClient, NULL);
}

static unsigned __stdcall InetScanThread(void* arg) {
    OsWindow* w = (OsWindow*)arg;
    if (!w->inetMsg[0]) wcscpy(w->inetMsg, L"\u0412\u044b\u0431\u0435\u0440\u0438 Wi-Fi \u0441\u0435\u0442\u044c \u2014 \u0438\u043b\u0438 \u043f\u0440\u043e\u0432\u0435\u0440\u044c Ethernet");
    InetScan(*w);
    w->inetBusy = false;
    InvalidateRect(g_hwnd, NULL, FALSE);
    return 0;
}

static unsigned __stdcall InetConnectThread(void* arg) {
    OsWindow* w = (OsWindow*)arg;
    InetConnectTo(*w, w->inetSel);
    Sleep(4000);
    InetScan(*w);
    if (w->inetNetName[0]) {
        wchar_t* colon = wcschr(w->inetNetName, L':');
        bool realName = w->inetConnected && !(colon && colon == w->inetNetName + 3 && wcsstr(w->inetNetName, L"Wi-Fi"));
        if (realName) wsprintfW(w->inetMsg, L"\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u043e \u043a %s", w->inetNetName);
        else wcscpy(w->inetMsg, L"\u041d\u0435 \u0443\u0434\u0430\u043b\u043e\u0441\u044c \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0438\u0442\u044c\u0441\u044f");
    } else {
        wcscpy(w->inetMsg, L"\u041d\u0435 \u0443\u0434\u0430\u043b\u043e\u0441\u044c \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0438\u0442\u044c\u0441\u044f");
    }
    w->inetBusy = false;
    InvalidateRect(g_hwnd, NULL, FALSE);
    return 0;
}

static void DrawInetApp(HDC hdc, OsWindow& w) {
    int ex = w.x + 4, ey = w.y + TITLEBAR_H + 4;
    int ew = w.w - 8, eh = w.h - TITLEBAR_H - 8;
    HBRUSH bg = CreateSolidBrush(RGB(238,240,246));
    RECT br = {ex, ey, ex+ew, ey+eh};
    FillRect(hdc, &br, bg); DeleteObject(bg);
    HBRUSH hdr = CreateSolidBrush(RGB(0,110,140));
    RECT hr = {ex, ey, ex+ew, ey+40};
    FillRect(hdc, &hr, hdr); DeleteObject(hdr);
    HFONT hf = MakeFont(15, TRUE);
    HFONT old = (HFONT)SelectObject(hdc, hf);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255,255,255));
    RECT tr = {ex+12, ey+6, ex+ew-72, ey+34};
    DrawTextW(hdc, L"\u0418\u043d\u0442\u0435\u0440\u043d\u0435\u0442 \u041a\u0430\u0448\u043c\u0430\u041e\u0421", -1, &tr, DT_LEFT | DT_TOP);
    SelectObject(hdc, old); DeleteObject(hf);
    HFONT hf3 = MakeFont(11, FALSE);
    old = (HFONT)SelectObject(hdc, hf3);
    SetTextColor(hdc, RGB(220,240,255));
    RECT tr3 = {ex+ew-72, ey+10, ex+ew-12, ey+30};
    DrawTextW(hdc, L"Windows", 7, &tr3, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old); DeleteObject(hf3);

    HFONT s1 = MakeFont(14, TRUE);
    old = (HFONT)SelectObject(hdc, s1);
    SetTextColor(hdc, w.inetConnected ? RGB(0,130,0) : RGB(190,40,40));
    RECT st = {ex+12, ey+46, ex+ew-12, ey+72};
    DrawTextW(hdc, w.inetStatus[0] ? w.inetStatus : L"...", -1, &st, DT_LEFT | DT_TOP);
    SelectObject(hdc, old); DeleteObject(s1);

    HFONT s2 = MakeFont(12, FALSE);
    old = (HFONT)SelectObject(hdc, s2);
    SetTextColor(hdc, RGB(40,40,40));
    RECT ipr = {ex+12, ey+76, ex+ew-12, ey+96};
    DrawTextW(hdc, w.inetIp[0] ? w.inetIp : L"IP: -", -1, &ipr, DT_LEFT | DT_TOP);
    SetTextColor(hdc, w.inetConnected ? RGB(0,110,60) : RGB(120,120,120));
    RECT wr = {ex+12, ey+96, ex+ew-12, ey+116};
    DrawTextW(hdc, w.inetNetName[0] ? w.inetNetName : L"Wi-Fi: -", -1, &wr, DT_LEFT | DT_TOP);
    if (w.inetMsg[0]) {
        SetTextColor(hdc, RGB(170,60,0));
        RECT mrx = {ex+12, ey+120, ex+ew-12, ey+140};
        DrawTextW(hdc, w.inetMsg, -1, &mrx, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS);
    }
    SelectObject(hdc, old); DeleteObject(s2);

    if (!w.inetPassOpen) {
        HFONT nl = MakeFont(12, TRUE);
        old = (HFONT)SelectObject(hdc, nl);
        SetTextColor(hdc, RGB(20,20,20));
        RECT lr = {ex+12, ey+146, ex+ew-12, ey+166};
        DrawTextW(hdc, L"Wi-Fi \u0441\u0435\u0442\u0438:", -1, &lr, DT_LEFT | DT_TOP);
        SelectObject(hdc, old); DeleteObject(nl);
        int ry0 = ey + 168;
        for (int i = 0; i < w.inetCount && i < 7; i++) {
            int y0 = ry0 + i * 26;
            if (w.inetConn[i]) {
                HBRUSH gb = CreateSolidBrush(RGB(210,240,210));
                RECT gr = {ex+2, y0, ex+ew-2, y0+26};
                FillRect(hdc, &gr, gb); DeleteObject(gb);
            } else if (i == w.inetSel) {
                HBRUSH sb = CreateSolidBrush(RGB(212,220,245));
                RECT sr = {ex+2, y0, ex+ew-2, y0+26};
                FillRect(hdc, &sr, sb); DeleteObject(sb);
            }
            int bars = (w.inetSig[i] + 20) / 30;
            if (bars < 1) bars = 1;
            if (bars > 4) bars = 4;
            int bx = ex + 12;
            for (int b = 0; b < 4; b++) {
                HBRUSH gb2 = CreateSolidBrush(b <= bars-1 ? RGB(0,130,0) : RGB(200,200,200));
                RECT gra = {bx + b*8, y0 + 22 - b*4, bx + b*8 + 6, y0 + 26};
                FillRect(hdc, &gra, gb2); DeleteObject(gb2);
            }
            HFONT fr = MakeFont(12, FALSE);
            old = (HFONT)SelectObject(hdc, fr);
            SetTextColor(hdc, RGB(20,20,20));
            RECT nr2 = {ex+46, y0, ex+ew-150, y0+26};
            DrawTextW(hdc, w.inetSsid[i][0] ? w.inetSsid[i] : L"(\u0441\u043a\u0440\u044b\u0442\u0430\u044f \u0441\u0435\u0442\u044c)", -1, &nr2, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            const wchar_t* tag = w.inetConn[i] ? L"(\u043f\u043e\u0434\u043a\u043b.)" : (w.inetSec[i] ? (w.inetKnown[i] ? L"(\u0437\u0430\u043a\u0440., \u0441\u043e\u0445\u0440.)" : L"(\u0437\u0430\u043a\u0440.)") : L"(\u043e\u0442\u043a\u0440.)");
            SetTextColor(hdc, w.inetConn[i] ? RGB(0,140,0) : (w.inetSec[i] ? RGB(150,120,0) : RGB(120,120,120)));
            RECT tr2 = {ex+ew-140, y0, ex+ew-12, y0+26};
            DrawTextW(hdc, tag, -1, &tr2, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, old); DeleteObject(fr);
        }
        if (w.inetCount == 0) {
            HFONT em = MakeFont(12, FALSE);
            old = (HFONT)SelectObject(hdc, em);
            SetTextColor(hdc, RGB(130,130,130));
            RECT er2 = {ex+12, ry0 + 10, ex+ew-12, ry0 + 40};
            DrawTextW(hdc, w.inetBusy ? L"\u041f\u043e\u0438\u0441\u043a \u0441\u0435\u0442\u0435\u0439..." : L"Wi-Fi \u0441\u0435\u0442\u0438 \u043d\u0435 \u043d\u0430\u0439\u0434\u0435\u043d\u044b (\u0438\u043b\u0438 \u043d\u0435\u0442 \u0430\u0434\u0430\u043f\u0442\u0435\u0440\u0430)", -1, &er2, DT_LEFT | DT_TOP);
            SelectObject(hdc, old); DeleteObject(em);
        }
        HFONT hnb = MakeFont(11, FALSE);
        old = (HFONT)SelectObject(hdc, hnb);
        SetTextColor(hdc, RGB(90,90,90));
        RECT htr = {ex+12, ey+eh-58, ex+ew-12, ey+eh-36};
        DrawTextW(hdc, L"\u041f\u043e\u0441\u043b\u0435 \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u044f \u043e\u0442\u043a\u0440\u043e\u0439 \u0431\u0440\u0430\u0443\u0437\u0435\u0440 \u00ab\u0418\u043d\u0442\u0435\u0440\u043d\u0435\u0442\u00bb \u2014 \u0437\u0430\u0440\u0430\u0431\u043e\u0442\u0430\u0435\u0442 \u0447\u0430\u0442 \u0438 \u0437\u0432\u043e\u043d\u043a\u0438.", -1, &htr, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS);
        SelectObject(hdc, old); DeleteObject(hnb);
    } else {
        int px = ex + 10, py = ey + 142, pw = ew - 20, phh = 130;
        HBRUSH pb = CreateSolidBrush(RGB(255,255,246));
        RECT pr = {px, py, px+pw, py+phh};
        FillRect(hdc, &pr, pb); DeleteObject(pb);
        HPEN pen2 = CreatePen(PS_SOLID, 1, RGB(0,110,140));
        SelectObject(hdc, pen2); Rectangle(hdc, px, py, px+pw, py+phh); DeleteObject(pen2);
        HFONT pt = MakeFont(13, TRUE);
        old = (HFONT)SelectObject(hdc, pt);
        SetTextColor(hdc, RGB(20,20,20));
        RECT ptr = {px+14, py+12, px+pw-14, py+36};
        wchar_t pline[96];
        wsprintfW(pline, L"\u041f\u0430\u0440\u043e\u043b\u044c \u043e\u0442 \u0441\u0435\u0442\u0438 %s:", w.inetSsid[w.inetSel][0] ? w.inetSsid[w.inetSel] : L"?");
        DrawTextW(hdc, pline, -1, &ptr, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS);
        SelectObject(hdc, old); DeleteObject(pt);
        HBRUSH inb = CreateSolidBrush(RGB(255,255,255));
        RECT irr = {px+14, py+44, px+pw-14, py+68};
        FillRect(hdc, &irr, inb); DeleteObject(inb);
        HPEN ip3 = CreatePen(PS_SOLID, 1, RGB(120,120,120));
        SelectObject(hdc, ip3); Rectangle(hdc, px+14, py+44, px+pw-14, py+68); DeleteObject(ip3);
        wchar_t dots[66];
        for (int i = 0; i < w.inetPassLen && i < 64; i++) dots[i] = L'*';
        dots[w.inetPassLen > 64 ? 64 : w.inetPassLen] = 0;
        HFONT ps = MakeFont(13, FALSE);
        old = (HFONT)SelectObject(hdc, ps);
        SetTextColor(hdc, RGB(0,0,0));
        RECT pr2 = {px+20, py+44, px+pw-20, py+68};
        DrawTextW(hdc, dots, -1, &pr2, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        SetTextColor(hdc, RGB(120,120,120));
        RECT hintr = {px+14, py+76, px+pw-14, py+96};
        DrawTextW(hdc, L"\u0412\u0432\u0435\u0434\u0438 \u043f\u0430\u0440\u043e\u043b\u044c (Enter=\u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0438\u0442\u044c, Esc=\u043e\u0442\u043c\u0435\u043d\u0430)", -1, &hintr, DT_LEFT | DT_TOP);
        SelectObject(hdc, old); DeleteObject(ps);
        DrawButtonW(hdc, px+14, py+100, 110, 24, L"\u041f\u043e\u0434\u043a\u043b\u044e\u0447\u0438\u0442\u044c", false, false);
        DrawButtonW(hdc, px+132, py+100, 100, 24, L"\u041e\u0442\u043c\u0435\u043d\u0430", false, false);
    }
    DrawButtonW(hdc, ex+12, ey+eh-30, 150, 26, w.inetBusy ? L"\u041f\u043e\u0434\u043e\u0436\u0434\u0438\u0442\u0435..." : L"\u041f\u0435\u0440\u0435\u0441\u043a\u0430\u043d\u0438\u0440\u043e\u0432\u0430\u0442\u044c", false, false);
    if (w.inetBusy) {
        HFONT bb = MakeFont(12, FALSE);
        old = (HFONT)SelectObject(hdc, bb);
        SetTextColor(hdc, RGB(0,90,150));
        RECT mst = {ex+ew-220, ey+eh-30, ex+ew-12, ey+eh-4};
        DrawTextW(hdc, L"\u0418\u0434\u0451\u0442 \u043f\u043e\u0438\u0441\u043a / \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u0435...", -1, &mst, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, old); DeleteObject(bb);
    }
}

static void DrawUpdApp(HDC hdc, OsWindow& w) {
    int ex = w.x + 4, ey = w.y + TITLEBAR_H + 4;
    int ew = w.w - 8, eh = w.h - TITLEBAR_H - 8;
    HBRUSH bg = CreateSolidBrush(RGB(238,240,246));
    RECT br = {ex, ey, ex+ew, ey+eh};
    FillRect(hdc, &br, bg); DeleteObject(bg);
    HBRUSH hdr = CreateSolidBrush(RGB(30,70,150));
    RECT hr = {ex, ey, ex+ew, ey+40};
    FillRect(hdc, &hr, hdr); DeleteObject(hdr);
    HFONT hf = MakeFont(15, TRUE);
    HFONT old = (HFONT)SelectObject(hdc, hf);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255,255,255));
    RECT tr = {ex+12, ey+8, ex+ew-12, ey+32};
    DrawTextW(hdc, L"Update Center  -  auto rebuild", -1, &tr, DT_LEFT | DT_TOP);
    SelectObject(hdc, old); DeleteObject(hf);

    HFONT sf = MakeFont(12, FALSE);
    old = (HFONT)SelectObject(hdc, sf);
    int uy = ey + 50, uh = 24;
    int labX = ex + 12, fldX = ex + 130, fldW = ew - 160;
    SetTextColor(hdc, RGB(0,0,0));
    RECT lr = {labX, uy, fldX, uy+uh};
    DrawTextA(hdc, "Update URL:", -1, &lr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    HBRUSH wb = CreateSolidBrush(RGB(255,255,255));
    RECT fr = {fldX, uy, fldX+fldW, uy+uh};
    FillRect(hdc, &fr, wb); DeleteObject(wb);
    HPEN fp = CreatePen(PS_SOLID, 1, RGB(30,70,150));
    SelectObject(hdc, fp); SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, fr.left, fr.top, fr.right, fr.bottom);
    DeleteObject(fp);
    RECT vr = {fldX+4, uy, fldX+fldW, uy+uh};
    DrawTextA(hdc, w.updUrl, -1, &vr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    int by = uy + uh + 12, bW = 120, bH = 28;
    RECT cb = {ex+12, by, ex+12+bW, by+bH};
    HBRUSH sb = CreateSolidBrush(RGB(30,110,190));
    FillRect(hdc, &cb, sb); DeleteObject(sb);
    SetTextColor(hdc, RGB(255,255,255));
    DrawTextA(hdc, "Check", -1, &cb, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    RECT ub = {ex+12+bW+8, by, ex+12+bW+8+bW, by+bH};
    HBRUSH ub2 = CreateSolidBrush(g_updAvailable ? RGB(20,150,80) : RGB(120,120,120));
    FillRect(hdc, &ub, ub2); DeleteObject(ub2);
    SetTextColor(hdc, RGB(255,255,255));
    DrawTextA(hdc, "Download & Rebuild", -1, &ub, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);

    char verText[160];
    wsprintfA(verText, "Current: %s    Remote: %s", OS_VERSION, g_updRemoteVer[0] ? (const char*)g_updRemoteVer : "-");
    RECT vy = {ex+12, by+bH+8, ex+ew-12, by+bH+28};
    SetTextColor(hdc, RGB(20,20,20));
    DrawTextA(hdc, verText, -1, &vy, DT_LEFT | DT_TOP | DT_SINGLELINE);

    int lgy = by + bH + 32;
    RECT lbr = {ex+12, lgy, ex+ew-12, ey+eh-10};
    HBRUSH lw = CreateSolidBrush(RGB(255,255,255));
    FillRect(hdc, &lbr, lw); DeleteObject(lw);
    HPEN lp = CreatePen(PS_SOLID, 1, RGB(150,150,150));
    SelectObject(hdc, lp); SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, lbr.left, lbr.top, lbr.right, lbr.bottom);
    DeleteObject(lp);
    SelectObject(hdc, old); DeleteObject(sf);

    HFONT lf2 = MakeFont(11, FALSE);
    old = (HFONT)SelectObject(hdc, lf2);
    SetTextColor(hdc, RGB(40,40,40));
    RECT cl = {lbr.left+5, lbr.top+3, lbr.right-5, lbr.bottom-3};
    DrawTextA(hdc, w.updLog, -1, &cl, DT_LEFT | DT_TOP | DT_NOPREFIX);
    SelectObject(hdc, old); DeleteObject(lf2);
}

static void DrawWebApp(HDC hdc, OsWindow& w) {
    int ex = w.x + 4, ey = w.y + TITLEBAR_H + 4;
    int ew = w.w - 8, eh = w.h - TITLEBAR_H - 8;
    HBRUSH bg = CreateSolidBrush(RGB(238,240,246));
    RECT br = {ex, ey, ex+ew, ey+eh};
    FillRect(hdc, &br, bg); DeleteObject(bg);
    HBRUSH hdr = CreateSolidBrush(RGB(50,120,210));
    RECT hr = {ex, ey, ex+ew, ey+44};
    FillRect(hdc, &hr, hdr); DeleteObject(hdr);
    HFONT hf = MakeFont(16, TRUE);
    HFONT old = (HFONT)SelectObject(hdc, hf);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255,255,255));
    RECT tr = {ex+12, ey+8, ex+ew-12, ey+36};
    DrawTextW(hdc, L"Kashma Internet", -1, &tr, DT_LEFT | DT_TOP);
    SelectObject(hdc, old); DeleteObject(hf);

    HFONT sf = MakeFont(12, FALSE);
    old = (HFONT)SelectObject(hdc, sf);
    int uy = ey + 56, uh = 24;
    int fldX = ex + 12, fldW = ew - 24;
    SetTextColor(hdc, RGB(0,0,0));
    RECT t1 = {ex+12, uy-20, ex+ew-12, uy-2};
    DrawTextW(hdc, L"\u0421\u0442\u0430\u0440\u0442\u043e\u0432\u0430\u044f \u0441\u0442\u0440\u0430\u043d\u0438\u0446\u0430: \u0432\u044b\u0431\u0435\u0440\u0438 \u2014 \u041c\u0435\u0441\u0441\u0435\u043d\u0434\u0436\u0435\u0440, \u0417\u043c\u0435\u0439\u043a\u0430 \u0438\u043b\u0438 \u0417\u0432\u043e\u043d\u043e\u043a.", -1, &t1, DT_LEFT | DT_TOP | DT_SINGLELINE);
    HBRUSH wb = CreateSolidBrush(RGB(255,255,255));
    RECT fr = {fldX, uy, fldX+fldW, uy+uh};
    FillRect(hdc, &fr, wb); DeleteObject(wb);
    HPEN fp = CreatePen(PS_SOLID, 1, RGB(50,120,210));
    SelectObject(hdc, fp); SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, fr.left, fr.top, fr.right, fr.bottom);
    DeleteObject(fp);
    RECT vr = {fldX+4, uy, fldX+fldW, uy+uh};
    DrawTextA(hdc, MESSENGER_URL, -1, &vr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    SelectObject(hdc, old); DeleteObject(sf);

    HFONT lf = MakeFont(11, FALSE);
    old = (HFONT)SelectObject(hdc, lf);
    SetTextColor(hdc, RGB(60,60,60));
    RECT c2 = {ex+12, uy+uh+14, ex+ew-12, ey+eh-6};
    DrawTextW(hdc, L"\u0421\u0442\u0440\u0430\u043d\u0438\u0446\u044b \u043e\u0442\u043a\u0440\u044b\u0432\u0430\u044e\u0442\u0441\u044f \u0432\u043d\u0443\u0442\u0440\u0438 KashmaOS. \u041a\u043e\u043c\u043d\u0430\u0442\u0430 \u0447\u0430\u0442\u0430 = \u043e\u0431\u0449\u0438\u0439 \u043f\u0430\u0440\u043e\u043b\u044c, \u043f\u0443\u0441\u0442\u043e\u0439 \u043f\u0430\u0440\u043e\u043b\u044c = \u043f\u0443\u0431\u043b\u0438\u0447\u043d\u0430\u044f \u043a\u043e\u043c\u043d\u0430\u0442\u0430. \u0417\u043c\u0435\u0439\u043a\u0430 \u0438 \u0437\u0432\u043e\u043d\u043e\u043a \u2014 \u043f\u043e \u043a\u043e\u0434\u0443 \u043a\u043e\u043c\u043d\u0430\u0442\u044b.", -1, &c2, DT_CENTER | DT_TOP | DT_WORDBREAK);
    SelectObject(hdc, old); DeleteObject(lf);
}

static void DrawSettingsApp(HDC hdc, OsWindow& w) {
    int px = w.x + 8, py = w.y + TITLEBAR_H + 8;
    int sideW = SET_SBW, contentX = px + sideW + 12;

    HBRUSH sideBg = CreateSolidBrush(RGB(225, 225, 225));
    RECT sr = {w.x, py, w.x + sideW + 8, w.y + w.h};
    FillRect(hdc, &sr, sideBg); DeleteObject(sideBg);

    const wchar_t* pages[] = { L"\u0424\u043e\u043d", L"\u041c\u044b\u0448\u044c", L"\u0421\u0438\u0441\u0442\u0435\u043c\u0430", L"\u0417\u0432\u0443\u043a", L"\u041f\u0430\u0440\u043e\u043b\u044c", L"\u042f\u0437\u044b\u043a" };
    HFONT pf = MakeFont(13, TRUE);
    HFONT old = (HFONT)SelectObject(hdc, pf);
    SetBkMode(hdc, TRANSPARENT);
    for (int i = 0; i < 6; i++) {
        int iy = py + 8 + i * 34;
        if (i == g_settingsPage) {
            HBRUSH act = CreateSolidBrush(RGB(200, 80, 0));
            RECT ar = {w.x + 6, iy, w.x + sideW + 8, iy + 28};
            FillRect(hdc, &ar, act); DeleteObject(act);
            SetTextColor(hdc, RGB(255, 255, 255));
        } else {
            SetTextColor(hdc, RGB(40, 40, 40));
        }
        RECT itr = {w.x + 8, iy, w.x + sideW + 8, iy + 28};
        DrawTextW(hdc, pages[i], -1, &itr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
    SelectObject(hdc, old); DeleteObject(pf);

    if (g_settingsPage == 0) {
        SetBkMode(hdc, TRANSPARENT);
        HFONT hf = MakeFont(16, TRUE);
        old = (HFONT)SelectObject(hdc, hf);
        SetTextColor(hdc, RGB(200, 80, 0));
        RECT tr = {contentX, py, w.x + w.w - 8, py + 22};
        DrawTextW(hdc, L"\u041e\u0431\u043e\u0438 \u0438 \u0446\u0432\u0435\u0442", -1, &tr, DT_LEFT);
        SelectObject(hdc, old); DeleteObject(hf);

        HFONT sf = MakeFont(12, FALSE);
        old = (HFONT)SelectObject(hdc, sf);
        SetTextColor(hdc, RGB(60, 60, 60));
        int lrY = py + 28;
        for (int i = 0; i < 10; i++) {
            int bx = contentX + (i % 5) * 76;
            int by = lrY + (i / 5) * 40 + 20;
            int bw = 70, bh = 32;
            HBRUSH wbr = CreateSolidBrush(WALL_COLORS2[i][0]);
            RECT wr = {bx, by, bx + bw, by + bh};
            FillRect(hdc, &wr, wbr); DeleteObject(wbr);
            if (i == g_wallIdx) {
                HPEN wp = CreatePen(PS_SOLID, 3, RGB(255, 255, 0));
                SelectObject(hdc, wp); Rectangle(hdc, bx, by, bx + bw, by + bh); DeleteObject(wp);
            } else {
                HPEN wp = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
                SelectObject(hdc, wp); Rectangle(hdc, bx, by, bx + bw, by + bh); DeleteObject(wp);
            }
        }
        int stY = lrY + 90;
        SetTextColor(hdc, RGB(60, 60, 60));
        RECT str = {contentX, stY, w.x + w.w - 8, stY + 18};
        DrawTextW(hdc, L"\u0421\u0442\u0438\u043b\u044c:", -1, &str, DT_LEFT);
        DrawButtonW(hdc, contentX, stY + 22, 150, 26, L"\u0412\u0435\u0440\u0442\u0438\u043a\u0430\u043b\u044c\u043d\u044b\u0439", false, g_wallStyle == 0);
        DrawButtonW(hdc, contentX + 156, stY + 22, 150, 26, L"\u0413\u043e\u0440\u0438\u0437\u043e\u043d\u0442\u0430\u043b\u044c\u043d\u044b\u0439", false, g_wallStyle == 1);
        SelectObject(hdc, old); DeleteObject(sf);

        HFONT sf2 = MakeFont(12, FALSE);
        old = (HFONT)SelectObject(hdc, sf2);
        SetTextColor(hdc, RGB(60, 60, 60));
        RECT br2 = {contentX, stY + 62, w.x + w.w - 8, stY + 80};
        DrawTextW(hdc, S.brightness, -1, &br2, DT_LEFT);
        RECT barBg = {contentX, stY + 84, contentX + 200, stY + 100};
        HBRUSH bgBr = CreateSolidBrush(RGB(200, 200, 200));
        FillRect(hdc, &barBg, bgBr); DeleteObject(bgBr);
        RECT barFill = {contentX, stY + 84, contentX + g_brightness * 2, stY + 100};
        HBRUSH fillBr = CreateSolidBrush(RGB(200, 80, 0));
        FillRect(hdc, &barFill, fillBr); DeleteObject(fillBr);
        HPEN barP = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        SelectObject(hdc, barP); Rectangle(hdc, contentX, stY + 84, contentX + 200, stY + 100); DeleteObject(barP);
        char bstr[32]; sprintf(bstr, "%d%%", g_brightness);
        wchar_t wbstr[32]; MultiByteToWideChar(CP_ACP, 0, bstr, -1, wbstr, 32);
        HFONT bf = MakeFont(11, FALSE);
        old = (HFONT)SelectObject(hdc, bf);
        SetTextColor(hdc, RGB(0, 0, 0));
        RECT btr = {contentX + 210, stY + 82, contentX + 260, stY + 100};
        DrawTextW(hdc, wbstr, -1, &btr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, old); DeleteObject(bf);
        SelectObject(hdc, old); DeleteObject(sf2);
    }
    else if (g_settingsPage == 1) {
        SetBkMode(hdc, TRANSPARENT);
        HFONT hf = MakeFont(16, TRUE);
        old = (HFONT)SelectObject(hdc, hf);
        SetTextColor(hdc, RGB(200, 80, 0));
        RECT tr = {contentX, py, w.x + w.w - 8, py + 22};
        DrawTextW(hdc, L"\u041c\u044b\u0448\u044c \u0438 \u043a\u0443\u0440\u0441\u043e\u0440", -1, &tr, DT_LEFT);
        SelectObject(hdc, old); DeleteObject(hf);

        const wchar_t* cursors[] = { L"Arrow", L"Cross", L"Hand", L"Beam", L"Wait" };
        HFONT sf = MakeFont(13, TRUE);
        old = (HFONT)SelectObject(hdc, sf);
        SetTextColor(hdc, RGB(200, 80, 0));
        RECT sr2 = {contentX, py + 34, w.x + w.w - 8, py + 52};
        DrawTextW(hdc, L"\u0424\u043e\u0440\u043c\u0430 \u0443\u043a\u0430\u0437\u0430\u0442\u0435\u043b\u044f:", -1, &sr2, DT_LEFT);
        SelectObject(hdc, old); DeleteObject(sf);
        for (int i = 0; i < 5; i++) {
            int bx = contentX + i * 92;
            DrawButtonW(hdc, bx, py + 60, 86, 40, cursors[i], false, g_cursorIdx == i);
        }
        SetBkMode(hdc, TRANSPARENT);
        HFONT cs = MakeFont(11, FALSE);
        old = (HFONT)SelectObject(hdc, cs);
        SetTextColor(hdc, RGB(90, 90, 90));
        RECT cr = {contentX, py + 112, w.x + w.w - 8, py + 140};
        DrawTextW(hdc, L"\u0421\u043c\u0435\u043d\u0438\u0442\u044c \u0441\u0442\u0438\u043b\u044c \u043c\u044b\u0448\u0438 \u043d\u0430\u0436\u0430\u0442\u0438\u0435\u043c \u043d\u0430 \u043a\u043d\u043e\u043f\u043a\u0443.", -1, &cr, DT_LEFT | DT_TOP | DT_WORDBREAK);
        SelectObject(hdc, old); DeleteObject(cs);
    }
    else if (g_settingsPage == 2) {
        SetBkMode(hdc, TRANSPARENT);
        HFONT hf = MakeFont(16, TRUE);
        old = (HFONT)SelectObject(hdc, hf);
        SetTextColor(hdc, RGB(200, 80, 0));
        RECT tr = {contentX, py, w.x + w.w - 8, py + 22};
        DrawTextW(hdc, S.sys_info, -1, &tr, DT_LEFT);
        SelectObject(hdc, old); DeleteObject(hf);
        py += 30;
        HFONT sf = MakeFont(13, FALSE);
        old = (HFONT)SelectObject(hdc, sf);
        SetTextColor(hdc, RGB(0, 0, 0));
        const wchar_t* labels[] = { S.display, S.audio, S.keyboard, S.cpu, S.ram };
        const wchar_t* vals[] = { L"1024x768", L"SoundBlaster 16", L"QWERTY", L"x86 Protected", L"640K + A20" };
        for (int i = 0; i < 5; i++) {
            wchar_t line[128]; swprintf(line, 128, L"%s %s", labels[i], vals[i]);
            RECT ir = {contentX + 8, py + i * 20, w.x + w.w - 24, py + (i + 1) * 20};
            DrawTextW(hdc, line, -1, &ir, DT_LEFT);
        }
        wchar_t secval[192];
        if (g_foundTamper) swprintf(secval, 192, L"%s: %s!", S.sec, S.sec_trig);
        else swprintf(secval, 192, L"%s: %s", S.sec, S.on);
        SetTextColor(hdc, g_foundTamper ? RGB(200, 0, 0) : RGB(0, 120, 0));
        RECT sr3 = {contentX + 8, py + 5 * 20, w.x + w.w - 24, py + (6) * 20 + 18};
        DrawTextW(hdc, secval, -1, &sr3, DT_LEFT | DT_WORDBREAK);
        SetTextColor(hdc, RGB(80, 80, 80));
        HFONT sf3 = MakeFont(11, FALSE);
        old = (HFONT)SelectObject(hdc, sf3);
        RECT sr4 = {contentX + 8, py + 6 * 20 + 8, w.x + w.w - 24, py + (7) * 20 + 8};
        DrawTextW(hdc, L"ASM XOR | C FNV+CRC | Ada Sig | Pascal Struct | Fortran Sum | Rust Core | Honeypot | Quarantine | Kvorum-RAM | ROM | 200 LAYERS", -1, &sr4, DT_LEFT | DT_WORDBREAK);
        SetTextColor(hdc, RGB(0, 110, 0));
        wchar_t layerLine[192];
        wsprintfW(layerLine, L"Defense matrix: %d/200 layers armed [%S]", (int)g_layersArmed, (const char*)g_lastLayerInfo);
        RECT sr4b = {contentX + 8, py + (7) * 20 + 8, w.x + w.w - 24, py + (8) * 20 + 8};
        DrawTextW(hdc, layerLine, -1, &sr4b, DT_LEFT | DT_WORDBREAK);
        SetTextColor(hdc, RGB(80, 80, 80));
        wchar_t patrolLine[128];
        swprintf(patrolLine, 128, L"%s %02d:%02d:%02d", S.last_patrol, g_lastCheckH, g_lastCheckM, g_lastCheckS);
        RECT sr5 = {contentX + 8, py + (8) * 20 + 8, w.x + w.w - 24, py + (9) * 20 + 8};
        DrawTextW(hdc, patrolLine, -1, &sr5, DT_LEFT);
        SelectObject(hdc, old); DeleteObject(sf3);
        SetTextColor(hdc, RGB(0, 0, 0));
        SelectObject(hdc, old); DeleteObject(sf);
    }
    else if (g_settingsPage == 3) {
        SetBkMode(hdc, TRANSPARENT);
        HFONT hf = MakeFont(16, TRUE);
        old = (HFONT)SelectObject(hdc, hf);
        SetTextColor(hdc, RGB(200, 80, 0));
        RECT tr = {contentX, py, w.x + w.w - 8, py + 22};
        DrawTextW(hdc, L"\u0417\u0432\u0443\u043a", -1, &tr, DT_LEFT);
        SelectObject(hdc, old); DeleteObject(hf);
        HFONT pf = MakeFont(13, TRUE);
        old = (HFONT)SelectObject(hdc, pf);
        SetTextColor(hdc, RGB(200, 80, 0));
        RECT sndr = {contentX, py + 34, w.x + w.w - 8, py + 52};
        DrawTextW(hdc, S.sound, -1, &sndr, DT_LEFT);
        SelectObject(hdc, old); DeleteObject(pf);
        DrawButtonW(hdc, contentX, py + 58, 96, 30, S.on, !g_soundOn, g_soundOn);
        DrawButtonW(hdc, contentX + 104, py + 58, 96, 30, S.off, g_soundOn, !g_soundOn);
    }
    else if (g_settingsPage == 4) {
        SetBkMode(hdc, TRANSPARENT);
        HFONT hf = MakeFont(16, TRUE);
        old = (HFONT)SelectObject(hdc, hf);
        SetTextColor(hdc, RGB(200, 80, 0));
        RECT tr = {contentX, py, w.x + w.w - 8, py + 22};
        DrawTextW(hdc, S.pass_label, -1, &tr, DT_LEFT);
        SelectObject(hdc, old); DeleteObject(hf);
        int by = py + 44;
        HBRUSH pbg = CreateSolidBrush(RGB(255, 255, 255));
        RECT pbox = {contentX, by, contentX + 170, by + 26};
        FillRect(hdc, &pbox, pbg); DeleteObject(pbg);
        HPEN ppn = CreatePen(PS_SOLID, w.passFocus ? 2 : 1, w.passFocus ? RGB(200, 80, 0) : RGB(150, 150, 150));
        SelectObject(hdc, ppn); Rectangle(hdc, contentX, by, contentX + 170, by + 26); DeleteObject(ppn);
        HFONT pif = MakeFont(13, FALSE);
        old = (HFONT)SelectObject(hdc, pif);
        SetTextColor(hdc, RGB(0, 0, 0));
        wchar_t pMask[65];
        if (w.passLen > 0) {
            int n = w.passLen > 63 ? 63 : w.passLen;
            for (int i = 0; i < n; i++) pMask[i] = L'\u25cf';
            pMask[n] = 0;
            RECT pitr = {contentX + 6, by, contentX + 164, by + 26};
            DrawTextW(hdc, pMask, -1, &pitr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
        if (w.passFocus && (g_bootFrame / 3) % 2 == 0) {
            SIZE sz; GetTextExtentPoint32W(hdc, L"\u25cf", 1, &sz);
            MoveToEx(hdc, contentX + 9, by + 4, NULL);
            LineTo(hdc, contentX + 9, by + 22);
        }
        SelectObject(hdc, old); DeleteObject(pif);
        int sbx = contentX + 176 + (g_passSet ? 36 : 0);
        if (g_passSet) DrawButtonW(hdc, sbx, by, 70, 26, S.pass_btn_clear, false, false);
        else DrawButtonW(hdc, sbx, by, 70, 26, S.pass_btn_set, w.passLen == 0, false);
    }
    else if (g_settingsPage == 5) {
        SetBkMode(hdc, TRANSPARENT);
        HFONT hf = MakeFont(16, TRUE);
        old = (HFONT)SelectObject(hdc, hf);
        SetTextColor(hdc, RGB(200, 80, 0));
        RECT tr = {contentX, py, w.x + w.w - 8, py + 22};
        DrawTextW(hdc, S.lang_label, -1, &tr, DT_LEFT);
        SelectObject(hdc, old); DeleteObject(hf);
        const wchar_t* langNames[] = { L"English", L"\u0420\u0443\u0441\u0441\u043a\u0438\u0439", L"Fran\u00e7ais", L"Deutsch" };
        const Lang langIds[] = { LANG_EN, LANG_RU, LANG_FR, LANG_DE };
        for (int i = 0; i < 4; i++)
            DrawButtonW(hdc, contentX, py + 44 + i * 40, 140, 32, langNames[i], false, g_lang == langIds[i]);
    }
}

static void DrawSettingsNow(HDC hdc, OsWindow& w) {
    DrawSettingsApp(hdc, w);
}

static void DrawAboutApp(HDC hdc, OsWindow& w) {
    int py = w.y + TITLEBAR_H + 20;
    HFONT hf = MakeFont(20, TRUE);
    HFONT old = (HFONT)SelectObject(hdc, hf);
    SetTextColor(hdc, RGB(200,80,0)); SetBkMode(hdc, TRANSPARENT);
    RECT lr = {w.x, py, w.x+w.w, py+30};
    DrawTextA(hdc, "KashmaOS 7", -1, &lr, DT_CENTER);
    SelectObject(hdc, old); DeleteObject(hf);
    HFONT sf = MakeFont(12, FALSE);
    old = (HFONT)SelectObject(hdc, sf);
    SetTextColor(hdc, RGB(0,0,0));
    const wchar_t* info[] = { S.about_os, L"", S.about_feat, S.feat_windows, S.feat_calc, S.feat_notepad, S.feat_icons, S.feat_start, S.feat_taskbar };
    for (int i = 0; i < 9; i++) {
        RECT ir = {w.x+20, py+40+i*18, w.x+w.w-20, py+40+(i+1)*18};
        if (i >= 3) { wchar_t line[128]; swprintf(line, 128, L"- %s", info[i]); DrawTextW(hdc, line, -1, &ir, DT_LEFT); }
        else DrawTextW(hdc, info[i], -1, &ir, DT_LEFT);
    }
    SelectObject(hdc, old); DeleteObject(sf);
}

static void DrawRedSteamApp(HDC hdc, OsWindow& w) {
    HBRUSH rbr = CreateSolidBrush(RGB(160,0,0));
    RECT rb = {w.x, w.y+TITLEBAR_H, w.x+w.w, w.y+w.h};
    FillRect(hdc, &rb, rbr); DeleteObject(rbr);
    HFONT hf = MakeFont(24, TRUE);
    HFONT old = (HFONT)SelectObject(hdc, hf);
    SetTextColor(hdc, RGB(255,255,255)); SetBkMode(hdc, TRANSPARENT);
    RECT lr = {w.x, w.y+TITLEBAR_H+20, w.x+w.w, w.y+TITLEBAR_H+60};
    DrawTextA(hdc, "RED STEAM", -1, &lr, DT_CENTER);
    SelectObject(hdc, old); DeleteObject(hf);
    HFONT sf = MakeFont(13, FALSE);
    old = (HFONT)SelectObject(hdc, sf);
    SetTextColor(hdc, RGB(255,255,255));
    RECT sr = {w.x, w.y+TITLEBAR_H+70, w.x+w.w, w.y+TITLEBAR_H+90};
    DrawTextW(hdc, S.gaming_platform, -1, &sr, DT_CENTER);
    int bx = w.x + w.w/2 - 50, by = w.y + TITLEBAR_H + 110;
    DrawButtonW(hdc, bx, by, 100, 30, S.minebild, false, true);
    SelectObject(hdc, old); DeleteObject(sf);
}

static void ScanDir(OsWindow& w) {
    w.filesCount = 0;
    char pattern[520];
    wsprintfA(pattern, "%s\\*", w.filesPath);
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0) continue;
        if (w.filesCount >= 256) break;
        strcpy(w.filesList[w.filesCount], fd.cFileName);
        w.filesIsDir[w.filesCount] = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        if (w.filesIsDir[w.filesCount]) {
            wcscpy(w.filesSize[w.filesCount], S.folder);
        } else {
            ULONGLONG sz = ((ULONGLONG)fd.nFileSizeHigh << 32) | fd.nFileSizeLow;
            if (sz >= 1048576) swprintf(w.filesSize[w.filesCount], 24, L"%.1f \u041c\u0411", sz/1048576.0);
            else if (sz >= 1024) swprintf(w.filesSize[w.filesCount], 24, L"%.0f \u041a\u0411", sz/1024.0);
            else swprintf(w.filesSize[w.filesCount], 24, L"%I64u \u0411", sz);
        }
        w.filesCount++;
    } while (FindNextFileA(h, &fd));
    FindClose(h);
}

static void SearchRecurse(const char* dir, OsWindow& w, const char* q) {
    char pattern[560];
    wsprintfA(pattern, "%s\\*", dir);
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (strcmp(fd.cFileName,".")==0||strcmp(fd.cFileName,"..")==0) continue;
        if (w.sCount >= 256) break;
        char full[600];
        wsprintfA(full, "%s\\%s", dir, fd.cFileName);
        bool isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        char ln[300];
        int li = 0;
        for (int i = 0; fd.cFileName[i] && i < 299; i++) ln[li++] = (char)tolower((unsigned char)fd.cFileName[i]);
        ln[li] = 0;
        bool nameHit = strstr(ln, q) != 0;
        bool contentHit = false;
        if (!isDir && fd.nFileSizeLow < 16384) {
            HANDLE hf = CreateFileA(full, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
            if (hf != INVALID_HANDLE_VALUE) {
                char buf[16385]; DWORD rd = 0;
                ReadFile(hf, buf, 16384, &rd, NULL); CloseHandle(hf);
                for (DWORD k = 0; k < rd; k++) buf[k] = (char)tolower((unsigned char)buf[k]);
                buf[rd] = 0;
                if (strstr(buf, q)) contentHit = true;
            }
        }
        if (nameHit || contentHit) {
            strcpy(w.sPaths[w.sCount], full);
            w.sIsDir[w.sCount] = isDir;
            w.sCount++;
        }
        if (isDir) SearchRecurse(full, w, q);
    } while (FindNextFileA(h, &fd));
    FindClose(h);
}

static void DoSearch(OsWindow& w) {
    w.sCount = 0; w.sScroll = 0;
    char ql[64];
    WideCharToMultiByte(CP_ACP, 0, w.searchQ, -1, ql, 64, NULL, NULL);
    for (int i = 0; ql[i] && i < 63; i++) ql[i] = (char)tolower((unsigned char)ql[i]);
    if (!ql[0]) { w.filesSearch = false; return; }
    SearchRecurse(G_DRIVE_ROOT, w, ql);
}

static void OpenSearchResult(OsWindow& w, int idx) {
    if (idx < 0 || idx >= w.sCount) return;
    if (w.sIsDir[idx]) {
        if (strlen(w.sPaths[idx]) < 250) {
            strcpy(w.filesPath, w.sPaths[idx]);
            w.filesSearch = false;
            w.filesScroll = 0; w.filesSel = -1;
            w.sCount = 0;
        }
    } else {
        char dfp[520]; strcpy(dfp, w.sPaths[idx]);
        if (strend(dfp, ".drau")) OpenDrau(dfp);
        else OpenNotepad(dfp);
    }
}

static void OpenError(const wchar_t* title, const wchar_t* msg) {
    g_errorOpen = true;
    wcscpy(g_errorTitle, title);
    wcscpy(g_errorMsg, msg);
}

static void OpenSecurityAlert(const wchar_t* file, const wchar_t* layer, int severity) {
    g_alertOpen = true;
    g_alertTime = GetTickCount();
    g_alertSeverity = severity;
    wcscpy(g_alertFile, file);
    wcscpy(g_alertLayer, layer);
    InvalidateRect(g_hwnd, NULL, FALSE);
}

static unsigned long Fnv1aFile(const char* path) {
    HANDLE hf = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hf == INVALID_HANDLE_VALUE) return 0xFFFFFFFFUL;
    unsigned long hash = 2166136261UL;
    BYTE buf[4096]; DWORD rd;
    while (ReadFile(hf, buf, sizeof(buf), &rd, NULL) && rd > 0)
        for (DWORD i = 0; i < rd; i++) { hash ^= buf[i]; hash *= 16777619UL; }
    CloseHandle(hf);
    return hash;
}

static unsigned long Fnv1aStr(const char* s) {
    unsigned long hash = 2166136261UL;
    while (*s) { hash ^= (unsigned char)*s++; hash *= 16777619UL; }
    return hash;
}

struct RamFile { char name[64]; long len; unsigned char data[65536]; };
static RamFile g_ramSys[32];
static int g_ramSysCount = 0;

static void HashFileBoth(const char* path, unsigned long* fnv, unsigned long* djb) {
    HANDLE hf = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hf == INVALID_HANDLE_VALUE) { *fnv = 0xFFFFFFFFUL; *djb = 0xFFFFFFFFUL; return; }
    unsigned long f = 2166136261UL, j = 5381UL;
    BYTE buf[4096]; DWORD rd;
    while (ReadFile(hf, buf, sizeof(buf), &rd, NULL) && rd > 0)
        for (DWORD i = 0; i < rd; i++) {
            f ^= buf[i]; f *= 16777619UL;
            j = ((j << 5) + j) + buf[i];
        }
    CloseHandle(hf);
    *fnv = f; *djb = j;
}

static void HashFileAll(const char* path, unsigned long* fnv, unsigned long* djb, unsigned long* xr, unsigned long* fsum) {
    HANDLE hf = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hf == INVALID_HANDLE_VALUE) { *fnv=*djb=*xr=*fsum=0xFFFFFFFFUL; return; }
    unsigned long f=2166136261UL, j=5381UL, x=0, s=0;
    BYTE buf[4096]; DWORD rd;
    while (ReadFile(hf, buf, sizeof(buf), &rd, NULL) && rd > 0)
        for (DWORD i = 0; i < rd; i++) {
            f ^= buf[i]; f *= 16777619UL;
            j = ((j << 5) + j) + buf[i];
            x ^= (unsigned long)buf[i];
            s += (unsigned long)buf[i];
        }
    CloseHandle(hf);
    *fnv=f; *djb=j; *xr=x; *fsum=s;
}

static bool ValidateIntegrity(const char* name, const unsigned char* data, long len) {
    if (len == 0) return false;
    if (_stricmp(name, "kernel.c") == 0) return len > 2 && data[0]=='/' && data[1]=='/';
    if (_stricmp(name, "boot.asm") == 0) return len > 0 && data[0]==';';
    if (_stricmp(name, "start.asm") == 0) return len > 0 && data[0]==';';
    if (_stricmp(name, "version.txt") == 0) return len > 3 && data[0]=='K';
    if (_stricmp(name, "config.ini") == 0) return len > 0 && data[0]=='[';
    if (_stricmp(name, "modules.txt") == 0) return len > 5 && (data[0]=='L' || data[0]=='-');
    if (_stricmp(name, "memory.txt") == 0) return len > 0 && data[0] != '\0';
    if (_stricmp(name, "shield_trap.bin") == 0) return true;
    return true;
}

static void CaptureRustRam() {
    const char** sysFiles = sysFiles7;
    g_ramSysCount = 0;
    for (int i = 0; i < SYS_FILE_COUNT; i++) {
        char sp[560];
        wsprintfA(sp, "%s\\System\\%s", G_DRIVE_ROOT, sysFiles[i]);
        HANDLE hf = CreateFileA(sp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hf == INVALID_HANDLE_VALUE) continue;
        DWORD rd = 0, tot = 0;
        while (tot < 65536 && ReadFile(hf, g_ramSys[g_ramSysCount].data + tot, 65536 - tot, &rd, NULL) && rd > 0) tot += rd;
        CloseHandle(hf);
        strcpy(g_ramSys[g_ramSysCount].name, sysFiles[i]);
        g_ramSys[g_ramSysCount].len = (long)tot;
        g_ramSysCount++;
    }
}

static bool RestoreFromRam(const char* name) {
    for (int i = 0; i < g_ramSysCount; i++)
        if (_stricmp(g_ramSys[i].name, name) == 0) {
            char sp[560];
            wsprintfA(sp, "%s\\System\\%s", G_DRIVE_ROOT, name);
            SetFileAttributesA(sp, FILE_ATTRIBUTE_NORMAL);
            HANDLE hf = CreateFileA(sp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
            if (hf == INVALID_HANDLE_VALUE) return false;
            DWORD wr;
            WriteFile(hf, g_ramSys[i].data, (DWORD)g_ramSys[i].len, &wr, NULL);
            CloseHandle(hf);
            SetFileAttributesA(sp, FILE_ATTRIBUTE_READONLY);
            return true;
        }
    return false;
}

static const char* SRGG_FILE[] = {"src_guard.dat", "src_guard.db1"};
static const unsigned char SRGG_XOR[] = {0x7D, 0x29};

static const char* PASS_FILE[] = {"pass.dat", "pass.db1"};
static const unsigned char PASS_XOR[] = {0x4C, 0x9E};
static const unsigned long PASS_SALT = 0x9E3779B9UL;

static void PassWrite(bool has, unsigned long h1, unsigned long h2) {
    char content[128];
    if (has) sprintf(content, "p %08lX %08lX\n", h1, h2);
    else sprintf(content, "p 0 0\n");
    for (int i = 0; i < 2; i++) {
        char pp[560];
        wsprintfA(pp, "%s\\System\\RustCore\\%s", G_DRIVE_ROOT, PASS_FILE[i]);
        FILE* f2 = fopen(pp, "wb");
        if (!f2) continue;
        size_t ln = strlen(content);
        for (size_t k = 0; k < ln; k++) {
            unsigned char c = (unsigned char)content[k] ^ PASS_XOR[i];
            fwrite(&c, 1, 1, f2);
        }
        fclose(f2);
    }
}

static bool PassRead(unsigned long* h1, unsigned long* h2) {
    char buf[256];
    for (int i = 0; i < 2; i++) {
        char pp[560];
        wsprintfA(pp, "%s\\System\\RustCore\\%s", G_DRIVE_ROOT, PASS_FILE[i]);
        if (GetFileAttributesA(pp) == INVALID_FILE_ATTRIBUTES) continue;
        FILE* f = fopen(pp, "rb");
        if (!f) continue;
        size_t ln = fread(buf, 1, sizeof(buf) - 1, f);
        fclose(f);
        for (size_t k = 0; k < ln; k++) buf[k] = (char)((unsigned char)buf[k] ^ PASS_XOR[i]);
        buf[ln] = 0;
        char tag[8];
        if (sscanf(buf, "%7s %lx %lx", tag, h1, h2) == 3 && _stricmp(tag, "p") == 0)
            return (*h1 != 0 || *h2 != 0);
    }
    *h1 = 0; *h2 = 0;
    return false;
}

static void PassSet(const wchar_t* pw) {
    unsigned long h1 = PASS_SALT, h2 = 5381UL;
    for (int i = 0; pw[i]; i++) {
        wchar_t c = pw[i];
        unsigned char lo = (unsigned char)c, hi = (unsigned char)(c >> 8);
        h1 ^= lo; h1 *= 16777619UL; h2 = ((h2 << 5) + h2) + lo;
        h1 ^= hi; h1 *= 16777619UL; h2 = ((h2 << 5) + h2) + hi;
    }
    g_passHash = h1; g_passHash2 = h2; g_passSet = (wcslen(pw) > 0);
    PassWrite(g_passSet, g_passHash, g_passHash2);
}

static bool PassCheck(const wchar_t* pw) {
    if (!g_passSet) return true;
    unsigned long h1 = PASS_SALT, h2 = 5381UL;
    for (int i = 0; pw[i]; i++) {
        wchar_t c = pw[i];
        unsigned char lo = (unsigned char)c, hi = (unsigned char)(c >> 8);
        h1 ^= lo; h1 *= 16777619UL; h2 = ((h2 << 5) + h2) + lo;
        h1 ^= hi; h1 *= 16777619UL; h2 = ((h2 << 5) + h2) + hi;
    }
    return (h1 == g_passHash && h2 == g_passHash2);
}

static void PassLoad() {
    g_passSet = PassRead(&g_passHash, &g_passHash2);
}

static void PassClear() {
    g_passHash = 0; g_passHash2 = 0; g_passSet = false;
    PassWrite(false, 0, 0);
}

static void SrcGuardWrite(const char* content) {
    for (int i = 0; i < 2; i++) {
        char pp[560];
        wsprintfA(pp, "%s\\System\\RustCore\\%s", G_DRIVE_ROOT, SRGG_FILE[i]);
        FILE* f2 = fopen(pp, "wb");
        if (!f2) continue;
        size_t ln = strlen(content);
        for (size_t k = 0; k < ln; k++) {
            unsigned char c = (unsigned char)content[k] ^ SRGG_XOR[i];
            fwrite(&c, 1, 1, f2);
        }
        fclose(f2);
    }
}

static char* SrcGuardRead() {
    char buf[1024];
    for (int i = 0; i < 2; i++) {
        char pp[560];
        wsprintfA(pp, "%s\\System\\RustCore\\%s", G_DRIVE_ROOT, SRGG_FILE[i]);
        if (GetFileAttributesA(pp) == INVALID_FILE_ATTRIBUTES) continue;
        FILE* f = fopen(pp, "rb");
        if (!f) continue;
        size_t ln = fread(buf, 1, sizeof(buf) - 1, f);
        fclose(f);
        for (size_t k = 0; k < ln; k++) buf[k] = (char)((unsigned char)buf[k] ^ SRGG_XOR[i]);
        buf[ln] = 0;
        return _strdup(buf);
    }
    return NULL;
}

static void SrcGuardSrcPath(char* out, size_t cap) {
    char tmp[520];
    strncpy(tmp, G_DRIVE_ROOT, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = 0;
    char* tail = strstr(tmp, "\\DriveG");
    if (tail) *tail = 0;
    if (tmp[strlen(tmp) - 1] == '\\') tmp[strlen(tmp) - 1] = 0;
    wsprintfA(out, "%s\\main.cpp", tmp);
}

static void SrcGuardSave() {
    char srcPath[MAX_PATH];
    SrcGuardSrcPath(srcPath, sizeof(srcPath));
    unsigned long sF = 0, sJ = 0, sX = 0, sS = 0, eF = 0, eJ = 0, eX = 0, eS = 0;
    if (GetFileAttributesA(srcPath) != INVALID_FILE_ATTRIBUTES)
        HashFileAll(srcPath, &sF, &sJ, &sX, &sS);
    else sF = sJ = sX = sS = 0;
    HashFileAll(g_exePath, &eF, &eJ, &eX, &eS);
    char content[512];
    sprintf(content, "src %08lX %08lX %08lX %08lX\nexe %08lX %08lX %08lX %08lX\n", sF, sJ, sX, sS, eF, eJ, eX, eS);
    SrcGuardWrite(content);
    if (sF != 0) {
        char bk[620];
        wsprintfA(bk, "%s\\System\\RustCore\\main.cpp.bak", G_DRIVE_ROOT);
        CopyFileA(srcPath, bk, TRUE);
    }
    char bk2[620];
    wsprintfA(bk2, "%s\\System\\RustCore\\kashmaos7.exe.bak", G_DRIVE_ROOT);
    CopyFileA(g_exePath, bk2, TRUE);
}

static void SrcGuardQuarantine(const char* srcPath, const char* what) {
    char qd[600];
    wsprintfA(qd, "%s\\System\\RustCore\\quarantine", G_DRIVE_ROOT);
    CreateDirectoryA(qd, NULL);
    SYSTEMTIME st; GetLocalTime(&st);
    char qf[700];
    wsprintfA(qf, "%s\\%s_%02d%02d%02d.bin", qd, what, st.wHour, st.wMinute, st.wSecond);
    CopyFileA(srcPath, qf, FALSE);
}

static void WriteAttackLog(const char* name, const char* layer, unsigned long oF, unsigned long nF, unsigned long oJ, unsigned long nJ);

static void SrcGuardPatrol() {
    if (g_updBusy) return;
    char* c = SrcGuardRead();
    if (!c) { SrcGuardSave(); return; }
    unsigned long sF = 0, sJ = 0, sX = 0, sS = 0, eF = 0, eJ = 0, eX = 0, eS = 0;
    char l1[64], l2[64];
    bool ok = false;
    if (sscanf(c, "%63s %lx %lx %lx %lx\n%63s %lx %lx %lx %lx",
        l1, &sF, &sJ, &sX, &sS, l2, &eF, &eJ, &eX, &eS) == 10)
        ok = true;
    free(c);
    if (!ok) { SrcGuardSave(); return; }

    char srcPath[MAX_PATH];
    SrcGuardSrcPath(srcPath, sizeof(srcPath));
    char bk[620];
    wsprintfA(bk, "%s\\System\\RustCore\\main.cpp.bak", G_DRIVE_ROOT);

    for (int which = 0; which < 2; which++) {
        char chkPath[MAX_PATH];
        unsigned long refF, refJ;
        if (which == 0) { wsprintfA(chkPath, "%s", srcPath); refF = sF; refJ = sJ; }
        else { wsprintfA(chkPath, "%s", g_exePath); refF = eF; refJ = eJ; }
        unsigned long cF, cJ, cX, cS;
        HashFileAll(chkPath, &cF, &cJ, &cX, &cS);
        if (refF == 0) continue;
        bool tampered = false;
        if (cF == 0xFFFFFFFFUL) tampered = (refF != 0xFFFFFFFFUL);
        else tampered = (cF != refF || cJ != refJ);
        if (!tampered) continue;
        SrcGuardQuarantine(chkPath, which == 0 ? "src_main" : "src_exe");
        SetFileAttributesA(chkPath, FILE_ATTRIBUTE_NORMAL);
        bool restored = CopyFileA(bk, chkPath, FALSE) != FALSE;
        if (!restored && which == 1) {
            char bk2[620];
            wsprintfA(bk2, "%s\\System\\RustCore\\kashmaos7.exe.bak", G_DRIVE_ROOT);
            restored = CopyFileA(bk2, chkPath, FALSE) != FALSE;
        }
        SetFileAttributesA(chkPath, FILE_ATTRIBUTE_NORMAL);
        static DWORD lastSrcLog = 0;
        DWORD now = GetTickCount();
        if (now - lastSrcLog > 30000) {
            lastSrcLog = now;
            WriteAttackLog(which == 0 ? "main.cpp" : "kashmaos7.exe",
                restored ? "SRC-GUARD" : "SRC-GUARD(locked)", refF, cF, refJ, cJ);
            OpenSecurityAlert(which == 0 ? L"main.cpp" : L"kashmaos7.exe", restored ? L"Source guard restore" : L"Source tamper (locked)", 2);
        }
        g_foundTamper = true;
        InvalidateRect(g_hwnd, NULL, FALSE);
    }
}

static void LockSysFiles() {
    const char** sysFiles = sysFiles7;
    for (int i = 0; i < SYS_FILE_COUNT; i++) {
        char sp[560];
        wsprintfA(sp, "%s\\System\\%s", G_DRIVE_ROOT, sysFiles[i]);
        SetFileAttributesA(sp, FILE_ATTRIBUTE_READONLY);
    }
}

static const char* MAN_KEYS[] = {"manifest.dat", "manifest.db1", "manifest.db2"};
static const unsigned char MAN_XOR[] = {0x00, 0x53, 0x71};

static void ManifestWriteAll(const char* content) {
    for (int i = 0; i < 3; i++) {
        char pp[560];
        wsprintfA(pp, "%s\\System\\RustCore\\%s", G_DRIVE_ROOT, MAN_KEYS[i]);
        FILE* f2 = fopen(pp, "wb");
        if (!f2) continue;
        size_t ln = strlen(content);
        for (size_t k = 0; k < ln; k++) {
            unsigned char c = (unsigned char)content[k];
            if (i > 0) c ^= MAN_XOR[i];
            fwrite(&c, 1, 1, f2);
        }
        fclose(f2);
    }
}

static char* ManifestRead() {
    char buf[8192];
    for (int i = 0; i < 3; i++) {
        char pp[560];
        wsprintfA(pp, "%s\\System\\RustCore\\%s", G_DRIVE_ROOT, MAN_KEYS[i]);
        if (GetFileAttributesA(pp) == INVALID_FILE_ATTRIBUTES) continue;
        FILE* f = fopen(pp, "rb");
        if (!f) continue;
        size_t ln = fread(buf, 1, sizeof(buf) - 1, f);
        fclose(f);
        if (i > 0) for (size_t k = 0; k < ln; k++) buf[k] = (char)((unsigned char)buf[k] ^ MAN_XOR[i]);
        buf[ln] = 0;
        return _strdup(buf);
    }
    return NULL;
}

static void QuarantineFile(const char* name) {
    char qd[600];
    wsprintfA(qd, "%s\\System\\RustCore\\quarantine", G_DRIVE_ROOT);
    CreateDirectoryA(qd, NULL);
    char sp[560], qf[620];
    wsprintfA(sp, "%s\\System\\%s", G_DRIVE_ROOT, name);
    SYSTEMTIME st; GetLocalTime(&st);
    wsprintfA(qf, "%s\\%s_%02d%02d%02d.bin", qd, name, st.wHour, st.wMinute, st.wSecond);
    CopyFileA(sp, qf, FALSE);
}

static void EnsureRustCore() {
    char dir[520];
    wsprintfA(dir, "%s\\System\\RustCore", G_DRIVE_ROOT);
    CreateDirectoryA(dir, NULL);
    char qd[600];
    wsprintfA(qd, "%s\\System\\RustCore\\quarantine", G_DRIVE_ROOT);
    CreateDirectoryA(qd, NULL);
    const char* honeys[] = {"shield_trap.bin", "shield_trap2.dat", "core_guard.dat"};
    const char* baits[] = {
        "RUST_CORE_HONEYPOT v1: trojan that touched this file just declared war on an OS with 4 cores and 2 hash ciphers.",
        "SECONDARY BAIT: lucky? Rust Core has 3 honeypots, 3 manifests and a quarantine lab.",
        "CORE GUARD: only the kernel ring writes here. It is not you."
    };
    for (int i = 0; i < 3; i++) {
        char hp[560];
        wsprintfA(hp, "%s\\System\\%s", G_DRIVE_ROOT, honeys[i]);
        if (GetFileAttributesA(hp) == INVALID_FILE_ATTRIBUTES)
            WriteFileA_(hp, baits[i]);
    }
    const char** sysFiles = sysFiles7;
    char content[8192];
    int pos = 0;
    for (int i = 0; i < SYS_FILE_COUNT; i++) {
        char sp[560], bk[620];
        wsprintfA(sp, "%s\\System\\%s", G_DRIVE_ROOT, sysFiles[i]);
        wsprintfA(bk, "%s\\System\\RustCore\\%s.bak", G_DRIVE_ROOT, sysFiles[i]);
        SetFileAttributesA(sp, FILE_ATTRIBUTE_NORMAL);
        CopyFileA(sp, bk, FALSE);
        unsigned long fh, jh, xh, sh;
        HashFileAll(sp, &fh, &jh, &xh, &sh);
        pos += sprintf(content + pos, "%s %08lX %08lX %08lX %08lX\n", sysFiles[i], fh, jh, xh, sh);
    }
    ManifestWriteAll(content);
    CaptureRustRam();
    LockSysFiles();
}

static void WriteAttackLog(const char* name, const char* layer, unsigned long oF, unsigned long nF, unsigned long oJ, unsigned long nJ) {
    SYSTEMTIME st; GetLocalTime(&st);
    char line[512];
    sprintf(line, "[%02d.%02d.%04d %02d:%02d:%02d] %s FILE %s TAMPERED (fnv %08lX->%08lX, djb %08lX->%08lX)\n",
        st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, layer, name, oF, nF, oJ, nJ);
    size_t ln = strlen(line);
    char lg[560];
    wsprintfA(lg, "%s\\System\\RustCore\\attacks.log", G_DRIVE_ROOT);
    FILE* lf = fopen(lg, "ab");
    if (lf) { fwrite(line, 1, ln, lf); fclose(lf); }
    char mx[560];
    wsprintfA(mx, "%s\\System\\RustCore\\attacks.dbx", G_DRIVE_ROOT);
    FILE* mf = fopen(mx, "ab");
    if (mf) {
        for (size_t k = 0; k < ln; k++) { char c = line[k] ^ 0x5A; fwrite(&c, 1, 1, mf); }
        fclose(mf);
    }
}

static void RebuildAllFromRam() {
    const char** sysFiles = sysFiles7;
    for (int i = 0; i < SYS_FILE_COUNT; i++) RestoreFromRam(sysFiles[i]);
    char man[560];
    wsprintfA(man, "%s\\System\\RustCore\\manifest.dat", G_DRIVE_ROOT);
    SetFileAttributesA(man, FILE_ATTRIBUTE_NORMAL);
}

static void RomInitPath() {
    GetModuleFileNameA(NULL, g_exePath, MAX_PATH);
    char tmp[MAX_PATH];
    strcpy(tmp, g_exePath);
    char* slash = strrchr(tmp, '\\');
    if (slash) slash[1] = 0;

    char nearDrive[MAX_PATH];
    wsprintfA(nearDrive, "%sDriveG", tmp);
    bool portable = GetFileAttributesA(nearDrive) != INVALID_FILE_ATTRIBUTES;

    G_DRIVE_ROOT[0] = 0;
    if (portable) {
        strcpy(G_DRIVE_ROOT, nearDrive);
        wsprintfA(g_romDir, "%sSystemROM", tmp);
    } else {
        char appData[MAX_PATH];
        if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appData) == S_OK) {
            char ad[MAX_PATH];
            wsprintfA(ad, "%s\\KashmaOS", appData);
            CreateDirectoryA(ad, NULL);
            wsprintfA(G_DRIVE_ROOT, "%s\\DriveG", ad);
            wsprintfA(g_romDir, "%s\\SystemROM", ad);
        }
    }
    if (G_DRIVE_ROOT[0] == 0) {
        strcpy(G_DRIVE_ROOT, G_DRIVE_DEV);
        char proj[MAX_PATH];
        strcpy(proj, G_DRIVE_DEV);
        char* tail = strstr(proj, "\\DriveG");
        if (tail) *tail = 0;
        wsprintfA(g_romDir, "%s\\SystemROM", proj);
    }
}

static bool RomWriteOne(const char* name, const unsigned char* data, DWORD len) {
    char p[MAX_PATH + 24];
    wsprintfA(p, "%s\\%s.gold", g_romDir, name);
    HANDLE hf = CreateFileA(p, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
    if (hf == INVALID_HANDLE_VALUE) return false;
    DWORD wr;
    WriteFile(hf, &len, 4, &wr, NULL);
    for (DWORD i = 0; i < len; i++) { char c = (char)(data[i] ^ 0xA5); WriteFile(hf, &c, 1, &wr, NULL); }
    CloseHandle(hf);
    return true;
}

static bool RomReadOne(const char* name, unsigned char* out, DWORD* outLen) {
    char p[MAX_PATH + 24];
    wsprintfA(p, "%s\\%s.gold", g_romDir, name);
    HANDLE hf = CreateFileA(p, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hf == INVALID_HANDLE_VALUE) return false;
    DWORD len = 0, rd = 0;
    ReadFile(hf, &len, 4, &rd, NULL);
    if (len > 65536) len = 0;
    DWORD i = 0;
    while (i < len && ReadFile(hf, out + i, len - i, &rd, NULL) && rd > 0) i += rd;
    CloseHandle(hf);
    for (DWORD k = 0; k < i; k++) out[k] ^= 0xA5;
    *outLen = i;
    return i == len && len > 0;
}

static bool RomHas() {
    return GetFileAttributesA(g_romDir) != INVALID_FILE_ATTRIBUTES;
}

static void RomSave() {
    if (strlen(g_romDir) == 0) RomInitPath();
    CreateDirectoryA(g_romDir, NULL);
    char* manC = ManifestRead();
    if (manC) {
        RomWriteOne("manifest", (const unsigned char*)manC, (DWORD)strlen(manC));
        free(manC);
    }
    for (int i = 0; i < SYS_FILE_COUNT; i++) {
        char sp[560];
        wsprintfA(sp, "%s\\System\\%s", G_DRIVE_ROOT, sysFiles7[i]);
        HANDLE hf = CreateFileA(sp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hf == INVALID_HANDLE_VALUE) continue;
        unsigned char data[65536];
        DWORD rd = 0, tot = 0;
        while (tot < 65536 && ReadFile(hf, data + tot, 65536 - tot, &rd, NULL) && rd > 0) tot += rd;
        CloseHandle(hf);
        RomWriteOne(sysFiles7[i], data, tot);
    }
}

static bool RomRestoreFile(const char* name) {
    unsigned char data[65536];
    DWORD len = 0;
    if (!RomReadOne(name, data, &len)) return false;
    char sp[560];
    wsprintfA(sp, "%s\\System\\%s", G_DRIVE_ROOT, name);
    SetFileAttributesA(sp, FILE_ATTRIBUTE_NORMAL);
    HANDLE hf = CreateFileA(sp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    bool ok = false;
    if (hf != INVALID_HANDLE_VALUE) {
        DWORD wr;
        WriteFile(hf, data, len, &wr, NULL);
        CloseHandle(hf);
        ok = true;
    }
    SetFileAttributesA(sp, FILE_ATTRIBUTE_READONLY);
    return ok;
}

static void RomReconcile() {
    if (!RomHas()) return;
    for (int i = 0; i < SYS_FILE_COUNT; i++) {
        char sp[560];
        wsprintfA(sp, "%s\\System\\%s", G_DRIVE_ROOT, sysFiles7[i]);
        unsigned char cur[65536];
        DWORD curLen = 0;
        HANDLE hf = CreateFileA(sp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hf == INVALID_HANDLE_VALUE) { RomRestoreFile(sysFiles7[i]); continue; }
        DWORD rd = 0, tot = 0;
        while (tot < 65536 && ReadFile(hf, cur + tot, 65536 - tot, &rd, NULL) && rd > 0) tot += rd;
        CloseHandle(hf);
        curLen = tot;
        unsigned char g[65536];
        DWORD gLen = 0;
        if (!RomReadOne(sysFiles7[i], g, &gLen)) continue;
        if (curLen != gLen || memcmp(cur, g, curLen) != 0) RomRestoreFile(sysFiles7[i]);
    }
    unsigned char m[65536];
    DWORD ml = 0;
    if (RomReadOne("manifest", m, &ml) && ml > 0 && ml < 8192) {
        char manBuf[8192];
        memcpy(manBuf, m, ml);
        manBuf[ml] = 0;
        ManifestWriteAll(manBuf);
    }
}

static bool MainIsUp() {
    HANDLE m = CreateMutexA(NULL, TRUE, "KashmaOS7_Main");
    bool exists = GetLastError() == ERROR_ALREADY_EXISTS;
    if (!exists) ReleaseMutex(m);
    CloseHandle(m);
    return exists;
}

static bool GuardIsUp() {
    HANDLE m = CreateMutexA(NULL, TRUE, "KashmaOS7_Guard");
    bool exists = GetLastError() == ERROR_ALREADY_EXISTS;
    if (!exists) ReleaseMutex(m);
    CloseHandle(m);
    return exists;
}

static void EnableShutdownPrivilege() {
    HANDLE tok;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tok)) return;
    LUID luid;
    if (LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &luid)) {
        TOKEN_PRIVILEGES tp;
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(tok, FALSE, &tp, 0, NULL, NULL);
    }
    CloseHandle(tok);
}

static void SetClosedFlag() {
    char flag[600];
    wsprintfA(flag, "%s\\System\\RustCore\\closed.flag", G_DRIVE_ROOT);
    FILE* f = fopen(flag, "wb");
    if (f) { fputs("closed", f); fclose(f); }
}

static void RunGuardMode() {
    g_guardMutex = CreateMutexA(NULL, TRUE, "KashmaOS7_Guard");
    Sleep(1500);
    for (;;) {
        Sleep(2000);
        if (RomHas()) {
            RomReconcile();
            char lf[600];
            wsprintfA(lf, "%s\\System\\RustCore\\attacks.log", G_DRIVE_ROOT);
            FILE* l = fopen(lf, "ab");
            if (l) {
                SYSTEMTIME st; GetLocalTime(&st);
                char ln[128];
                sprintf(ln, "[%02d.%02d.%04d %02d:%02d:%02d] GUARDIAN PATROL OK\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
                fwrite(ln, 1, strlen(ln), l);
                fclose(l);
            }
        }
        if (!MainIsUp()) {
            char flag[600];
            wsprintfA(flag, "%s\\System\\RustCore\\closed.flag", G_DRIVE_ROOT);
            if (GetFileAttributesA(flag) == INVALID_FILE_ATTRIBUTES)
                ShellExecuteA(NULL, "open", g_exePath, NULL, NULL, SW_SHOWNORMAL);
            else
                DeleteFileA(flag);
        }
    }
}

static void EnsureGuardian() {
    if (strlen(g_exePath) == 0) RomInitPath();
    if (GuardIsUp()) return;
    ShellExecuteA(NULL, "open", g_exePath, "--guard", NULL, SW_HIDE);
}

#define LAYER_COUNT 200

static unsigned long LayerDigest(const BYTE* d, DWORD len, int layer) {
    int fam = layer / 25;
    int sub = layer % 25;
    unsigned long h;
    switch (fam) {
        case 0:
            h = 2166136261UL + layer * 9973UL;
            for (DWORD i = 0; i < len; i++) { h ^= (BYTE)(d[i] + sub * 31 + (i % 7)); h *= 16777619UL; h ^= h >> 13; }
            break;
        case 1: {
            h = layer * 12345UL;
            for (DWORD i = 0; i < len; i++) {
                h ^= d[i];
                for (int b = 0; b <= sub % 8; b++) h = (h >> 1) ^ (0xEDB88320UL & -(h & 1));
                h += layer;
            }
            break;
        }
        case 2:
            h = layer * 0x9E3779B9UL;
            for (DWORD i = 0; i < len; i += 8) {
                unsigned long w = 0;
                for (int j = 0; j < 8 && i + j < len; j++) w ^= (unsigned long)d[i + j] << ((j * 4) % 24);
                h ^= w; h *= 0x85EBCA6BUL;
            }
            break;
        case 3:
            h = layer;
            for (DWORD i = 0; i < len; i++) h += (unsigned long)d[i] * (sub + 1) + (i % 17);
            h ^= h >> 16;
            break;
        case 4: {
            unsigned long a = 1 + sub, b = sub;
            for (DWORD i = 0; i < len; i++) { a = (a + d[i]) % 65521; b = (b + a) % 65521; }
            h = (b << 16) | a; h ^= layer * 0x5BD1E995UL;
            break;
        }
        case 5:
            h = 0xDEADBEEFUL ^ layer;
            for (DWORD i = 0; i < len; i += 4096) {
                unsigned long pg = 0;
                for (DWORD j = i; j < len && j < i + 4096; j++) pg = pg * 31 + d[j];
                h ^= pg; h = (h << 13) | (h >> 19);
            }
            break;
        case 6:
            h = 0xA5A5A5A5UL ^ (layer * 0x2545F491UL);
            for (DWORD i = 0; i < len; i++) { BYTE x = d[i]; h += (x >> 4) * (sub + 3) + (x & 0xF) * (sub + 1); h ^= h >> 11; }
            break;
        default:
            h = layer * 0x85EBCA6BUL;
            for (DWORD i = 0; i < len; i++) { h += d[i] * ((i % 13) + 1); h = (h * 1664525UL) + 1013904223UL; }
            break;
    }
    return h;
}

static const char* LayerFamilyName(int layer) {
    switch (layer / 25) {
        case 0: return "FNV-Ridge";
        case 1: return "CRC-Rotate";
        case 2: return "XOR-Fold";
        case 3: return "Sum-Weights";
        case 4: return "Rolling-Adler";
        case 5: return "RustCore-Margin";
        case 6: return "Honeypot-Nibble";
        default: return "Kvorum-RAM";
    }
}

static void LayersSaveAll() {
    char storeB[64 + 18 * (48 + LAYER_COUNT * 4)];
    long pos = 0;
    memcpy(storeB + pos, "K7LAYER200", 10); pos += 10;
    unsigned int sc = (unsigned int)SYS_FILE_COUNT; memcpy(storeB + pos, &sc, 4); pos += 4;
    for (int f = 0; f < SYS_FILE_COUNT; f++) {
        char sp[560];
        wsprintfA(sp, "%s\\System\\%s", G_DRIVE_ROOT, sysFiles7[f]);
        HANDLE hf = CreateFileA(sp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        DWORD sz = 0;
        BYTE* data = NULL;
        if (hf != INVALID_HANDLE_VALUE) {
            sz = GetFileSize(hf, NULL); if (sz > 65535) sz = 65535;
            data = (BYTE*)malloc(sz + 1);
            DWORD rd; if (data) { ReadFile(hf, data, sz, &rd, NULL); data[rd] = 0; }
            CloseHandle(hf);
        }
        memset(storeB + pos, 0, 48); strncpy(storeB + pos, sysFiles7[f], 47); pos += 48;
        if (data) {
            for (int l = 0; l < LAYER_COUNT; l++) {
                unsigned long v = LayerDigest(data, sz, l);
                memcpy(storeB + pos, &v, 4); pos += 4;
            }
            free(data);
        } else {
            memset(storeB + pos, 0, LAYER_COUNT * 4); pos += LAYER_COUNT * 4;
        }
    }
    char ld1[560]; wsprintfA(ld1, "%s\\System\\RustCore\\layers.dat", G_DRIVE_ROOT);
    HANDLE h1 = CreateFileA(ld1, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h1 != INVALID_HANDLE_VALUE) {
        DWORD wr; for (long i2 = 0; i2 < pos; i2++) { char c = storeB[i2] ^ LAYER_STORE_NAM; WriteFile(h1, &c, 1, &wr, NULL); }
        CloseHandle(h1);
    }
    char ld2[560]; wsprintfA(ld2, "%s\\System\\RustCore\\layers.db1", G_DRIVE_ROOT);
    HANDLE h2 = CreateFileA(ld2, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h2 != INVALID_HANDLE_VALUE) {
        DWORD wr; for (long i2 = 0; i2 < pos; i2++) { char c = storeB[i2] ^ LAYER_STORE_DB1; WriteFile(h2, &c, 1, &wr, NULL); }
        CloseHandle(h2);
    }
    RomWriteOne("layers", (const unsigned char*)storeB, (DWORD)pos);
}

static bool LayersVerifyAll(int& fileIdx, int& layerId) {
    char lp[560];
    wsprintfA(lp, "%s\\System\\RustCore\\layers.dat", G_DRIVE_ROOT);
    HANDLE hf = CreateFileA(lp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hf == INVALID_HANDLE_VALUE) { LayersSaveAll(); return true; }
    DWORD sz = GetFileSize(hf, NULL); if (sz > 65536) sz = 65536;
    BYTE* store = (BYTE*)malloc(sz);
    DWORD rd = 0; ReadFile(hf, store, sz, &rd, NULL); CloseHandle(hf);
    for (DWORD k = 0; k < rd; k++) store[k] = (BYTE)(store[k] ^ LAYER_STORE_NAM);
    bool ok = true;
    long pos = 10 + 4;
    for (int f = 0; f < SYS_FILE_COUNT && ok && pos + 48 + 4 * LAYER_COUNT <= (long)rd; f++) {
        char nm[48]; memset(nm, 0, 48); memcpy(nm, store + pos, 48); pos += 48;
        if (strcmp(nm, sysFiles7[f]) != 0) { ok = false; fileIdx = f; layerId = -1; break; }
        char sp[560];
        wsprintfA(sp, "%s\\System\\%s", G_DRIVE_ROOT, sysFiles7[f]);
        HANDLE hff = CreateFileA(sp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        DWORD sz2 = 0;
        BYTE* data = NULL;
        if (hff != INVALID_HANDLE_VALUE) {
            sz2 = GetFileSize(hff, NULL); if (sz2 > 65535) sz2 = 65535;
            data = (BYTE*)malloc(sz2 + 1);
            DWORD rd2; if (data) { ReadFile(hff, data, sz2, &rd2, NULL); data[rd2] = 0; }
            CloseHandle(hff);
        } else { ok = false; fileIdx = f; layerId = 0; free(store); if (data) free(data); return false; }
        for (int l = 0; l < LAYER_COUNT && ok; l++) {
            unsigned long v = 0;
            memcpy(&v, store + pos, 4); pos += 4;
            unsigned long cur = LayerDigest(data, sz2, l);
            if (cur != v) { ok = false; fileIdx = f; layerId = l; }
        }
        free(data);
    }
    free(store);
    return ok;
}

static void LayersPatrol() {
    int f = -1, l = -1;
    bool okk = LayersVerifyAll(f, l);
    if (okk) { g_layersArmed = LAYER_COUNT; sprintf(g_lastLayerInfo, "L%03d..%03d [%s] OK", 1, LAYER_COUNT, LayerFamilyName(199)); return; }
    g_layersArmed = (l > 0) ? (unsigned int)l : 0;
    if (f >= 0 && f < SYS_FILE_COUNT) {
        char nm[260];
        sprintf(nm, "%s", sysFiles7[f]);
        char sp[560];
        wsprintfA(sp, "%s\\System\\%s", G_DRIVE_ROOT, nm);
        QuarantineFile(nm);
        char bk[620];
        wsprintfA(bk, "%s\\System\\RustCore\\%s.bak", G_DRIVE_ROOT, nm);
        SetFileAttributesA(sp, FILE_ATTRIBUTE_NORMAL);
        bool rok = CopyFileA(bk, sp, FALSE) != FALSE;
        if (!rok) rok = RestoreFromRam(nm);
        SetFileAttributesA(sp, FILE_ATTRIBUTE_READONLY);
        WriteAttackLog(nm, "LAYER-BREACH", (unsigned long)(l < 0 ? 0 : l), 0, 0, 0);
        sprintf(g_lastLayerInfo, "L%03d [%s] breached", l >= 0 ? l + 1 : 0, LayerFamilyName(l >= 0 ? l : 0));
        wchar_t wl[160];
        swprintf(wl, 160, L"L%03d [%hs]", l >= 0 ? l + 1 : 0, LayerFamilyName(l >= 0 ? l : 0));
        OpenSecurityAlert(L"layer breach", wl, 2);
        g_foundTamper = true;
        InvalidateRect(g_hwnd, NULL, FALSE);
        LayersSaveAll();
    }
}

static void RunSecurityCheckInner() {
    char* manC = ManifestRead();
    if (!manC) {
        RebuildAllFromRam();
        EnsureRustCore();
        WriteAttackLog("(STORAGE)", "RAM-REBUILD", 0, 0, 0, 0);
        OpenSecurityAlert(L"RustCore manifest", L"RAM Emergency Restore", 2);
        g_foundTamper = true;
        InvalidateRect(g_hwnd, NULL, FALSE);
        SYSTEMTIME ck; GetLocalTime(&ck);
        g_lastCheckH = ck.wHour; g_lastCheckM = ck.wMinute; g_lastCheckS = ck.wSecond;
        return;
    }
    char reassert[8192];
    strncpy(reassert, manC, sizeof(reassert) - 1);
    reassert[sizeof(reassert) - 1] = 0;
    char name[260]; unsigned long crcF, crcJ, crcX, crcS;
    static DWORD lastLockLog = 0;
    bool first = true;
    char* line = manC;
    char* next = NULL;
    while (line && *line) {
        char* nl = strchr(line, '\n');
        if (nl) { *nl = 0; next = nl + 1; } else next = NULL;
        if (sscanf(line, " %259s %lx %lx %lx %lx", name, &crcF, &crcJ, &crcX, &crcS) == 5) {
            char sp[560];
            wsprintfA(sp, "%s\\System\\%s", G_DRIVE_ROOT, name);
            unsigned long curF, curJ, curX, curS;
            HashFileAll(sp, &curF, &curJ, &curX, &curS);
            if (curF == 0xFFFFFFFFUL) {
                QuarantineFile(name);
                char bk[620];
                wsprintfA(bk, "%s\\System\\RustCore\\%s.bak", G_DRIVE_ROOT, name);
                SetFileAttributesA(sp, FILE_ATTRIBUTE_NORMAL);
                bool ok = CopyFileA(bk, sp, FALSE) != FALSE;
                if (!ok) ok = RestoreFromRam(name);
                SetFileAttributesA(sp, FILE_ATTRIBUTE_READONLY);
                unsigned long vF, vJ, vX, vS;
                HashFileAll(sp, &vF, &vJ, &vX, &vS);
                if (vF != crcF || vJ != crcJ) {
                    SetFileAttributesA(sp, FILE_ATTRIBUTE_NORMAL);
                    RestoreFromRam(name);
                    SetFileAttributesA(sp, FILE_ATTRIBUTE_READONLY);
                    WriteAttackLog(name, "RAM-REVERT", crcF, vF, crcJ, vJ);
                }
                WriteAttackLog(name, "FILE-MISSING", crcF, curF, crcJ, curJ);
                if (first) { OpenSecurityAlert(L"(deleted file)", L"RAM Emergency Restore", 2); first = false; }
                g_foundTamper = true;
                InvalidateRect(g_hwnd, NULL, FALSE);
            } else {
                const char* caughtBy = NULL;
                if (curF != crcF || curJ != crcJ) caughtBy = "FNV+CRC32 dual-hash";
                else if (curX != crcX) caughtBy = "XOR Sentinel (ASM)";
                else if (curS != crcS) caughtBy = "ByteSum (Fortran)";
                if (caughtBy) {
                    QuarantineFile(name);
                    char bk[620];
                    wsprintfA(bk, "%s\\System\\RustCore\\%s.bak", G_DRIVE_ROOT, name);
                    SetFileAttributesA(sp, FILE_ATTRIBUTE_NORMAL);
                    bool ok = CopyFileA(bk, sp, FALSE) != FALSE;
                    if (!ok) ok = RestoreFromRam(name);
                    unsigned char* tmp = (unsigned char*)malloc(65536);
                    if (tmp) {
                        HANDLE hf2 = CreateFileA(sp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
                        DWORD rd2 = 0; long tl = 0;
                        if (hf2 != INVALID_HANDLE_VALUE) {
                            while (tl < 65535 && ReadFile(hf2, tmp + tl, 65535 - tl, &rd2, NULL) && rd2 > 0) tl += rd2;
                            CloseHandle(hf2);
                        }
                        if (!ValidateIntegrity(name, tmp, tl)) caughtBy = "Ada Signature + Structural";
                        free(tmp);
                    }
                    SetFileAttributesA(sp, FILE_ATTRIBUTE_READONLY);
                    unsigned long vF, vJ, vX, vS;
                    HashFileAll(sp, &vF, &vJ, &vX, &vS);
                    if (vF != crcF || vJ != crcJ) {
                        SetFileAttributesA(sp, FILE_ATTRIBUTE_NORMAL);
                        RestoreFromRam(name);
                        SetFileAttributesA(sp, FILE_ATTRIBUTE_READONLY);
                        WriteAttackLog(name, "RAM-REVERT", crcF, vF, crcJ, vJ);
                    }
                    WriteAttackLog(name, caughtBy, crcF, curF, crcJ, curJ);
                    wchar_t wname[260]; MultiByteToWideChar(CP_ACP, 0, name, -1, wname, 260);
                    wchar_t wlayer[256]; MultiByteToWideChar(CP_ACP, 0, caughtBy, -1, wlayer, 256);
                    if (first) { OpenSecurityAlert(wname, wlayer, 1); first = false; }
                    g_foundTamper = true;
                    InvalidateRect(g_hwnd, NULL, FALSE);
                }
            }
            DWORD attrs = GetFileAttributesA(sp);
            DWORD now = GetTickCount();
            if (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_READONLY)) {
                SetFileAttributesA(sp, FILE_ATTRIBUTE_READONLY);
                if (now - lastLockLog > 5000) {
                    lastLockLog = now;
                    WriteAttackLog(name, "LOCK-BREACH", 0, 0, 0, 0);
                    g_foundTamper = true;
                }
            }
        }
        line = next;
    }
    free(manC);
    ManifestWriteAll(reassert);
    char hp[560];
    wsprintfA(hp, "%s\\System\\shield_trap.bin", G_DRIVE_ROOT);
    if (GetFileAttributesA(hp) != INVALID_FILE_ATTRIBUTES) {
        unsigned long hf_, hj, hx, hs;
        HashFileAll(hp, &hf_, &hj, &hx, &hs);
        char hpBk[620];
        wsprintfA(hpBk, "%s\\System\\RustCore\\shield_trap.bin.bak", G_DRIVE_ROOT);
        unsigned long bf_, bj, bx, bs;
        HashFileAll(hpBk, &bf_, &bj, &bx, &bs);
        if (hf_ != bf_ || hj != bj) {
            QuarantineFile("shield_trap.bin");
            WriteAttackLog("shield_trap.bin", "HONEYPOT", bf_, hf_, bj, hj);
            OpenSecurityAlert(L"shield_trap.bin", L"Honeypot Triggered", 0);
            g_foundTamper = true;
            InvalidateRect(g_hwnd, NULL, FALSE);
            SetFileAttributesA(hp, FILE_ATTRIBUTE_NORMAL);
            CopyFileA(hpBk, hp, FALSE);
            SetFileAttributesA(hp, FILE_ATTRIBUTE_READONLY);
        }
    }
    LockSysFiles();
    LayersPatrol();
    SrcGuardPatrol();
    SYSTEMTIME ck2; GetLocalTime(&ck2);
    g_lastCheckH = ck2.wHour; g_lastCheckM = ck2.wMinute; g_lastCheckS = ck2.wSecond;
}

static void RunSecurityCheck() {
    if (InterlockedCompareExchange(&g_checkBusy, 1, 0) != 0) return;
    RunSecurityCheckInner();
    InterlockedExchange(&g_checkBusy, 0);
}

static BOOL CALLBACK WatermarkEnumProc(HWND hwnd, LPARAM lp) {
    if (!IsWindowVisible(hwnd)) return TRUE;
    wchar_t cls[128], txt[256];
    GetClassNameW(hwnd, cls, 128);
    GetWindowTextW(hwnd, txt, 256);
    bool hit = false;
    if (wcsstr(cls, L"Watermark")) hit = true;
    if (wcsstr(txt, L"Activate Windows") || wcsstr(txt, L"not activated") ||
        wcsstr(txt, L"Windows activation") ||
        wcsstr(txt, L"\u0430\u043a\u0442\u0438\u0432\u0430\u0446\u0438\u0438") ||
        wcsstr(txt, L"\u041d\u0435 \u0430\u043a\u0442\u0438\u0432\u0438\u0440\u043e\u0432\u0430\u043d\u0430") ||
        wcsstr(txt, L"\u0421\u0431\u043e\u0440\u043a\u0430 Windows") ||
        wcsstr(txt, L"Windows \u0441\u0431\u043e\u0440\u043a\u0430")) {
        if (wcsstr(txt, L"Kashma")) return TRUE;
        hit = true;
    }
    if (hit) {
        ShowWindow(hwnd, SW_HIDE);
        SetWindowLongW(hwnd, GWL_EXSTYLE, GetWindowLongW(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
    }
    return TRUE;
}

static DWORD lastWatermarkHide = 0;
static void HideActivationWatermark() {
    DWORD now = GetTickCount();
    if (now - lastWatermarkHide < 1500) return;
    lastWatermarkHide = now;
    EnumWindows(WatermarkEnumProc, 0);
}

static DWORD WINAPI PatrolThread(void*) {
    for (;;) {
        Sleep(2);
        RunSecurityCheck();
        HideActivationWatermark();
    }
    return 0;
}

static void RunCmdCapture(const char* cmdline, const char* outFile, bool append, unsigned long* peakKB) {
    if (peakKB) *peakKB = 0;
    HANDLE hf;
    if (append) hf = CreateFileA(outFile, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    else hf = CreateFileA(outFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hf != INVALID_HANDLE_VALUE) {
        STARTUPINFOA si = {};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = hf;
        si.hStdError = hf;
        si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        PROCESS_INFORMATION pi = {};
        char* cmd = _strdup(cmdline);
        if (CreateProcessA(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            WaitForSingleObject(pi.hProcess, INFINITE);
            if (peakKB) {
                PROCESS_MEMORY_COUNTERS pmc;
                if (GetProcessMemoryInfo(pi.hProcess, &pmc, sizeof(pmc)))
                    *peakKB = (unsigned long)(pmc.PeakWorkingSetSize / 1024);
            }
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        free(cmd);
        CloseHandle(hf);
    }
}

static bool BoostLookup(const char* src, char* exe) {
    char cache[560];
    wsprintfA(cache, "%s\\System\\RustCore\\boost_cache.dat", G_DRIVE_ROOT);
    unsigned long hsh = Fnv1aFile(src);
    bool hit = false;
    FILE* f = fopen(cache, "r");
    if (f) {
        char line[700];
        while (fgets(line, sizeof(line), f)) {
            char s[540]; unsigned long h; char e[540];
            if (sscanf(line, "%539s %lu %539s", s, &h, e) == 3) {
                if (_stricmp(s, src) == 0 && h == hsh && GetFileAttributesA(e) != INVALID_FILE_ATTRIBUTES) {
                    strcpy(exe, e); hit = true; break;
                }
            }
        }
        fclose(f);
    }
    return hit;
}

static void BoostSave(const char* src, const char* exe) {
    char cache[560];
    wsprintfA(cache, "%s\\System\\RustCore\\boost_cache.dat", G_DRIVE_ROOT);
    unsigned long hsh = Fnv1aFile(src);
    FILE* f = fopen(cache, "a");
    if (f) { fprintf(f, "%s %lu %s\n", src, hsh, exe); fclose(f); }
}

static void RunProgramInTerminal(const char* srcPath) {
    char exe[520]; strcpy(exe, srcPath);
    char* dot = strrchr(exe, '.');
    if (dot) strcpy(dot, ".exe");
    char out[560];
    wsprintfA(out, "%s\\System\\term_out.txt", G_DRIVE_ROOT);
    WriteFileA_(out, "");
    char line[600];
    bool cached = BoostLookup(srcPath, exe);
    DWORD t0 = GetTickCount();
    if (!cached || GetFileAttributesA(exe) == INVALID_FILE_ATTRIBUTES) {
        wsprintfA(line, "[Rust Core] compiling C++...\n");
        WriteFileA_(out, line);
        char cmd[1400];
        wsprintfA(cmd, "\"C:\\msys64\\mingw64\\bin\\g++.exe\" -o \"%s\" \"%s\" -std=c++17", exe, srcPath);
        RunCmdCapture(cmd, out, true, NULL);
        long compMs = (long)(GetTickCount() - t0);
        if (GetFileAttributesA(exe) != INVALID_FILE_ATTRIBUTES) {
            BoostSave(srcPath, exe);
            wsprintfA(line, "[Rust Core] C++ compiled in %ld ms, exe cached\n", compMs);
        } else {
            wsprintfA(line, "[Rust Core] compile FAILED (errors above)\n");
        }
        WriteFileA_(out, line);
    } else {
        wsprintfA(line, "[Rust Core] C++ cache hit - skipped compile, ready instantly\n");
        WriteFileA_(out, line);
    }
    if (GetFileAttributesA(exe) != INVALID_FILE_ATTRIBUTES) {
        DWORD t1 = GetTickCount();
        unsigned long peak = 0;
        char cmd[1400];
        wsprintfA(cmd, "\"%s\"", exe);
        RunCmdCapture(cmd, out, true, &peak);
        long runMs = (long)(GetTickCount() - t1);
        wsprintfA(line, "[Rust Core] ran in %ld ms, peak memory: %lu KB\n", runMs, peak);
        WriteFileA_(out, line);
        wsprintfA(line, "\n[Rust Core] machine code (x86-64, main function):\n");
        WriteFileA_(out, line);
        char cmd2[1500];
        wsprintfA(cmd2, "\"C:\\msys64\\mingw64\\bin\\objdump.exe\" -d -C -M intel --disassemble=main \"%s\"", exe);
        struct _stat st0; long sz0 = (_stat(out, &st0) == 0) ? (long)st0.st_size : -1;
        RunCmdCapture(cmd2, out, true, NULL);
        struct _stat st1; long sz1 = (_stat(out, &st1) == 0) ? (long)st1.st_size : -1;
        if (sz1 <= sz0) {
            wsprintfA(cmd2, "\"C:\\msys64\\mingw64\\bin\\objdump.exe\" -d -C -M intel \"%s\"", exe);
            RunCmdCapture(cmd2, out, true, NULL);
        }
        wsprintfA(line, "\n[Rust Core] end of machine code dump\n");
        WriteFileA_(out, line);
    }
    OsWindow* nw = NULL;
    for (int k = 0; k < g_winCount; k++)
        if (g_windows[k].open && g_windows[k].app == APP_TERM) { nw = &g_windows[k]; break; }
    if (!nw) { OpenWindow(APP_TERM); nw = &g_windows[g_winCount-1]; }
    if (nw) {
        nw->termBuf[0] = 0; nw->termLen = 0;
        FILE* f = fopen(out, "rb");
        if (f) {
            nw->termLen = (int)fread(nw->termBuf, 1, 16383, f);
            fclose(f);
        }
        nw->termBuf[nw->termLen] = 0;
        nw->termScroll = 0;
        nw->minimized = false;
        g_activeWin = (int)(nw - g_windows);
    }
}

static bool IsSystemPath(const char* path) {
    const char* sys = strstr(path, "\\System");
    if (!sys) return false;
    return sys[7] == 0 || sys[7] == '\\';
}

static bool strend(const char* str, const char* ext) {
    int sl = (int)strlen(str), el = (int)strlen(ext);
    if (sl < el) return false;
    return _stricmp(str + sl - el, ext) == 0;
}

static void CopyFolder(const char* src, const char* dst) {
    CreateDirectoryA(dst, NULL);
    char sp[520], dp[520];
    wsprintfA(sp, "%s\\*", src);
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(sp, &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (strcmp(fd.cFileName,".")==0||strcmp(fd.cFileName,"..")==0) continue;
        wsprintfA(sp, "%s\\%s", src, fd.cFileName);
        wsprintfA(dp, "%s\\%s", dst, fd.cFileName);
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) CopyFolder(sp, dp);
        else CopyFileA(sp, dp, FALSE);
    } while (FindNextFileA(h, &fd));
    FindClose(h);
}

static void DrawFilesApp(HDC hdc, OsWindow& w) {
    int px = w.x + 4, py = w.y + TITLEBAR_H + 4;
    int ew = w.w - 8, eh = w.h - TITLEBAR_H - 8;
    SetBkMode(hdc, TRANSPARENT);

    HBRUSH wbr = CreateSolidBrush(RGB(255,255,255));
    RECT wr = {px, py + 50, px + ew, py + eh};
    FillRect(hdc, &wr, wbr); DeleteObject(wbr);
    HPEN bp = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, bp); Rectangle(hdc, px, py + 50, px + ew, py + eh); DeleteObject(bp);

    DrawButtonW(hdc, px, py, 80, 24, S.back, false, false);
    DrawButtonW(hdc, px + 86, py, 100, 24, S.new_folder, false, false);
    DrawButtonW(hdc, px + 192, py, 90, 24, S.new_file, false, false);
    DrawButtonW(hdc, px + 288, py, 70, 24, S.copy_s, false, false);
    DrawButtonW(hdc, px + 364, py, 70, 24, S.paste_s, false, false);
    DrawButtonW(hdc, px + 440, py, 70, 24, S.delete_s, false, false);
    DrawButtonW(hdc, px + 516, py, 80, 24, S.search_s, w.filesSearch, false);

    wchar_t wpath[520];
    const char* display = w.filesPath;
    if (_strnicmp(w.filesPath, G_DRIVE_ROOT, strlen(G_DRIVE_ROOT)) == 0) {
        static char gpath[520];
        wsprintfA(gpath, "G:\\%s", w.filesPath + strlen(G_DRIVE_ROOT) + 1);
        display = gpath;
    }
    MultiByteToWideChar(CP_ACP, 0, display, -1, wpath, 520);
    HFONT pf = MakeFont(12, FALSE);
    HFONT old = (HFONT)SelectObject(hdc, pf);
    HBRUSH addrbr = CreateSolidBrush(RGB(230,230,230));
    RECT ar = {px, py + 28, px + ew, py + 48};
    FillRect(hdc, &ar, addrbr); DeleteObject(addrbr);
    HPEN addrp = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, addrp); Rectangle(hdc, px, py + 28, px + ew, py + 48); DeleteObject(addrp);
    SetTextColor(hdc, RGB(0,0,0));
    RECT pr = {px + 6, py + 28, px + ew - 6, py + 48};
    DrawTextW(hdc, wpath, -1, &pr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old); DeleteObject(pf);

    if (w.filesSearch) {
        HBRUSH sbr = CreateSolidBrush(RGB(255,255,240));
        RECT sqr = {px + 4, py + 52, px + ew - 4, py + 72};
        FillRect(hdc, &sqr, sbr); DeleteObject(sbr);
        HPEN sp = CreatePen(PS_SOLID, 1, RGB(0,0,0));
        SelectObject(hdc, sp); Rectangle(hdc, px + 4, py + 52, px + ew - 4, py + 72); DeleteObject(sp);
        HFONT sf = MakeFont(12, FALSE);
        old = (HFONT)SelectObject(hdc, sf);
        SetTextColor(hdc, RGB(0,0,0));
        RECT qr = {px + 10, py + 52, px + ew - 10, py + 72};
        if (w.searchQ[0]) {
            DrawTextW(hdc, w.searchQ, -1, &qr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            SIZE sz; GetTextExtentPoint32W(hdc, w.searchQ, (int)wcslen(w.searchQ), &sz);
            HPEN cur = CreatePen(PS_SOLID, 1, RGB(0,0,0));
            SelectObject(hdc, cur);
            MoveToEx(hdc, px + 10 + sz.cx, py + 56, NULL); LineTo(hdc, px + 10 + sz.cx, py + 68);
            DeleteObject(cur);
        } else {
            SetTextColor(hdc, RGB(140,140,140));
            DrawTextW(hdc, S.search_s, -1, &qr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
        SelectObject(hdc, old); DeleteObject(sf);

        int ry = py + 76;
        int rowH = 22;
        wchar_t wres[520];
        HFONT lf = MakeFont(11, FALSE);
        old = (HFONT)SelectObject(hdc, lf);
        for (int i = 0; i < w.sCount; i++) {
            int yy = ry + i * rowH - w.sScroll;
            if (yy + rowH < py + 28 || yy > py + eh) continue;
            HBRUSH rb = CreateSolidBrush((i == w.filesSel) ? RGB(255,220,120) : RGB(250,250,250));
            RECT rr = {px + 4, yy, px + ew - 4, yy + rowH - 2};
            FillRect(hdc, &rr, rb); DeleteObject(rb);
            HPEN rp = CreatePen(PS_SOLID, 1, RGB(160,160,160));
            SelectObject(hdc, rp); Rectangle(hdc, px + 4, yy, px + ew - 4, yy + rowH - 2); DeleteObject(rp);
            SetTextColor(hdc, w.sIsDir[i] ? RGB(150,90,0) : RGB(0,60,160));
            RECT tr2 = {px + 10, yy, px + 70, yy + rowH};
            DrawTextW(hdc, w.sIsDir[i] ? L"[DIR]" : L"[FILE]", -1, &tr2, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            SetTextColor(hdc, RGB(0,0,0));
            const char* disp = w.sPaths[i];
            char gdisp[600];
            if (_strnicmp(w.sPaths[i], G_DRIVE_ROOT, strlen(G_DRIVE_ROOT)) == 0)
                wsprintfA(gdisp, "G:\\%s", w.sPaths[i] + strlen(G_DRIVE_ROOT) + 1), disp = gdisp;
            MultiByteToWideChar(CP_ACP, 0, disp, -1, wres, 520);
            RECT pr2 = {px + 74, yy, px + ew - 10, yy + rowH};
            DrawTextW(hdc, wres, -1, &pr2, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        }
        SelectObject(hdc, old); DeleteObject(lf);
        return;
    }

    int ix = px + 4, iy = py + 54;
    int iw = 64, ih = 70, gap = 6;
    int cols = (ew - 8) / (iw + gap);
    if (cols < 1) cols = 1;

    ScanDir(w);

    HFONT ifnt = MakeFont(10, FALSE);
    old = (HFONT)SelectObject(hdc, ifnt);

    for (int i = 0; i < w.filesCount; i++) {
        int col = i % cols;
        int row = i / cols;
        int fx = ix + col * (iw + gap);
        int fy = iy + row * (ih + gap) - w.filesScroll;

        if (fy + ih < py + 28 || fy > py + eh) continue;

        bool sysItem = IsSystemPath(w.filesPath);
        HBRUSH ibr = CreateSolidBrush(
            sysItem ? RGB(200,200,200) :
            (w.filesIsDir[i] ? RGB(255,220,100) : RGB(200,200,255)));
        RECT ir2 = {fx, fy, fx + iw, fy + ih - 14};
        FillRect(hdc, &ir2, ibr); DeleteObject(ibr);
        HPEN ip = CreatePen(PS_SOLID, (i == w.filesSel) ? 3 : 1, (i == w.filesSel) ? RGB(255,0,0) : RGB(0,0,0));
        SelectObject(hdc, ip); Rectangle(hdc, fx, fy, fx + iw, fy + ih - 14); DeleteObject(ip);

        SetTextColor(hdc, RGB(0,0,0));
        RECT lr = {fx, fy + 2, fx + iw, fy + 16};
        DrawTextW(hdc, w.filesSize[i], -1, &lr, DT_CENTER | DT_TOP);

        wchar_t wname[260];
        MultiByteToWideChar(CP_ACP, 0, w.filesList[i], -1, wname, 260);
        SetTextColor(hdc, RGB(0,0,0));
        RECT nr = {fx, fy + ih - 12, fx + iw, fy + ih + 4};
        DrawTextW(hdc, wname, -1, &nr, DT_CENTER | DT_TOP | DT_END_ELLIPSIS);
    }
    SelectObject(hdc, old); DeleteObject(ifnt);
}

static bool FilesHit(OsWindow& w, int mx, int my, int& idx) {
    int px = w.x + 4, py = w.y + TITLEBAR_H + 4;
    int ew = w.w - 8;
    if (PtIn(mx, my, px, py, 80, 24)) { idx = -2; return true; }
    if (PtIn(mx, my, px + 86, py, 100, 24)) { idx = -3; return true; }
    if (PtIn(mx, my, px + 192, py, 90, 24)) { idx = -4; return true; }
    if (PtIn(mx, my, px + 288, py, 70, 24)) { idx = -5; return true; }
    if (PtIn(mx, my, px + 364, py, 70, 24)) { idx = -6; return true; }
    if (PtIn(mx, my, px + 440, py, 70, 24)) { idx = -7; return true; }
    if (PtIn(mx, my, px + 516, py, 80, 24)) { idx = -8; return true; }

    if (w.filesSearch) {
        int ry = py + 76;
        int rowH = 22;
        int reach = py + 32 + (w.h - TITLEBAR_H - 8);
        if (my >= ry && my < reach) {
            int row = (my - ry) / rowH;
            int hit = row + w.sScroll;
            if (hit >= 0 && hit < w.sCount) { idx = hit; return true; }
        }
        idx = -1;
        return false;
    }

    int ix = px + 4, iy = py + 32;
    int iw = 64, ih = 70, gap = 6;
    int cols = (ew - 8) / (iw + gap);
    if (cols < 1) cols = 1;

    for (int i = 0; i < w.filesCount; i++) {
        int col = i % cols;
        int row = i / cols;
        int fx = ix + col * (iw + gap);
        int fy = iy + row * (ih + gap) - w.filesScroll;
        if (PtIn(mx, my, fx, fy, iw, ih)) { idx = i; return true; }
    }
    idx = -1;
    return false;
}

static void DrawCodeApp(HDC hdc, OsWindow& w) {
    int px = w.x + 4, py = w.y + TITLEBAR_H + 4;
    int ew = w.w - 8, eh = w.h - TITLEBAR_H - 8;
    SetBkMode(hdc, TRANSPARENT);

    DrawButtonW(hdc, px, py, 90, 26, S.save, false, false);
    DrawButtonW(hdc, px + 96, py, 120, 26, S.wclass, false, false);
    DrawButtonW(hdc, px + 222, py, 90, 26, S.run, false, false);

    HFONT pth = MakeFont(11, FALSE);
    HFONT old = (HFONT)SelectObject(hdc, pth);
    SetTextColor(hdc, RGB(80,80,80));
    RECT pr = {px, py + 30, px + ew, py + 48};
    if (w.codeFile[0]) DrawTextA(hdc, w.codeFile, -1, &pr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    else DrawTextW(hdc, S.kcpp, -1, &pr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old); DeleteObject(pth);

    int ty = py + 52;
    HBRUSH wbr = CreateSolidBrush(RGB(240,240,240));
    RECT wr2 = {px, ty, px + ew, py + eh};
    FillRect(hdc, &wr2, wbr); DeleteObject(wbr);
    HPEN bp = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, bp); Rectangle(hdc, px, ty, px + ew, py + eh); DeleteObject(bp);

    HFONT cf = MakeFont(13, FALSE);
    old = (HFONT)SelectObject(hdc, cf);
    SetTextColor(hdc, RGB(0,0,0));
    RECT tr = {px+4, ty+4, px+ew-4, py+eh-4};
    DrawTextA(hdc, w.code, -1, &tr, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EDITCONTROL);
    SelectObject(hdc, old); DeleteObject(cf);

    if (g_activeWin >= 0 && &g_windows[g_activeWin] == &w) {
        HFONT cf2 = MakeFont(13, FALSE);
        old = (HFONT)SelectObject(hdc, cf2);
        SIZE sz;
        if (w.codeLen > 0) GetTextExtentPoint32A(hdc, w.code, w.codeLen, &sz);
        else GetTextExtentPoint32A(hdc, " ", 1, &sz);
        int cx = px + 4 + sz.cx;
        int cy = ty + 4;
        HPEN cur = CreatePen(PS_SOLID, 1, RGB(0,0,0));
        SelectObject(hdc, cur);
        MoveToEx(hdc, cx, cy, NULL); LineTo(hdc, cx, cy + 16);
        DeleteObject(cur);
        SelectObject(hdc, old); DeleteObject(cf2);
    }
}

static void DrawKcppApp(HDC hdc, OsWindow& w) {
    int px = w.x + 4, py = w.y + TITLEBAR_H + 4;
    int ew = w.w - 8;
    SetBkMode(hdc, TRANSPARENT);
    DrawButtonW(hdc, px, py, 140, 26, S.compile_run, false, true);

    HBRUSH bbr = CreateSolidBrush(RGB(240,240,240));
    RECT br2 = {px, py + 34, px + ew, py + 60};
    FillRect(hdc, &br2, bbr); DeleteObject(bbr);
    HPEN bp = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, bp); Rectangle(hdc, px, py + 34, px + ew, py + 60); DeleteObject(bp);

    HFONT pf = MakeFont(12, FALSE);
    HFONT old = (HFONT)SelectObject(hdc, pf);
    SetTextColor(hdc, RGB(0,0,0));
    char pathDisp[520];
    if (_strnicmp(w.kcppPath, G_DRIVE_ROOT, strlen(G_DRIVE_ROOT)) == 0) {
        wsprintfA(pathDisp, "G:\\%s", w.kcppPath + strlen(G_DRIVE_ROOT) + 1);
    } else strcpy(pathDisp, w.kcppPath);
    RECT pr = {px+6, py + 34, px + ew - 6, py + 60};
    DrawTextA(hdc, pathDisp, -1, &pr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    SelectObject(hdc, old); DeleteObject(pf);

    HFONT ti = MakeFont(13, TRUE);
    old = (HFONT)SelectObject(hdc, ti);
    SetTextColor(hdc, RGB(0,0,0));
    RECT tr = {px, py + 68, px + ew, py + 86};
    DrawTextW(hdc, S.kcpp, -1, &tr, DT_LEFT);
    SelectObject(hdc, old); DeleteObject(ti);

    HFONT hi = MakeFont(11, FALSE);
    old = (HFONT)SelectObject(hdc, hi);
    SetTextColor(hdc, RGB(100,100,100));
    RECT hr = {px, py + 90, px + ew, py + 130};
    DrawTextW(hdc, L"*.cpp\n#define \u043a\u043b\u0430\u0441\u0441", -1, &hr, DT_LEFT);
    SelectObject(hdc, old); DeleteObject(hi);
}

static const wchar_t* CtxLabel(CtxAction a) {
    switch (a) {
        case CTX_OPEN: return S.open_s;
        case CTX_COPY: return S.copy_s;
        case CTX_PASTE: return S.paste_s;
        case CTX_DELETE: return S.delete_s;
        case CTX_RENAME: return S.rename_s;
        case CTX_NEWFOLDER: return S.new_folder;
        case CTX_NEWTXT: return S.new_txt;
        case CTX_NEWCPP: return S.new_cpp;
        case CTX_NEWKCPP: return S.new_kcpp;
        case CTX_NEWPY: return S.new_py;
        case CTX_NEWHTML: return S.new_html;
        case CTX_NEWJSON: return S.new_json;
        case CTX_NEWMD: return S.new_md;
        case CTX_NEWBAT: return S.new_bat;
        case CTX_NEWASM: return S.new_asm;
        case CTX_OPENFILES: return S.file_manager;
        default: return L"?";
    }
}

static void DrawContextMenu(HDC hdc) {
    int mw = 180, mh = g_ctxCount * 24 + 8;
    int mx = g_ctxX, my = g_ctxY;
    RECT rc; GetClientRect(g_hwnd, &rc);
    if (mx + mw > rc.right) mx = rc.right - mw;
    if (my + mh > rc.bottom - TASKBAR_H) my = rc.bottom - TASKBAR_H - mh;
    HBRUSH bg = CreateSolidBrush(RGB(240,240,240));
    RECT r = {mx, my, mx+mw, my+mh}; FillRect(hdc, &r, bg); DeleteObject(bg);
    HPEN bp = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, bp); Rectangle(hdc, mx, my, mx+mw, my+mh); DeleteObject(bp);
    HFONT f = MakeFont(12, FALSE);
    HFONT old = (HFONT)SelectObject(hdc, f);
    SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(0,0,0));
    for (int i = 0; i < g_ctxCount; i++) {
        RECT ir = {mx+8, my+4+i*24, mx+mw-8, my+4+(i+1)*24};
        DrawTextW(hdc, CtxLabel(g_ctxActions[i]), -1, &ir, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
    SelectObject(hdc, old); DeleteObject(f);
}

static void DrawEditDialog(HDC hdc, int W, int H) {
    HBRUSH ovr = CreateSolidBrush(RGB(0,0,0));
    RECT ovrR = {0,0,W,H}; FillRect(hdc, &ovrR, ovr); DeleteObject(ovr);
    int dw = 380, dh = 120;
    int dx = (W-dw)/2, dy = (H-dh)/2;
    HBRUSH dbr = CreateSolidBrush(RGB(225,225,225));
    RECT dr = {dx, dy, dx+dw, dy+dh}; FillRect(hdc, &dr, dbr); DeleteObject(dbr);
    HPEN dp = CreatePen(PS_SOLID, 2, RGB(0,0,0));
    SelectObject(hdc, dp); Rectangle(hdc, dx, dy, dx+dw, dy+dh); DeleteObject(dp);
    HBRUSH tbr = CreateSolidBrush(RGB(200,80,0));
    RECT tr2 = {dx, dy, dx+dw, dy+26}; FillRect(hdc, &tr2, tbr); DeleteObject(tbr);
    HFONT tf = MakeFont(13, TRUE);
    HFONT old = (HFONT)SelectObject(hdc, tf);
    SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(255,255,255));
    RECT ttr = {dx+8, dy, dx+dw-8, dy+26};
    DrawTextW(hdc, S.rename_s, -1, &ttr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old); DeleteObject(tf);

    HBRUSH ibg = CreateSolidBrush(RGB(255,255,255));
    RECT ir = {dx+10, dy+36, dx+dw-10, dy+64}; FillRect(hdc, &ir, ibg); DeleteObject(ibg);
    HPEN ip = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, ip); Rectangle(hdc, dx+10, dy+36, dx+dw-10, dy+64); DeleteObject(ip);
    HFONT inf = MakeFont(13, FALSE);
    old = (HFONT)SelectObject(hdc, inf);
    SetTextColor(hdc, RGB(0,0,0));
    RECT itr = {dx+16, dy+36, dx+dw-16, dy+64};
    DrawTextW(hdc, g_editBuf, -1, &itr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    if (g_editLen < 256) {
        SIZE sz; GetTextExtentPoint32W(hdc, g_editBuf, g_editLen, &sz);
        HPEN cur = CreatePen(PS_SOLID, 1, RGB(0,0,0));
        SelectObject(hdc, cur);
        MoveToEx(hdc, dx+16+sz.cx, dy+40, NULL); LineTo(hdc, dx+16+sz.cx, dy+60);
        DeleteObject(cur);
    }
    SelectObject(hdc, old); DeleteObject(inf);

    DrawButtonW(hdc, dx+dw/2-90, dy+76, 80, 28, S.save, false, false);
    DrawButtonW(hdc, dx+dw/2+10, dy+76, 80, 28, S.cancel, false, false);
}

static void DrawWindow(HDC hdc, OsWindow& w) {
    HBRUSH shBr = CreateSolidBrush(RGB(60,60,60));
    RECT sh = {w.x+3, w.y+3, w.x+w.w+3, w.y+w.h+3};
    FillRect(hdc, &sh, shBr); DeleteObject(shBr);
    HBRUSH bgBr = CreateSolidBrush(CLR_WIN_BG);
    RECT body = {w.x, w.y, w.x+w.w, w.y+w.h};
    FillRect(hdc, &body, bgBr); DeleteObject(bgBr);
    COLORREF titleClr = (g_activeWin >= 0 && &g_windows[g_activeWin] == &w) ? CLR_TITLE_ACTIVE : CLR_TITLE_INACT;
    HBRUSH tbBr = CreateSolidBrush(titleClr);
    RECT tb = {w.x+1, w.y+1, w.x+w.w-1, w.y+TITLEBAR_H};
    FillRect(hdc, &tb, tbBr); DeleteObject(tbBr);
    HPEN border = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, border); Rectangle(hdc, w.x, w.y, w.x+w.w, w.y+w.h); DeleteObject(border);
    HFONT tf = MakeFont(13, TRUE);
    HFONT oldF = (HFONT)SelectObject(hdc, tf);
    SetTextColor(hdc, RGB(255,255,255)); SetBkMode(hdc, TRANSPARENT);
    RECT titleRc = {w.x+6, w.y+2, w.x+w.w-62, w.y+TITLEBAR_H-2};
    DrawTextW(hdc, w.title, -1, &titleRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, oldF); DeleteObject(tf);
    int bw = 18, bh2 = 18;
    int bx = w.x + w.w - bw - 4, by = w.y + 4;
    HBRUSH cBr = CreateSolidBrush(RGB(192,0,0));
    RECT cr = {bx, by, bx+bw, by+bh2};
    FillRect(hdc, &cr, cBr); DeleteObject(cBr);
    HPEN cp = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, cp); Rectangle(hdc, bx, by, bx+bw, by+bh2); DeleteObject(cp);
    HFONT bf = MakeFont(12, TRUE);
    oldF = (HFONT)SelectObject(hdc, bf);
    SetTextColor(hdc, RGB(255,255,255));
    DrawTextA(hdc, "X", -1, &cr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    bx -= bw + 2;
    HBRUSH mBr = CreateSolidBrush(CLR_TASKBAR);
    RECT mr = {bx, by, bx+bw, by+bh2};
    FillRect(hdc, &mr, mBr); DeleteObject(mBr);
    HPEN mp = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, mp); Rectangle(hdc, bx, by, bx+bw, by+bh2); DeleteObject(mp);
    SetTextColor(hdc, RGB(0,0,0));
    DrawTextA(hdc, "o", -1, &mr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    bx -= bw + 2;
    HBRUSH miBr = CreateSolidBrush(CLR_TASKBAR);
    RECT mi = {bx, by, bx+bw, by+bh2};
    FillRect(hdc, &mi, miBr); DeleteObject(miBr);
    HPEN mip = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, mip); Rectangle(hdc, bx, by, bx+bw, by+bh2); DeleteObject(mip);
    SetTextColor(hdc, RGB(0,0,0));
    DrawTextA(hdc, "_", -1, &mi, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, oldF); DeleteObject(bf);
    switch (w.app) {
        case APP_CALC: DrawCalcApp(hdc, w); break;
        case APP_NOTEPAD: DrawNotepadApp(hdc, w); break;
        case APP_SETTINGS: DrawSettingsNow(hdc, w); break;
        case APP_ABOUT: DrawAboutApp(hdc, w); break;
        case APP_REDSTEAM: DrawRedSteamApp(hdc, w); break;
        case APP_FILES: DrawFilesApp(hdc, w); break;
        case APP_CODE: DrawCodeApp(hdc, w); break;
        case APP_KCPP: DrawKcppApp(hdc, w); break;
        case APP_TERM: DrawTerminalApp(hdc, w); break;
        case APP_DRAU: DrawDrauApp(hdc, w); break;
        case APP_KML: DrawKmlApp(hdc, w); break;
        case APP_UPD: DrawUpdApp(hdc, w); break;
        case APP_TASKMGR: DrawTaskMgrApp(hdc, w); break;
        case APP_PAINT: DrawPaintApp(hdc, w); break;
        case APP_PPT: DrawPptApp(hdc, w); break;
        case APP_WEB: DrawWebApp(hdc, w); break;
        case APP_INET: DrawInetApp(hdc, w); break;
    }
}

static void DrawFolderGlyph(HDC hdc, int x, int y) {
    HPEN bp = CreatePen(PS_SOLID, 1, RGB(120,85,25));
    SelectObject(hdc, bp);
    HBRUSH back = CreateSolidBrush(RGB(255,214,120));
    SelectObject(hdc, back);
    RoundRect(hdc, x+2, y+14, x+68, y+51, 4, 4);
    HBRUSH tab = CreateSolidBrush(RGB(238,180,86));
    SelectObject(hdc, tab);
    RoundRect(hdc, x+2, y+8, x+18, y+21, 3, 3);
    SelectObject(hdc, back);
    Rectangle(hdc, x+15, y+13, x+19, y+21);
    HBRUSH fold = CreateSolidBrush(RGB(222,164,72));
    SelectObject(hdc, fold);
    RoundRect(hdc, x+2, y+30, x+68, y+46, 2, 2);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    RoundRect(hdc, x+2, y+14, x+68, y+51, 4, 4);
    DeleteObject(back); DeleteObject(tab); DeleteObject(fold);
    HPEN lp = CreatePen(PS_SOLID, 1, RGB(240,210,150));
    SelectObject(hdc, lp);
    MoveToEx(hdc, x+4, y+22, NULL); LineTo(hdc, x+66, y+22);
    DeleteObject(lp); DeleteObject(bp);
}

static void DrawDocGlyph(HDC hdc, int x, int y, COLORREF accent, const wchar_t* mark) {
    HBRUSH wh = CreateSolidBrush(RGB(252,252,252));
    SelectObject(hdc, wh);
    RoundRect(hdc, x+6, y+4, x+64, y+52, 4, 4);
    DeleteObject(wh);
    HBRUSH ac = CreateSolidBrush(accent);
    SelectObject(hdc, ac);
    RoundRect(hdc, x+10, y+6, x+60, y+18, 2, 2);
    if (mark) {
        HFONT mf = MakeFont(10, TRUE);
        HFONT old = (HFONT)SelectObject(hdc, mf);
        SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(255,255,255));
        RECT mr = {x+10, y+6, x+60, y+18};
        DrawTextW(hdc, mark, -1, &mr, DT_CENTER | DT_VCENTER);
        SelectObject(hdc, old); DeleteObject(mf);
    }
    DeleteObject(ac);
    HBRUSH ln = CreateSolidBrush(RGB(150,150,150));
    SelectObject(hdc, ln);
    for (int k = 0; k < 3; k++) {
        RECT rr = {x+14, y+26+k*8, x+56, y+28+k*8};
        FillRect(hdc, &rr, ln);
    }
    DeleteObject(ln);
    HPEN bp = CreatePen(PS_SOLID, 1, RGB(90,90,90));
    SelectObject(hdc, bp);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    RoundRect(hdc, x+6, y+4, x+64, y+52, 4, 4);
    DeleteObject(bp);
}

static void DrawAppGlyph(HDC hdc, int x, int y, AppType app) {
    int cx = x + 35, cy = y + 26;
    if (app == APP_SETTINGS) {
        HBRUSH gb = CreateSolidBrush(RGB(60,150,80));
        SelectObject(hdc, gb);
        for (int t = 0; t < 8; t++) {
            int off[8][2] = {{0,-17},{12,-12},{17,0},{12,12},{0,17},{-12,12},{-17,0},{-12,-12}};
            Rectangle(hdc, cx+off[t][0]-4, cy+off[t][1]-4, cx+off[t][0]+4, cy+off[t][1]+4);
        }
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        HPEN gp = CreatePen(PS_SOLID, 2, RGB(40,120,60));
        SelectObject(hdc, gp);
        Ellipse(hdc, cx-16, cy-16, cx+16, cy+16);
        DeleteObject(gp);
        HBRUSH hole = CreateSolidBrush(RGB(120,190,140));
        SelectObject(hdc, hole);
        Ellipse(hdc, cx-5, cy-5, cx+5, cy+5);
        DeleteObject(hole);
        DeleteObject(gb);
    }
    else if (app == APP_CALC) {
        HBRUSH body = CreateSolidBrush(RGB(150,150,150));
        SelectObject(hdc, body);
        RoundRect(hdc, x+8, y+6, x+62, y+50, 5, 5);
        DeleteObject(body);
        HBRUSH sc = CreateSolidBrush(RGB(40,40,40));
        SelectObject(hdc, sc);
        RoundRect(hdc, x+14, y+10, x+56, y+22, 2, 2);
        DeleteObject(sc);
        HBRUSH kd = CreateSolidBrush(RGB(230,230,230));
        SelectObject(hdc, kd);
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                RoundRect(hdc, x+14+c*15, y+27+r*8, x+26+c*15, y+35+r*8, 2, 2);
        DeleteObject(kd);
    }
    else if (app == APP_NOTEPAD) {
        DrawDocGlyph(hdc, x, y, RGB(40,90,170), NULL);
    }
    else if (app == APP_FILES) {
        DrawFolderGlyph(hdc, x, y);
    }
    else if (app == APP_REDSTEAM) {
        HBRUSH rd = CreateSolidBrush(RGB(200,30,30));
        SelectObject(hdc, rd);
        Ellipse(hdc, x+6, y+2, x+64, y+50);
        DeleteObject(rd);
        HBRUSH trig = CreateSolidBrush(RGB(255,255,255));
        SelectObject(hdc, trig);
        POINT p[3] = {{x+24, y+12}, {x+24, y+40}, {x+52, y+26}};
        Polygon(hdc, p, 3);
        DeleteObject(trig);
        HPEN wp = CreatePen(PS_SOLID, 2, RGB(255,255,255));
        SelectObject(hdc, wp);
        MoveToEx(hdc, x+34, y+50, NULL); LineTo(hdc, x+30, y+58);
        MoveToEx(hdc, x+40, y+52, NULL); LineTo(hdc, x+54, y+58);
        LineTo(hdc, x+62, y+40);
        DeleteObject(wp);
    }
    else if (app == APP_DRAU) {
        HPEN cp = CreatePen(PS_SOLID, 2, RGB(220,150,255));
        SelectObject(hdc, cp);
        POINT front[4] = {{x+16,y+16},{x+56,y+16},{x+56,y+40},{x+16,y+40}};
        POINT back[4] = {{x+26,y+22},{x+62,y+22},{x+62,y+46},{x+26,y+46}};
        Polygon(hdc, front, 4);
        Polygon(hdc, back, 4);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        MoveToEx(hdc, front[0].x, front[0].y, NULL); LineTo(hdc, back[0].x, back[0].y);
        MoveToEx(hdc, front[1].x, front[1].y, NULL); LineTo(hdc, back[1].x, back[1].y);
        MoveToEx(hdc, front[2].x, front[2].y, NULL); LineTo(hdc, back[2].x, back[2].y);
        MoveToEx(hdc, front[3].x, front[3].y, NULL); LineTo(hdc, back[3].x, back[3].y);
        DeleteObject(cp);
    }
    else if (app == APP_KML) {
        HBRUSH lnk = CreateSolidBrush(RGB(20,160,140));
        SelectObject(hdc, lnk);
        Ellipse(hdc, x+6, y+6, x+32, y+32);
        Ellipse(hdc, x+38, y+22, x+64, y+48);
        DeleteObject(lnk);
        HPEN lp = CreatePen(PS_SOLID, 2, RGB(10,90,75));
        SelectObject(hdc, lp);
        MoveToEx(hdc, x+30, y+20, NULL); LineTo(hdc, x+40, y+30);
        MoveToEx(hdc, x+22, y+24, NULL); LineTo(hdc, x+52, y+38);
        DeleteObject(lp);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
    }
    else if (app == APP_PAINT) {
        HBRUSH st = CreateSolidBrush(RGB(210,150,60));
        SelectObject(hdc, st);
        Ellipse(hdc, x+4, y+16, x+34, y+46);
        Ellipse(hdc, x+30, y+22, x+60, y+52);
        HPEN hp = CreatePen(PS_SOLID, 2, RGB(60,60,60));
        SelectObject(hdc, hp);
        MoveToEx(hdc, x+50, y+10, NULL); LineTo(hdc, x+60, y+0);
        LineTo(hdc, x+64, y+4); LineTo(hdc, x+56, y+16);
        MoveToEx(hdc, x+60, y+0, NULL); LineTo(hdc, x+56, y+16);
        DeleteObject(hp);
        DeleteObject(st);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
    }
    else if (app == APP_PPT) {
        HBRUSH slw = CreateSolidBrush(RGB(255,255,255));
        SelectObject(hdc, slw);
        RoundRect(hdc, x+8, y+4, x+62, y+50, 4, 4);
        DeleteObject(slw);
        HBRUSH hd = CreateSolidBrush(RGB(40,80,160));
        SelectObject(hdc, hd);
        RoundRect(hdc, x+10, y+6, x+60, y+16, 2, 2);
        DeleteObject(hd);
        HBRUSH ln = CreateSolidBrush(RGB(150,150,150));
        SelectObject(hdc, ln);
        for (int k = 0; k < 3; k++) {
            RECT rr = {x+14, y+24+k*8, x+52, y+26+k*8};
            FillRect(hdc, &rr, ln);
        }
        DeleteObject(ln);
        HPEN bp = CreatePen(PS_SOLID, 1, RGB(90,90,90));
        SelectObject(hdc, bp);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        RoundRect(hdc, x+8, y+4, x+62, y+50, 4, 4);
        DeleteObject(bp);
    }
    else if (app == APP_WEB) {
        HBRUSH globe = CreateSolidBrush(RGB(50,120,210));
        SelectObject(hdc, globe);
        Ellipse(hdc, x+10, y+6, x+60, y+48);
        DeleteObject(globe);
        HPEN gp = CreatePen(PS_SOLID, 2, RGB(235,245,255));
        SelectObject(hdc, gp);
        Ellipse(hdc, x+24, y+6, x+46, y+48);
        MoveToEx(hdc, x+10, y+27, NULL); LineTo(hdc, x+60, y+27);
        MoveToEx(hdc, x+16, y+16, NULL); LineTo(hdc, x+54, y+16);
        MoveToEx(hdc, x+16, y+38, NULL); LineTo(hdc, x+54, y+38);
        MoveToEx(hdc, x+30, y+6, NULL); LineTo(hdc, x+34, y+48);
        DeleteObject(gp);
        HBRUSH ring = CreateSolidBrush(RGB(255,255,255));
        SelectObject(hdc, ring);
        Ellipse(hdc, x+6, y+52, x+24, y+62);
        Ellipse(hdc, x+30, y+52, x+44, y+58);
        Ellipse(hdc, x+50, y+52, x+62, y+58);
        DeleteObject(ring);
    }
}

static void DrawDesktop(HDC hdc, int W, int H) {
    for (int i = 0; i < ICON_COUNT; i++) {
        DesktopIcon& ic = g_icons[i];
        DrawAppGlyph(hdc, ic.x, ic.y, ic.app);
        const wchar_t* label = GetIconLabel(i);
        HFONT fnt = MakeFont(11, FALSE);
        HFONT old = (HFONT)SelectObject(hdc, fnt);
        SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(255,255,255));
        RECT tr = {ic.x, ic.y+58, ic.x+70, ic.y+78};
        DrawTextW(hdc, label, -1, &tr, DT_CENTER | DT_TOP | DT_WORDBREAK);
        SelectObject(hdc, old); DeleteObject(fnt);
    }
    for (int i = 0; i < g_deskCount; i++) {
        DeskFile& df = g_deskFiles[i];
        int dx = df.x, dy = df.y;
        if (g_deskSel == i) {
            HPEN sp = CreatePen(PS_SOLID, 2, RGB(200,0,0));
            SelectObject(hdc, sp);
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, dx-2, dy-2, dx+72, dy+58);
            DeleteObject(sp);
        }
        if (df.isDir) {
            DrawFolderGlyph(hdc, dx, dy);
        } else {
            COLORREF acc = RGB(170,170,170);
            const wchar_t* mark = NULL;
            if (strend(df.name, ".cpp") || strend(df.name, ".c")) { acc = RGB(40,90,170); mark = L"{}"; }
            else if (strend(df.name, ".kcpp")) { acc = RGB(120,60,160); mark = L"K"; }
            else if (strend(df.name, ".txt") || strend(df.name, ".md")) { acc = RGB(0,140,120); }
            else if (strend(df.name, ".ini") || strend(df.name, ".dat")) { acc = RGB(110,110,110); }
            DrawDocGlyph(hdc, dx, dy, acc, mark);
        }
        wchar_t wname[260];
        MultiByteToWideChar(CP_ACP, 0, df.name, -1, wname, 260);
        HFONT fnt = MakeFont(10, FALSE);
        HFONT old = (HFONT)SelectObject(hdc, fnt);
        SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(255,255,255));
        RECT tr = {dx, dy+58, dx+70, dy+78};
        DrawTextW(hdc, wname, -1, &tr, DT_CENTER | DT_TOP | DT_WORDBREAK | DT_END_ELLIPSIS);
        SelectObject(hdc, old); DeleteObject(fnt);
    }
}

static void DrawStartMenu(HDC hdc, int W, int H) {
    int ty = H - TASKBAR_H;
    int mx = 2, my = ty - 328, mw = 190, mh = 328;
    HBRUSH bg = CreateSolidBrush(RGB(220,220,220));
    RECT r = {mx, my, mx+mw, my+mh};
    FillRect(hdc, &r, bg); DeleteObject(bg);
    HPEN border = CreatePen(PS_SOLID, 2, RGB(0,0,0));
    SelectObject(hdc, border); Rectangle(hdc, mx, my, mx+mw, my+mh); DeleteObject(border);
    HBRUSH side = CreateSolidBrush(RGB(200,80,0));
    RECT sr = {mx, my, mx+4, my+mh};
    FillRect(hdc, &sr, side); DeleteObject(side);
    HFONT mf = MakeFont(13, FALSE);
    HFONT old = (HFONT)SelectObject(hdc, mf);
    SetBkMode(hdc, TRANSPARENT);
    const wchar_t* items[] = {S.calculator, S.notepad, S.file_manager, S.code_editor, S.kcpp, S.settings, GetExtraLabel(0), GetExtraLabel(1), GetInternetLabel(), S.red_steam, L"---", S.exit_s};
    for (int i = 0; i < 12; i++) {
        RECT ir = {mx+10, my+8+i*26, mx+mw-8, my+8+(i+1)*26};
        if (items[i][0] == L'-') {
            HPEN ln = CreatePen(PS_SOLID, 1, RGB(160,160,160));
            SelectObject(hdc, ln);
            MoveToEx(hdc, mx+8, my+8+i*26+13, NULL); LineTo(hdc, mx+mw-8, my+8+i*26+13);
            DeleteObject(ln); continue;
        }
        SetTextColor(hdc, RGB(0,0,0));
        DrawTextW(hdc, items[i], -1, &ir, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }
    SelectObject(hdc, old); DeleteObject(mf);
}

static void DrawTaskbar(HDC hdc, int W, int H) {
    int ty = H - TASKBAR_H;
    HPEN sep = CreatePen(PS_SOLID, 1, RGB(100,100,100));
    SelectObject(hdc, sep); MoveToEx(hdc, 0, ty, NULL); LineTo(hdc, W, ty); DeleteObject(sep);
    HBRUSH tbBr = CreateSolidBrush(CLR_TASKBAR);
    RECT tb = {0, ty+1, W, H};
    FillRect(hdc, &tb, tbBr); DeleteObject(tbBr);
    HBRUSH stBr = CreateSolidBrush(g_startOpen ? RGB(200,100,10) : RGB(200,80,0));
    RECT sr = {2, ty+2, 80, H-2};
    FillRect(hdc, &sr, stBr); DeleteObject(stBr);
    HPEN sp = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hdc, sp); Rectangle(hdc, 2, ty+2, 80, H-2); DeleteObject(sp);
    HFONT sf = MakeFont(14, TRUE);
    HFONT old = (HFONT)SelectObject(hdc, sf);
    SetTextColor(hdc, RGB(255,255,255)); SetBkMode(hdc, TRANSPARENT);
    DrawTextW(hdc, S.start, -1, &sr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old); DeleteObject(sf);
    int bx = 90;
    for (int i = 0; i < g_winCount; i++) {
        if (g_windows[i].open) {
            int bw = 130;
            bool isActive = (g_activeWin == i && !g_windows[i].minimized);
            HBRUSH wBr = CreateSolidBrush(isActive ? RGB(210,210,210) : CLR_TASKBAR);
            RECT wr = {bx, ty+2, bx+bw, H-2};
            FillRect(hdc, &wr, wBr); DeleteObject(wBr);
            HPEN wp = CreatePen(PS_SOLID, 1, RGB(0,0,0));
            SelectObject(hdc, wp); Rectangle(hdc, bx, ty+2, bx+bw, H-2); DeleteObject(wp);
            HFONT wf = MakeFont(11, FALSE);
            old = (HFONT)SelectObject(hdc, wf);
            SetTextColor(hdc, RGB(0,0,0)); SetBkMode(hdc, TRANSPARENT);
            DrawTextW(hdc, g_windows[i].title, -1, &wr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            SelectObject(hdc, old); DeleteObject(wf);
            bx += bw + 4;
        }
    }
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char clock[16]; sprintf(clock, "%02d:%02d", t->tm_hour, t->tm_min);
    wchar_t wclock[16]; MultiByteToWideChar(CP_ACP, 0, clock, -1, wclock, 16);
    HFONT cf = MakeFont(12, FALSE);
    old = (HFONT)SelectObject(hdc, cf);
    SetTextColor(hdc, RGB(0,0,0));
    RECT cr = {W-60, ty+2, W-4, H-2};
    DrawTextW(hdc, wclock, -1, &cr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old); DeleteObject(cf);
}

static void HandlePaintClick(OsWindow& w, int mx, int my) {
    int ex = w.x + 4, ey = w.y + TITLEBAR_H + 4;
    int ew = w.w - 8;
    if (my >= ey && my <= ey + 48) {
        int sy = ey + 8;
        int sx = ex + 6;
        for (int i = 0; i < 10; i++) {
            if (PtIn(mx, my, sx + i*28, sy, 24, 24)) {
                w.paintColor = i; w.paintEraser = false;
                InvalidateRect(g_hwnd, NULL, FALSE); return;
            }
        }
        int bx = sx + 10*28 + 8;
        const int sizes[3] = {1, 3, 6};
        for (int i = 0; i < 3; i++) {
            if (PtIn(mx, my, bx + i*38, sy, 34, 24)) {
                w.paintSize = sizes[i];
                InvalidateRect(g_hwnd, NULL, FALSE); return;
            }
        }
        bx += 3*38 + 4;
        if (PtIn(mx, my, bx, sy, 56, 24)) {
            w.paintEraser = !w.paintEraser;
            if (w.paintEraser) w.paintColor = 1;
            InvalidateRect(g_hwnd, NULL, FALSE); return;
        }
        if (PtIn(mx, my, bx + 56 + 8 + 26, sy, 60, 24)) {
            PaintFillWhite(w);
            InvalidateRect(g_hwnd, NULL, FALSE); return;
        }
        if (PtIn(mx, my, bx + 56 + 8 + 26 + 66, sy, 60, 24)) {
            if (PaintSaveFile(w)) OpenError(S.saved, S.saved);
            else OpenError(S.sys_error, S.sys_error_msg);
            InvalidateRect(g_hwnd, NULL, FALSE); return;
        }
        return;
    }
    int px, py;
    if (PaintMapPoint(w, mx, my, px, py)) {
        if (!w.paintDown) { SetCapture(g_hwnd); w.paintDown = true; }
        w.paintLastX = px; w.paintLastY = py;
        PaintLine(w, px, py, px, py);
        InvalidateRect(g_hwnd, NULL, FALSE);
    }
}

static void HandlePptClick(OsWindow& w, int mx, int my) {
    int ex, ey, ew, eh; PptContentRects(w, ex, ey, ew, eh);
    if (w.pptShow) {
        w.pptCur++;
        if (w.pptCur >= w.pptCount) { w.pptCur = 0; w.pptShow = false; }
        InvalidateRect(g_hwnd, NULL, FALSE);
        return;
    }
    int by = ey + 8;
    if (PtIn(mx, my, ex+6, by, 64, 24)) {
        if (w.pptCount < 16) {
            int n = w.pptCount;
            w.pptTitle[n][0] = 0;
            w.pptBody[n][0] = 0;
            w.pptBg[n] = RGB(18,80,150);
            w.pptCount++;
            w.pptCur = n;
        }
        InvalidateRect(g_hwnd, NULL, FALSE); return;
    }
    if (PtIn(mx, my, ex+76, by, 58, 24)) {
        if (w.pptCount > 1) {
            for (int i = w.pptCur; i < w.pptCount-1; i++) {
                wcscpy(w.pptTitle[i], w.pptTitle[i+1]);
                wcscpy(w.pptBody[i], w.pptBody[i+1]);
                w.pptBg[i] = w.pptBg[i+1];
            }
            w.pptCount--;
            if (w.pptCur >= w.pptCount) w.pptCur = w.pptCount-1;
        }
        InvalidateRect(g_hwnd, NULL, FALSE); return;
    }
    if (PtIn(mx, my, ex+140, by, 32, 24)) {
        w.pptCur--;
        if (w.pptCur < 0) w.pptCur = w.pptCount-1;
        InvalidateRect(g_hwnd, NULL, FALSE); return;
    }
    if (PtIn(mx, my, ex+178, by, 32, 24)) {
        w.pptCur++;
        if (w.pptCur >= w.pptCount) w.pptCur = 0;
        InvalidateRect(g_hwnd, NULL, FALSE); return;
    }
    if (PtIn(mx, my, ex+216, by, 56, 24)) {
        w.pptShow = true;
        InvalidateRect(g_hwnd, NULL, FALSE); return;
    }
    static const COLORREF slideBgs[6] = { RGB(18,80,150), RGB(150,30,30), RGB(20,130,60), RGB(180,120,10), RGB(60,50,140), RGB(40,40,40) };
    int cxx = ex+400;
    for (int i = 0; i < 6; i++) {
        if (PtIn(mx, my, cxx + i*26, by+2, 20, 20)) {
            w.pptBg[w.pptCur] = slideBgs[i];
            InvalidateRect(g_hwnd, NULL, FALSE); return;
        }
    }
    int thX = ex+8, thY = ey+46, thW = 82;
    int vis = (eh - 50) / 74;
    if (vis < 1) vis = 1;
    for (int i = 0; i < vis; i++) {
        int idx = w.pptScroll + i;
        if (idx >= w.pptCount) break;
        if (PtIn(mx, my, thX, thY + i*74, thW, 60)) {
            w.pptCur = idx;
            w.pptFocus = 0;
            InvalidateRect(g_hwnd, NULL, FALSE); return;
        }
    }
    int pvx = thX + thW + 10;
    int pvw = ex + ew - pvx - 8;
    int pvy = ey + 46;
    int pvh = eh - 50;
    if (pvw * 3 > pvh * 4) pvw = pvh * 4 / 3; else pvh = pvw * 3 / 4;
    pvx += (ex + ew - pvx - 8 - pvw) / 2;
    pvy += (eh - 50 - pvh) / 2;
    RECT titleRc = {pvx+14, pvy+14, pvx+pvw-14, pvy+14+pvh/4};
    RECT bodyRc = {pvx+16, pvy+14+pvh/4+10, pvx+pvw-16, pvy+pvh-16};
    if (PtIn(mx, my, titleRc.left, titleRc.top, titleRc.right-titleRc.left, titleRc.bottom-titleRc.top)) {
        w.pptFocus = 1;
    } else if (PtIn(mx, my, bodyRc.left, bodyRc.top, bodyRc.right-bodyRc.left, bodyRc.bottom-bodyRc.top)) {
        w.pptFocus = 2;
    } else {
        w.pptFocus = 0;
    }
    InvalidateRect(g_hwnd, NULL, FALSE);
}

static void HandleSettingsClick(OsWindow& w, int mx, int my) {
    int px = w.x + 8, py = w.y + TITLEBAR_H + 8;
    int sideW = SET_SBW, contentX = px + sideW + 12;

    for (int i = 0; i < 6; i++) {
        int iy = py + 8 + i * 34;
        if (PtIn(mx, my, w.x + 6, iy, sideW + 2, 28)) { g_settingsPage = i; InvalidateRect(g_hwnd, NULL, FALSE); return; }
    }

    if (g_settingsPage == 0) {
        int lrY = py + 28;
        for (int i = 0; i < 10; i++) {
            int bx = contentX + (i % 5) * 76;
            int by = lrY + (i / 5) * 40 + 20;
            if (PtIn(mx, my, bx, by, 70, 32)) {
                g_wallIdx = i;
                g_wallTop = WALL_COLORS2[i][0];
                g_wallBot = WALL_COLORS2[i][1];
                InvalidateRect(g_hwnd, NULL, FALSE); return;
            }
        }
        int stY = lrY + 90;
        if (PtIn(mx, my, contentX, stY + 22, 150, 26)) { g_wallStyle = 0; InvalidateRect(g_hwnd, NULL, FALSE); return; }
        if (PtIn(mx, my, contentX + 156, stY + 22, 150, 26)) { g_wallStyle = 1; InvalidateRect(g_hwnd, NULL, FALSE); return; }
        if (PtIn(mx, my, contentX, stY + 84, 200, 16)) {
            g_brightness = (mx - contentX) * 100 / 200;
            if (g_brightness < 0) g_brightness = 0; if (g_brightness > 100) g_brightness = 100;
            InvalidateRect(g_hwnd, NULL, FALSE); return;
        }
    }
    else if (g_settingsPage == 1) {
        for (int i = 0; i < 5; i++) {
            int bx = contentX + i * 92;
            if (PtIn(mx, my, bx, py + 60, 86, 40)) { g_cursorIdx = i; ApplyCursor(); InvalidateRect(g_hwnd, NULL, FALSE); return; }
        }
    }
    else if (g_settingsPage == 3) {
        if (PtIn(mx, my, contentX, py + 58, 96, 30)) { g_soundOn = true; InvalidateRect(g_hwnd, NULL, FALSE); return; }
        if (PtIn(mx, my, contentX + 104, py + 58, 96, 30)) { g_soundOn = false; InvalidateRect(g_hwnd, NULL, FALSE); return; }
    }
    else if (g_settingsPage == 4) {
        int by = py + 44;
        if (PtIn(mx, my, contentX, by, 170, 26)) { w.passFocus = true; InvalidateRect(g_hwnd, NULL, FALSE); return; }
        int sbx = contentX + 176 + (g_passSet ? 36 : 0);
        if (PtIn(mx, my, sbx, by, 70, 26)) {
            if (g_passSet) {
                PassClear();
                w.passLen = 0;
                w.passText[0] = 0;
                w.passFocus = false;
            } else {
                if (w.passLen > 0) PassSet(w.passText);
                w.passLen = 0;
                w.passText[0] = 0;
                w.passFocus = false;
            }
            InvalidateRect(g_hwnd, NULL, FALSE);
            return;
        }
    }
    else if (g_settingsPage == 5) {
        const Lang langIds[] = { LANG_EN, LANG_RU, LANG_FR, LANG_DE };
        for (int i = 0; i < 4; i++) {
            if (PtIn(mx, my, contentX, py + 44 + i * 40, 140, 32)) { g_lang = langIds[i]; UpdateAllTitles(); InvalidateRect(g_hwnd, NULL, FALSE); return; }
        }
    }
}

static void LoginSky(HDC hdc, int W, int H) {
    SYSTEMTIME st; GetLocalTime(&st);
    int hour = st.wHour;
    bool night = (hour >= 21 || hour < 3);
    bool sunrise = !night && hour >= 3 && hour < 9;
    bool day = !night && hour >= 9 && hour < 18;
    bool sunset = !night && hour >= 18 && hour < 21;
    int hor = H * 45 / 100;

    COLORREF topC, midC;
    if (night) { topC = RGB(4,6,32); midC = RGB(18,28,64); }
    else if (sunrise) { topC = RGB(22,32,84); midC = RGB(255,150,90); }
    else if (day) { topC = RGB(90,170,255); midC = RGB(170,225,255); }
    else { topC = RGB(70,30,100); midC = RGB(255,120,45); }
    int skyStop = hor > 0 ? hor : 1;
    for (int y = 0; y <= hor; y += 2) {
        int t = y * 100 / skyStop;
        int r = GetRValue(topC) + (GetRValue(midC)-GetRValue(topC))*t/100;
        int g = GetGValue(topC) + (GetGValue(midC)-GetGValue(topC))*t/100;
        int b = GetBValue(topC) + (GetBValue(midC)-GetBValue(topC))*t/100;
        HBRUSH br = CreateSolidBrush(RGB(r,g,b));
        RECT lr = {0,y,W,y+2}; FillRect(hdc,&lr,br); DeleteObject(br);
    }

    COLORREF gTop = night ? RGB(12,20,14) : (sunrise ? RGB(45,75,32) : day ? RGB(45,130,60) : RGB(42,48,22));
    COLORREF gBot = night ? RGB(3,6,4) : (sunrise ? RGB(20,42,16) : day ? RGB(20,80,35) : RGB(15,18,8));
    int gd = H - hor > 0 ? H - hor : 1;
    for (int y = hor; y < H; y += 2) {
        int t = (y - hor) * 100 / gd;
        int r = GetRValue(gTop) + (GetRValue(gBot)-GetRValue(gTop))*t/100;
        int g = GetGValue(gTop) + (GetGValue(gBot)-GetGValue(gTop))*t/100;
        int b = GetBValue(gTop) + (GetBValue(gBot)-GetBValue(gTop))*t/100;
        HBRUSH br = CreateSolidBrush(RGB(r,g,b));
        RECT lr = {0,y,W,y+2}; FillRect(hdc,&lr,br); DeleteObject(br);
    }
    HBRUSH hlb = CreateSolidBrush(night ? RGB(30,40,70) : sunrise ? RGB(255,180,120) : day ? RGB(200,240,255) : RGB(255,150,60));
    RECT hlr = {0, hor-1, W, hor+1}; FillRect(hdc,&hlr,hlb); DeleteObject(hlb);

    int anim = (g_bootFrame / 2) % 120;
    int bob = (anim < 60 ? anim : 120 - anim) - 30; /* -30..30 */

    if (night) {
        srand(7777);
        for (int i = 0; i < 110; i++) {
            int sx = rand() % W;
            int sy = rand() % (hor - 4) + 2;
            int sz = rand() % 2;
            int tw = ((g_bootFrame + i * 7) % 60) < 42 ? 255 : 110;
            HBRUSH stbr = CreateSolidBrush(RGB(tw, tw, tw));
            RECT sr = {sx, sy, sx + sz + 1, sy + sz + 1};
            FillRect(hdc, &sr, stbr); DeleteObject(stbr);
        }
        HBRUSH mb = CreateSolidBrush(RGB(235,235,240));
        Ellipse(hdc, W*78/100, H*10/100, W*78/100+46, H*10/100+46);
        DeleteObject(mb);
        HBRUSH mb2 = CreateSolidBrush(RGB(150,150,160));
        Ellipse(hdc, W*78/100+12, H*10/100+10, W*78/100+32, H*10/100+22);
        DeleteObject(mb2);
    } else {
        double p = 0.55;
        int sunR = 32;
        if (sunrise) { p = (hour - 3) / 6.0; sunR = 26; }
        else if (sunset) { p = (hour - 18) / 3.0; sunR = 30; }
        if (p < 0) p = 0; if (p > 1) p = 1;
        int sunY;
        if (sunset) sunY = hor - (int)(H * 30 / 100 * (1 - p)) + bob / 4;
        else sunY = hor - (int)(H * 30 / 100 * p) + bob / 4;
        int sunX = W / 2;
        HBRUSH glow = CreateSolidBrush(sunrise ? RGB(255,190,110) : sunset ? RGB(255,160,70) : RGB(255,235,130));
        Ellipse(hdc, sunX-sunR-10, sunY-sunR-10, sunX+sunR+10, sunY+sunR+10);
        DeleteObject(glow);
        HBRUSH core = CreateSolidBrush(sunrise ? RGB(255,220,150) : sunset ? RGB(255,200,100) : RGB(255,250,180));
        Ellipse(hdc, sunX-sunR, sunY-sunR, sunX+sunR, sunY+sunR);
        DeleteObject(core);
        int cloudCol = sunset ? RGB(255,170,120) : sunrise ? RGB(255,190,160) : RGB(255,255,255);
        for (int c = 0; c < 3; c++) {
            int cx = (c * W / 4 + g_bootFrame / 3) % (W + 120) - 60;
            int cy = H * (10 + c * 6) / 100;
            HBRUSH cb = CreateSolidBrush(cloudCol);
            Ellipse(hdc, cx, cy, cx+70, cy+20);
            Ellipse(hdc, cx+18, cy-6, cx+55, cy+14);
            DeleteObject(cb);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc; GetClientRect(hwnd, &rc);
        int W = rc.right, H = rc.bottom;
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBM = CreateCompatibleBitmap(hdc, W, H);
        SelectObject(memDC, memBM);

        if (g_boot == BOOT_LOGO) {
            HBRUSH bg = CreateSolidBrush(RGB(0,0,0));
            RECT full = {0,0,W,H}; FillRect(memDC, &full, bg); DeleteObject(bg);
            if (g_bootFrame >= 0) {
                HFONT tf = MakeFont(48, TRUE);
                HFONT old = (HFONT)SelectObject(memDC, tf);
                SetBkMode(memDC, TRANSPARENT);
                SetTextColor(memDC, RGB(255,140,0));
                RECT tr = {0, H/2-80, W, H/2-20};
                DrawTextW(memDC, L"KashmaOS 7", -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                SelectObject(memDC, old); DeleteObject(tf);
            }
            if (g_bootFrame >= 2) {
                HFONT sf = MakeFont(16, FALSE);
                HFONT old = (HFONT)SelectObject(memDC, sf);
                SetTextColor(memDC, RGB(180,180,180));
                SetBkMode(memDC, TRANSPARENT);
                RECT sr = {0, H/2+10, W, H/2+40};
                DrawTextW(memDC, L"\u0417\u0430\u0433\u0440\u0443\u0437\u043a\u0430 \u0441\u0438\u0441\u0442\u0435\u043c\u044b...", -1, &sr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                SelectObject(memDC, old); DeleteObject(sf);
            }
            if (g_bootFrame >= 4 && g_bootFrame % 2 == 0) {
                HFONT pf = MakeFont(14, FALSE);
                HFONT old = (HFONT)SelectObject(memDC, pf);
                SetTextColor(memDC, RGB(100,100,100));
                RECT pr = {0, H/2+60, W, H/2+80};
                DrawTextW(memDC, L"\u041d\u0430\u0436\u043c\u0438\u0442\u0435 \u043b\u044e\u0431\u0443\u044e \u043a\u043b\u0430\u0432\u0438\u0448\u0443...", -1, &pr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                SelectObject(memDC, old); DeleteObject(pf);
            }
        } else if (g_boot == BOOT_LOGIN) {
            LoginSky(memDC, W, H);
            HBRUSH vignR = CreateSolidBrush(RGB(0,0,0));
            RECT fullR = {0,0,W,H}; FillRect(memDC, &fullR, vignR); DeleteObject(vignR);
            HDC ovrDC = CreateCompatibleDC(memDC);
            HBITMAP ovrBM = CreateCompatibleBitmap(memDC, W, H);
            SelectObject(ovrDC, ovrBM);
            HBRUSH wbr = CreateSolidBrush(RGB(120,110,90));
            RECT wr3 = {0,0,W,H}; FillRect(ovrDC, &wr3, wbr); DeleteObject(wbr);
            BLENDFUNCTION bf = {AC_SRC_OVER, 0, 80, 0};
            AlphaBlend(memDC, 0, 0, W, H, ovrDC, 0, 0, W, H, bf);
            DeleteObject(ovrBM); DeleteDC(ovrDC);
            HFONT tf = MakeFont(40, TRUE);
            HFONT old = (HFONT)SelectObject(memDC, tf);
            SetBkMode(memDC, TRANSPARENT);
            SetTextColor(memDC, RGB(0,0,0));
            RECT trS = {0, H/2-118, W, H/2-58};
            DrawTextW(memDC, L"KashmaOS 7", -1, &trS, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SetTextColor(memDC, RGB(255,150,30));
            RECT tr = {0, H/2-120, W, H/2-60};
            DrawTextW(memDC, L"KashmaOS 7", -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(memDC, old); DeleteObject(tf);

            HFONT wf = MakeFont(20, FALSE);
            old = (HFONT)SelectObject(memDC, wf);
            SetTextColor(memDC, RGB(0,0,0));
            RECT wr2s = {0, H/2-48, W, H/2-18};
            DrawTextW(memDC, L"\u0414\u043e\u0431\u0440\u043e \u043f\u043e\u0436\u0430\u043b\u043e\u0432\u0430\u0442\u044c \u0432 KashmaOS!", -1, &wr2s, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SetTextColor(memDC, RGB(255,255,255));
            RECT wr2 = {0, H/2-50, W, H/2-20};
            DrawTextW(memDC, L"\u0414\u043e\u0431\u0440\u043e \u043f\u043e\u0436\u0430\u043b\u043e\u0432\u0430\u0442\u044c \u0432 KashmaOS!", -1, &wr2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(memDC, old); DeleteObject(wf);

            int bx = W/2-100, by = H/2+10, bw = 200, bh = 30;
            if (g_passSet) {
                HFONT hnt = MakeFont(12, FALSE);
                old = (HFONT)SelectObject(memDC, hnt);
                SetTextColor(memDC, RGB(0,0,0));
                RECT hrS = {bx+1, by-2, bx+bw+1, by+18};
                DrawTextW(memDC, S.pass_login_hint, -1, &hrS, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                SetTextColor(memDC, RGB(255,255,255));
                RECT hr = {bx, by-4, bx+bw, by+16};
                DrawTextW(memDC, S.pass_login_hint, -1, &hr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                SelectObject(memDC, old); DeleteObject(hnt);
            }
            HBRUSH ibg = CreateSolidBrush(RGB(255,255,255));
            RECT ir = {bx, by, bx+bw, by+bh}; FillRect(memDC, &ir, ibg); DeleteObject(ibg);
            HPEN ip = CreatePen(PS_SOLID, g_passSet ? 2 : 1, g_passSet ? RGB(200,80,0) : RGB(150,150,150));
            SelectObject(memDC, ip); Rectangle(memDC, bx, by, bx+bw, by+bh); DeleteObject(ip);
            HFONT inf = MakeFont(14, FALSE);
            old = (HFONT)SelectObject(memDC, inf);
            SetTextColor(memDC, RGB(0,0,0));
            RECT itr = {bx+6, by, bx+bw-6, by+bh};
            wchar_t passMask[64];
            if (g_passSet) {
                for (int i = 0; i < g_loginLen && i < 63; i++) passMask[i] = L'\u25cf';
                passMask[g_loginLen > 63 ? 63 : g_loginLen] = 0;
                DrawTextW(memDC, passMask, -1, &itr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            } else {
                DrawTextW(memDC, g_loginInput, -1, &itr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            }
            if (g_bootFrame % 4 < 2) {
                SIZE sz; GetTextExtentPoint32W(memDC, g_passSet ? passMask : g_loginInput, g_loginLen, &sz);
                HPEN cur = CreatePen(PS_SOLID, 1, RGB(0,0,0));
                SelectObject(memDC, cur);
                MoveToEx(memDC, bx+6+sz.cx, by+4, NULL); LineTo(memDC, bx+6+sz.cx, by+bh-4);
                DeleteObject(cur);
            }
            SelectObject(memDC, old); DeleteObject(inf);
            if (g_loginErr) {
                HFONT ef = MakeFont(12, FALSE);
                old = (HFONT)SelectObject(memDC, ef);
                SetTextColor(memDC, RGB(200,0,0));
                RECT er = {bx, by+bh+4, bx+bw, by+bh+24};
                DrawTextW(memDC, S.pass_wrong, -1, &er, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                SelectObject(memDC, old); DeleteObject(ef);
            }

            int lbx = W/2-60, lby = H/2+55, lbw = 120, lbh = 32;
            HBRUSH lbr = CreateSolidBrush(RGB(200,80,0));
            RECT lr2 = {lbx, lby, lbx+lbw, lby+lbh}; FillRect(memDC, &lr2, lbr); DeleteObject(lbr);
            HPEN lp = CreatePen(PS_SOLID, 1, RGB(150,50,0));
            SelectObject(memDC, lp); Rectangle(memDC, lbx, lby, lbx+lbw, lby+lbh); DeleteObject(lp);
            HFONT lf = MakeFont(14, TRUE);
            old = (HFONT)SelectObject(memDC, lf);
            SetTextColor(memDC, RGB(255,255,255));
            RECT ltr = {lbx, lby, lbx+lbw, lby+lbh};
            DrawTextW(memDC, L"\u0412\u043e\u0439\u0442\u0438", -1, &ltr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(memDC, old); DeleteObject(lf);
        } else {
            if (g_wallStyle == 1) {
                for (int x = 0; x < W; x += 2) {
                    int t = x * 100 / W;
                    int r = GetRValue(g_wallTop) + (GetRValue(g_wallBot) - GetRValue(g_wallTop)) * t / 100;
                    int g = GetGValue(g_wallTop) + (GetGValue(g_wallBot) - GetGValue(g_wallTop)) * t / 100;
                    int b = GetBValue(g_wallTop) + (GetBValue(g_wallBot) - GetBValue(g_wallTop)) * t / 100;
                    HBRUSH lineBr2 = CreateSolidBrush(RGB(r, g, b));
                    RECT lr2 = {x, 0, x+2, H}; FillRect(memDC, &lr2, lineBr2); DeleteObject(lineBr2);
                }
            } else {
                for (int y = 0; y < H; y += 2) {
                    int t = y * 100 / H;
                    int r = GetRValue(g_wallTop) + (GetRValue(g_wallBot) - GetRValue(g_wallTop)) * t / 100;
                    int g = GetGValue(g_wallTop) + (GetGValue(g_wallBot) - GetGValue(g_wallTop)) * t / 100;
                    int b = GetBValue(g_wallTop) + (GetBValue(g_wallBot) - GetBValue(g_wallTop)) * t / 100;
                    HBRUSH lineBr = CreateSolidBrush(RGB(r, g, b));
                    RECT lr = {0, y, W, y+2}; FillRect(memDC, &lr, lineBr); DeleteObject(lineBr);
                }
            }
            DrawDesktop(memDC, W, H);
            for (int i = 0; i < g_winCount; i++)
                if (g_windows[i].open && !g_windows[i].minimized) DrawWindow(memDC, g_windows[i]);
            DrawTaskbar(memDC, W, H);
            if (g_startOpen) DrawStartMenu(memDC, W, H);

            if (g_errorOpen) {
                HBRUSH ovr = CreateSolidBrush(RGB(0,0,0));
                RECT ovrR = {0,0,W,H}; FillRect(memDC, &ovrR, ovr); DeleteObject(ovr);
                int dw = 360, dh = 160;
                int dx = (W-dw)/2, dy = (H-dh)/2;
                HBRUSH dbr = CreateSolidBrush(RGB(220,220,220));
                RECT dr = {dx, dy, dx+dw, dy+dh}; FillRect(memDC, &dr, dbr); DeleteObject(dbr);
                HPEN dp = CreatePen(PS_SOLID, 2, RGB(0,0,0));
                SelectObject(memDC, dp); Rectangle(memDC, dx, dy, dx+dw, dy+dh); DeleteObject(dp);

                HBRUSH tbr = CreateSolidBrush(RGB(200,80,0));
                RECT tr2 = {dx, dy, dx+dw, dy+28}; FillRect(memDC, &tr2, tbr); DeleteObject(tbr);
                HFONT tf = MakeFont(13, TRUE);
                HFONT old = (HFONT)SelectObject(memDC, tf);
                SetBkMode(memDC, TRANSPARENT);
                SetTextColor(memDC, RGB(255,255,255));
                RECT ttr = {dx+8, dy, dx+dw-8, dy+28};
                DrawTextW(memDC, g_errorTitle, -1, &ttr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                SelectObject(memDC, old); DeleteObject(tf);

                HICON erri = LoadIcon(NULL, IDI_ERROR);
                DrawIconEx(memDC, dx+16, dy+44, erri, 32, 32, 0, NULL, DI_NORMAL);

                HFONT mf = MakeFont(14, FALSE);
                old = (HFONT)SelectObject(memDC, mf);
                SetTextColor(memDC, RGB(0,0,0));
                RECT mr = {dx+60, dy+40, dx+dw-16, dy+100};
                DrawTextW(memDC, g_errorMsg, -1, &mr, DT_LEFT | DT_TOP | DT_WORDBREAK);
                SelectObject(memDC, old); DeleteObject(mf);

                int obx = dx+dw/2-40, oby = dy+dh-40, obw = 80, obh = 28;
                HBRUSH obr = CreateSolidBrush(RGB(200,80,0));
                RECT obr2 = {obx, oby, obx+obw, oby+obh}; FillRect(memDC, &obr2, obr); DeleteObject(obr);
                HPEN op = CreatePen(PS_SOLID, 1, RGB(150,50,0));
                SelectObject(memDC, op); Rectangle(memDC, obx, oby, obx+obw, oby+obh); DeleteObject(op);
                HFONT of2 = MakeFont(13, TRUE);
                old = (HFONT)SelectObject(memDC, of2);
                SetTextColor(memDC, RGB(255,255,255));
                RECT otr = {obx, oby, obx+obw, oby+obh};
                DrawTextW(memDC, L"OK", -1, &otr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                SelectObject(memDC, old); DeleteObject(of2);
            }
            if (g_alertOpen) {
                DWORD elapsed = GetTickCount() - g_alertTime;
                if (elapsed > 4000) { g_alertOpen = false; }
                else {
                    COLORREF bg = g_alertSeverity == 2 ? RGB(140,0,0) : g_alertSeverity == 1 ? RGB(180,60,0) : RGB(160,140,0);
                    HBRUSH abr = CreateSolidBrush(bg);
                    RECT ar = {0, 0, W, H}; FillRect(memDC, &ar, abr); DeleteObject(abr);
                    HPEN ap = CreatePen(PS_SOLID, 6, RGB(255,0,0));
                    HPEN ap2 = CreatePen(PS_SOLID, 2, RGB(255,200,0));
                    SelectObject(memDC, ap); Rectangle(memDC, 6, 6, W-6, H-6);
                    SelectObject(memDC, ap2); Rectangle(memDC, 16, 16, W-16, H-16);
                    DeleteObject(ap); DeleteObject(ap2);
                    SetBkMode(memDC, TRANSPARENT);
                    HFONT af1 = MakeFont(56, TRUE);
                    HFONT old = (HFONT)SelectObject(memDC, af1);
                    SetTextColor(memDC, RGB(255,255,255));
                    RECT ar1 = {30, 40, W-30, 110};
                    wchar_t titleLine[128];
                    swprintf(titleLine, 128, L"\u26A0 %s \u26A0", S.alert_title);
                    DrawTextW(memDC, titleLine, -1, &ar1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    SelectObject(memDC, old); DeleteObject(af1);
                    HFONT af2 = MakeFont(30, FALSE);
                    old = (HFONT)SelectObject(memDC, af2);
                    SetTextColor(memDC, RGB(255,255,200));
                    wchar_t l1[512];
                    swprintf(l1, 512, L"%s %s", S.alert_file, g_alertFile);
                    RECT ar2 = {40, 120, W-40, 165};
                    DrawTextW(memDC, l1, -1, &ar2, DT_LEFT | DT_SINGLELINE);
                    wchar_t l2[512];
                    swprintf(l2, 512, L"%s %s", S.alert_layer, g_alertLayer);
                    RECT ar3 = {40, 170, W-40, 215};
                    DrawTextW(memDC, l2, -1, &ar3, DT_LEFT | DT_SINGLELINE);
                    SetTextColor(memDC, RGB(255,100,100));
                    RECT ar4 = {40, 220, W-40, 265};
                    DrawTextW(memDC, S.alert_restored, -1, &ar4, DT_LEFT | DT_SINGLELINE);
                    SelectObject(memDC, old); DeleteObject(af2);
                    HFONT af3 = MakeFont(20, FALSE);
                    old = (HFONT)SelectObject(memDC, af3);
                    SetTextColor(memDC, RGB(255,255,150));
                    wchar_t tline[256];
                    SYSTEMTIME st; GetLocalTime(&st);
                    swprintf(tline, 256, L"%02d.%02d.%04d %02d:%02d:%02d | %s", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond,
                        g_alertSeverity == 2 ? S.alert_crit : g_alertSeverity == 1 ? S.alert_high : S.alert_med);
                    RECT ar5 = {40, 270, W-40, 300};
                    DrawTextW(memDC, tline, -1, &ar5, DT_LEFT | DT_SINGLELINE);
                    SetTextColor(memDC, RGB(200,200,200));
                    RECT ar6 = {40, 305, W-40, 335};
                    DrawTextW(memDC, S.alert_languages, -1, &ar6, DT_LEFT | DT_SINGLELINE);
                    SelectObject(memDC, old); DeleteObject(af3);
                }
            }
            if (g_editMode != 0) DrawEditDialog(memDC, W, H);
            if (g_ctxOpen) DrawContextMenu(memDC);
        }

        BitBlt(hdc, 0, 0, W, H, memDC, 0, 0, SRCCOPY);
        DeleteObject(memBM); DeleteDC(memDC);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_ERASEBKGND: return 1;
    case WM_TIMER:
        SyncWebView();
        if (wp == 1) ClampCursorToOs();
        if (wp == 3) {
            bool anyDrau = false;
            for (int i = 0; i < g_winCount; i++) {
                OsWindow& d = g_windows[i];
                if (!d.open || d.minimized || d.app != APP_DRAU) continue;
                anyDrau = true;
                if (d.drauSpin) { d.drauRx += 0.018f; d.drauRy += 0.028f; }
                RECT dr = {d.x, d.y, d.x+d.w, d.y+d.h};
                InvalidateRect(hwnd, &dr, FALSE);
            }
            if (anyDrau) break;
            return 0;
        }
        g_secCount++;
        if (g_secCount >= g_nextCheck) { RunSecurityCheck(); g_nextCheck = g_secCount + 2 + (g_secCount % 3); }
        if (g_boot == BOOT_LOGO) {
            g_bootFrame++;
            if (g_bootFrame > 5) { g_boot = BOOT_LOGIN; g_bootFrame = 0; }
        } else if (g_boot == BOOT_LOGIN) {
            g_bootFrame++;
        } else {
            InvalidateRect(hwnd, NULL, FALSE);
        }
        InvalidateRect(hwnd, NULL, FALSE); return 0;

    case WM_CHAR: {
        if (g_editMode != 0) {
            if (wp == '\r') {
                ApplyEditDialog();
            } else if (wp == 27) {
                g_editMode = 0;
            } else if (wp == '\b') {
                if (g_editLen > 0) g_editLen--;
            } else if (wp >= 32 && g_editLen < 255) {
                g_editBuf[g_editLen++] = (wchar_t)wp;
            }
            g_editBuf[g_editLen] = 0;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        if (g_boot == BOOT_LOGIN) {
            if (wp == '\r') {
                if (g_passSet) {
                    if (PassCheck(g_loginInput)) {
                        g_boot = BOOT_DESKTOP;
                        InvalidateRect(hwnd, NULL, FALSE);
                        return 0;
                    }
                    g_loginErr = true;
                    g_loginLen = 0;
                    g_loginInput[0] = 0;
                    InvalidateRect(hwnd, NULL, FALSE);
                    return 0;
                }
                g_boot = BOOT_DESKTOP;
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
            if (wp == '\b') {
                if (g_loginLen > 0) g_loginLen--;
                g_loginErr = false;
            } else if (wp >= 32 && g_loginLen < 60) {
                g_loginInput[g_loginLen++] = (wchar_t)wp;
                g_loginErr = false;
            }
            g_loginInput[g_loginLen] = 0;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        if (g_activeWin >= 0 && g_windows[g_activeWin].open && g_windows[g_activeWin].app == APP_FILES &&
            g_windows[g_activeWin].filesSearch) {
            OsWindow& w = g_windows[g_activeWin];
            if (wp == '\r') {
                DoSearch(w);
            } else if (wp == 27) {
                w.filesSearch = false; w.sCount = 0;
            } else if (wp == '\b') {
                if ((int)wcslen(w.searchQ) > 0) w.searchQ[wcslen(w.searchQ)-1] = 0;
                DoSearch(w);
            } else if (wp >= 32 && wcslen(w.searchQ) < 60) {
                int wl = (int)wcslen(w.searchQ);
                w.searchQ[wl] = (wchar_t)wp;
                w.searchQ[wl+1] = 0;
                DoSearch(w);
            }
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        if (g_activeWin >= 0 && g_windows[g_activeWin].open && g_windows[g_activeWin].app == APP_INET &&
            g_windows[g_activeWin].inetPassOpen) {
            OsWindow& w = g_windows[g_activeWin];
            if (wp == '\r') {
                if (w.inetPassLen > 0 && !w.inetBusy) {
                    w.inetBusy = true;
                    _beginthreadex(NULL, 0, InetConnectThread, &w, 0, NULL);
                }
                w.inetPassOpen = false;
            } else if (wp == 27) {
                w.inetPassOpen = false;
                w.inetPass[0] = 0;
                w.inetPassLen = 0;
            } else if (wp == '\b') {
                if (w.inetPassLen > 0) w.inetPassLen--;
            } else if (wp >= 32 && w.inetPassLen < 63) {
                w.inetPass[w.inetPassLen++] = (wchar_t)wp;
            }
            w.inetPass[w.inetPassLen] = 0;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        if (g_activeWin >= 0 && g_windows[g_activeWin].open && g_windows[g_activeWin].app == APP_NOTEPAD) {
            OsWindow& w = g_windows[g_activeWin];
            if (!w.notepadRO) {
                if (wp == '\b') {
                    if (w.notepadLen > 0) w.notepadLen--;
                } else if (wp >= 32 && w.notepadLen < 4095) {
                    w.notepad[w.notepadLen++] = (char)wp;
                }
                w.notepad[w.notepadLen] = 0;
            }
            InvalidateRect(hwnd, NULL, FALSE);
        }
        if (g_activeWin >= 0 && g_windows[g_activeWin].open && g_windows[g_activeWin].app == APP_SETTINGS &&
            g_windows[g_activeWin].passFocus) {
            OsWindow& w = g_windows[g_activeWin];
            if (wp == '\r') {
                if (w.passLen > 0) PassSet(w.passText);
                w.passLen = 0;
                w.passText[0] = 0;
                w.passFocus = false;
            } else if (wp == 27) {
                w.passLen = 0;
                w.passText[0] = 0;
                w.passFocus = false;
            } else if (wp == '\b') {
                if (w.passLen > 0) w.passLen--;
            } else if (wp >= 32 && w.passLen < 64) {
                w.passText[w.passLen++] = (wchar_t)wp;
            }
            w.passText[w.passLen] = 0;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        if (g_activeWin >= 0 && g_windows[g_activeWin].open && g_windows[g_activeWin].app == APP_PPT &&
            (g_windows[g_activeWin].pptFocus == 1 || g_windows[g_activeWin].pptFocus == 2) &&
            !g_windows[g_activeWin].pptShow) {
            OsWindow& w = g_windows[g_activeWin];
            wchar_t* buf = (w.pptFocus == 1) ? w.pptTitle[w.pptCur] : w.pptBody[w.pptCur];
            int maxLen = (w.pptFocus == 1) ? 63 : 255;
            int len = (int)wcslen(buf);
            if (wp == '\r' && w.pptFocus == 2) {
                if (len < maxLen) { buf[len] = '\n'; buf[len+1] = 0; }
            } else if (wp == '\r') {
                w.pptFocus = 0;
            } else if (wp == 27) {
                w.pptFocus = 0;
            } else if (wp == '\b') {
                if (len > 0) buf[len-1] = 0;
            } else if (wp >= 32 && len < maxLen) {
                buf[len] = (wchar_t)wp;
                buf[len+1] = 0;
            }
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        if (g_activeWin >= 0 && g_windows[g_activeWin].open && g_windows[g_activeWin].app == APP_CODE) {
            OsWindow& w = g_windows[g_activeWin];
            if (wp == '\b') {
                if (w.codeLen > 0) w.codeLen--;
            } else if (wp >= 32 && w.codeLen < 16383) {
                w.code[w.codeLen++] = (char)wp;
            }
            w.code[w.codeLen] = 0;
            w.codeModified = true;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        if (g_activeWin >= 0 && g_windows[g_activeWin].open && g_windows[g_activeWin].app == APP_KML &&
            g_windows[g_activeWin].kmlFocused > 0) {
            OsWindow& w = g_windows[g_activeWin];
            char* val = NULL; int maxLen = 15;
            if (w.kmlHostMode) {
                val = (w.kmlFocused == 1) ? w.kmlGamePort : w.kmlListenPort;
            } else {
                if (w.kmlFocused == 1) { val = w.kmlHostIp; maxLen = 60; }
                else if (w.kmlFocused == 2) { val = w.kmlHostPort; }
                else { val = w.kmlLocalPort; }
            }
            if (wp == '\r') {
                w.kmlFocused = 0;
            } else if (wp == '\b') {
                int l = (int)strlen(val);
                if (l > 0) val[l-1] = 0;
            } else if (wp >= 32 && (int)strlen(val) < maxLen) {
                int l = (int)strlen(val);
                val[l] = (char)wp; val[l+1] = 0;
            }
            InvalidateRect(hwnd, NULL, FALSE);
        }
        if (g_activeWin >= 0 && g_windows[g_activeWin].open && g_windows[g_activeWin].app == APP_UPD) {
            OsWindow& w = g_windows[g_activeWin];
            if (wp == '\r') {
                if (!g_updBusy) {
                    _beginthreadex(NULL, 0, UpdCheckThread, &w, 0, NULL);
                    InterlockedExchange(&g_updBusy, 1);
                }
            } else if (wp == '\b') {
                int l = (int)strlen(w.updUrl);
                if (l > 0) w.updUrl[l-1] = 0;
            } else if (wp >= 32 && (int)strlen(w.updUrl) < 510) {
                int l = (int)strlen(w.updUrl);
                w.updUrl[l] = (char)wp; w.updUrl[l+1] = 0;
            }
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    }

    case WM_LBUTTONDBLCLK: {
        int mx = (int)(short)LOWORD(lp), my = (int)(short)HIWORD(lp);
        for (int i = g_winCount - 1; i >= 0; i--) {
            OsWindow& w = g_windows[i];
            if (!w.open || w.minimized) continue;
            if (HitTest(mx, my, w) >= 0 && w.app == APP_FILES) {
                g_activeWin = i;
                int fidx = -1;
                if (FilesHit(w, mx, my, fidx) && fidx >= 0) {
                    if (w.filesSearch) {
                        OpenSearchResult(w, fidx);
                    } else if (w.filesIsDir[fidx]) {
                        if (strlen(w.filesPath) + 1 + strlen(w.filesList[fidx]) < 250) {
                            strcat(w.filesPath, "\\");
                            strcat(w.filesPath, w.filesList[fidx]);
                            w.filesScroll = 0; w.filesSel = -1;
                        }
                    } else {
                        char full[520];
                        wsprintfA(full, "%s\\%s", w.filesPath, w.filesList[fidx]);
                        if (IsSystemPath(full)) OpenNotepad(full);
                        else if (strend(full, ".cpp")) {
                            OsWindow* nw = NULL;
                            for (int k = 0; k < g_winCount; k++)
                                if (g_windows[k].app == APP_CODE && g_windows[k].open) { nw = &g_windows[k]; break; }
                            if (!nw) { OpenWindow(APP_CODE); nw = &g_windows[g_winCount-1]; }
                            if (nw) {
                                strcpy(nw->codeFile, full);
                                HANDLE hf = CreateFileA(full, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
                                if (hf != INVALID_HANDLE_VALUE) {
                                    DWORD sz2 = GetFileSize(hf, NULL); if (sz2 > 16383) sz2 = 16383;
                                    DWORD rd; ReadFile(hf, nw->code, sz2, &rd, NULL);
                                    nw->code[rd] = 0; nw->codeLen = rd; CloseHandle(hf);
                                }
                                nw->codeModified = false; nw->minimized = false;
                                g_activeWin = (int)(nw - g_windows);
                            }
                        } else if (strend(full, ".kcpp")) {
                            OsWindow* nw = NULL;
                            for (int k = 0; k < g_winCount; k++)
                                if (g_windows[k].app == APP_KCPP && g_windows[k].open) { nw = &g_windows[k]; break; }
                            if (!nw) { OpenWindow(APP_KCPP); nw = &g_windows[g_winCount-1]; }
                            if (nw) { strcpy(nw->kcppPath, full); nw->minimized = false; g_activeWin = (int)(nw - g_windows); }
                        } else if (strend(full, ".drau")) {
                            char dfp[520]; strcpy(dfp, full);
                            OpenDrau(dfp);
                        } else {
                            OpenNotepad(full);
                        }
                    }
                }
                InvalidateRect(hwnd, NULL, FALSE); return 0;
            }
        }
        for (int i = 0; i < ICON_COUNT; i++) {
            if (PtIn(mx, my, g_icons[i].x, g_icons[i].y, 70, 78)) {
                OpenWindow(g_icons[i].app);
                InvalidateRect(hwnd, NULL, FALSE); return 0;
            }
        }
        for (int i = 0; i < g_deskCount; i++) {
            int dx = g_deskFiles[i].x, dy = g_deskFiles[i].y;
            if (PtIn(mx, my, dx, dy, 70, 78)) {
                OpenDeskFile(i);
                InvalidateRect(hwnd, NULL, FALSE); return 0;
            }
        }
        return 0;
    }

    case WM_RBUTTONDOWN: {
        int mx = (int)(short)LOWORD(lp), my = (int)(short)HIWORD(lp);
        RECT rc; GetClientRect(hwnd, &rc);
        int W = rc.right, H = rc.bottom;
        if (g_boot != BOOT_DESKTOP || g_errorOpen || g_editMode != 0 || g_alertOpen) { g_ctxOpen = false; return 0; }
        int taskY = H - TASKBAR_H;
        g_ctxOpen = false;
        for (int i = g_winCount - 1; i >= 0; i--) {
            OsWindow& w = g_windows[i];
            if (!w.open || w.minimized) continue;
            int ht = HitTest(mx, my, w);
            if (ht >= 0) {
                g_activeWin = i;
                if (w.app == APP_FILES) {
                    int fidx = -1;
                    FilesHit(w, mx, my, fidx);
                    if (w.filesSearch) {
                        g_ctxOpen = false;
                        if (fidx >= 0 && fidx < w.sCount) w.filesSel = fidx;
                        InvalidateRect(hwnd, NULL, FALSE);
                        return 0;
                    }
                    g_ctxWin = i;
                    g_ctxX = mx; g_ctxY = my;
                    if (fidx >= 0) {
                        w.filesSel = fidx;
                        CtxAction acts[5] = {CTX_OPEN, CTX_COPY, CTX_PASTE, CTX_DELETE, CTX_RENAME};
                        for (int k = 0; k < 5; k++) g_ctxActions[k] = acts[k];
                        g_ctxCount = 5;
                    } else {
                        CtxAction acts[11] = {CTX_NEWFOLDER, CTX_NEWTXT, CTX_NEWCPP, CTX_NEWKCPP, CTX_NEWPY, CTX_NEWHTML, CTX_NEWJSON, CTX_NEWMD, CTX_NEWBAT, CTX_NEWASM, CTX_PASTE};
                        for (int k = 0; k < 11; k++) g_ctxActions[k] = acts[k];
                        g_ctxCount = 11;
                    }
                    g_ctxOpen = true;
                } else {
                    g_ctxOpen = false;
                }
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
        }
        for (int i = 0; i < g_deskCount; i++) {
            int dx = g_deskFiles[i].x, dy = g_deskFiles[i].y;
            if (PtIn(mx, my, dx, dy, 70, 78)) {
                g_deskSel = i;
                g_deskCtxSel = i;
                g_ctxWin = -1;
                g_ctxX = mx; g_ctxY = my;
                CtxAction acts[5] = {CTX_OPEN, CTX_COPY, CTX_PASTE, CTX_DELETE, CTX_RENAME};
                for (int k = 0; k < 5; k++) g_ctxActions[k] = acts[k];
                g_ctxCount = 5;
                g_ctxOpen = true;
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
        }
        if (my < taskY) {
            g_ctxWin = -1;
            g_deskCtxSel = -1;
            g_ctxX = mx; g_ctxY = my;
            CtxAction acts[12] = {CTX_OPENFILES, CTX_NEWFOLDER, CTX_NEWTXT, CTX_NEWCPP, CTX_NEWKCPP, CTX_NEWPY, CTX_NEWHTML, CTX_NEWJSON, CTX_NEWMD, CTX_NEWBAT, CTX_NEWASM, CTX_PASTE};
                for (int k = 0; k < 12; k++) g_ctxActions[k] = acts[k];
                g_ctxCount = 12;
            g_ctxOpen = true;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    }

    case WM_LBUTTONDOWN: {
        int mx = (int)(short)LOWORD(lp), my = (int)(short)HIWORD(lp);
        RECT rc; GetClientRect(hwnd, &rc);
        int W = rc.right, H = rc.bottom;
        int taskY = H - TASKBAR_H;

        if (g_boot == BOOT_LOGIN) {
            int lbx = W/2-60, lby = H/2+55, lbw = 120, lbh = 32;
            if (mx >= lbx && mx <= lbx+lbw && my >= lby && my <= lby+lbh) {
                if (g_passSet) {
                    if (PassCheck(g_loginInput)) {
                        g_boot = BOOT_DESKTOP;
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    g_loginErr = true;
                    g_loginLen = 0;
                    g_loginInput[0] = 0;
                    InvalidateRect(hwnd, NULL, FALSE);
                } else {
                    g_boot = BOOT_DESKTOP;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            return 0;
        }
        if (g_errorOpen) {
            int dw = 360, dh = 160;
            int dx = (W-dw)/2, dy = (H-dh)/2;
            int obx = dx+dw/2-40, oby = dy+dh-40, obw = 80, obh = 28;
            if (mx >= obx && mx <= obx+obw && my >= oby && my <= oby+obh) {
                g_errorOpen = false;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }
        if (g_alertOpen) {
            g_alertOpen = false;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        if (g_editMode != 0) {
            int dw = 380, dh = 120;
            int dx = (W-dw)/2, dy = (H-dh)/2;
            if (PtIn(mx, my, dx+dw/2-90, dy+76, 80, 28)) {
                /* OK: apply rename or kcpp path */
                ApplyEditDialog();
                InvalidateRect(hwnd, NULL, FALSE);
            } else if (PtIn(mx, my, dx+dw/2+10, dy+76, 80, 28)) {
                g_editMode = 0;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }
        if (g_ctxOpen) {
            int mw = 180, mh = g_ctxCount * 24 + 8;
            int mx2 = g_ctxX, my2 = g_ctxY;
            if (mx2 + mw > W) mx2 = W - mw;
            if (my2 + mh > H - TASKBAR_H) my2 = H - TASKBAR_H - mh;
            if (PtIn(mx, my, mx2, my2, mw, mh)) {
                int item = (my - my2 - 4) / 24;
                if (item >= 0 && item < g_ctxCount) {
                    CtxAction a = g_ctxActions[item];
                    char tpath[520];
                    OsWindow* tw = NULL;
                    if (g_ctxWin >= 0) { tw = &g_windows[g_ctxWin]; strcpy(tpath, tw->filesPath); }
                    else wsprintfA(tpath, "%s\\Users\\Default\\Desktop", G_DRIVE_ROOT);
                    switch (a) {
                    case CTX_COPY:
                        if (g_ctxWin >= 0 && tw && tw->filesSel >= 0) {
                            wsprintfA(g_copyPath, "%s\\%s", tw->filesPath, tw->filesList[tw->filesSel]);
                            g_copyIsDir = tw->filesIsDir[tw->filesSel];
                        } else if (g_deskCtxSel >= 0) {
                            DeskFullPath(g_deskCtxSel, g_copyPath);
                            g_copyIsDir = g_deskFiles[g_deskCtxSel].isDir;
                        }
                        break;
                    case CTX_OPENFILES: {
                        char deskp[520];
                        wsprintfA(deskp, "%s\\Users\\Default\\Desktop", G_DRIVE_ROOT);
                        OpenFilesAt(deskp);
                        break;
                    }
                    case CTX_PASTE: {
                        if (g_copyPath[0] && !IsSystemPath(tpath)) {
                            char dst[520];
                            const char* name = g_copyPath;
                            const char* s = strrchr(g_copyPath, '\\');
                            if (s) name = s + 1;
                            if (g_copyIsDir) {
                                wsprintfA(dst, "%s\\%s", tpath, name);
                                if (GetFileAttributesA(dst) == INVALID_FILE_ATTRIBUTES)
                                    CopyFolder(g_copyPath, dst);
                            } else {
                                wsprintfA(dst, "%s\\%s", tpath, name);
                                int n2 = 1;
                                while (GetFileAttributesA(dst) != INVALID_FILE_ATTRIBUTES) {
                                    wsprintfA(dst, "%s\\Copy %d of %s", tpath, n2, name);
                                    n2++;
                                }
                                CopyFileA(g_copyPath, dst, FALSE);
                            }
                        } else if (IsSystemPath(tpath)) { OpenError(S.sys_error, S.sys_error_msg); }
                        break;
                    }
                    case CTX_DELETE:
                        if (tw && tw->filesSel >= 0) {
                            char full[520];
                            wsprintfA(full, "%s\\%s", tw->filesPath, tw->filesList[tw->filesSel]);
                            if (IsSystemPath(tw->filesPath)) OpenError(S.sys_error, S.sys_error_msg);
                            else if (tw->filesIsDir[tw->filesSel]) {
                                char pat[520]; wsprintfA(pat, "%s\\*", full);
                                WIN32_FIND_DATAA fd2; HANDLE h2 = FindFirstFileA(pat, &fd2);
                                bool empty = (h2 == INVALID_HANDLE_VALUE);
                                if (!empty) {
                                    do {
                                        if (strcmp(fd2.cFileName,".")==0||strcmp(fd2.cFileName,"..")==0) continue;
                                        empty = false; break;
                                    } while(FindNextFileA(h2,&fd2));
                                    FindClose(h2);
                                }
                                if (empty) RemoveDirectoryA(full); else OpenError(S.sys_error, S.sys_error_msg);
                            } else DeleteFileA(full);
                            tw->filesSel = -1;
                        } else if (g_deskCtxSel >= 0) {
                            char full[520];
                            DeskFullPath(g_deskCtxSel, full);
                            if (g_deskFiles[g_deskCtxSel].isDir) RemoveDirectoryA(full);
                            else DeleteFileA(full);
                            ScanDesktopFiles();
                            if (g_deskSel == g_deskCtxSel) g_deskSel = -1;
                            g_deskCtxSel = -1;
                        }
                        break;
                    case CTX_RENAME:
                        if (g_ctxWin >= 0 && tw && tw->filesSel >= 0 && !IsSystemPath(tw->filesPath)) {
                            MultiByteToWideChar(CP_ACP, 0, tw->filesList[tw->filesSel], -1, g_editBuf, 260);
                            g_editLen = (int)wcslen(g_editBuf);
                            g_editWin = g_ctxWin; g_editIdx = tw->filesSel; g_editMode = 1;
                        } else if (g_deskCtxSel >= 0) {
                            MultiByteToWideChar(CP_ACP, 0, g_deskFiles[g_deskCtxSel].name, -1, g_editBuf, 260);
                            g_editLen = (int)wcslen(g_editBuf);
                            g_editWin = -1; g_editIdx = -1; g_editMode = 1;
                        }
                        break;
                    case CTX_NEWFOLDER: {
                        if (!IsSystemPath(tpath)) {
                            char newPath[520];
                            for (int n2 = 1; n2 < 100; n2++) {
                                wsprintfA(newPath, "%s\\Folder%d", tpath, n2);
                                if (GetFileAttributesA(newPath) == INVALID_FILE_ATTRIBUTES) {
                                    CreateDirectoryA(newPath, NULL); break;
                                }
                            }
                        } else OpenError(S.sys_error, S.sys_error_msg);
                        break;
                    }
                    case CTX_NEWTXT: {
                        if (!IsSystemPath(tpath)) {
                            char newPath[520];
                            for (int n2 = 1; n2 < 100; n2++) {
                                wsprintfA(newPath, "%s\\Document%d.txt", tpath, n2);
                                if (GetFileAttributesA(newPath) == INVALID_FILE_ATTRIBUTES) {
                                    WriteFileA_(newPath, "");
                                    break;
                                }
                            }
                        } else OpenError(S.sys_error, S.sys_error_msg);
                        break;
                    }
                    case CTX_NEWCPP: {
                        if (!IsSystemPath(tpath)) {
                            char newPath[520];
                            for (int n2 = 1; n2 < 100; n2++) {
                                wsprintfA(newPath, "%s\\Program%d.cpp", tpath, n2);
                                if (GetFileAttributesA(newPath) == INVALID_FILE_ATTRIBUTES) {
                                    WriteFileA_(newPath, "#include <cstdio>\r\nint main() {\r\n    printf(\"Hello!\\n\");\r\n    return 0;\r\n}\r\n");
                                    break;
                                }
                            }
                        } else OpenError(S.sys_error, S.sys_error_msg);
                        break;
                    }
                    case CTX_NEWKCPP: {
                        if (!IsSystemPath(tpath)) {
                            char newPath[520];
                            for (int n2 = 1; n2 < 100; n2++) {
                                wsprintfA(newPath, "%s\\Program%d.kcpp", tpath, n2);
                                if (GetFileAttributesA(newPath) == INVALID_FILE_ATTRIBUTES) {
                                    WriteFileA_(newPath, "G:\\Users\\Default\\Documents\\Program1.cpp");
                                    break;
                                }
                            }
                        } else OpenError(S.sys_error, S.sys_error_msg);
                        break;
                    }
                    case CTX_NEWPY: {
                        if (!IsSystemPath(tpath)) {
                            char newPath[520];
                            for (int n2 = 1; n2 < 100; n2++) {
                                wsprintfA(newPath, "%s\\Script%d.py", tpath, n2);
                                if (GetFileAttributesA(newPath) == INVALID_FILE_ATTRIBUTES) {
                                    WriteFileA_(newPath, "print(\"Hello from KashmaOS!\")\r\n");
                                    break;
                                }
                            }
                        } else OpenError(S.sys_error, S.sys_error_msg);
                        break;
                    }
                    case CTX_NEWHTML: {
                        if (!IsSystemPath(tpath)) {
                            char newPath[520];
                            for (int n2 = 1; n2 < 100; n2++) {
                                wsprintfA(newPath, "%s\\Page%d.html", tpath, n2);
                                if (GetFileAttributesA(newPath) == INVALID_FILE_ATTRIBUTES) {
                                    WriteFileA_(newPath, "<html>\r\n<head><title>KashmaOS Page</title></head>\r\n<body>\r\n<h1>Hello KashmaOS!</h1>\r\n</body>\r\n</html>\r\n");
                                    break;
                                }
                            }
                        } else OpenError(S.sys_error, S.sys_error_msg);
                        break;
                    }
                    case CTX_NEWJSON: {
                        if (!IsSystemPath(tpath)) {
                            char newPath[520];
                            for (int n2 = 1; n2 < 100; n2++) {
                                wsprintfA(newPath, "%s\\Data%d.json", tpath, n2);
                                if (GetFileAttributesA(newPath) == INVALID_FILE_ATTRIBUTES) {
                                    WriteFileA_(newPath, "{\r\n  \"name\": \"KashmaOS\",\r\n  \"version\": \"7.0\",\r\n  \"status\": \"ok\"\r\n}\r\n");
                                    break;
                                }
                            }
                        } else OpenError(S.sys_error, S.sys_error_msg);
                        break;
                    }
                    case CTX_NEWMD: {
                        if (!IsSystemPath(tpath)) {
                            char newPath[520];
                            for (int n2 = 1; n2 < 100; n2++) {
                                wsprintfA(newPath, "%s\\Note%d.md", tpath, n2);
                                if (GetFileAttributesA(newPath) == INVALID_FILE_ATTRIBUTES) {
                                    WriteFileA_(newPath, "# KashmaOS Note\r\n\r\nWritten with KashmaOS 7.\r\n");
                                    break;
                                }
                            }
                        } else OpenError(S.sys_error, S.sys_error_msg);
                        break;
                    }
                    case CTX_NEWBAT: {
                        if (!IsSystemPath(tpath)) {
                            char newPath[520];
                            for (int n2 = 1; n2 < 100; n2++) {
                                wsprintfA(newPath, "%s\\Script%d.bat", tpath, n2);
                                if (GetFileAttributesA(newPath) == INVALID_FILE_ATTRIBUTES) {
                                    WriteFileA_(newPath, "@echo off\r\necho Hello from KashmaOS!\r\npause\r\n");
                                    break;
                                }
                            }
                        } else OpenError(S.sys_error, S.sys_error_msg);
                        break;
                    }
                    case CTX_NEWASM: {
                        if (!IsSystemPath(tpath)) {
                            char newPath[520];
                            for (int n2 = 1; n2 < 100; n2++) {
                                wsprintfA(newPath, "%s\\BootSeg%d.asm", tpath, n2);
                                if (GetFileAttributesA(newPath) == INVALID_FILE_ATTRIBUTES) {
                                    WriteFileA_(newPath, "; KashmaOS assembly source\r\nsection .text\r\n    hlt\r\n");
                                    break;
                                }
                            }
                        } else OpenError(S.sys_error, S.sys_error_msg);
                        break;
                    }
                    case CTX_OPEN: {
                        if (g_deskCtxSel >= 0 && g_ctxWin < 0) {
                            OpenDeskFile(g_deskCtxSel);
                        }
                        if (tw && tw->filesSel >= 0) {
                            OsWindow& w = *tw;
                            char fullpath[520];
                            wsprintfA(fullpath, "%s\\%s", w.filesPath, w.filesList[w.filesSel]);
                            if (w.filesIsDir[w.filesSel]) {
                                if (strlen(w.filesPath)+1+strlen(w.filesList[w.filesSel]) < 250) {
                                    strcat(w.filesPath, "\\"); strcat(w.filesPath, w.filesList[w.filesSel]);
                                    w.filesScroll = 0; w.filesSel = -1;
                                }
                            } else if (IsSystemPath(w.filesPath)) {
                                OpenNotepad(fullpath);
                            } else if (strend(fullpath, ".cpp")) {
                                OsWindow* nw = NULL;
                                for (int k = 0; k < g_winCount; k++)
                                    if (g_windows[k].app == APP_CODE && g_windows[k].open) { nw = &g_windows[k]; break; }
                                if (!nw) { OpenWindow(APP_CODE); nw = &g_windows[g_winCount-1]; }
                                if (nw) {
                                    strcpy(nw->codeFile, fullpath);
                                    HANDLE hf = CreateFileA(fullpath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
                                    if (hf != INVALID_HANDLE_VALUE) {
                                        DWORD s2 = GetFileSize(hf, NULL); if (s2 > 16383) s2 = 16383;
                                        DWORD rd; ReadFile(hf, nw->code, s2, &rd, NULL);
                                        nw->code[rd] = 0; nw->codeLen = rd; CloseHandle(hf);
                                    }
                                    nw->codeModified = false; nw->minimized = false;
                                    g_activeWin = (int)(nw - g_windows);
                                }
                            } else if (strend(fullpath, ".kcpp")) {
                                OsWindow* nw = NULL;
                                for (int k = 0; k < g_winCount; k++)
                                    if (g_windows[k].app == APP_KCPP && g_windows[k].open) { nw = &g_windows[k]; break; }
                                if (!nw) { OpenWindow(APP_KCPP); nw = &g_windows[g_winCount-1]; }
                                if (nw) { strcpy(nw->kcppPath, fullpath); nw->minimized = false; g_activeWin = (int)(nw - g_windows); }
                            } else {
                                HANDLE hf = CreateFileA(fullpath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
                                if (hf != INVALID_HANDLE_VALUE) {
                                    DWORD s2 = GetFileSize(hf, NULL); if (s2 > 4095) s2 = 4095;
                                    DWORD rd;
                                    OsWindow* nw = NULL;
                                    for (int k = 0; k < g_winCount; k++)
                                        if (g_windows[k].app == APP_NOTEPAD && g_windows[k].open) { nw = &g_windows[k]; break; }
                                    if (!nw) { OpenWindow(APP_NOTEPAD); nw = &g_windows[g_winCount-1]; }
                                    if (nw) {
                                        strcpy(nw->notepadFile, fullpath);
                                        ReadFile(hf, nw->notepad, s2, &rd, NULL);
                                        nw->notepad[rd]=0; nw->notepadLen=rd;
                                        nw->minimized = false; g_activeWin = (int)(nw - g_windows);
                                    }
                                    CloseHandle(hf);
                                }
                            }
                        }
                        break;
                    }
                    }
                    if (tw == NULL && a == CTX_OPENFILES) {
                        OpenFilesAt(tpath);
                    } else if (tw == NULL &&
                        (a == CTX_NEWFOLDER || a == CTX_NEWTXT || a == CTX_NEWCPP ||
                         a == CTX_NEWKCPP || a == CTX_NEWPY || a == CTX_NEWHTML ||
                         a == CTX_NEWJSON || a == CTX_NEWMD || a == CTX_NEWBAT ||
                         a == CTX_NEWASM || a == CTX_PASTE)) {
                        ScanDesktopFiles();
                    }
                    g_ctxOpen = false;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            } else {
                g_ctxOpen = false;
            }
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        if (my >= taskY) {
            if (mx >= 2 && mx <= 80) { g_startOpen = !g_startOpen; }
            else {
                g_startOpen = false;
                int bx = 90;
                for (int i = 0; i < g_winCount; i++) {
                    if (g_windows[i].open) {
                        int bw = 130;
                        if (mx >= bx && mx < bx + bw) {
                            if (g_windows[i].minimized) { g_windows[i].minimized = false; g_activeWin = i; }
                            else if (g_activeWin == i) { g_windows[i].minimized = true; g_activeWin = -1; }
                            else g_activeWin = i;
                            InvalidateRect(hwnd, NULL, FALSE); return 0;
                        }
                        bx += bw + 4;
                    }
                }
            }
            InvalidateRect(hwnd, NULL, FALSE); return 0;
        }
        if (g_startOpen) {
            int menuX = 2, menuY = taskY - 328, menuW = 190, menuH = 328;
            if (PtIn(mx, my, menuX, menuY, menuW, menuH)) {
                int item = (my - menuY - 8) / 26;
                AppType itemApps[] = {APP_CALC, APP_NOTEPAD, APP_FILES, APP_CODE, APP_KCPP, APP_SETTINGS, APP_PAINT, APP_PPT, APP_WEB, APP_REDSTEAM, APP_ABOUT, APP_ABOUT};
                if (item >= 0 && item < 12) {
                    if (item == 10) { /* separator, ignore */ }
                    else if (item == 11) {
                            g_startOpen = false;
                            InvalidateRect(hwnd, NULL, FALSE);
                            return 0;
                        }
                    else OpenWindow(itemApps[item]);
                }
            }
            g_startOpen = false;
            InvalidateRect(hwnd, NULL, FALSE); return 0;
        }
        for (int i = g_winCount - 1; i >= 0; i--) {
            OsWindow& w = g_windows[i];
            if (!w.open || w.minimized) continue;
            int ht = HitTest(mx, my, w);
            if (ht >= 0) {
                g_activeWin = i;
                if (ht == 10) { if (w.app == APP_KML && w.kmlRunning) KmlStop(w); w.open = false; if (g_activeWin == i) g_activeWin = -1; InvalidateRect(hwnd, NULL, FALSE); return 0; }
                if (ht == 11) { RECT scr; GetWindowRect(hwnd, &scr); w.x = 0; w.y = 0; w.w = scr.right; w.h = scr.bottom - TASKBAR_H; InvalidateRect(hwnd, NULL, FALSE); return 0; }
                if (ht == 12) { w.minimized = true; g_activeWin = -1; InvalidateRect(hwnd, NULL, FALSE); return 0; }
                if (ht == 1) { g_dragging = true; g_dragOffX = mx - w.x; g_dragOffY = my - w.y; SetCapture(hwnd); return 0; }
                if (ht == 2 || ht == 3) { g_resizing = true; g_resizeEdge = ht; SetCapture(hwnd); return 0; }
                if (w.app == APP_CALC) { char btn[8]; if (CalcHitAll(w, mx, my, btn)) { CalcPress(w, btn); InvalidateRect(hwnd, NULL, FALSE); return 0; } }
                if (w.app == APP_DRAU) { w.drauSpin = !w.drauSpin; InvalidateRect(hwnd, NULL, FALSE); return 0; }
                if (w.app == APP_PAINT) { HandlePaintClick(w, mx, my); return 0; }
                if (w.app == APP_PPT) { HandlePptClick(w, mx, my); return 0; }
                if (w.app == APP_KML) {
                    int ex = w.x + 4, ey = w.y + TITLEBAR_H + 4;
                    int ew = w.w - 8;
                    int tabW = 110, tabH = 26, taby = ey + 44;
                    if (PtIn(mx, my, ex+12, taby, tabW, tabH)) {
                        w.kmlHostMode = true; w.kmlFocused = 1;
                        InvalidateRect(hwnd, NULL, FALSE); return 0;
                    }
                    if (PtIn(mx, my, ex+12+tabW+6, taby, tabW, tabH)) {
                        w.kmlHostMode = false; w.kmlFocused = 1;
                        InvalidateRect(hwnd, NULL, FALSE); return 0;
                    }
                    int fy = taby + tabH + 14;
                    int fldX = ex + 140, fldW = 150, fldH = 24;
                    int fcount = w.kmlHostMode ? 2 : 3;
                    for (int i = 0; i < fcount; i++) {
                        if (PtIn(mx, my, fldX, fy + i*30, fldW, fldH)) {
                            w.kmlFocused = i + 1;
                            InvalidateRect(hwnd, NULL, FALSE); return 0;
                        }
                    }
                    int by = fy + fcount*30 + 14, bW = 130, bH = 30;
                    int startX = ex + 12;
                    if (PtIn(mx, my, startX, by, bW, bH)) {
                        if (w.kmlRunning) KmlStop(w);
                        else if (w.kmlHostMode) KmlStartHost(w);
                        else KmlStartJoin(w);
                        InvalidateRect(hwnd, NULL, FALSE); return 0;
                    }
                    return 0;
                }
                if (w.app == APP_UPD) {
                    int ex = w.x + 4, ey = w.y + TITLEBAR_H + 4;
                    int ew = w.w - 8;
                    int uy = ey + 50, uh = 24;
                    int fldX = ex + 130, fldW = ew - 160;
                    if (PtIn(mx, my, fldX, uy, fldW, uh)) {
                        InvalidateRect(hwnd, NULL, FALSE); return 0;
                    }
                    int by = uy + uh + 12, bW = 120, bH = 28;
                    if (PtIn(mx, my, ex+12, by, bW, bH)) {
                        if (!g_updBusy) {
                            _beginthreadex(NULL, 0, UpdCheckThread, &w, 0, NULL);
                            InterlockedExchange(&g_updBusy, 1);
                        }
                        InvalidateRect(hwnd, NULL, FALSE); return 0;
                    }
                    if (PtIn(mx, my, ex+12+bW+8, by, bW, bH)) {
                        if (!g_updBusy && g_updAvailable) {
                            _beginthreadex(NULL, 0, UpdApplyThread, &w, 0, NULL);
                            InterlockedExchange(&g_updBusy, 1);
                        }
                        InvalidateRect(hwnd, NULL, FALSE); return 0;
                    }
                    return 0;
                }
                if (w.app == APP_INET) {
                    int ex = w.x + 4, ey = w.y + TITLEBAR_H + 4;
                    int ew = w.w - 8, eh = w.h - TITLEBAR_H - 8;
                    if (w.inetPassOpen) {
                        int px = ex + 10, py = ey + 142;
                        if (PtIn(mx, my, px + 14, py + 100, 110, 24)) {
                            if (!w.inetBusy && w.inetPassLen > 0) {
                                w.inetBusy = true;
                                _beginthreadex(NULL, 0, InetConnectThread, &w, 0, NULL);
                            }
                            w.inetPassOpen = false;
                            InvalidateRect(hwnd, NULL, FALSE);
                            return 0;
                        }
                        if (PtIn(mx, my, px + 132, py + 100, 100, 24)) {
                            w.inetPassOpen = false;
                            w.inetPass[0] = 0;
                            w.inetPassLen = 0;
                            InvalidateRect(hwnd, NULL, FALSE);
                            return 0;
                        }
                        return 0;
                    }
                    if (PtIn(mx, my, ex + 12, ey + eh - 30, 150, 26)) {
                        if (!w.inetBusy) {
                            w.inetBusy = true;
                            _beginthreadex(NULL, 0, InetScanThread, &w, 0, NULL);
                        }
                        InvalidateRect(hwnd, NULL, FALSE);
                        return 0;
                    }
                    int ry0 = ey + 168;
                    for (int i = 0; i < w.inetCount && i < 7; i++) {
                        int y0 = ry0 + i * 26;
                        if (PtIn(mx, my, ex + 2, y0, ew - 4, 26)) {
                            w.inetSel = i;
                            if (w.inetSec[i] && !w.inetKnown[i]) {
                                w.inetPassOpen = true;
                                w.inetPass[0] = 0;
                                w.inetPassLen = 0;
                            } else if (!w.inetBusy) {
                                w.inetBusy = true;
                                _beginthreadex(NULL, 0, InetConnectThread, &w, 0, NULL);
                            }
                            InvalidateRect(hwnd, NULL, FALSE);
                            return 0;
                        }
                    }
                    return 0;
                }
                if (w.app == APP_TASKMGR) { return 0; }
                if (w.app == APP_NOTEPAD) {
                    int svx = w.x + 8, svy = w.y + TITLEBAR_H + 7, svw = 90, svh = 24;
                    if (PtIn(mx, my, svx, svy, svw, svh)) {
                        if (w.notepadRO) {
                            OpenError(S.sys_error, S.sys_error_msg);
                            InvalidateRect(hwnd, NULL, FALSE);
                            return 0;
                        }
                        HANDLE hf = CreateFileA(w.notepadFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
                        if (hf != INVALID_HANDLE_VALUE) {
                            DWORD wr; WriteFile(hf, w.notepad, (DWORD)w.notepadLen, &wr, NULL); CloseHandle(hf);
                            OpenError(S.saved, S.saved);
                        } else {
                            OpenError(S.sys_error, S.sys_error_msg);
                        }
                        RunSecurityCheck();
                        for (int k = 0; k < g_winCount; k++)
                            if (g_windows[k].open && g_windows[k].app == APP_FILES) {
                                char* ls = strrchr(w.notepadFile, '\\');
                                if (ls) {
                                    *ls = 0;
                                    if (strcmp(g_windows[k].filesPath, w.notepadFile) == 0) ScanDir(g_windows[k]);
                                    *ls = '\\';
                                }
                            }
                        char deskp[520];
                        wsprintfA(deskp, "%s\\Users\\Default\\Desktop", G_DRIVE_ROOT);
                        char dirbuf[520]; strcpy(dirbuf, w.notepadFile);
                        for (int k = (int)strlen(dirbuf) - 1; k >= 0; k--)
                            if (dirbuf[k] == '\\') { dirbuf[k] = 0; break; }
                        if (strcmp(dirbuf, deskp) == 0) ScanDesktopFiles();
                        InvalidateRect(hwnd, NULL, FALSE);
                        return 0;
                    }
                }
                if (w.app == APP_SETTINGS) { HandleSettingsClick(w, mx, my); return 0; }
                if (w.app == APP_FILES) {
                    int fidx = -1;
                    if (FilesHit(w, mx, my, fidx)) {
                        if (fidx == -2) {
                            if (w.filesSearch) {
                                w.filesSearch = false; w.sCount = 0;
                            }
                            char* lastSlash = strrchr(w.filesPath, '\\');
                            if (lastSlash && lastSlash > w.filesPath) *lastSlash = 0;
                            if (strlen(w.filesPath) < strlen(G_DRIVE_ROOT))
                                strcpy(w.filesPath, G_DRIVE_ROOT);
                            w.filesScroll = 0; w.filesSel = -1;
                        } else if (fidx == -3 || fidx == -4) {
                            g_ctxWin = i;
                            g_ctxX = w.x + 100; g_ctxY = w.y + 90;
                            CtxAction acts[10] = {CTX_NEWFOLDER, CTX_NEWTXT, CTX_NEWCPP, CTX_NEWKCPP, CTX_NEWPY, CTX_NEWHTML, CTX_NEWJSON, CTX_NEWMD, CTX_NEWBAT, CTX_NEWASM};
                            for (int k = 0; k < 10; k++) g_ctxActions[k] = acts[k];
                            g_ctxCount = 10;
                            g_ctxOpen = true;
                            InvalidateRect(hwnd, NULL, FALSE); return 0;
                        } else if (fidx == -5) {
                            if (w.filesSel >= 0) {
                                wsprintfA(g_copyPath, "%s\\%s", w.filesPath, w.filesList[w.filesSel]);
                                g_copyIsDir = w.filesIsDir[w.filesSel];
                            }
                        } else if (fidx == -6) {
                            if (g_copyPath[0]) {
                                char dst[520];
                                const char* name = g_copyPath;
                                const char* s = strrchr(g_copyPath, '\\');
                                if (s) name = s + 1;
                                if (IsSystemPath(w.filesPath)) { OpenError(S.sys_error, S.sys_error_msg); }
                                else if (GetFileAttributesA(g_copyPath) & FILE_ATTRIBUTE_DIRECTORY) {
                                    wsprintfA(dst, "%s\\%s", w.filesPath, name);
                                    if (GetFileAttributesA(dst) == INVALID_FILE_ATTRIBUTES)
                                        CopyFolder(g_copyPath, dst);
                                } else {
                                    wsprintfA(dst, "%s\\%s", w.filesPath, name);
                                    int n = 1;
                                    while (GetFileAttributesA(dst) != INVALID_FILE_ATTRIBUTES) {
                                        wsprintfA(dst, "%s\\Copy %d of %s", w.filesPath, n, name);
                                        n++;
                                    }
                                    CopyFileA(g_copyPath, dst, FALSE);
                                }
                            }
                            RunSecurityCheck();
                        } else if (fidx == -7) {
                            if (w.filesSel >= 0) {
                                char full[520];
                                wsprintfA(full, "%s\\%s", w.filesPath, w.filesList[w.filesSel]);
                                if (IsSystemPath(full) || IsSystemPath(w.filesPath)) {
                                    OpenError(S.sys_error, S.sys_error_msg);
                                } else {
                                    if (w.filesIsDir[w.filesSel]) {
                                        char pat[520]; wsprintfA(pat, "%s\\*", full);
                                        WIN32_FIND_DATAA fd2;
                                        HANDLE h2 = FindFirstFileA(pat, &fd2);
                                        bool empty = (h2 == INVALID_HANDLE_VALUE);
                                        if (!empty) {
                                            do {
                                                if (strcmp(fd2.cFileName,".")==0||strcmp(fd2.cFileName,"..")==0) continue;
                                                empty = false; break;
                                            } while(FindNextFileA(h2,&fd2));
                                            FindClose(h2);
                                        }
                                        if (empty) RemoveDirectoryA(full); else OpenError(S.sys_error, S.sys_error_msg);
                                    } else {
                                        DeleteFileA(full);
                                    }
                                    w.filesSel = -1;
                                }
                            }
                            RunSecurityCheck();
                        } else if (fidx == -8) {
                            w.filesSearch = true;
                            w.searchQ[0] = 0;
                            w.sCount = 0; w.sScroll = 0; w.filesSel = -1;
                        } else if (w.filesSearch && fidx >= 0 && fidx < w.sCount) {
                            if (i == w.filesSel) OpenSearchResult(w, fidx);
                            else w.filesSel = fidx;
                        } else if (w.filesIsDir[fidx]) {
                            if (i == w.filesSel) {
                                if (strlen(w.filesPath) + 1 + strlen(w.filesList[fidx]) < 250) {
                                    strcat(w.filesPath, "\\");
                                    strcat(w.filesPath, w.filesList[fidx]);
                                    w.filesScroll = 0; w.filesSel = -1;
                                }
                            } else {
                                w.filesSel = fidx;
                            }
                        } else if (fidx >= 0 && !w.filesIsDir[fidx]) {
                            if (i != w.filesSel) {
                                w.filesSel = fidx;
                            } else {
                                if (IsSystemPath(w.filesPath)) {
                                    OpenError(S.sys_error, S.sys_error_msg);
                                } else {
                                    char fullpath[520];
                                    wsprintfA(fullpath, "%s\\%s", w.filesPath, w.filesList[fidx]);
                                    if (strend(fullpath, ".cpp")) {
                                        OsWindow* nw = NULL;
                                        for (int k = 0; k < g_winCount; k++)
                                            if (g_windows[k].app == APP_CODE && g_windows[k].open) { nw = &g_windows[k]; break; }
                                        if (!nw) { OpenWindow(APP_CODE); nw = &g_windows[g_winCount-1]; }
                                        if (nw) {
                                            strcpy(nw->codeFile, fullpath);
                                            HANDLE hf = CreateFileA(fullpath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
                                            if (hf != INVALID_HANDLE_VALUE) {
                                                DWORD sz2 = GetFileSize(hf, NULL);
                                                if (sz2 > 16383) sz2 = 16383;
                                                DWORD rd; ReadFile(hf, nw->code, sz2, &rd, NULL);
                                                nw->code[rd] = 0; nw->codeLen = rd; CloseHandle(hf);
                                            }
                                            nw->codeModified = false;
                                            nw->minimized = false; g_activeWin = (int)(nw - g_windows);
                                        }
                                    } else if (strend(fullpath, ".kcpp")) {
                                        OsWindow* nw = NULL;
                                        for (int k = 0; k < g_winCount; k++)
                                            if (g_windows[k].app == APP_KCPP && g_windows[k].open) { nw = &g_windows[k]; break; }
                                        if (!nw) { OpenWindow(APP_KCPP); nw = &g_windows[g_winCount-1]; }
                                        if (nw) { strcpy(nw->kcppPath, fullpath); nw->minimized = false; g_activeWin = (int)(nw - g_windows); }
                                    } else {
                                        HANDLE hf = CreateFileA(fullpath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
                                        if (hf != INVALID_HANDLE_VALUE) {
                                            DWORD sz = GetFileSize(hf, NULL);
                                            if (sz > 4095) sz = 4095;
                                            DWORD rd;
                                            OsWindow* nw = NULL;
                                            for (int k = 0; k < g_winCount; k++)
                                                if (g_windows[k].app == APP_NOTEPAD && g_windows[k].open) { nw = &g_windows[k]; break; }
                                            if (!nw) { OpenWindow(APP_NOTEPAD); nw = &g_windows[g_winCount-1]; }
                                            if (nw) {
                                                strcpy(nw->notepadFile, fullpath);
                                                ReadFile(hf, nw->notepad, sz, &rd, NULL);
                                                nw->notepad[rd] = 0;
                                                nw->notepadLen = rd;
                                                nw->minimized = false;
                                                g_activeWin = (int)(nw - g_windows);
                                            }
                                            CloseHandle(hf);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    InvalidateRect(hwnd, NULL, FALSE);
                    return 0;
                }
                if (w.app == APP_REDSTEAM) {
                    int bx2 = w.x + w.w/2 - 50, by2 = w.y + TITLEBAR_H + 110;
                    if (PtIn(mx, my, bx2, by2, 100, 30)) { OpenWindow(APP_NOTEPAD); InvalidateRect(hwnd, NULL, FALSE); return 0; }
                }
                if (w.app == APP_CODE) {
                    int px = w.x + 4, py = w.y + TITLEBAR_H + 4;
                    if (PtIn(mx, my, px, py, 90, 26)) {
                        char dst[520];
                        if (w.codeFile[0]) strcpy(dst, w.codeFile);
                        else { for (int n = 1; n < 1000; n++) {
                            wsprintfA(dst, "%s\\Users\\Default\\Documents\\Program%d.cpp", G_DRIVE_ROOT, n);
                            if (GetFileAttributesA(dst) == INVALID_FILE_ATTRIBUTES) break;
                        } }
                        WriteFileA_(dst, w.code);
                        strcpy(w.codeFile, dst);
                        w.codeModified = false;
                        OpenError(S.saved, S.saved);
                        InvalidateRect(hwnd, NULL, FALSE); return 0;
                    }
                    if (PtIn(mx, my, px + 96, py, 120, 26)) {
                        char dst[520];
                        for (int n = 1; n < 1000; n++) {
                            wsprintfA(dst, "%s\\Users\\Default\\Documents\\Program%d.cpp", G_DRIVE_ROOT, n);
                            if (GetFileAttributesA(dst) == INVALID_FILE_ATTRIBUTES) break;
                        }
                        WriteFileA_(dst,
                            "#include <cstdio>\nstruct MyClass {\n    int x;\n    MyClass() : x(42) {}\n    void say() {\n        printf(\"Hello from class! %d\\n\", x);\n    }\n};\nint main() {\n    MyClass obj;\n    obj.say();\n    return 0;\n}\n");
                        strcpy(w.codeFile, dst);
                        w.codeLen = 0; w.code[0] = 0; w.codeModified = false;
                        OpenError(S.saved, S.saved);
                        InvalidateRect(hwnd, NULL, FALSE); return 0;
                    }
                    if (PtIn(mx, my, px + 222, py, 90, 26)) {
                        if (!w.codeFile[0]) {
                            char dst[520];
                            for (int n = 1; n < 1000; n++) {
                                wsprintfA(dst, "%s\\Users\\Default\\Documents\\Program%d.cpp", G_DRIVE_ROOT, n);
                                if (GetFileAttributesA(dst) == INVALID_FILE_ATTRIBUTES) break;
                            }
                            WriteFileA_(dst, w.code);
                            strcpy(w.codeFile, dst);
                            w.codeModified = false;
                        }
                        RunProgramInTerminal(w.codeFile);
                        InvalidateRect(hwnd, NULL, FALSE); return 0;
                    }
                }
                if (w.app == APP_KCPP) {
                    int px = w.x + 4, py = w.y + TITLEBAR_H + 4;
                    if (PtIn(mx, my, px, py, 140, 26)) {
                        char full[520];
                        if (strlen(w.kcppPath) > 0 && GetFileAttributesA(w.kcppPath) != INVALID_FILE_ATTRIBUTES) {
                            strcpy(full, w.kcppPath);
                        } else {
                            OpenError(S.run, S.kcpp);
                            InvalidateRect(hwnd, NULL, FALSE); return 0;
                        }
                        char line[520]; line[0] = 0;
                        FILE* f = fopen(full, "r");
                        bool ok = false;
                        if (f) {
                            if (fgets(line, sizeof(line), f)) {
                                char* nl = strchr(line, '\n'); if (nl) *nl = 0;
                                char* cr = strchr(line, '\r'); if (cr) *cr = 0;
                                ok = true;
                            }
                            fclose(f);
                        }
                        if (!ok || GetFileAttributesA(line) == INVALID_FILE_ATTRIBUTES) {
                            OpenError(S.run, S.kcpp);
                            InvalidateRect(hwnd, NULL, FALSE); return 0;
                        }
                        RunProgramInTerminal(line);
                        InvalidateRect(hwnd, NULL, FALSE); return 0;
                    }
                }
                InvalidateRect(hwnd, NULL, FALSE); return 0;
            }
        }
        for (int i = 0; i < ICON_COUNT; i++) {
            DesktopIcon& ic = g_icons[i];
            if (PtIn(mx, my, ic.x, ic.y, 70, 78)) {
                g_iconDragging = true;
                g_dragIcon = i;
                g_iconDragOffX = mx - ic.x;
                g_iconDragOffY = my - ic.y;
                SetCapture(hwnd);
                InvalidateRect(hwnd, NULL, FALSE); return 0;
            }
        }
        for (int i = 0; i < g_deskCount; i++) {
            int dx = g_deskFiles[i].x, dy = g_deskFiles[i].y;
            if (PtIn(mx, my, dx, dy, 70, 78)) {
                g_deskSel = i;
                g_deskDragging = true;
                g_deskDragIdx = i;
                g_deskDragOffX = mx - g_deskFiles[i].x;
                g_deskDragOffY = my - g_deskFiles[i].y;
                SetCapture(hwnd);
                InvalidateRect(hwnd, NULL, FALSE); return 0;
            }
        }
        if (g_deskSel >= 0) { g_deskSel = -1; }
        g_iconDragging = false; g_dragIcon = -1;
        g_activeWin = -1;
        InvalidateRect(hwnd, NULL, FALSE); return 0;
    }

    case WM_MOUSEMOVE: {
        int cmx = (int)(short)LOWORD(lp), cmy = (int)(short)HIWORD(lp);
        if (g_activeWin >= 0) {
            OsWindow& wa = g_windows[g_activeWin];
            if (wa.open && !wa.minimized && wa.app == APP_PAINT && wa.paintDown) {
                int px, py;
                if (PaintMapPoint(wa, cmx, cmy, px, py)) {
                    PaintLine(wa, wa.paintLastX, wa.paintLastY, px, py);
                    wa.paintLastX = px; wa.paintLastY = py;
                }
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
        }
        if (g_deskDragging && g_deskDragIdx >= 0) {
            DeskFile& df = g_deskFiles[g_deskDragIdx];
            df.x = cmx - g_deskDragOffX;
            df.y = cmy - g_deskDragOffY;
            RECT rcg; GetClientRect(hwnd, &rcg);
            int mw = rcg.right, mh = rcg.bottom;
            if (df.x < 0) df.x = 0; if (df.y < 0) df.y = 0;
            if (df.x > mw - 70) df.x = mw - 70; if (df.y > mh - TASKBAR_H - 78) df.y = mh - TASKBAR_H - 78;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        if (g_iconDragging && g_dragIcon >= 0) {
            DesktopIcon& ic = g_icons[g_dragIcon];
            ic.x = cmx - g_iconDragOffX;
            ic.y = cmy - g_iconDragOffY;
            RECT rcg; GetClientRect(hwnd, &rcg);
            int mw = rcg.right, mh = rcg.bottom;
            if (ic.x < 0) ic.x = 0; if (ic.y < 0) ic.y = 0;
            if (ic.x > mw - 70) ic.x = mw - 70; if (ic.y > mh - TASKBAR_H - 78) ic.y = mh - TASKBAR_H - 78;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        if ((g_dragging || g_resizing) && g_activeWin >= 0) {
            int mx = (int)(short)LOWORD(lp), my = (int)(short)HIWORD(lp);
            OsWindow& w = g_windows[g_activeWin];
            if (g_dragging) { w.x = mx - g_dragOffX; w.y = my - g_dragOffY; }
            if (g_resizing) {
                if (g_resizeEdge == 2 || g_resizeEdge == 0) w.w = mx - w.x;
                if (g_resizeEdge == 2 || g_resizeEdge == 1) w.h = my - w.y;
                if (w.w < 200) w.w = 200; if (w.h < 120) w.h = 120;
            }
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    }
    case WM_LBUTTONUP: {
        if (g_activeWin >= 0 && g_windows[g_activeWin].open && g_windows[g_activeWin].app == APP_PAINT)
            g_windows[g_activeWin].paintDown = false;
        if (g_deskDragging && g_deskDragIdx >= 0) {
            DeskFile& df = g_deskFiles[g_deskDragIdx];
            SnapDeskPos(df.x, df.y);
            g_deskDragging = false; g_deskDragIdx = -1;
            SaveDeskPositions();
        }
        if (g_iconDragging && g_dragIcon >= 0) {
            DesktopIcon& ic = g_icons[g_dragIcon];
            SnapDeskPos(ic.x, ic.y);
            g_iconDragging = false; g_dragIcon = -1;
        }
        g_dragging = false; g_resizing = false; ReleaseCapture(); return 0;
    }
    case WM_MOUSEWHEEL: {
        int delta = (short)HIWORD(wp);
        if (g_activeWin >= 0 && g_windows[g_activeWin].open && g_windows[g_activeWin].app == APP_PAINT) {
            OsWindow& w = g_windows[g_activeWin];
            w.paintSize += (delta / 120);
            if (w.paintSize < 1) w.paintSize = 1;
            if (w.paintSize > 12) w.paintSize = 12;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        if (g_activeWin >= 0 && g_windows[g_activeWin].open && g_windows[g_activeWin].app == APP_PPT &&
            !g_windows[g_activeWin].pptShow) {
            OsWindow& w = g_windows[g_activeWin];
            w.pptScroll -= (delta / 120);
            int maxS = w.pptCount - ((w.h - TITLEBAR_H - 8 - 50) / 74);
            if (maxS < 0) maxS = 0;
            if (w.pptScroll < 0) w.pptScroll = 0;
            if (w.pptScroll > maxS) w.pptScroll = maxS;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        if (g_activeWin >= 0 && g_windows[g_activeWin].open && g_windows[g_activeWin].app == APP_TERM) {
            g_windows[g_activeWin].termScroll -= (delta / 120) * 3;
            if (g_windows[g_activeWin].termScroll > 0) g_windows[g_activeWin].termScroll = 0;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        if (g_activeWin >= 0 && g_windows[g_activeWin].open && g_windows[g_activeWin].app == APP_FILES) {
            OsWindow& w = g_windows[g_activeWin];
            if (w.filesSearch && w.sCount > 0) {
                w.sScroll += (delta / 120) * -1;
                int eh = w.h - TITLEBAR_H - 8;
                int visible = (eh - 28) / 22;
                if (visible < 1) visible = 1;
                if (w.sScroll < 0) w.sScroll = 0;
                if (w.sScroll > w.sCount - visible) w.sScroll = w.sCount - visible;
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
        }
        return 0;
    }
    case WM_KEYDOWN: {
        {
            if (g_keySeq == 0 && wp == 'J') g_keySeq = 1;
            else if (g_keySeq == 1 && wp == 'K') g_keySeq = 2;
            else if (g_keySeq == 2 && wp == 'L') {
                g_keySeq = 0;
                if (g_boot == BOOT_DESKTOP && g_editMode == 0 && !TextAppActive()) {
                    PostQuitMessage(0);
                    return 0;
                }
            }
            else if (g_keySeq != 0) g_keySeq = 0;
        }
        {
            bool ctrlHeld = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            if (wp == VK_CONTROL) { g_shutdownSeq = 1; return 0; }
            if (g_shutdownSeq == 1 && ctrlHeld && wp == 'G') g_shutdownSeq = 2;
            else if (g_shutdownSeq == 2 && ctrlHeld && wp == 'E') { PostQuitMessage(0); return 0; }
            else g_shutdownSeq = 0;
        }
        if (g_activeWin >= 0 && g_windows[g_activeWin].open && !g_windows[g_activeWin].minimized &&
            g_windows[g_activeWin].app == APP_PPT && g_windows[g_activeWin].pptShow) {
            OsWindow& w = g_windows[g_activeWin];
            if (wp == VK_ESCAPE) {
                w.pptShow = false;
            } else if (wp == VK_RIGHT || wp == VK_RETURN || wp == VK_SPACE) {
                w.pptCur++;
                if (w.pptCur >= w.pptCount) { w.pptCur = 0; w.pptShow = false; }
            } else if (wp == VK_LEFT) {
                w.pptCur--;
                if (w.pptCur < 0) w.pptCur = w.pptCount - 1;
            }
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    }
    case WM_SYSKEYDOWN:
        return 0;
    case WM_KEYUP:
    case WM_SYSKEYUP: if (wp == VK_CONTROL) g_shutdownSeq = 0; return 0;
    case WM_SYSCOMMAND: if (wp == SC_CLOSE) return 0; return DefWindowProc(hwnd, msg, wp, lp);
    case WM_ACTIVATE:
        if (LOWORD(wp) != WA_INACTIVE) {
            SetForegroundWindow(hwnd);
            BringWindowToTop(hwnd);
            ClampCursorToOs();
        }
        return 0;
    case WM_CLOSE: return 0;
    case WM_DESTROY:
        SetClosedFlag();
        if (g_mainMutex) { ReleaseMutex(g_mainMutex); CloseHandle(g_mainMutex); g_mainMutex = NULL; }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

static void EnsureAutorun() {
    char curPath[MAX_PATH];
    GetModuleFileNameA(NULL, curPath, MAX_PATH);
    char appData[MAX_PATH];
    if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appData) != S_OK) return;
    char dstDir[MAX_PATH], dstExe[MAX_PATH];
    wsprintfA(dstDir, "%s\\KashmaOS", appData);
    CreateDirectoryA(dstDir, NULL);
    wsprintfA(dstExe, "%s\\kashmaos7.exe", dstDir);
    if (_stricmp(curPath, dstExe) != 0)
        CopyFileA(curPath, dstExe, FALSE);

    HKEY hk;
    if (RegCreateKeyExA(HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0, NULL, 0, KEY_SET_VALUE, NULL, &hk, NULL) == ERROR_SUCCESS) {
        char cmd[MAX_PATH + 16];
        wsprintfA(cmd, "\"%s\"", dstExe);
        RegSetValueExA(hk, "KashmaOS", 0, REG_SZ, (const BYTE*)cmd, (DWORD)(strlen(cmd) + 1));
        RegCloseKey(hk);
    }
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, int nShow) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (lpCmdLine && strstr(lpCmdLine, "--guard")) {
        RomInitPath();
        RunGuardMode();
        return 0;
    }
    RomInitPath();
    EnsureAutorun();
    if (MainIsUp()) return 0;
    g_mainMutex = CreateMutexA(NULL, TRUE, "KashmaOS7_Main");
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "KashmaOS7";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    RegisterClassExA(&wc);
    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (vw <= 0 || vh <= 0) {
        vx = 0; vy = 0;
        vw = GetSystemMetrics(SM_CXSCREEN);
        vh = GetSystemMetrics(SM_CYSCREEN);
    }
    g_hwnd = CreateWindowExA(WS_EX_TOPMOST, "KashmaOS7", "KashmaOS 7",
        WS_POPUP | WS_CLIPCHILDREN, vx, vy, vw, vh, NULL, NULL, hInst, NULL);
    ShowWindow(g_hwnd, SW_SHOW);
    SetWindowPos(g_hwnd, HWND_TOPMOST, vx, vy, vw, vh, SWP_SHOWWINDOW);
    ClampCursorToOs();
    ApplyCursor();
    UpdateWindow(g_hwnd);
    InitGDrive();
    ScanDesktopFiles();
    if (RomHas()) RomReconcile();
    EnsureRustCore();
    PassLoad();
    SrcGuardSave();
    LayersSaveAll();
    RomSave();
    RunSecurityCheck();
    CreateThread(NULL, 0, PatrolThread, NULL, 0, NULL);
    _beginthreadex(NULL, 0, AutoUpdThread, NULL, 0, NULL);
    SetTimer(g_hwnd, 1, 500, NULL);
    SetTimer(g_hwnd, 3, 16, NULL);
    g_winHook = SetWindowsHookExW(WH_KEYBOARD_LL, WinKeyHook, hInst, 0);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    if (g_winHook) UnhookWindowsHookEx(g_winHook);

    if (g_updateRestart && g_updateNewExe[0]) {
        char dir[MAX_PATH];
        strcpy(dir, g_exePath);
        char* sl = strrchr(dir, '\\');
        if (sl) sl[1] = 0;
        char oldExe[MAX_PATH], newExe[MAX_PATH], bat[MAX_PATH + 64];
        wsprintfA(oldExe, "%skashmaos7.exe", dir);
        strncpy(newExe, g_updateNewExe, MAX_PATH - 1); newExe[MAX_PATH - 1] = 0;
        wsprintfA(bat, "%skashmaos_upd.bat", dir);
        FILE* f = fopen(bat, "w");
        if (f) {
            fprintf(f, "@echo off\r\n");
            fprintf(f, ":wait\r\n");
            fprintf(f, "set /a c=c+1\r\n");
            fprintf(f, "tasklist /fi \"imagename eq kashmaos7.exe\" 2>nul | findstr /i kashmaos7 >nul\r\n");
            fprintf(f, "if not errorlevel 1 (\r\n");
            fprintf(f, "  if %c% LSS 60 (\r\n", 0);
            fprintf(f, "    ping 127.0.0.1 -n 2 >nul\r\n");
            fprintf(f, "    goto wait\r\n");
            fprintf(f, "  )\r\n");
            fprintf(f, "  taskkill /f /im kashmaos7.exe >nul 2>&1\r\n");
            fprintf(f, "  ping 127.0.0.1 -n 2 >nul\r\n");
            fprintf(f, ")\r\n");
            fprintf(f, "del /f /q \"%s\" >nul 2>&1\r\n", oldExe);
            fprintf(f, "move /y \"%s\" \"%s\" >nul 2>&1\r\n", newExe, oldExe);
            fprintf(f, "if exist \"%s\" start \"\" \"%s\"\r\n", oldExe, oldExe);
            fprintf(f, "del /f /q \"%~f0\"\r\n");
            fclose(f);
            STARTUPINFOA su = {};
            su.cb = sizeof(su);
            PROCESS_INFORMATION pi = {};
            char cmdLine[MAX_PATH + 80];
            wsprintfA(cmdLine, "cmd.exe /c \"%s\"", bat);
            CreateProcessA(NULL, cmdLine, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, dir, &su, &pi);
            if (pi.hProcess) { CloseHandle(pi.hProcess); CloseHandle(pi.hThread); }
        }
    }

    return (int)msg.wParam;
}
