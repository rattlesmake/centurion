from PIL import Image
import numpy as np
import os

def find_height(pil_back, cell_height):
    im1 = np.asarray(pil_back)
    im2 = np.rot90(im1, 2)
    h0 = h1 = 0
    for row in im1:
        if sum(row)[3] == 0:
            h0+=1
        else: 
            break
    for row in im2:
        if sum(row)[3] == 0:
            h1+=1
        else: 
            break
    h1 = cell_height-h1
    return (h0,h1)

def find_width(pil_back, cell_width):
    im1 = np.rot90(np.asarray(pil_back), 1)
    im2 = np.rot90(np.asarray(pil_back), 3)
    w0 = w1 = 0
    for row in im1:
        if sum(row)[3] == 0:
            w0+=1
        else: 
            break
    for row in im2:
        if sum(row)[3] == 0:
            w1+=1
        else: 
            break
    w1 = cell_width-w1
    return (w0,w1)

def fix_image(image_name, frames, directions):
    img_path = os.getcwd() + '\\' + image_name + ".png"
    pil_im = Image.open(img_path)
    im = np.asarray(pil_im.convert('RGBA'))
    cell_width = int(im.shape[1] / frames)
    cell_height = int(im.shape[0] / directions)

    # store data

    image_data = {}
    pil_back = Image.new('RGBA', (cell_width,cell_height), (255, 255, 255, 0))
    back = np.asarray(pil_back)
    back.setflags(write=1)

    for i in range(frames):
        for j in range(directions):
            cell_im = im[(j*cell_height):(j*cell_height+cell_height), (i*cell_width):(i*cell_width+cell_width)]
            cell_im.setflags(write=1)
            image_data[(j,i)] = cell_im
            pil_img = Image.fromarray(cell_im)
            pil_back.paste(pil_img, (0, 0), pil_img)
            
    # crop cells

    h0, h1 = find_height(pil_back, cell_height)
    w0, w1 = find_width(pil_back, cell_width)
    new_cell_width = w1 - w0
    new_cell_height = h1 - h0

    for (k,v) in image_data.items():
        image_data[k]=v[h0:h1,w0:w1]
        
    # CREATE FINAL SPRITE

    new_width = new_cell_width * frames
    new_height = new_cell_height * directions

    back_image = np.asarray(Image.new('RGBA', (new_width,new_height), (255, 255, 255, 0)))
    back_image.setflags(write=1)        

    for i in range(frames):
        for j in range(directions):
            back_image[(j*new_cell_height):(j*new_cell_height+new_cell_height), (i*new_cell_width):(i*new_cell_width+new_cell_width)] = image_data[(j,i)]

    return Image.fromarray(back_image)

if __name__=="__main__":
    try:
        image_name = input('png name (without .png extension): ')
        frames = int(input('n frames: '))
        directions = int(input('n directions: '))

        image = fix_image(image_name, frames, directions)
        image.save(image_name+'_new.png', 'PNG', optimize=True)
        
    except Exception as e: print(e)