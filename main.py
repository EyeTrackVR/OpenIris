import math

import cv2 as cv
import threading
import numpy as np
import random as rng
from matplotlib import pyplot as plt


class ThreadedCamera:
    def __init__(self, camera_index=0):
        self.cam = cv.VideoCapture(camera_index)
        self.status = False
        self.frame = None
        if not self.cam.isOpened():
            raise Exception("Could not connect to a camera")

        self.cam.set(cv.CAP_PROP_BUFFERSIZE, 3)
        self.camera_thread = threading.Thread(target=self.update, args=(), daemon=True)
        self.camera_thread.start()

    def update(self):
        while True:
            ret, frame = self.cam.read()
            if not ret:
                print("something went wrong with reading frame, exiting")
                break

            self.status, self.frame = ret, frame


def display_frame(camera):
    if camera.frame is not None:
        image = cv.resize(
            camera.frame,
            dsize=(int(camera.frame.shape[1]), int(camera.frame.shape[0])),
        )
        # image = cv.imread("testImages/eye2.png")
        grey = cv.cvtColor(cv.flip(image, 0), cv.COLOR_BGR2GRAY)
        thresholded = cv.adaptiveThreshold(
            grey, 255, cv.ADAPTIVE_THRESH_GAUSSIAN_C, cv.THRESH_BINARY_INV, 27, 7
        )

        kernel = np.ones((2, 2))
        eroded = cv.erode(thresholded, kernel, iterations=1)
        dilated = cv.dilate(thresholded, kernel, iterations=1)

        canny = cv.Canny(dilated, threshold1=100, threshold2=300)

        detector = cv.ORB_create()
        # Initiate ORB detector
        orb = cv.ORB_create()
        # find the keypoints with ORB
        kp = orb.detect(canny, None)
        # compute the descriptors with ORB
        kp, des = orb.compute(thresholded, kp)

        img_with_keypoints = cv.drawKeypoints(grey, kp, None, color=(0, 255, 0), flags=0)
        cv.imshow("thresholded", thresholded)
        cv.imshow("eroded", eroded)
        cv.imshow("dialted", dilated)
        cv.imshow("canny", canny)
        cv.imshow("features", img_with_keypoints)

        fps = camera.cam.get(cv.CAP_PROP_FPS)
        print(
            "Frames per second using video.get(cv2.CAP_PROP_FPS) : {0}".format(fps)
        )
        cv.imshow("test", image)
        if cv.waitKey(1) == ord("q"):
            exit()


def main():
    eye_cascade = cv.CascadeClassifier('haarcascade_eye.xml')
    camera = ThreadedCamera("http://192.168.1.43:81/")

    while True:
        display_frame(camera, eye_cascade)
        if cv.waitKey(1) == ord("q"):
            exit()


if __name__ == "__main__":
    main()
