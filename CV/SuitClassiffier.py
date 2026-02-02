import os
os.environ["KERAS_BACKEND"] = "torch"
import keras
from keras.layers import Conv2D,BatchNormalization,MaxPool2D,Dropout,Flatten,Dense
from keras import Sequential
from keras.utils import to_categorical
from keras.optimizers import RMSprop, SGD
# from keras.preprocessing.image import ImageDataGenerator
import pandas as pd
from sklearn.model_selection import train_test_split
import numpy as np
import cv2 as cv

df1 = pd.read_csv('SuitsRDataset/Train.csv')
df2 = pd.read_csv('SuitsRDataset/Test.csv')


Y_train = [1 if i=="Heart" else 0 for i in df1["Label"]]
X_train = df1.drop(labels = ["Label"],axis = 1).values

X_test = df2.drop(labels = ["Label"],axis = 1)
Y_test= [1 if i=="Heart" else 0 for i in df1["Label"]]

X_train = X_train.reshape(-1,28,28)
X_test = X_test.values.reshape(-1,28,28,1)


# Y_train = to_categorical(Y_train, num_classes = 2)
Y_train=np.array(Y_train)

X_train, X_val, Y_train, Y_val = train_test_split(X_train, Y_train, test_size = 0.1, random_state=35)


model = Sequential()
model.add(Conv2D(filters=32, kernel_size=(5,5), padding='Same', activation='relu', input_shape=(28,28,1)))
model.add(BatchNormalization())
model.add(Conv2D(filters=32, kernel_size=(5,5), padding='Same', activation='relu'))
model.add(BatchNormalization())
model.add(MaxPool2D(pool_size=(2,2)))
model.add(Dropout(0.25))
model.add(Flatten())
model.add(Dense(256, activation='relu'))
model.add(BatchNormalization())
model.add(Dropout(0.5))
model.add(Dense(1, activation='sigmoid'))

optimizer = RMSprop(learning_rate=0.001, rho=0.9, epsilon=1e-08)
# Compiling our model
model.compile(optimizer = optimizer , loss = "binary_crossentropy", metrics=["accuracy"])
model.fit(X_train, Y_train, batch_size=64, epochs=5, validation_data = (X_val, Y_val))
# model.summary()
results = model.predict(X_test)
res=[ 1 if i[0]>=0.5 else 0   for i in results]
print(np.array_equal(np.array(res),np.array(Y_test)))
img = keras.utils.load_img("diamond.jpg", target_size=(28,28),color_mode="grayscale")
# plt.imshow(img)

img_array = keras.utils.img_to_array(img)
# img_array = keras.ops.expand_dims(img_array, 0)
img_array=img_array.reshape(1,28,28,1)
# cv.imshow("test",img_array)
results = model.predict(img_array)
print("Diamond")
print(results)
print([ "Heart" if i[0]>=0.5 else "Diamond"  for i in results])
print([np.argmax(i)  for i in results])

img = keras.utils.load_img("heart.jpg", target_size=(28,28),color_mode="grayscale")

img_array = keras.utils.img_to_array(img)
img_array=img_array.reshape(1,28,28,1)

results = model.predict(img_array)
print("Heart")
print(results)
print([ "Heart" if i[0]>=0.5 else "Diamond"  for i in results])


img = keras.utils.load_img("test.jpg", target_size=(28,28),color_mode="grayscale")

img_array = keras.utils.img_to_array(img)
img_array=img_array.reshape(1,28,28,1)
results = model.predict(img_array)
print("Heart")
print(results)
print([ "Heart" if i[0]>=0.5 else "Diamond"  for i in results])