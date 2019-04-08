; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFastModelDoc
LastTemplate=CView
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "fastmodel.h"
LastPage=0

ClassCount=18
Class1=CFastModelApp
Class2=CFastModelDoc
Class3=CFastModelView
Class4=CMainFrame

ResourceCount=33
Resource1=CG_IDR_POPUP_MAIN_FRAME
Resource2=IDR_FASTHENRYTYPE (English (U.S.))
Resource3=IDR_FASTMOTYPE
Resource4=IDR_FASTMOTYPE_CNTR_IP
Resource5=IDR_FASTMOTYPE_CNTR_IP (English (U.S.))
Resource6=IDR_MAINFRAME (English (U.S.))
Resource7=IDR_FASTMOTYPE (English (U.S.))
Class5=CAboutDlg
Class6=CChildFrame
Class7=CImportLibrary
Class8=CFastHenryView
Resource8=IDD_OGLVIEWPROPS (English (U.S.))
Class9=CFastCapView
Resource9="IDR_FASTMOTYPE" (English (U.S.))
Resource10=IDR_FASTCAPTYPE (English (U.S.))
Resource11="IDR_FASTMODELTYPE" (English (U.S.))
Resource12="IDR_FASTMODELTYPE_CNTR_IP" (English (U.S.))
Resource13=CG_IDR_POPUP_FAST_MODEL_VIEW
Resource14=IDD_ABOUTBOX
Resource15=IDR_FASTHENRYTYPE
Resource16=IDR_FASTMODELTYPE (English (U.S.))
Resource17=IDR_POPUP_FASTCAPTYPE
Resource18=IDD_DIALOG_PROJECT (Italian (Italy))
Class10=CIntroDialog
Class11=CColorEditView
Resource19=IDD_DIALOG_LICENSE
Resource20=IDD_ABOUTBOX (English (U.S.))
Class12=CImportInColorView
Class13=CProjectCtrlPage
Class14=COutputEdit
Class15=CControlSolvers
Resource21=IDD_OGLVIEWPROPS
Resource22=IDR_MAINFRAME
Resource23=IDR_POPUP_FASTCAPTYPE (Italian (Italy))
Resource24=IDR_FASTMODELTYPE_CNTR_IP
Resource25=IDR_POPUP_FASTHENRYTYPE
Resource26=IDR_FASTMODELTYPE_CNTR_IP (English (U.S.))
Resource27=CG_IDR_POPUP_FAST_MODEL_VIEW (Italian (Italy))
Resource28=IDR_POPUP_FASTHENRYTYPE (Italian (Italy))
Resource29=CG_IDR_POPUP_MAIN_FRAME (Italian (Italy))
Resource30=IDR_FASTCAPTYPE
Resource31=IDR_FASTMODELTYPE
Class16=COGLViewProps
Class17=COpenGlView
Class18=CStateView
Resource32=IDD_DIALOG_LICENSE (Italian (Italy))
Resource33=IDD_DIALOG_PROJECT

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=CMainFrame
BaseClass=CMDIFrameWnd
VirtualFilter=fWC


[CLS:CChildFrame]
Type=0
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
Filter=M
BaseClass=CMDIChildWnd
VirtualFilter=mfWC
LastObject=CChildFrame

[CLS:CAboutDlg]
Type=0
HeaderFile=fastmodel.cpp
ImplementationFile=fastmodel.cpp
Filter=D
LastObject=CAboutDlg
BaseClass=CDialog
VirtualFilter=dWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW_PROJ
Command2=ID_FILE_OPEN_PROJ
Command3=ID_FILE_CLOSE_PROJ
Command4=ID_FILE_SAVE_PROJ
Command5=ID_FILE_SAVE_AS_PROJ
Command6=ID_IMPORT_FILE
Command7=ID_FILE_PRINT_SETUP
Command8=ID_FILE_MRU_FILE1
Command9=ID_APP_EXIT
Command10=ID_VIEW_TOOLBAR
Command11=ID_VIEW_STATUS_BAR
Command12=ID_VIEW_FASTCAP_VIEW
Command13=ID_VIEW_FASTHENRY_VIEW
Command14=ID_VIEW_PROJECT
Command15=ID_VIEW_OUTPUT
Command16=ID_HELP
Command17=ID_APP_ABOUT
CommandCount=17

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_RUN_FASTHENRY
Command8=ID_RUN_FASTCAP
Command9=ID_VIEW_DISPLAY_WIREFRAME
Command10=ID_VIEW_DISPLAY_HIDDENLINES
Command11=ID_VIEW_DISPLAY_SOLID
Command12=ID_FILE_PRINT
Command13=ID_APP_ABOUT
Command14=ID_CONTEXT_HELP
CommandCount=14

