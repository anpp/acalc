#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QMenu>
#include <QMenuBar>
#include <QDebug>
#include <QTranslator>
#include <QTextCodec>
#include <QStyle>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMap>
#include <QWhatsThis>
#include <QClipboard>
#include <QMutex>
#include <QWaitCondition>
#include <QTime>
#include <QMessageBox>
#include <algorithm>

#include "qcalcwidget.h"
#include "common.h"
#include "../CalcIt/calcparser.h"

#ifndef _QT4
    #define PACKAGE "aCalc"
    #ifdef _UNIX
    #include </usr/local/include/libintl.h>
    #else
    #include "../../gettext/gettext-runtime/include/libintl.h"
    #endif
#endif


namespace Ui {
class MainWidget;
}


struct sKeyMod {
    int key;
    int mod;

    inline friend bool operator<(sKeyMod km1, sKeyMod km2){return (km1.key + km1.mod < km2.key + km2.mod);}
};


class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
    void ResizeAll(unsigned new_button_w = WIDTH_BUT, unsigned new_button_h = HEIGHT_BUT);
    
protected:
    void focusOutEvent(QFocusEvent* fe);
    void keyPressEvent(QKeyEvent* pe);
    void mousePressEvent(QMouseEvent *mpe);
    bool event(QEvent *e);
private:
    void InitLocale(elangs indexLang = EN);
    void LoadLocale(const QString& sloc);
    void SetLocale(int indexLang);
    void CreateMenus(void);
    void SetLocaleTexts(void);
    bool InitLayouts();
    void FillLayoutWidgets(QLayout *l, pnl atype, bool bGrid = true);
    void LayoutOriginal(void);
    void LayoutSimple(void);
    void LayoutProgrammable(void);
    void InitModesLayouts();
    void FreeLayouts(void);
    void CreateWidgets();
    void CreateButtons(pnl atype);
    void SetSizeOfWidgets();
    void ResizeWidgets(unsigned w, unsigned h, pnl atype);
    void UpdateDisplay(ud how_update = RESEMPTY);
    bool AddToken(const QString& stok);
    QString GetExpression(bool bHtml = false);
    QString GetErrors(void);
    QString NumberToString(double n, int precision = 34);
    void DefaultKeysMap(void);
    QCalcWidget* FindButtonByValue(QString value);
    void AssignKeyToButton(QString button_value, int key, int mod = 0);
    QCalcWidget* FindWidgetByKey(sKeyMod km);
    sKeyMod FindKeyByValue(QString value);
    void SendKey(int Key, int Mod = 0);
    void ClickToWidget(QWidget *widget, int msec = 30);
    void SetMode(SMODES mode, bool on);
    void Alert(void);
    void ReCreateMouseEvents();


    Ui::MainWidget *ui;

    QFrame *wDisplay;
    QFrame *wMode;
    QFrame *wCentral;
    QFrame *wBottom;
    QFrame *wDigits;
    QFrame *wOps;
    QFrame *wMem;
    QFrame *wAbc;
    QFrame *wScale;
    QFrame *wDRG;
    QFrame *wFuncModes;
    QLabel *Display;
    QLabel *wResult;

    QColor colorDigits;
    QColor colorOps;
    QColor colorAbc;
    QColor colorMem;
    QColor colorFunc;

    std::vector<QCalcWidget*> vec_btns;

    QMap<sKeyMod, QCalcWidget*> map_keys;
    QMap<QString, sKeyMod> map_val_key;


    QMenuBar *MenuBar;
    QMenu *MenuView;
    QMenu *MenuEdit;
    QMenu *MenuHelp;
    QMenu *MenuLanguages;
    QAction *ActionCopy;
    QAction *ActionPaste;
    QAction *ActionSettings;
    QAction *ActionContextHelp;
    QAction *ActionAbout;
    QAction *ActionWhatIs;
    QVector<QAction*> ActionViews;
    QVector<QAction*> ActionLanguages;

    QMenu *MenuOnButton;

    QTranslator *qtTrans;
    QTranslator *qtTransPopup;
    QTranslator *qtTransErrors;

    QLabel *lblMem;

    int shape;

    unsigned button_w;
    unsigned button_h;
    unsigned button_func_w;
    unsigned spacing;

    CalcParser *parser;

    double inMemory;

    QWidget *curr_widget;
    QPoint curr_GlobalPos;

    QMouseEvent *mePress;
    QMouseEvent *meRelease;
    QPoint posMousePress;

    bool bPasting;

    VIEWCALC viewCalc;

public slots:
    void ProcessClick(const QString& sButtonValue);
    void ProcessClickMem(const QString& sButtonValue);
    void ProcessClickScale(const QString& sButtonValue);
    void ProcessClickServ(const QString& sButtonValue);
    void ProcessClickDRG(const QString& sButtonValue);
    void ProcessClickFuncModes(const QString& sModeValue);
    void slotMenuWhatIsThis(void);
    void slotEnterWhatIsThis(void);
    void slotCopy(void);
    void slotPaste(void);
    void slotView(QAction* action);
    void slotAbout(void);
    void slotSettings(void);
    void slotLanguage(QAction* action);

};

#endif // MAINWIDGET_H
