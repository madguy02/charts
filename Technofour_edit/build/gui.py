#!/usr/bin/python
import Tkinter
import tkMessageBox
import os

top = Tkinter.Tk()

def helloCallBack():
   	os.system('sudo nodejs exec.js')

B = Tkinter.Button(top, text ="Start", command = helloCallBack)



B.pack()
top.mainloop()



#B=Tkinter.Button(top,text="hello",command= helloCallBack)
#B.pack()
#top.mainloop()