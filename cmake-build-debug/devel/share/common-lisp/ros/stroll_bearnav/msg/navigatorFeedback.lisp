; Auto-generated. Do not edit!


(cl:in-package stroll_bearnav-msg)


;//! \htmlinclude navigatorFeedback.msg.html

(cl:defclass <navigatorFeedback> (roslisp-msg-protocol:ros-message)
  ((distance
    :reader distance
    :initarg :distance
    :type cl:float
    :initform 0.0)
   (diffRot
    :reader diffRot
    :initarg :diffRot
    :type cl:float
    :initform 0.0)
   (histogram
    :reader histogram
    :initarg :histogram
    :type (cl:vector cl:integer)
   :initform (cl:make-array 0 :element-type 'cl:integer :initial-element 0))
   (max
    :reader max
    :initarg :max
    :type cl:integer
    :initform 0)
   (correct
    :reader correct
    :initarg :correct
    :type cl:integer
    :initform 0)
   (outliers
    :reader outliers
    :initarg :outliers
    :type cl:integer
    :initform 0)
   (keypoints_avg
    :reader keypoints_avg
    :initarg :keypoints_avg
    :type cl:integer
    :initform 0)
   (matches
    :reader matches
    :initarg :matches
    :type cl:integer
    :initform 0)
   (stats
    :reader stats
    :initarg :stats
    :type (cl:vector cl:integer)
   :initform (cl:make-array 0 :element-type 'cl:integer :initial-element 0)))
)

(cl:defclass navigatorFeedback (<navigatorFeedback>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <navigatorFeedback>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'navigatorFeedback)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name stroll_bearnav-msg:<navigatorFeedback> is deprecated: use stroll_bearnav-msg:navigatorFeedback instead.")))

(cl:ensure-generic-function 'distance-val :lambda-list '(m))
(cl:defmethod distance-val ((m <navigatorFeedback>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:distance-val is deprecated.  Use stroll_bearnav-msg:distance instead.")
  (distance m))

(cl:ensure-generic-function 'diffRot-val :lambda-list '(m))
(cl:defmethod diffRot-val ((m <navigatorFeedback>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:diffRot-val is deprecated.  Use stroll_bearnav-msg:diffRot instead.")
  (diffRot m))

(cl:ensure-generic-function 'histogram-val :lambda-list '(m))
(cl:defmethod histogram-val ((m <navigatorFeedback>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:histogram-val is deprecated.  Use stroll_bearnav-msg:histogram instead.")
  (histogram m))

(cl:ensure-generic-function 'max-val :lambda-list '(m))
(cl:defmethod max-val ((m <navigatorFeedback>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:max-val is deprecated.  Use stroll_bearnav-msg:max instead.")
  (max m))

(cl:ensure-generic-function 'correct-val :lambda-list '(m))
(cl:defmethod correct-val ((m <navigatorFeedback>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:correct-val is deprecated.  Use stroll_bearnav-msg:correct instead.")
  (correct m))

(cl:ensure-generic-function 'outliers-val :lambda-list '(m))
(cl:defmethod outliers-val ((m <navigatorFeedback>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:outliers-val is deprecated.  Use stroll_bearnav-msg:outliers instead.")
  (outliers m))

(cl:ensure-generic-function 'keypoints_avg-val :lambda-list '(m))
(cl:defmethod keypoints_avg-val ((m <navigatorFeedback>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:keypoints_avg-val is deprecated.  Use stroll_bearnav-msg:keypoints_avg instead.")
  (keypoints_avg m))

(cl:ensure-generic-function 'matches-val :lambda-list '(m))
(cl:defmethod matches-val ((m <navigatorFeedback>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:matches-val is deprecated.  Use stroll_bearnav-msg:matches instead.")
  (matches m))

(cl:ensure-generic-function 'stats-val :lambda-list '(m))
(cl:defmethod stats-val ((m <navigatorFeedback>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:stats-val is deprecated.  Use stroll_bearnav-msg:stats instead.")
  (stats m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <navigatorFeedback>) ostream)
  "Serializes a message object of type '<navigatorFeedback>"
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'distance))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'diffRot))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'histogram))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let* ((signed ele) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    ))
   (cl:slot-value msg 'histogram))
  (cl:let* ((signed (cl:slot-value msg 'max)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'correct)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'outliers)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'keypoints_avg)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'matches)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'stats))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let* ((signed ele) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    ))
   (cl:slot-value msg 'stats))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <navigatorFeedback>) istream)
  "Deserializes a message object of type '<navigatorFeedback>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'distance) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'diffRot) (roslisp-utils:decode-single-float-bits bits)))
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'histogram) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'histogram)))
    (cl:dotimes (i __ros_arr_len)
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:aref vals i) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296)))))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'max) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'correct) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'outliers) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'keypoints_avg) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'matches) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'stats) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'stats)))
    (cl:dotimes (i __ros_arr_len)
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:aref vals i) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296)))))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<navigatorFeedback>)))
  "Returns string type for a message object of type '<navigatorFeedback>"
  "stroll_bearnav/navigatorFeedback")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'navigatorFeedback)))
  "Returns string type for a message object of type 'navigatorFeedback"
  "stroll_bearnav/navigatorFeedback")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<navigatorFeedback>)))
  "Returns md5sum for a message object of type '<navigatorFeedback>"
  "7b676e19f8b4915c865123ddcb157234")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'navigatorFeedback)))
  "Returns md5sum for a message object of type 'navigatorFeedback"
  "7b676e19f8b4915c865123ddcb157234")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<navigatorFeedback>)))
  "Returns full string definition for message of type '<navigatorFeedback>"
  (cl:format cl:nil "# ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======~%#feedback~%float32 distance~%float32 diffRot~%int32[] histogram~%int32 max~%int32 correct~%int32 outliers~%int32 keypoints_avg~%int32 matches~%int32[] stats~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'navigatorFeedback)))
  "Returns full string definition for message of type 'navigatorFeedback"
  (cl:format cl:nil "# ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======~%#feedback~%float32 distance~%float32 diffRot~%int32[] histogram~%int32 max~%int32 correct~%int32 outliers~%int32 keypoints_avg~%int32 matches~%int32[] stats~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <navigatorFeedback>))
  (cl:+ 0
     4
     4
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'histogram) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
     4
     4
     4
     4
     4
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'stats) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <navigatorFeedback>))
  "Converts a ROS message object to a list"
  (cl:list 'navigatorFeedback
    (cl:cons ':distance (distance msg))
    (cl:cons ':diffRot (diffRot msg))
    (cl:cons ':histogram (histogram msg))
    (cl:cons ':max (max msg))
    (cl:cons ':correct (correct msg))
    (cl:cons ':outliers (outliers msg))
    (cl:cons ':keypoints_avg (keypoints_avg msg))
    (cl:cons ':matches (matches msg))
    (cl:cons ':stats (stats msg))
))
