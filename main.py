import cv2 as cv
import threading
import numpy as np


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

	def display_frame(self):
		if self.frame is not None:
			image = cv.cvtColor(self.frame, cv.COLOR_BGR2GRAY)
			image = cv.resize(image, dsize=(int(self.frame.shape[1]/3), int(self.frame.shape[0]/3)))
			cv.imshow("frame", image)
			fps = self.cam.get(cv.CAP_PROP_FPS)
			print("Frames per second using video.get(cv2.CAP_PROP_FPS) : {0}".format(fps))
			if cv.waitKey(1) == ord("q"):
				exit()


def main():
	camera = ThreadedCamera(0)
	while True:
		camera.display_frame()


if __name__ == "__main__":
	main()
