from PIL import Image
import numpy as np

def gridCreate():
    try:
        fileName = input('Write the "Grid" file name with .png extension: ').strip()
        objName = input('Write the building class name: ').strip() 
        img = Image.open(fileName, 'r')
        data = np.array(img)
        data = np.array(img.split()[-1])
        width = data.shape[1]
        height = data.shape[0]
        out_data = []
        for j in range(round(height/20)): 
            d = []
            for i in range(round(width/20)):
                d += [int(data[j*20, i*20]/255)]
            out_data += [d]
        f = open(objName+".grid", "w")
        for j in range(len(out_data)):
            for i in range(len(out_data[j])):
                f.write(str(out_data[j][i]))
                if (i < len(out_data[j]) - 1):
                    f.write(",")
            f.write("\n")
        f.close()
    
    except Exception as e: 
        input(e)

if __name__=="__main__":
    gridCreate()
    input('Press Enter to close.')