[MNU:IDR_FASTMOTYPE]
Type=1
Class=CFastModelView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_EDIT_UNDO
Command12=ID_EDIT_CUT
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_EDIT_PASTE_SPECIAL
Command16=ID_OLE_INSERT_NEW
Command17=ID_OLE_EDIT_LINKS
CommandCount=25
Command18=ID_VIEW_TOOLBAR
Command19=ID_VIEW_STATUS_BAR
Command20=ID_WINDOW_NEW
Command21=ID_WINDOW_CASCADE
Command22=ID_WINDOW_TILE_HORZ
Command23=ID_WINDOW_ARRANGE
Command24=ID_HELP_FINDER
Command25=ID_APP_ABOUT

[MNU:IDR_FASTMOTYPE_CNTR_IP]
Type=1
Class=CFastModelView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_WINDOW_NEW
Command12=ID_WINDOW_CASCADE
Command13=ID_WINDOW_TILE_HORZ
Command14=ID_WINDOW_ARRANGE
CommandCount=14

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_EDIT_COPY
Command2=ID_FILE_NEW_PROJ
Command3=ID_FILE_OPEN_PROJ
Command4=ID_FILE_PRINT
Command5=ID_FILE_SAVE_PROJ
Command6=ID_EDIT_PASTE
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_CANCEL_EDIT_CNTR
Command10=ID_HELP
Command11=ID_CONTEXT_HELP
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
Command14=ID_EDIT_COPY
Command15=ID_EDIT_PASTE
Command16=ID_EDIT_CUT
Command17=ID_EDIT_UNDO
CommandCount=17

[ACL:IDR_FASTMOTYPE_CNTR_IP]
Type=1
Class=CFastmodelView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_NEXT_PANE
Command6=ID_PREV_PANE
Command7=ID_CONTEXT_HELP
Command8=ID_HELP
CommandCount=8


