# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'qt.ui'
#
# Created: Sat Mar 25 19:17:07 2017
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName("Dialog")
        Dialog.resize(400, 300)
        self.pbA = QtGui.QPushButton(Dialog)
        self.pbA.setGeometry(QtCore.QRect(20, 30, 99, 27))
        self.pbA.setObjectName("pbA")
        self.pbB = QtGui.QPushButton(Dialog)
        self.pbB.setGeometry(QtCore.QRect(130, 30, 99, 27))
        self.pbB.setObjectName("pbB")
        self.pbC = QtGui.QPushButton(Dialog)
        self.pbC.setGeometry(QtCore.QRect(240, 30, 99, 27))
        self.pbC.setObjectName("pbC")
        self.lblFoo = QtGui.QLabel(Dialog)
        self.lblFoo.setGeometry(QtCore.QRect(150, 90, 68, 17))
        self.lblFoo.setObjectName("lblFoo")

        self.retranslateUi(Dialog)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(QtGui.QApplication.translate("Dialog", "Dialog", None, QtGui.QApplication.UnicodeUTF8))
        self.pbA.setText(QtGui.QApplication.translate("Dialog", "Foo", None, QtGui.QApplication.UnicodeUTF8))
        self.pbB.setText(QtGui.QApplication.translate("Dialog", "Bar", None, QtGui.QApplication.UnicodeUTF8))
        self.pbC.setText(QtGui.QApplication.translate("Dialog", "Baz", None, QtGui.QApplication.UnicodeUTF8))
        self.lblFoo.setText(QtGui.QApplication.translate("Dialog", "Untouched", None, QtGui.QApplication.UnicodeUTF8))

