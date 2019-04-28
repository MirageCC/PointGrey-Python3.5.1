from ctypes import *
from ctypes.wintypes import HWND, LPCSTR, UINT
import sys
import time
import numpy as np
import cv2
import numpy
import unicodedata
import ctypes
dll = ctypes.windll.LoadLibrary("PointGrey.dll")



class Buff(Structure):
        _fields_ =[('Rgbdata', (c_ubyte) * (1* 2448 * 2048))]
    
m_pRgbData=Buff()
m_pRgbData_p=pointer(m_pRgbData)
##print(m_pRgbData)
##str0 = u"123.bmp"
##import unicodedata
##FileName=unicodedata.normalize('NFKD',str0).encode('ascii','ignore')


Init = dll.PT_Init
Grab = dll.PT_Grab
Stop = dll.PT_Stop

Success =  Init()
if Success!=0:
    print("启动相机失败")
else:
    print("启动相机成功")
    count = 0
    for count in range(0,100):
        Name = u"Images/%s.bmp"%count
        FileName=unicodedata.normalize('NFKD',Name).encode('ascii','ignore')
        Success = Grab(FileName)
        if Success!=0:
            print("抓取图像失败")
            break;
        else:
            print("保存图像 --- %s.bmp"%count)
        count+=1
        time.sleep(1)
    Success =  Stop()
    print("关闭相机")
