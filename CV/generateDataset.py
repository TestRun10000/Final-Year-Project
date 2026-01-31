
import os
import random
from csv import writer
import numpy as np
from PIL import Image, ImageDraw, ImageFont

suitsR={
    "Heart":"♥",
    "Diamond":"♦"

}
suitsB={
    "Club":"♣",
    "Spade":"♠"
}
ranks={
    "13":"K",
    "12":"Q",
    "11":"J",
    "10":"10",
    "9":"9",
    "8":"8",
    "7":"7",
    "6":"6",
    "5":"5",
    "4":"4",
    "3":"3",
    "2":"2",
    "1":"A",
}
fonts=[
    "C:/Windows/Fonts/arial.ttf",
]



def createImage(text,pos,font):
    img=Image.new("1",(28,28),color='black')
    drawing=ImageDraw.Draw(img)
    drawing.text(pos,text,fill=1,font=font)
    return img

def genFonts(index,size):
    return ImageFont.truetype(fonts[index],size)


def dataGen(numberOfImages,data:dict[str,str],basePos:tuple[int,int],fontSizeRange:tuple[int,int],shift:tuple[int,int],filePath):
    f=open(filePath,"w+",newline='',encoding="utf-8")
    header=["pixle"+str(i-1) if i>0  else "Label" for i in range((28*28)+1)]
    write=writer(f)
    write.writerow(header)
    for i in range(numberOfImages):
        for label, text in data.items():
            fontSize=random.randint(fontSizeRange[0],fontSizeRange[1])
            xShift=random.randint(shift[0],shift[1])
            yShift=random.randint(shift[0],shift[1])
            im=createImage(text,(basePos[0]+xShift,basePos[1]+yShift),genFonts(0,fontSize))
            im=np.array(im,dtype=np.int64).flatten()
            x=np.array([[label]],dtype=object).flatten()
            im=np.append(arr=x,values=im,axis=0)
            write.writerow(im)
    f.close()

rankStarSize=(2,-4)

if not os.path.exists("SuitsRDataset"):
    os.makedirs("SuitsRDataset")
if not os.path.exists("SuitsBDataset"):
    os.makedirs("SuitsBDataset")

dataGen(400,suitsR,(2,-8),(30,40),(0,10),"SuitsRDataset/Train.csv")
dataGen(400,suitsB,(2,-8),(30,40),(0,10),"SuitsBDataset/Train.csv")
dataGen(100,suitsR,(2,-8),(30,40),(0,10),"SuitsRDataset/Test.csv")
dataGen(100,suitsB,(2,-8),(30,40),(0,10),"SuitsBDataset/Test.csv")

if not os.path.exists("RankDataset"):
    os.makedirs("RankDataset")
dataGen(800,ranks,(2,-4),(30,40),(0,10),"RankDataset/Train.csv")
dataGen(200,ranks,(2,-4),(30,40),(0,10),"RankDataset/Test.csv")
