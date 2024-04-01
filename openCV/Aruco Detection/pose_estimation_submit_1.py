import cv2 as cv
from cv2 import aruco
import numpy as np
from scipy.spatial.transform import Rotation as R

calib_path = "../camera calibration/calibration_data_directory/Matrix.npz"

calib_data = np.load(calib_path)


cam_mat = calib_data["cam_matrix"]
dist_coef = calib_data["dist"]
r_vectors = calib_data["rot_vec"]
t_vectors = calib_data["trans_vec"]

MARKER_SIZE = 8.8

marker_dict = aruco.getPredefinedDictionary(aruco.DICT_4X4_50)

param_markers = aruco.DetectorParameters()

cap = cv.VideoCapture(0)

while True:

    ret, frame = cap.read()
    if not ret:
        break
    gray_frame = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    marker_corners, marker_IDs, reject = aruco.detectMarkers(
        gray_frame, marker_dict, parameters=param_markers
    )   

    if marker_corners:
        rVec, tVec, _ = aruco.estimatePoseSingleMarkers(
            marker_corners, MARKER_SIZE, cam_mat, dist_coef
        )
        total_markers = range(0, marker_IDs.size)

        for id, corners, i in zip(marker_IDs, marker_corners, total_markers):
            cv.polylines(
                frame, [corners.astype(np.int32)], True, (0, 255, 255), 4, cv.LINE_AA
            )
            corners = corners.reshape(4, 2)
            corners = corners.astype(int)
            top_right = corners[0].reshape(-1)
            top_left = corners[1].reshape(-1)
            bottom_right = corners[2].reshape(-1)
            bottom_left = corners[3].reshape(-1)

            distance = np.sqrt(
                tVec[i][0][2] ** 2 + tVec[i][0][0] ** 2 + tVec[i][0][1] ** 2
            )

            rotation_matrix, _ = cv.Rodrigues(rVec[i])
            rotation = R.from_matrix(rotation_matrix)
            rpy = []
            quaternion = []
            rpy = rotation.as_euler("xyz", degrees=True)
            quaternion = rotation.as_quat()

            point = cv.drawFrameAxes(frame, cam_mat, dist_coef, rVec[i], tVec[i], 4, 4)

            cv.putText(
                frame,
                f"id: {id[0]} Dist: {round(distance, 2)}",
                (top_left),  # + [-10, 0]),
                cv.FONT_HERSHEY_DUPLEX,
                0.5,
                (120, 10, 255),
                2,
                cv.LINE_AA,
            )

            cv.putText(
                frame,
                f"x:{round(tVec[i][0][0],1)} y: {-round(tVec[i][0][1],1)} ",
                (bottom_right + [50, 0]),
                cv.FONT_HERSHEY_DUPLEX,
                0.5,
                (120, 50, 255),
                2,
                cv.LINE_AA,
            )

            cv.putText(
                frame,
                f"roll:{round(rpy[0],1)} pitch: {round(rpy[1],1)}, yaw: {round(rpy[2],1)} ",
                (bottom_left + [-50, 0]),
                cv.FONT_HERSHEY_DUPLEX,
                0.5,
                (120, 50, 255),
                2,
                cv.LINE_AA,
            )

            cv.putText(
                frame,
                f"w: {round(quaternion[0],1)}, x:{round(quaternion[1],1)} y:{round(quaternion[2],1)} z:{round(quaternion[3],1)} ",
                (top_right + [-50, 0]),
                cv.FONT_HERSHEY_DUPLEX,
                0.5,
                (120, 50, 255),
                2,
                cv.LINE_AA,
            )

    cv.imshow("frame", frame)
    key = cv.waitKey(1)
    if key == ord("q"):
        break
cap.release()
cv.destroyAllWindows()
