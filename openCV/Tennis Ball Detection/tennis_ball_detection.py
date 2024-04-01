import cv2 as cv
import numpy as np

# Funda: Hough circles to identify circles
# Mask to isolate yellow
# https://gigl.scs.carleton.ca/sites/default/files/chujia_wei/background_subtraction_for_realtime_tracking_of_a_tennis_ball.pdf
# ^^ something called background subtraction(look into it later)

# Vipul tennis ball HSVs: H->35,99 S->70,149 V->086,250

hue_min = 35
sat_min = 70
val_min = 86

hue_max = 99
sat_max = 149
val_max = 250


# Trackbars to tweak HSV on the fly
cv.namedWindow("HSV_Trackbar")


def onTrack1(val):
    global hue_min
    hue_min = val


def onTrack2(val):
    global hue_max
    hue_max = val


def onTrack3(val):
    global sat_min
    sat_min = val


def onTrack4(val):
    global sat_max
    sat_max = val


def onTrack5(val):
    global val_min
    val_min = val


def onTrack6(val):
    global val_max
    val_max = val


cv.createTrackbar("Hue Min", "HSV_Trackbar", 10, 179, onTrack1)
cv.createTrackbar("Hue Max", "HSV_Trackbar", 20, 179, onTrack2)
cv.createTrackbar("Sat Min", "HSV_Trackbar", 10, 255, onTrack3)
cv.createTrackbar("Sat Max", "HSV_Trackbar", 250, 255, onTrack4)
cv.createTrackbar("Val Min", "HSV_Trackbar", 10, 255, onTrack5)
cv.createTrackbar("Val Max", "HSV_Trackbar", 250, 255, onTrack6)

prevCircle = None
# macro func
dist = lambda x1, y1, x2, y2: ((x1 - x2) ** 2) + ((y1 - y2) ** 2)

cap = cv.VideoCapture(0)

while True:
    ret, frame = cap.read()

    lower_bound = np.array([[[hue_min, sat_min, val_min]]])
    upper_bound = np.array([[[hue_max, sat_max, val_max]]])

    frame = cv.GaussianBlur(frame, (23, 23), 0)
    frame = cv.medianBlur(frame, 15, 0)

    hsv_frame = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
    hsv_frame = cv.inRange(hsv_frame, lower_bound, upper_bound)

    hsv_frame = cv.erode(hsv_frame, None, iterations=2)
    hsv_frame = cv.dilate(hsv_frame, None, iterations=2)

    hsv_frame = cv.GaussianBlur(hsv_frame, (15, 15), 0)

    combined_frame = cv.bitwise_and(frame, frame, mask=hsv_frame)

    circles = cv.HoughCircles(
        hsv_frame,
        cv.HOUGH_GRADIENT,
        1.175,
        50,
        param1=75,
        param2=30,
        minRadius=10,
        maxRadius=200,
    )
    if circles is not None:
        circles = np.uint16(np.around(circles))
        chosen = None
        for i in circles[0, :]:
            if chosen is None:
                chosen = i
            if prevCircle is not None:
                if dist(chosen[0], chosen[1], prevCircle[0], prevCircle[1]) <= dist(
                    i[0], i[1], prevCircle[0], prevCircle[1]
                ):
                    chosen = i

        cv.circle(frame, (chosen[0], chosen[1]), 1, (0, 100, 100), 3)
        cv.circle(frame, (chosen[0], chosen[1]), chosen[2], (255, 0, 255), 3)
        prevCircle = chosen

    cv.imshow("circles", frame)
    cv.imshow("masked feed", combined_frame)

    if cv.waitKey(1) == ord("q"):
        break

cap.release()
cv.destroyAllWindows()
