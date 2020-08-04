;; Auto-generated. Do not edit!


(when (boundp 'stroll_bearnav::FeatureArray)
  (if (not (find-package "STROLL_BEARNAV"))
    (make-package "STROLL_BEARNAV"))
  (shadow 'FeatureArray (find-package "STROLL_BEARNAV")))
(unless (find-package "STROLL_BEARNAV::FEATUREARRAY")
  (make-package "STROLL_BEARNAV::FEATUREARRAY"))

(in-package "ROS")
;;//! \htmlinclude FeatureArray.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass stroll_bearnav::FeatureArray
  :super ros::object
  :slots (_header _distance _id _feature ))

(defmethod stroll_bearnav::FeatureArray
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:distance __distance) 0.0)
    ((:id __id) "")
    ((:feature __feature) (let (r) (dotimes (i 0) (push (instance stroll_bearnav::Feature :init) r)) r))
    )
   (send-super :init)
   (setq _header __header)
   (setq _distance (float __distance))
   (setq _id (string __id))
   (setq _feature __feature)
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:distance
   (&optional __distance)
   (if __distance (setq _distance __distance)) _distance)
  (:id
   (&optional __id)
   (if __id (setq _id __id)) _id)
  (:feature
   (&rest __feature)
   (if (keywordp (car __feature))
       (send* _feature __feature)
     (progn
       (if __feature (setq _feature (car __feature)))
       _feature)))
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; float32 _distance
    4
    ;; string _id
    4 (length _id)
    ;; stroll_bearnav/Feature[] _feature
    (apply #'+ (send-all _feature :serialization-length)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; float32 _distance
       (sys::poke _distance (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; string _id
       (write-long (length _id) s) (princ _id s)
     ;; stroll_bearnav/Feature[] _feature
     (write-long (length _feature) s)
     (dolist (elem _feature)
       (send elem :serialize s)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; float32 _distance
     (setq _distance (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; string _id
     (let (n) (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4) (setq _id (subseq buf ptr- (+ ptr- n))) (incf ptr- n))
   ;; stroll_bearnav/Feature[] _feature
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _feature (let (r) (dotimes (i n) (push (instance stroll_bearnav::Feature :init) r)) r))
     (dolist (elem- _feature)
     (send elem- :deserialize buf ptr-) (incf ptr- (send elem- :serialization-length))
     ))
   ;;
   self)
  )

(setf (get stroll_bearnav::FeatureArray :md5sum-) "74b52b411857590176b1f86a6b4897da")
(setf (get stroll_bearnav::FeatureArray :datatype-) "stroll_bearnav/FeatureArray")
(setf (get stroll_bearnav::FeatureArray :definition-)
      "Header header
float32 distance
string id 
stroll_bearnav/Feature[] feature

================================================================================
MSG: std_msgs/Header
# Standard metadata for higher-level stamped data types.
# This is generally used to communicate timestamped data 
# in a particular coordinate frame.
# 
# sequence ID: consecutively increasing ID 
uint32 seq
#Two-integer timestamp that is expressed as:
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
# time-handling sugar is provided by the client library
time stamp
#Frame this data is associated with
# 0: no frame
# 1: global frame
string frame_id

================================================================================
MSG: stroll_bearnav/Feature
float32 x
float32 y
float32 size
float32 angle
float32 response
int32 octave
int32 class_id
float32[] descriptor
float32 rating

")



(provide :stroll_bearnav/FeatureArray "74b52b411857590176b1f86a6b4897da")


