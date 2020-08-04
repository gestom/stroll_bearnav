;; Auto-generated. Do not edit!


(when (boundp 'stroll_bearnav::Feature)
  (if (not (find-package "STROLL_BEARNAV"))
    (make-package "STROLL_BEARNAV"))
  (shadow 'Feature (find-package "STROLL_BEARNAV")))
(unless (find-package "STROLL_BEARNAV::FEATURE")
  (make-package "STROLL_BEARNAV::FEATURE"))

(in-package "ROS")
;;//! \htmlinclude Feature.msg.html


(defclass stroll_bearnav::Feature
  :super ros::object
  :slots (_x _y _size _angle _response _octave _class_id _descriptor _rating ))

(defmethod stroll_bearnav::Feature
  (:init
   (&key
    ((:x __x) 0.0)
    ((:y __y) 0.0)
    ((:size __size) 0.0)
    ((:angle __angle) 0.0)
    ((:response __response) 0.0)
    ((:octave __octave) 0)
    ((:class_id __class_id) 0)
    ((:descriptor __descriptor) (make-array 0 :initial-element 0.0 :element-type :float))
    ((:rating __rating) 0.0)
    )
   (send-super :init)
   (setq _x (float __x))
   (setq _y (float __y))
   (setq _size (float __size))
   (setq _angle (float __angle))
   (setq _response (float __response))
   (setq _octave (round __octave))
   (setq _class_id (round __class_id))
   (setq _descriptor __descriptor)
   (setq _rating (float __rating))
   self)
  (:x
   (&optional __x)
   (if __x (setq _x __x)) _x)
  (:y
   (&optional __y)
   (if __y (setq _y __y)) _y)
  (:size
   (&optional __size)
   (if __size (setq _size __size)) _size)
  (:angle
   (&optional __angle)
   (if __angle (setq _angle __angle)) _angle)
  (:response
   (&optional __response)
   (if __response (setq _response __response)) _response)
  (:octave
   (&optional __octave)
   (if __octave (setq _octave __octave)) _octave)
  (:class_id
   (&optional __class_id)
   (if __class_id (setq _class_id __class_id)) _class_id)
  (:descriptor
   (&optional __descriptor)
   (if __descriptor (setq _descriptor __descriptor)) _descriptor)
  (:rating
   (&optional __rating)
   (if __rating (setq _rating __rating)) _rating)
  (:serialization-length
   ()
   (+
    ;; float32 _x
    4
    ;; float32 _y
    4
    ;; float32 _size
    4
    ;; float32 _angle
    4
    ;; float32 _response
    4
    ;; int32 _octave
    4
    ;; int32 _class_id
    4
    ;; float32[] _descriptor
    (* 4    (length _descriptor)) 4
    ;; float32 _rating
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _x
       (sys::poke _x (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _y
       (sys::poke _y (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _size
       (sys::poke _size (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _angle
       (sys::poke _angle (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _response
       (sys::poke _response (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; int32 _octave
       (write-long _octave s)
     ;; int32 _class_id
       (write-long _class_id s)
     ;; float32[] _descriptor
     (write-long (length _descriptor) s)
     (dotimes (i (length _descriptor))
       (sys::poke (elt _descriptor i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;; float32 _rating
       (sys::poke _rating (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _x
     (setq _x (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _y
     (setq _y (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _size
     (setq _size (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _angle
     (setq _angle (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _response
     (setq _response (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; int32 _octave
     (setq _octave (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _class_id
     (setq _class_id (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; float32[] _descriptor
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _descriptor (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _descriptor i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;; float32 _rating
     (setq _rating (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get stroll_bearnav::Feature :md5sum-) "4fee28b7b26e4a1bbd92f2b4f4726cd5")
(setf (get stroll_bearnav::Feature :datatype-) "stroll_bearnav/Feature")
(setf (get stroll_bearnav::Feature :definition-)
      "float32 x
float32 y
float32 size
float32 angle
float32 response
int32 octave
int32 class_id
float32[] descriptor
float32 rating

")



(provide :stroll_bearnav/Feature "4fee28b7b26e4a1bbd92f2b4f4726cd5")


