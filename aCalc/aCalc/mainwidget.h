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
#include "settings.h"
#include "pnlbutton.h"
#include "calcdisplay.h"


namespace Ui {
class MainWidget;
}


struct sKeyMod {
    int key;
    unsigned int mod;

    inline friend bool operator<(sKeyMod km1, sKeyMod km2){return (km1.key + static_cast<int>(km1.mod) < km2.key + static_cast<int>(km2.mod));}
};


class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
    void ResizeAll(unsigned new_button_w, unsigned new_button_h);
    
protected:
    void focusOutEvent(QFocusEvent* fe);
    void keyPressEvent(QKeyEvent* pe);
    void mousePressEvent(QMouseEvent *mpe);
    bool event(QEvent *e);
    void showEvent(QShowEvent *event);
private:
    void InitLocale();
    void LoadLocale(const QString& sloc);
    void SetLocale(Langs indexLang);
    void SetView(int indexView, bool isFirst = false);
    void CreateMenus(void);
    void SetLocaleTexts(void);
    bool InitLayouts();
    void FillLayoutWidgets(Pnl atype);
    void LayoutOriginal(void);
    void LayoutSimple(void);
    void LayoutProgrammable(void);
    void InitModesLayouts();
    void FreeLayouts(void);
    void CreateWidgets();
    void CreateButtons(Pnl atype);
    void SetSizeOfWidgets(int button_w, int button_h);
    void ResizeWidgets(int w, int h, Pnl atype);
    void UpdateDisplay(ud how_update = ud::Empty);
    bool AddToken(const QString& stok);
    void DefaultKeysMap(void);
    QCalcWidget* FindButtonByValue(QString value);
    void AssignKeyToButton(QString button_value, int key, unsigned int mod = 0);
    QCalcWidget* FindWidgetByKey(sKeyMod km);
    sKeyMod FindKeyByValue(QString value);
    void SendKey(int Key, unsigned int Mod = 0);
    void ClickToWidget(QWidget *widget, int msec = 30);
    void SetMode(SModes mode, bool on);
    void Alert(void);
    void ReCreateMouseEvents();


    Ui::MainWidget *ui;

    CalcDisplay *wDisplay;
    QFrame *wMode;
    QFrame *wBottom;

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

    int shape = QFrame::WinPanel | QFrame::Sunken;
    int spacing = SPACING;

    CalcParser *parser = new CalcParser;

    int button_func_w = 0;

    double inMemory;

    QWidget *curr_widget = nullptr;
    QPoint curr_GlobalPos;

    QMouseEvent *mePress = nullptr;
    QMouseEvent *meRelease = nullptr;
    QPoint posMousePress;

    bool bPasting = false;

    Settings settings{this, "acalc", "acalc"};
    PnlButton pb{this};

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
