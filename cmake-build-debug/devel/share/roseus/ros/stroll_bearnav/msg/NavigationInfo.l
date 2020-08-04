;; Auto-generated. Do not edit!


(when (boundp 'stroll_bearnav::NavigationInfo)
  (if (not (find-package "STROLL_BEARNAV"))
    (make-package "STROLL_BEARNAV"))
  (shadow 'NavigationInfo (find-package "STROLL_BEARNAV")))
(unless (find-package "STROLL_BEARNAV::NAVIGATIONINFO")
  (make-package "STROLL_BEARNAV::NAVIGATIONINFO"))

(in-package "ROS")
;;//! \htmlinclude NavigationInfo.msg.html


(defclass stroll_bearnav::NavigationInfo
  :super ros::object
  :slots (_ratio _map _view _histogram _mapMatchIndex _mapMatchEval _correct _matches _distance _diffRot _mapChanges _updated ))

(defmethod stroll_bearnav::NavigationInfo
  (:init
   (&key
    ((:ratio __ratio) 0.0)
    ((:map __map) (instance stroll_bearnav::FeatureArray :init))
    ((:view __view) (instance stroll_bearnav::FeatureArray :init))
    ((:histogram __histogram) (make-array 0 :initial-element 0 :element-type :integer))
    ((:mapMatchIndex __mapMatchIndex) (make-array 0 :initial-element 0 :element-type :integer))
    ((:mapMatchEval __mapMatchEval) (make-array 0 :initial-element 0 :element-type :integer))
    ((:correct __correct) 0)
    ((:matches __matches) 0)
    ((:distance __distance) 0.0)
    ((:diffRot __diffRot) 0.0)
    ((:mapChanges __mapChanges) 0)
    ((:updated __updated) nil)
    )
   (send-super :init)
   (setq _ratio (float __ratio))
   (setq _map __map)
   (setq _view __view)
   (setq _histogram __histogram)
   (setq _mapMatchIndex __mapMatchIndex)
   (setq _mapMatchEval __mapMatchEval)
   (setq _correct (round __correct))
   (setq _matches (round __matches))
   (setq _distance (float __distance))
   (setq _diffRot (float __diffRot))
   (setq _mapChanges (round __mapChanges))
   (setq _updated __updated)
   self)
  (:ratio
   (&optional __ratio)
   (if __ratio (setq _ratio __ratio)) _ratio)
  (:map
   (&rest __map)
   (if (keywordp (car __map))
       (send* _map __map)
     (progn
       (if __map (setq _map (car __map)))
       _map)))
  (:view
   (&rest __view)
   (if (keywordp (car __view))
       (send* _view __view)
     (progn
       (if __view (setq _view (car __view)))
       _view)))
  (:histogram
   (&optional __histogram)
   (if __histogram (setq _histogram __histogram)) _histogram)
  (:mapMatchIndex
   (&optional __mapMatchIndex)
   (if __mapMatchIndex (setq _mapMatchIndex __mapMatchIndex)) _mapMatchIndex)
  (:mapMatchEval
   (&optional __mapMatchEval)
   (if __mapMatchEval (setq _mapMatchEval __mapMatchEval)) _mapMatchEval)
  (:correct
   (&optional __correct)
   (if __correct (setq _correct __correct)) _correct)
  (:matches
   (&optional __matches)
   (if __matches (setq _matches __matches)) _matches)
  (:distance
   (&optional __distance)
   (if __distance (setq _distance __distance)) _distance)
  (:diffRot
   (&optional __diffRot)
   (if __diffRot (setq _diffRot __diffRot)) _diffRot)
  (:mapChanges
   (&optional __mapChanges)
   (if __mapChanges (setq _mapChanges __mapChanges)) _mapChanges)
  (:updated
   (&optional __updated)
   (if __updated (setq _updated __updated)) _updated)
  (:serialization-length
   ()
   (+
    ;; float32 _ratio
    4
    ;; stroll_bearnav/FeatureArray _map
    (send _map :serialization-length)
    ;; stroll_bearnav/FeatureArray _view
    (send _view :serialization-length)
    ;; int32[] _histogram
    (* 4    (length _histogram)) 4
    ;; int32[] _mapMatchIndex
    (* 4    (length _mapMatchIndex)) 4
    ;; int32[] _mapMatchEval
    (* 4    (length _mapMatchEval)) 4
    ;; int32 _correct
    4
    ;; int32 _matches
    4
    ;; float32 _distance
    4
    ;; float32 _diffRot
    4
    ;; int32 _mapChanges
    4
    ;; bool _updated
    1
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _ratio
       (sys::poke _ratio (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; stroll_bearnav/FeatureArray _map
       (send _map :serialize s)
     ;; stroll_bearnav/FeatureArray _view
       (send _view :serialize s)
     ;; int32[] _histogram
     (write-long (length _histogram) s)
     (dotimes (i (length _histogram))
       (write-long (elt _histogram i) s)
       )
     ;; int32[] _mapMatchIndex
     (write-long (length _mapMatchIndex) s)
     (dotimes (i (length _mapMatchIndex))
       (write-long (elt _mapMatchIndex i) s)
       )
     ;; int32[] _mapMatchEval
     (write-long (length _mapMatchEval) s)
     (dotimes (i (length _mapMatchEval))
       (write-long (elt _mapMatchEval i) s)
       )
     ;; int32 _correct
       (write-long _correct s)
     ;; int32 _matches
       (write-long _matches s)
     ;; float32 _distance
       (sys::poke _distance (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _diffRot
       (sys::poke _diffRot (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; int32 _mapChanges
       (write-long _mapChanges s)
     ;; bool _updated
       (if _updated (write-byte -1 s) (write-byte 0 s))
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _ratio
     (setq _ratio (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; stroll_bearnav/FeatureArray _map
     (send _map :deserialize buf ptr-) (incf ptr- (send _map :serialization-length))
   ;; stroll_bearnav/FeatureArray _view
     (send _view :deserialize buf ptr-) (incf ptr- (send _view :serialization-length))
   ;; int32[] _histogram
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _histogram (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _histogram i) (sys::peek buf ptr- :integer)) (incf ptr- 4)
     ))
   ;; int32[] _mapMatchIndex
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _mapMatchIndex (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _mapMatchIndex i) (sys::peek buf ptr- :integer)) (incf ptr- 4)
     ))
   ;; int32[] _mapMatchEval
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _mapMatchEval (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _mapMatchEval i) (sys::peek buf ptr- :integer)) (incf ptr- 4)
     ))
   ;; int32 _correct
     (setq _correct (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _matches
     (setq _matches (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; float32 _distance
     (setq _distance (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _diffRot
     (setq _diffRot (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; int32 _mapChanges
     (setq _mapChanges (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; bool _updated
     (setq _updated (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;;
   self)
  )

(setf (get stroll_bearnav::NavigationInfo :md5sum-) "82d671060d2954f62b3d0cce462cfb6c")
(setf (get stroll_bearnav::NavigationInfo :datatype-) "stroll_bearnav/NavigationInfo")
(setf (get stroll_bearnav::NavigationInfo :definition-)
      "float32 ratio                           #matching ratio
stroll_bearnav/FeatureArray map         #features from the current map
stroll_bearnav/FeatureArray view	#features from the current view
int32[] histogram    			#navigation histogram
int32[] mapMatchIndex			#contains indices of 'view' features corresponding to a given mapFeature
int32[] mapMatchEval			#contains information of mapFeatures - correct 1, incorrect -1, unmatched 0
int32 correct                   	#number of correct matches
int32 matches                   	#number of good matches
float32 distance                	#traveled distance
float32 diffRot                 	#difference between features
int32 mapChanges                	#number of traversals
bool updated                    	#map contains rating

================================================================================
MSG: stroll_bearnav/FeatureArray
Header header
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



(provide :stroll_bearnav/NavigationInfo "82d671060d2954f62b3d0cce462cfb6c")


