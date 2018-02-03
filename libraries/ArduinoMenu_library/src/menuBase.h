/* -*- C++ -*- */
/********************
Sept. 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

base for menu library
definitions and enumerations

www.r-site.net

***/

// #define DEBUG
//DRAW_2D adds 260 bytes to flash
//#define DRAW_2D

#ifndef RSITE_ARDUINO_MENU_SYSTEM_BASE
  #define RSITE_ARDUINO_MENU_SYSTEM_BASE

  #ifdef DEBUG
    extern bool debugFlag;
    extern const char* libMemMode;
  #endif

  #include <Arduino.h>
  #ifndef assert
    #define assert(x)
  #endif
  #include "macros.h"

  #define _MAX(a,b) (((a)>(b))?(a):(b))

  namespace Menu {
    //menu structure objects
    class menuIn;
    class menuOut;
    class panel;
    class panelsList;
    class navNode;
    class navRoot;
    class prompt;
    template<typename T> class menuField;
    //struct menuNodeShadow;
    class menuNode;
    class menu;
    template<typename T> class menuVariant;
    template<typename T> class toggle;
    template<typename T> class select;
    template<typename T> class choose;

    typedef int8_t idx_t;
    idx_t print_P(Print& s,const char* at,idx_t sz=0);

    enum result {proceed=0,quit};
    enum systemStyles {
      _noStyle=0,
      _menuData=1,
      _canNav=1<<1,
      _parentDraw=1<<2,
      _isVariant=1<<3,
      _asPad=1<<4,
      _Exit=1<<5
    };
    //showTitle and noTitle override the default
    enum styles {
      noStyle=0,
      wrapStyle=1,
      showTitle=2,
      noTitle=4
    };//,dontEnter=2};
    //representing parsed input codes (can be used with switch/case)
    enum navCmds {
      noCmd=0,
      escCmd,
      enterCmd,
      upCmd,
      downCmd,
      leftCmd,
      rightCmd,
      idxCmd,
      selCmd,
      scrlUpCmd,
      scrlDownCmd
    };
    struct navCmd {
      navCmds cmd;
      idx_t param;
      inline navCmd(navCmds cmd):cmd(cmd),param(-1) {}
      inline navCmd(navCmds cmd,idx_t p):cmd(cmd),param(p) {}
      inline bool operator==(navCmds n) const {return cmd==n;}
      inline operator navCmds() const {return cmd;}
    };

    navCmds getCmd(String &name);

    //----------------------------------------------------
    //events generated by the menu system
    enum eventMask {
      noEvent=0,//just ignore all stuff
      activateEvent=1,//this item is about to be active (system event)
      enterEvent=1<<1,//entering navigation level (this menu is now active)
      exitEvent=1<<2,//leaving navigation level
      returnEvent=1<<3,//TODO:entering previous level (return)
      focusEvent=1<<4,//element just gained focus
      blurEvent=1<<5,//element about to lose focus
      selFocusEvent=1<<6,//TODO:child just gained focus
      selBlurEvent=1<<7,//TODO:child about to lose focus
      updateEvent=1<<8,//Field value has been updated
      anyEvent=~0
    };
    //events for the idle function
    enum idleEvent {idleStart,idling,idleEnd};

    //color defs, then indexing by selected and enabled status
    enum colorDefs {
      bgColor=0,
      fgColor,
      valColor,
      unitColor,
      cursorColor,
      titleColor,
      nColors//total number of colors
    };

    //this defines an array of disabled and enabled colors of yet unspecified color format C
    template<typename C>
    struct colorDef {
      C disabled[2];
      C enabled[3];
    };

    enum status {disabledStatus=0,enabledStatus=1};

    #define SYS_FUNC_PARAMS eventMask event, navNode& nav, prompt &item
    #define SYS_FUNC_VALUES event,nav,item
    #define FUNC_PARAMS eventMask event, navNode& nav, prompt &item
    #define FUNC_VALUES event,nav,item

    //callback function type
    typedef result (*callback)(FUNC_PARAMS);
    typedef void (*vCall)(FUNC_PARAMS);//now also accept void return

    //functions to hook as common prompt actions
    result doNothing();//just proceed with menu
    result doExit();//do not enter a menu or exit the current one

    //idle function  type
    typedef result (*idleFunc)(menuOut& o,idleEvent);
    result inaction(menuOut& o,idleEvent e);//default idle function

    //template<void (*A)(eventMask event, navNode& nav, prompt &item, Stream &in)> result callCaster(eventMask event, navNode& nav, prompt &item, Stream &in);
    template<void (*A)(eventMask event, navNode& nav, prompt &item)> result callCaster(eventMask event, navNode& nav, prompt &item);
    template<void (*A)(eventMask event, navNode& nav)> result callCaster(eventMask event, navNode& nav);
    template<void (*A)(eventMask event)> result callCaster(eventMask event);
    template<void (*A)()> result callCaster();

    //menu element associated function (called for all element registered events)
    struct actionRaw {callback hFn;};
    class action {
      public:
        callback hFn;//the hooked callback function
        inline action() {}
        //inline action(void (*f)()):hFn((callback)f) {}
        inline action(result (*f)()):hFn((callback)f) {}
        inline action(result (*f)(eventMask)):hFn((callback)f) {}
        inline action(result (*f)(eventMask,navNode&)):hFn((callback)f) {}
        // inline action(result (*f)(eventMask,navNode&,prompt&)):hFn((callback)f) {}
        //inline action(result (*f)(eventMask,navNode&,prompt&,Stream&)):hFn((callback)f) {}
        inline action(callback f):hFn(f) {}
        inline result operator()(FUNC_PARAMS) const {return ((callback)memPtr(hFn))(FUNC_VALUES);}
    };

    extern action noAction;

    //config
    ///////////////////////////////////////////////////////////////////////////
    struct navCode {navCmds cmd;char ch;};
    typedef navCode navCodesDef[10];
    extern const navCodesDef defaultNavCodes;

    struct config {
      config(
        char ecur='>',
        char dcur='-',
        const navCodesDef &nc=defaultNavCodes,
        bool invKeys=true
      ):selectedCursor(ecur),
      disabledCursor(dcur),
      navCodes(nc),
      invertFieldKeys(invKeys) {}
      //NOTE:this can be output specific
      char selectedCursor;//='>';
      char disabledCursor;//='-';
      const navCodesDef &navCodes;//=defaultNavCodes;
      bool invertFieldKeys;
      inline char getCmdChar(navCmds cmd) const {return navCodes[cmd].ch;}//return character assigned to this command
    };

    extern config defaultOptions;
    extern config* options;


    #ifdef DRAW_2D
      typedef class Area {
        protected:
          inline Area op(Area& (Area::*o)(const Area&),const Area&p) const {
            Area tmp(*this);
            return (tmp.*o)(p);
          }
        public:
          idx_t w;
          idx_t h;
          inline Area() {}
          inline Area(idx_t w,idx_t h):w(w),h(h) {}
          inline Area(const Area& o):w(o.w),h(o.h) {}
          inline Area(idx_t x):w(x),h(0) {}
          inline operator idx_t() {return w;}
          inline Area  operator-() {return Area(-h,-w);}
          inline Area& operator+=(const Area& o) {w+=o.w;h+=o.h;return *this;}
          inline Area& operator-=(const Area& o) {w-=o.w;h-=o.h;return *this;}
          inline Area& operator&=(const Area& o) {
            if (w<o.w) w=o.w;
            if (h<o.h) h=o.h;
            return *this;
          }
          inline Area  operator+ (const Area& o) const {return op(&Area::operator+=,o);}
          inline Area  operator- (const Area& o) const {return op(&Area::operator-=,o);}
      } Used;
    #else
      typedef idx_t Used;
    #endif

    #ifdef DEBUG
      Print& operator<<(Print& o,bool b);
      Print& operator<<(Print& o,navCmds cmd);
      Print& operator<<(Print& o,navCmd cmd);
      Print& operator<<(Print& o,result cmd);
      Print& operator<<(Print& o,eventMask e);
      Print& operator<<(Print& o,colorDefs c);
      Print& operator<<(Print& o,idleEvent e);
      Print& operator<<(Print& o,systemStyles s);
      // Print& operator<<(Print& o,prompt &p);
    #endif

  }

#endif
