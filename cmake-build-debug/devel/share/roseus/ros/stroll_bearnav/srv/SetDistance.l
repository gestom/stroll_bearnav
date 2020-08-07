;; Auto-generated. Do not edit!


(when (boundp 'stroll_bearnav::SetDistance)
  (if (not (find-package "STROLL_BEARNAV"))
    (make-package "STROLL_BEARNAV"))
  (shadow 'SetDistance (find-package "STROLL_BEARNAV")))
(unless (find-package "STROLL_BEARNAV::SETDISTANCE")
  (make-package "STROLL_BEARNAV::SETDISTANCE"))
(unless (find-package "STROLL_BEARNAV::SETDISTANCEREQUEST")
  (make-package "STROLL_BEARNAV::SETDISTANCEREQUEST"))
(unless (find-package "STROLL_BEARNAV::SETDISTANCERESPONSE")
  (make-package "STROLL_BEARNAV::SETDISTANCERESPONSE"))

(in-package "ROS")





(defclass stroll_bearnav::SetDistanceRequest
  :super ros::object
  :slots (_distance ))

(defmethod stroll_bearnav::SetDistanceRequest
  (:init
   (&key
    ((:distance __distance) 0.0)
    )
   (send-super :init)
   (setq _distance (float __distance))
   self)
  (:distance
   (&optional __distance)
   (if __distance (setq _distance __distance)) _distance)
  (:serialization-length
   ()
   (+
    ;; float32 _distance
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _distance
       (sys::poke _distance (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _distance
     (setq _distance (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(defclass stroll_bearnav::SetDistanceResponse
  :super ros::object
  :slots (_distance ))

(defmethod stroll_bearnav::SetDistanceResponse
  (:init
   (&key
    ((:distance __distance) 0.0)
    )
   (send-super :init)
   (setq _distance (float __distance))
   self)
  (:distance
   (&optional __distance)
   (if __distance (setq _distance __distance)) _distance)
  (:serialization-length
   ()
   (+
    ;; float32 _distance
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _distance
       (sys::poke _distance (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _distance
     (setq _distance (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(defclass stroll_bearnav::SetDistance
  :super ros::object
  :slots ())

(setf (get stroll_bearnav::SetDistance :md5sum-) "3ff5ee2fe3eb1f4b34a6e4483099be55")
(setf (get stroll_bearnav::SetDistance :datatype-) "stroll_bearnav/SetDistance")
(setf (get stroll_bearnav::SetDistance :request) stroll_bearnav::SetDistanceRequest)
(setf (get stroll_bearnav::SetDistance :response) stroll_bearnav::SetDistanceResponse)

(defmethod stroll_bearnav::SetDistanceRequest
  (:response () (instance stroll_bearnav::SetDistanceResponse :init)))

(setf (get stroll_bearnav::SetDistanceRequest :md5sum-) "3ff5ee2fe3eb1f4b34a6e4483099be55")
(setf (get stroll_bearnav::SetDistanceRequest :datatype-) "stroll_bearnav/SetDistanceRequest")
(setf (get stroll_bearnav::SetDistanceRequest :definition-)
      "float32 distance
---
float32 distance

")

(setf (get stroll_bearnav::SetDistanceResponse :md5sum-) "3ff5ee2fe3eb1f4b34a6e4483099be55")
(setf (get stroll_bearnav::SetDistanceResponse :datatype-) "stroll_bearnav/SetDistanceResponse")
(setf (get stroll_bearnav::SetDistanceResponse :definition-)
      "float32 distance
---
float32 distance

")



(provide :stroll_bearnav/SetDistance "3ff5ee2fe3eb1f4b34a6e4483099be55")


