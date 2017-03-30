import sys

from PySide.QtCore import *
from PySide.QtGui import *

import QtUI

class MainDialog(QDialog, QtUI.Ui_Dialog):

    def __init__(self, parent=None):
        super(MainDialog, self).__init__(parent)
        self.setupUi(self)
        self.pbA.clicked.connect(self.changeLabel)
    def changeLabel(self):
        self.lblFoo.setText("clicked!")




app = QApplication(sys.argv)
form = MainDialog()
form.show()
app.exec_()