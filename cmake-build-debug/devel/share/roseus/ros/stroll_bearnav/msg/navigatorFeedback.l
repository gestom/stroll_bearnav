;; Auto-generated. Do not edit!


(when (boundp 'stroll_bearnav::navigatorFeedback)
  (if (not (find-package "STROLL_BEARNAV"))
    (make-package "STROLL_BEARNAV"))
  (shadow 'navigatorFeedback (find-package "STROLL_BEARNAV")))
(unless (find-package "STROLL_BEARNAV::NAVIGATORFEEDBACK")
  (make-package "STROLL_BEARNAV::NAVIGATORFEEDBACK"))

(in-package "ROS")
;;//! \htmlinclude navigatorFeedback.msg.html


(defclass stroll_bearnav::navigatorFeedback
  :super ros::object
  :slots (_distance _diffRot _histogram _max _correct _outliers _keypoints_avg _matches _stats ))

(defmethod stroll_bearnav::navigatorFeedback
  (:init
   (&key
    ((:distance __distance) 0.0)
    ((:diffRot __diffRot) 0.0)
    ((:histogram __histogram) (make-array 0 :initial-element 0 :element-type :integer))
    ((:max __max) 0)
    ((:correct __correct) 0)
    ((:outliers __outliers) 0)
    ((:keypoints_avg __keypoints_avg) 0)
    ((:matches __matches) 0)
    ((:stats __stats) (make-array 0 :initial-element 0 :element-type :integer))
    )
   (send-super :init)
   (setq _distance (float __distance))
   (setq _diffRot (float __diffRot))
   (setq _histogram __histogram)
   (setq _max (round __max))
   (setq _correct (round __correct))
   (setq _outliers (round __outliers))
   (setq _keypoints_avg (round __keypoints_avg))
   (setq _matches (round __matches))
   (setq _stats __stats)
   self)
  (:distance
   (&optional __distance)
   (if __distance (setq _distance __distance)) _distance)
  (:diffRot
   (&optional __diffRot)
   (if __diffRot (setq _diffRot __diffRot)) _diffRot)
  (:histogram
   (&optional __histogram)
   (if __histogram (setq _histogram __histogram)) _histogram)
  (:max
   (&optional __max)
   (if __max (setq _max __max)) _max)
  (:correct
   (&optional __correct)
   (if __correct (setq _correct __correct)) _correct)
  (:outliers
   (&optional __outliers)
   (if __outliers (setq _outliers __outliers)) _outliers)
  (:keypoints_avg
   (&optional __keypoints_avg)
   (if __keypoints_avg (setq _keypoints_avg __keypoints_avg)) _keypoints_avg)
  (:matches
   (&optional __matches)
   (if __matches (setq _matches __matches)) _matches)
  (:stats
   (&optional __stats)
   (if __stats (setq _stats __stats)) _stats)
  (:serialization-length
   ()
   (+
    ;; float32 _distance
    4
    ;; float32 _diffRot
    4
    ;; int32[] _histogram
    (* 4    (length _histogram)) 4
    ;; int32 _max
    4
    ;; int32 _correct
    4
    ;; int32 _outliers
    4
    ;; int32 _keypoints_avg
    4
    ;; int32 _matches
    4
    ;; int32[] _stats
    (* 4    (length _stats)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _distance
       (sys::poke _distance (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _diffRot
       (sys::poke _diffRot (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; int32[] _histogram
     (write-long (length _histogram) s)
     (dotimes (i (length _histogram))
       (write-long (elt _histogram i) s)
       )
     ;; int32 _max
       (write-long _max s)
     ;; int32 _correct
       (write-long _correct s)
     ;; int32 _outliers
       (write-long _outliers s)
     ;; int32 _keypoints_avg
       (write-long _keypoints_avg s)
     ;; int32 _matches
       (write-long _matches s)
     ;; int32[] _stats
     (write-long (length _stats) s)
     (dotimes (i (length _stats))
       (write-long (elt _stats i) s)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _distance
     (setq _distance (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _diffRot
     (setq _diffRot (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; int32[] _histogram
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _histogram (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _histogram i) (sys::peek buf ptr- :integer)) (incf ptr- 4)
     ))
   ;; int32 _max
     (setq _max (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _correct
     (setq _correct (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _outliers
     (setq _outliers (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _keypoints_avg
     (setq _keypoints_avg (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _matches
     (setq _matches (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32[] _stats
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _stats (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _stats i) (sys::peek buf ptr- :integer)) (incf ptr- 4)
     ))
   ;;
   self)
  )

(setf (get stroll_bearnav::navigatorFeedback :md5sum-) "7b676e19f8b4915c865123ddcb157234")
(setf (get stroll_bearnav::navigatorFeedback :datatype-) "stroll_bearnav/navigatorFeedback")
(setf (get stroll_bearnav::navigatorFeedback :definition-)
      "# ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======
#feedback
float32 distance
float32 diffRot
int32[] histogram
int32 max
int32 correct
int32 outliers
int32 keypoints_avg
int32 matches
int32[] stats


")



(provide :stroll_bearnav/navigatorFeedback "7b676e19f8b4915c865123ddcb157234")