[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW_PROJ
Command2=ID_FILE_OPEN_PROJ
Command3=ID_FILE_CLOSE_PROJ
Command4=ID_FILE_SAVE_PROJ
Command5=ID_FILE_SAVE_AS_PROJ
Command6=ID_IMPORT_FILE
Command7=ID_FILE_PRINT_SETUP
Command8=ID_FILE_MRU_FILE1
Command9=ID_APP_EXIT
Command10=ID_VIEW_TOOLBAR
Command11=ID_VIEW_STATUS_BAR
Command12=ID_VIEW_FASTCAP_VIEW
Command13=ID_VIEW_FASTHENRY_VIEW
Command14=ID_VIEW_PROJECT
Command15=ID_VIEW_OUTPUT
Command16=ID_VIEW_CLEAR_OUTPUT_WIN
Command17=ID_HELP
Command18=ID_APP_ABOUT
CommandCount=18

[MNU:IDR_FASTMOTYPE (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_IMPORT_FILE
Command7=ID_FILE_PRINT
Command8=ID_FILE_PRINT_PREVIEW
Command9=ID_FILE_PRINT_SETUP
Command10=ID_FILE_MRU_FILE1
Command11=ID_APP_EXIT
Command12=ID_EDIT_UNDO
Command13=ID_EDIT_CUT
Command14=ID_EDIT_COPY
Command15=ID_EDIT_PASTE
Command16=ID_EDIT_PASTE_SPECIAL
Command17=ID_OLE_INSERT_NEW
Command18=ID_OLE_EDIT_LINKS
Command19=ID_OLE_VERB_FIRST
Command20=ID_VIEW_TOOLBAR
Command21=ID_VIEW_STATUS_BAR
Command22=ID_WINDOW_NEW
Command23=ID_WINDOW_CASCADE
Command24=ID_WINDOW_TILE_HORZ
Command25=ID_WINDOW_ARRANGE
Command26=ID_HELP_FINDER
Command27=ID_APP_ABOUT
CommandCount=27

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_RUN_FASTHENRY
Command8=ID_RUN_FASTCAP
Command9=ID_RUN_FASTERCAP
Command10=ID_VIEW_DISPLAY_WIREFRAME
Command11=ID_VIEW_DISPLAY_HIDDENLINES
Command12=ID_VIEW_DISPLAY_SOLID
Command13=ID_FILE_PRINT
Command14=ID_APP_ABOUT
Command15=ID_CONTEXT_HELP
CommandCount=15

[MNU:IDR_FASTMOTYPE_CNTR_IP (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_WINDOW_NEW
Command12=ID_WINDOW_CASCADE
Command13=ID_WINDOW_TILE_HORZ
Command14=ID_WINDOW_ARRANGE
CommandCount=14

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_FILE_NEW_PROJ
Command3=ID_FILE_OPEN_PROJ
Command4=ID_FILE_PRINT
Command5=ID_FILE_SAVE_PROJ
Command6=ID_EDIT_PASTE
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_CANCEL_EDIT_CNTR
Command10=ID_HELP
Command11=ID_CONTEXT_HELP
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
Command14=ID_EDIT_COPY
Command15=ID_EDIT_PASTE
Command16=ID_EDIT_CUT
Command17=ID_EDIT_UNDO
CommandCount=17

[ACL:IDR_FASTMOTYPE_CNTR_IP (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_NEXT_PANE
Command6=ID_PREV_PANE
Command7=ID_CONTEXT_HELP
Command8=ID_HELP
Command9=ID_CANCEL_EDIT_CNTR
CommandCount=9

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=6
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352
Control6=IDC_SHOW_LICENSE,button,1342242816

[CLS:CImportLibrary]
Type=0
HeaderFile=ImportLibrary.h
ImplementationFile=ImportLibrary.cpp
BaseClass=generic CWnd
Filter=W
LastObject=CImportLibrary

[CLS:CFastHenryView]
Type=0
HeaderFile=FastHenryView.h
ImplementationFile=FastHenryView.cpp
BaseClass=CColorEditView
Filter=D
VirtualFilter=VWC
LastObject=CFastHenryView

[CLS:CFastCapView]
Type=0
HeaderFile=FastCapView.h
ImplementationFile=FastCapView.cpp
BaseClass=CColorEditView
Filter=W
VirtualFilter=VWC
LastObject=ID_FILE_NEW

[MNU:"IDR_FASTMOTYPE" (English (U.S.))]
Type=1
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_IMPORT_FILE
Command7=ID_FILE_PRINT
Command8=ID_FILE_PRINT_PREVIEW
Command9=ID_FILE_PRINT_SETUP
Command10=ID_FILE_MRU_FILE1
Command11=ID_APP_EXIT
Command12=ID_EDIT_UNDO
Command13=ID_EDIT_CUT
Command14=ID_EDIT_COPY
Command15=ID_EDIT_PASTE
Command16=ID_EDIT_PASTE_SPECIAL
Command17=ID_OLE_INSERT_NEW
Command18=ID_OLE_EDIT_LINKS
Command19=ID_OLE_VERB_FIRST
Command20=ID_VIEW_TOOLBAR
Command21=ID_VIEW_STATUS_BAR
Command22=ID_WINDOW_NEW
Command23=ID_WINDOW_CASCADE
Command24=ID_WINDOW_TILE_HORZ
Command25=ID_WINDOW_ARRANGE
Command26=ID_HELP_FINDER
Command27=ID_APP_ABOUT
CommandCount=27

[MNU:"IDR_FASTMOTYPE_CNTR_IP" (English (U.S.))]
Type=1
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_WINDOW_NEW
Command12=ID_WINDOW_CASCADE
Command13=ID_WINDOW_TILE_HORZ
Command14=ID_WINDOW_ARRANGE
CommandCount=14

[MNU:"IDR_FASTMODELTYPE" (English (U.S.))]
Type=1
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_IMPORT_FILE
Command7=ID_FILE_PRINT
Command8=ID_FILE_PRINT_PREVIEW
Command9=ID_FILE_PRINT_SETUP
Command10=ID_FILE_MRU_FILE1
Command11=ID_APP_EXIT
Command12=ID_EDIT_UNDO
Command13=ID_EDIT_CUT
Command14=ID_EDIT_COPY
Command15=ID_EDIT_PASTE
Command16=ID_EDIT_PASTE_SPECIAL
Command17=ID_OLE_INSERT_NEW
Command18=ID_OLE_EDIT_LINKS
Command19=ID_OLE_VERB_FIRST
Command20=ID_VIEW_TOOLBAR
Command21=ID_VIEW_STATUS_BAR
Command22=ID_WINDOW_NEW
Command23=ID_WINDOW_CASCADE
Command24=ID_WINDOW_TILE_HORZ
Command25=ID_WINDOW_ARRANGE
Command26=ID_HELP_FINDER
Command27=ID_APP_ABOUT
CommandCount=27

[MNU:"IDR_FASTMODELTYPE_CNTR_IP" (English (U.S.))]
Type=1
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_WINDOW_NEW
Command12=ID_WINDOW_CASCADE
Command13=ID_WINDOW_TILE_HORZ
Command14=ID_WINDOW_ARRANGE
CommandCount=14

[MNU:IDR_FASTMODELTYPE (English (U.S.))]
Type=1
Class=CFastModelView
Command1=ID_FILE_NEW_PROJ
Command2=ID_FILE_OPEN_PROJ
Command3=ID_FILE_CLOSE_PROJ
Command4=ID_FILE_SAVE_PROJ
Command5=ID_FILE_SAVE_AS_PROJ
Command6=ID_IMPORT_FILE
Command7=ID_FILE_PRINT
Command8=ID_FILE_PRINT_TO_EPS
Command9=ID_FILE_PRINT_PREVIEW
Command10=ID_FILE_PRINT_SETUP
Command11=ID_FILE_MRU_FILE1
Command12=ID_APP_EXIT
Command13=ID_EDIT_UNDO
Command14=ID_EDIT_CUT
Command15=ID_EDIT_COPY
Command16=ID_EDIT_PASTE
Command17=ID_VIEW_TOOLBAR
Command18=ID_VIEW_STATUS_BAR
Command19=ID_VIEW_FASTCAP_VIEW
Command20=ID_VIEW_FASTHENRY_VIEW
Command21=ID_VIEW_PROJECT
Command22=ID_VIEW_OUTPUT
Command23=ID_VIEW_ZOOM_ALL
Command24=ID_VIEW_DISPLAY_WIREFRAME
Command25=ID_VIEW_DISPLAY_HIDDENLINES
Command26=ID_VIEW_DISPLAY_SOLID
Command27=ID_VIEW_DISPLAY_3D
Command28=ID_VIEW_DISPLAY_2D
Command29=ID_VIEW_AUTOSET_XY
Command30=ID_VIEW_AUTOSET_YZ
Command31=ID_VIEW_AUTOSET_ZX
Command32=ID_VIEW_PROPERTIES
Command33=ID_VIEW_CLEAR_OUTPUT_WIN
Command34=ID_WINDOW_NEW
Command35=ID_WINDOW_CASCADE
Command36=ID_WINDOW_TILE_HORZ
Command37=ID_WINDOW_ARRANGE
Command38=ID_HELP
Command39=ID_APP_ABOUT
CommandCount=39

[MNU:IDR_FASTMODELTYPE_CNTR_IP (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW_PROJ
Command2=ID_FILE_OPEN_PROJ
Command3=ID_FILE_CLOSE_PROJ
Command4=ID_FILE_SAVE_PROJ
Command5=ID_FILE_SAVE_AS_PROJ
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_WINDOW_NEW
Command12=ID_WINDOW_CASCADE
Command13=ID_WINDOW_TILE_HORZ
Command14=ID_WINDOW_ARRANGE
CommandCount=14

[MNU:IDR_FASTCAPTYPE (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW_FASTCAP
Command2=ID_FILE_OPEN_FASTCAP
Command3=ID_FILE_SAVE_FASTCAP
Command4=ID_FILE_SAVE_AS_FASTCAP
Command5=ID_FILE_SAVE_FASTCAP_ALL
Command6=ID_FILE_CLOSE_FASTCAP
Command7=ID_FILE_NEW_PROJ
Command8=ID_FILE_OPEN_PROJ
Command9=ID_FILE_CLOSE_PROJ
Command10=ID_FILE_SAVE_PROJ
Command11=ID_FILE_SAVE_AS_PROJ
Command12=ID_FILE_PRINT
Command13=ID_FILE_PRINT_PREVIEW
Command14=ID_FILE_PRINT_SETUP
Command15=ID_FILE_MRU_FILE1
Command16=ID_APP_EXIT
Command17=ID_EDIT_UNDO
Command18=ID_EDIT_CUT
Command19=ID_EDIT_COPY
Command20=ID_EDIT_PASTE
Command21=ID_VIEW_TOOLBAR
Command22=ID_VIEW_STATUS_BAR
Command23=ID_VIEW_FASTCAP_VIEW
Command24=ID_VIEW_FASTHENRY_VIEW
Command25=ID_VIEW_PROJECT
Command26=ID_VIEW_OUTPUT
Command27=ID_VIEW_CLEAR_OUTPUT_WIN
Command28=ID_FASTCAP_UPDATE_MODEL
Command29=ID_MODEL_NEW_WINDOW
Command30=ID_WINDOW_NEW
Command31=ID_WINDOW_CASCADE
Command32=ID_WINDOW_TILE_HORZ
Command33=ID_WINDOW_ARRANGE
Command34=ID_HELP
Command35=ID_APP_ABOUT
CommandCount=35

[MNU:IDR_FASTHENRYTYPE (English (U.S.))]
Type=1
Class=CFastHenryView
Command1=ID_FILE_OPEN_FASTHENRY
Command2=ID_FILE_SAVE_FASTHENRY
Command3=ID_FILE_SAVE_AS_FASTHENRY
Command4=ID_FILE_CLOSE_FASTHENRY
Command5=ID_FILE_NEW_PROJ
Command6=ID_FILE_OPEN_PROJ
Command7=ID_FILE_CLOSE_PROJ
Command8=ID_FILE_SAVE_PROJ
Command9=ID_FILE_SAVE_AS_PROJ
Command10=ID_FILE_PRINT
Command11=ID_FILE_PRINT_PREVIEW
Command12=ID_FILE_PRINT_SETUP
Command13=ID_FILE_MRU_FILE1
Command14=ID_APP_EXIT
Command15=ID_EDIT_UNDO
Command16=ID_EDIT_CUT
Command17=ID_EDIT_COPY
Command18=ID_EDIT_PASTE
Command19=ID_VIEW_TOOLBAR
Command20=ID_VIEW_STATUS_BAR
Command21=ID_VIEW_FASTCAP_VIEW
Command22=ID_VIEW_FASTHENRY_VIEW
Command23=ID_VIEW_PROJECT
Command24=ID_VIEW_OUTPUT
Command25=ID_VIEW_CLEAR_OUTPUT_WIN
Command26=ID_FASTHENRY_UPDATE_MODEL
Command27=ID_MODEL_NEW_WINDOW
Command28=ID_WINDOW_CASCADE
Command29=ID_WINDOW_TILE_HORZ
Command30=ID_WINDOW_ARRANGE
Command31=ID_HELP
Command32=ID_APP_ABOUT
CommandCount=32

[ACL:"IDR_FASTMODELTYPE_CNTR_IP" (English (U.S.))]
Type=1
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_NEXT_PANE
Command6=ID_PREV_PANE
Command7=ID_CONTEXT_HELP
Command8=ID_HELP
Command9=ID_CANCEL_EDIT_CNTR
CommandCount=9

[ACL:IDR_FASTMODELTYPE_CNTR_IP (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW_PROJ
Command2=ID_FILE_OPEN_PROJ
Command3=ID_FILE_PRINT
Command4=ID_FILE_SAVE_PROJ
Command5=ID_CANCEL_EDIT_CNTR
Command6=ID_HELP
Command7=ID_CONTEXT_HELP
Command8=ID_NEXT_PANE
Command9=ID_PREV_PANE
CommandCount=9

[CLS:CFastModelView]
Type=0
HeaderFile=fastmodelview.h
ImplementationFile=fastmodelview.cpp
BaseClass=COpenGlPrintView
Filter=C
VirtualFilter=VWC
LastObject=ID_VIEW_CLEAR_OUTPUT_WIN

[MNU:IDR_POPUP_FASTHENRYTYPE]
Type=1
Class=?
Command1=ID_EDIT_CUT
Command2=ID_EDIT_COPY
Command3=ID_EDIT_PASTE
Command4=ID_FASTHENRY_UPDATE_MODEL
CommandCount=4

[MNU:CG_IDR_POPUP_MAIN_FRAME]
Type=1
Class=?
Command1=ID_VIEW_FASTCAP_VIEW
Command2=ID_VIEW_FASTHENRY_VIEW
Command3=ID_VIEW_PROJECT
Command4=ID_VIEW_OUTPUT
CommandCount=4

[MNU:CG_IDR_POPUP_FAST_MODEL_VIEW]
Type=1
Class=?
Command1=ID_EDIT_CUT
Command2=ID_EDIT_COPY
Command3=ID_EDIT_PASTE
Command4=ID_VIEW_ZOOM_ALL
Command5=ID_VIEW_DISPLAY_WIREFRAME
Command6=ID_VIEW_DISPLAY_HIDDENLINES
Command7=ID_VIEW_DISPLAY_SOLID
Command8=ID_VIEW_DISPLAY_3D
Command9=ID_VIEW_DISPLAY_2D
Command10=ID_VIEW_AUTOSET_XY
Command11=ID_VIEW_AUTOSET_YZ
Command12=ID_VIEW_AUTOSET_ZX
Command13=ID_VIEW_PROPERTIES
CommandCount=13

[DLG:IDD_DIALOG_LICENSE]
Type=1
Class=CIntroDialog
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LICENSE,edit,1352665156
Control4=IDC_EXPIRY,static,1342308353
Control5=IDC_STATIC,static,1342308353

[CLS:CIntroDialog]
Type=0
HeaderFile=IntroDialog.h
ImplementationFile=IntroDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CIntroDialog
VirtualFilter=dWC

[CLS:CColorEditView]
Type=0
HeaderFile=ColorEditView.h
ImplementationFile=ColorEditView.cpp
BaseClass=CRichEditView
Filter=C
VirtualFilter=VWC
LastObject=CColorEditView

[MNU:IDR_POPUP_FASTCAPTYPE]
Type=1
Class=?
Command1=ID_EDIT_CUT
Command2=ID_EDIT_COPY
Command3=ID_EDIT_PASTE
Command4=ID_FASTCAP_UPDATE_MODEL
CommandCount=4

[DLG:IDD_DIALOG_PROJECT]
Type=1
Class=CProjectCtrlPage
ControlCount=1
Control1=IDC_TREE_PROJECT,SysTreeView32,1342242823

[CLS:CProjectCtrlPage]
Type=0
HeaderFile=ProjectCtrlPage.h
ImplementationFile=ProjectCtrlPage.cpp
BaseClass=CPropertyPage
Filter=W
LastObject=CProjectCtrlPage
VirtualFilter=idWC

[CLS:CFastModelDoc]
Type=0
HeaderFile=fastmodeldoc.h
ImplementationFile=fastmodeldoc.cpp
BaseClass=COleDocument
LastObject=ID_RUN_FASTERCAP
Filter=N
VirtualFilter=ODC

[CLS:COutputEdit]
Type=0
HeaderFile=outputedit.h
ImplementationFile=outputedit.cpp
BaseClass=CEdit
LastObject=COutputEdit
Filter=W
VirtualFilter=WC

[MNU:IDR_FASTMODELTYPE]
Type=1
Class=?
Command1=ID_FILE_NEW_PROJ
Command2=ID_FILE_OPEN_PROJ
Command3=ID_FILE_CLOSE_PROJ
Command4=ID_FILE_SAVE_PROJ
Command5=ID_FILE_SAVE_AS_PROJ
Command6=ID_IMPORT_FILE
Command7=ID_FILE_PRINT
Command8=ID_FILE_PRINT_TO_EPS
Command9=ID_FILE_PRINT_PREVIEW
Command10=ID_FILE_PRINT_SETUP
Command11=ID_FILE_MRU_FILE1
Command12=ID_APP_EXIT
Command13=ID_EDIT_UNDO
Command14=ID_EDIT_CUT
Command15=ID_EDIT_COPY
Command16=ID_EDIT_PASTE
Command17=ID_VIEW_TOOLBAR
Command18=ID_VIEW_STATUS_BAR
Command19=ID_VIEW_FASTCAP_VIEW
Command20=ID_VIEW_FASTHENRY_VIEW
Command21=ID_VIEW_PROJECT
Command22=ID_VIEW_OUTPUT
Command23=ID_VIEW_ZOOM_ALL
Command24=ID_VIEW_DISPLAY_WIREFRAME
Command25=ID_VIEW_DISPLAY_HIDDENLINES
Command26=ID_VIEW_DISPLAY_SOLID
Command27=ID_VIEW_DISPLAY_3D
Command28=ID_VIEW_DISPLAY_2D
Command29=ID_VIEW_AUTOSET_XY
Command30=ID_VIEW_AUTOSET_YZ
Command31=ID_VIEW_AUTOSET_ZX
Command32=ID_VIEW_PROPERTIES
Command33=ID_WINDOW_NEW
Command34=ID_WINDOW_CASCADE
Command35=ID_WINDOW_TILE_HORZ
Command36=ID_WINDOW_ARRANGE
Command37=ID_HELP
Command38=ID_APP_ABOUT
CommandCount=38

[MNU:IDR_FASTMODELTYPE_CNTR_IP]
Type=1
Class=?
Command1=ID_FILE_NEW_PROJ
Command2=ID_FILE_OPEN_PROJ
Command3=ID_FILE_CLOSE_PROJ
Command4=ID_FILE_SAVE_PROJ
Command5=ID_FILE_SAVE_AS_PROJ
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_WINDOW_NEW
Command12=ID_WINDOW_CASCADE
Command13=ID_WINDOW_TILE_HORZ
Command14=ID_WINDOW_ARRANGE
CommandCount=14

[MNU:IDR_FASTHENRYTYPE]
Type=1
Class=?
Command1=ID_FILE_OPEN_FASTHENRY
Command2=ID_FILE_SAVE_FASTHENRY
Command3=ID_FILE_SAVE_AS_FASTHENRY
Command4=ID_FILE_CLOSE_FASTHENRY
Command5=ID_FILE_NEW_PROJ
Command6=ID_FILE_OPEN_PROJ
Command7=ID_FILE_CLOSE_PROJ
Command8=ID_FILE_SAVE_PROJ
Command9=ID_FILE_SAVE_AS_PROJ
Command10=ID_FILE_PRINT
Command11=ID_FILE_PRINT_PREVIEW
Command12=ID_FILE_PRINT_SETUP
Command13=ID_FILE_MRU_FILE1
Command14=ID_APP_EXIT
Command15=ID_EDIT_UNDO
Command16=ID_EDIT_CUT
Command17=ID_EDIT_COPY
Command18=ID_EDIT_PASTE
Command19=ID_VIEW_TOOLBAR
Command20=ID_VIEW_STATUS_BAR
Command21=ID_VIEW_FASTCAP_VIEW
Command22=ID_VIEW_FASTHENRY_VIEW
Command23=ID_VIEW_PROJECT
Command24=ID_VIEW_OUTPUT
Command25=ID_FASTHENRY_UPDATE_MODEL
Command26=ID_MODEL_NEW_WINDOW
Command27=ID_WINDOW_CASCADE
Command28=ID_WINDOW_TILE_HORZ
Command29=ID_WINDOW_ARRANGE
Command30=ID_HELP
Command31=ID_APP_ABOUT
CommandCount=31

[MNU:IDR_FASTCAPTYPE]
Type=1
Class=?
Command1=ID_FILE_NEW_FASTCAP
Command2=ID_FILE_OPEN_FASTCAP
Command3=ID_FILE_SAVE_FASTCAP
Command4=ID_FILE_SAVE_AS_FASTCAP
Command5=ID_FILE_SAVE_FASTCAP_ALL
Command6=ID_FILE_CLOSE_FASTCAP
Command7=ID_FILE_NEW_PROJ
Command8=ID_FILE_OPEN_PROJ
Command9=ID_FILE_CLOSE_PROJ
Command10=ID_FILE_SAVE_PROJ
Command11=ID_FILE_SAVE_AS_PROJ
Command12=ID_FILE_PRINT
Command13=ID_FILE_PRINT_PREVIEW
Command14=ID_FILE_PRINT_SETUP
Command15=ID_FILE_MRU_FILE1
Command16=ID_APP_EXIT
Command17=ID_EDIT_UNDO
Command18=ID_EDIT_CUT
Command19=ID_EDIT_COPY
Command20=ID_EDIT_PASTE
Command21=ID_VIEW_TOOLBAR
Command22=ID_VIEW_STATUS_BAR
Command23=ID_VIEW_FASTCAP_VIEW
Command24=ID_VIEW_FASTHENRY_VIEW
Command25=ID_VIEW_PROJECT
Command26=ID_VIEW_OUTPUT
Command27=ID_FASTCAP_UPDATE_MODEL
Command28=ID_MODEL_NEW_WINDOW
Command29=ID_WINDOW_NEW
Command30=ID_WINDOW_CASCADE
Command31=ID_WINDOW_TILE_HORZ
Command32=ID_WINDOW_ARRANGE
Command33=ID_HELP
Command34=ID_APP_ABOUT
CommandCount=34

[MNU:IDR_POPUP_FASTHENRYTYPE (Italian (Italy))]
Type=1
Class=?
Command1=ID_EDIT_CUT
Command2=ID_EDIT_COPY
Command3=ID_EDIT_PASTE
Command4=ID_FASTHENRY_UPDATE_MODEL
CommandCount=4

[MNU:CG_IDR_POPUP_MAIN_FRAME (Italian (Italy))]
Type=1
Class=?
Command1=ID_VIEW_FASTCAP_VIEW
Command2=ID_VIEW_FASTHENRY_VIEW
Command3=ID_VIEW_PROJECT
Command4=ID_VIEW_OUTPUT
CommandCount=4

[MNU:CG_IDR_POPUP_FAST_MODEL_VIEW (Italian (Italy))]
Type=1
Class=?
Command1=ID_EDIT_CUT
Command2=ID_EDIT_COPY
Command3=ID_EDIT_PASTE
Command4=ID_VIEW_ZOOM_ALL
Command5=ID_VIEW_DISPLAY_WIREFRAME
Command6=ID_VIEW_DISPLAY_HIDDENLINES
Command7=ID_VIEW_DISPLAY_SOLID
Command8=ID_VIEW_DISPLAY_3D
Command9=ID_VIEW_DISPLAY_2D
Command10=ID_VIEW_AUTOSET_XY
Command11=ID_VIEW_AUTOSET_YZ
Command12=ID_VIEW_AUTOSET_ZX
Command13=ID_VIEW_PROPERTIES
CommandCount=13

[MNU:IDR_POPUP_FASTCAPTYPE (Italian (Italy))]
Type=1
Class=?
Command1=ID_EDIT_CUT
Command2=ID_EDIT_COPY
Command3=ID_EDIT_PASTE
Command4=ID_FASTCAP_UPDATE_MODEL
CommandCount=4

[ACL:IDR_FASTMODELTYPE_CNTR_IP]
Type=1
Class=?
Command1=ID_FILE_NEW_PROJ
Command2=ID_FILE_OPEN_PROJ
Command3=ID_FILE_PRINT
Command4=ID_FILE_SAVE_PROJ
Command5=ID_CANCEL_EDIT_CNTR
Command6=ID_HELP
Command7=ID_CONTEXT_HELP
Command8=ID_NEXT_PANE
Command9=ID_PREV_PANE
CommandCount=9

[DLG:IDD_DIALOG_LICENSE (Italian (Italy))]
Type=1
Class=CIntroDialog
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LICENSE,edit,1352665156
Control4=IDC_EXPIRY,static,1342308353
Control5=IDC_STATIC,static,1342308353

[DLG:IDD_DIALOG_PROJECT (Italian (Italy))]
Type=1
Class=CProjectCtrlPage
ControlCount=1
Control1=IDC_TREE_PROJECT,SysTreeView32,1342242823

[CLS:CFastModelApp]
Type=0
HeaderFile=fastmodel.h
ImplementationFile=fastmodel.cpp
BaseClass=CWinApp
Filter=N
VirtualFilter=AC
LastObject=CFastModelApp

[DLG:IDD_OGLVIEWPROPS]
Type=1
Class=COGLViewProps
ControlCount=12
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_BUTTON_WIREFRAME_COLOR,button,1342242944
Control4=IDC_STATIC,static,1342308352
Control5=IDC_BUTTON_SOLID_COLOR,button,1342242944
Control6=IDC_STATIC,static,1342308352
Control7=IDC_BUTTON_AMBIENT_LIGHT_COLOR,button,1342242944
Control8=IDC_STATIC,static,1342308352
Control9=IDC_BUTTON_BACK_TOP_COLOR,button,1342242944
Control10=IDC_STATIC,static,1342308352
Control11=IDC_BUTTON_BACK_BOT_COLOR,button,1342242944
Control12=IDC_STATIC,static,1342308352

[CLS:COGLViewProps]
Type=0
HeaderFile=OGLViewProps.h
ImplementationFile=OGLViewProps.cpp
BaseClass=CDialog
Filter=D
LastObject=COGLViewProps
VirtualFilter=dWC

[CLS:COpenGlView]
Type=0
HeaderFile=openglview.h
ImplementationFile=openglview.cpp
BaseClass=CView
Filter=C
VirtualFilter=VWC
LastObject=ID_VIEW_PROPERTIES

[CLS:CStateView]
Type=0
HeaderFile=StateView.h
ImplementationFile=StateView.cpp
BaseClass=CView
Filter=C
LastObject=CStateView

[DLG:IDD_OGLVIEWPROPS (English (U.S.))]
Type=1
Class=?
ControlCount=12
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_BUTTON_WIREFRAME_COLOR,button,1342242944
Control4=IDC_STATIC,static,1342308352
Control5=IDC_BUTTON_SOLID_COLOR,button,1342242944
Control6=IDC_STATIC,static,1342308352
Control7=IDC_BUTTON_AMBIENT_LIGHT_COLOR,button,1342242944
Control8=IDC_STATIC,static,1342308352
Control9=IDC_BUTTON_BACK_TOP_COLOR,button,1342242944
Control10=IDC_STATIC,static,1342308352
Control11=IDC_BUTTON_BACK_BOT_COLOR,button,1342242944
Control12=IDC_STATIC,static,1342308352

