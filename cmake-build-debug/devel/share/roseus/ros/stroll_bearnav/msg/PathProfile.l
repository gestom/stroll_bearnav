;; Auto-generated. Do not edit!


(when (boundp 'stroll_bearnav::PathProfile)
  (if (not (find-package "STROLL_BEARNAV"))
    (make-package "STROLL_BEARNAV"))
  (shadow 'PathProfile (find-package "STROLL_BEARNAV")))
(unless (find-package "STROLL_BEARNAV::PATHPROFILE")
  (make-package "STROLL_BEARNAV::PATHPROFILE"))

(in-package "ROS")
;;//! \htmlinclude PathProfile.msg.html


(defclass stroll_bearnav::PathProfile
  :super ros::object
  :slots (_distance _forwardSpeed _angularSpeed _flipper ))

(defmethod stroll_bearnav::PathProfile
  (:init
   (&key
    ((:distance __distance) (make-array 0 :initial-element 0.0 :element-type :float))
    ((:forwardSpeed __forwardSpeed) (make-array 0 :initial-element 0.0 :element-type :float))
    ((:angularSpeed __angularSpeed) (make-array 0 :initial-element 0.0 :element-type :float))
    ((:flipper __flipper) (make-array 0 :initial-element 0.0 :element-type :float))
    )
   (send-super :init)
   (setq _distance __distance)
   (setq _forwardSpeed __forwardSpeed)
   (setq _angularSpeed __angularSpeed)
   (setq _flipper __flipper)
   self)
  (:distance
   (&optional __distance)
   (if __distance (setq _distance __distance)) _distance)
  (:forwardSpeed
   (&optional __forwardSpeed)
   (if __forwardSpeed (setq _forwardSpeed __forwardSpeed)) _forwardSpeed)
  (:angularSpeed
   (&optional __angularSpeed)
   (if __angularSpeed (setq _angularSpeed __angularSpeed)) _angularSpeed)
  (:flipper
   (&optional __flipper)
   (if __flipper (setq _flipper __flipper)) _flipper)
  (:serialization-length
   ()
   (+
    ;; float32[] _distance
    (* 4    (length _distance)) 4
    ;; float32[] _forwardSpeed
    (* 4    (length _forwardSpeed)) 4
    ;; float32[] _angularSpeed
    (* 4    (length _angularSpeed)) 4
    ;; float32[] _flipper
    (* 4    (length _flipper)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32[] _distance
     (write-long (length _distance) s)
     (dotimes (i (length _distance))
       (sys::poke (elt _distance i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;; float32[] _forwardSpeed
     (write-long (length _forwardSpeed) s)
     (dotimes (i (length _forwardSpeed))
       (sys::poke (elt _forwardSpeed i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;; float32[] _angularSpeed
     (write-long (length _angularSpeed) s)
     (dotimes (i (length _angularSpeed))
       (sys::poke (elt _angularSpeed i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;; float32[] _flipper
     (write-long (length _flipper) s)
     (dotimes (i (length _flipper))
       (sys::poke (elt _flipper i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32[] _distance
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _distance (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _distance i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;; float32[] _forwardSpeed
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _forwardSpeed (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _forwardSpeed i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;; float32[] _angularSpeed
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _angularSpeed (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _angularSpeed i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;; float32[] _flipper
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _flipper (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _flipper i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;;
   self)
  )

(setf (get stroll_bearnav::PathProfile :md5sum-) "b1915db84c0f5e0644fc19db32af4bed")
(setf (get stroll_bearnav::PathProfile :datatype-) "stroll_bearnav/PathProfile")
(setf (get stroll_bearnav::PathProfile :definition-)
      "float32[] distance
float32[] forwardSpeed
float32[] angularSpeed
float32[] flipper

")



(provide :stroll_bearnav/PathProfile "b1915db84c0f5e0644fc19db32af4bed")


