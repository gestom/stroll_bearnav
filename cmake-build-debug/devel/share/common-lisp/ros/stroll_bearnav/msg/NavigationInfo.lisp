; Auto-generated. Do not edit!


(cl:in-package stroll_bearnav-msg)


;//! \htmlinclude NavigationInfo.msg.html

(cl:defclass <NavigationInfo> (roslisp-msg-protocol:ros-message)
  ((ratio
    :reader ratio
    :initarg :ratio
    :type cl:float
    :initform 0.0)
   (map
    :reader map
    :initarg :map
    :type stroll_bearnav-msg:FeatureArray
    :initform (cl:make-instance 'stroll_bearnav-msg:FeatureArray))
   (view
    :reader view
    :initarg :view
    :type stroll_bearnav-msg:FeatureArray
    :initform (cl:make-instance 'stroll_bearnav-msg:FeatureArray))
   (histogram
    :reader histogram
    :initarg :histogram
    :type (cl:vector cl:integer)
   :initform (cl:make-array 0 :element-type 'cl:integer :initial-element 0))
   (mapMatchIndex
    :reader mapMatchIndex
    :initarg :mapMatchIndex
    :type (cl:vector cl:integer)
   :initform (cl:make-array 0 :element-type 'cl:integer :initial-element 0))
   (mapMatchEval
    :reader mapMatchEval
    :initarg :mapMatchEval
    :type (cl:vector cl:integer)
   :initform (cl:make-array 0 :element-type 'cl:integer :initial-element 0))
   (correct
    :reader correct
    :initarg :correct
    :type cl:integer
    :initform 0)
   (matches
    :reader matches
    :initarg :matches
    :type cl:integer
    :initform 0)
   (distance
    :reader distance
    :initarg :distance
    :type cl:float
    :initform 0.0)
   (diffRot
    :reader diffRot
    :initarg :diffRot
    :type cl:float
    :initform 0.0)
   (mapChanges
    :reader mapChanges
    :initarg :mapChanges
    :type cl:integer
    :initform 0)
   (updated
    :reader updated
    :initarg :updated
    :type cl:boolean
    :initform cl:nil))
)

(cl:defclass NavigationInfo (<NavigationInfo>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <NavigationInfo>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'NavigationInfo)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name stroll_bearnav-msg:<NavigationInfo> is deprecated: use stroll_bearnav-msg:NavigationInfo instead.")))

(cl:ensure-generic-function 'ratio-val :lambda-list '(m))
(cl:defmethod ratio-val ((m <NavigationInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:ratio-val is deprecated.  Use stroll_bearnav-msg:ratio instead.")
  (ratio m))

(cl:ensure-generic-function 'map-val :lambda-list '(m))
(cl:defmethod map-val ((m <NavigationInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:map-val is deprecated.  Use stroll_bearnav-msg:map instead.")
  (map m))

(cl:ensure-generic-function 'view-val :lambda-list '(m))
(cl:defmethod view-val ((m <NavigationInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:view-val is deprecated.  Use stroll_bearnav-msg:view instead.")
  (view m))

(cl:ensure-generic-function 'histogram-val :lambda-list '(m))
(cl:defmethod histogram-val ((m <NavigationInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:histogram-val is deprecated.  Use stroll_bearnav-msg:histogram instead.")
  (histogram m))

(cl:ensure-generic-function 'mapMatchIndex-val :lambda-list '(m))
(cl:defmethod mapMatchIndex-val ((m <NavigationInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:mapMatchIndex-val is deprecated.  Use stroll_bearnav-msg:mapMatchIndex instead.")
  (mapMatchIndex m))

(cl:ensure-generic-function 'mapMatchEval-val :lambda-list '(m))
(cl:defmethod mapMatchEval-val ((m <NavigationInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:mapMatchEval-val is deprecated.  Use stroll_bearnav-msg:mapMatchEval instead.")
  (mapMatchEval m))

(cl:ensure-generic-function 'correct-val :lambda-list '(m))
(cl:defmethod correct-val ((m <NavigationInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:correct-val is deprecated.  Use stroll_bearnav-msg:correct instead.")
  (correct m))

(cl:ensure-generic-function 'matches-val :lambda-list '(m))
(cl:defmethod matches-val ((m <NavigationInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:matches-val is deprecated.  Use stroll_bearnav-msg:matches instead.")
  (matches m))

(cl:ensure-generic-function 'distance-val :lambda-list '(m))
(cl:defmethod distance-val ((m <NavigationInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:distance-val is deprecated.  Use stroll_bearnav-msg:distance instead.")
  (distance m))

(cl:ensure-generic-function 'diffRot-val :lambda-list '(m))
(cl:defmethod diffRot-val ((m <NavigationInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:diffRot-val is deprecated.  Use stroll_bearnav-msg:diffRot instead.")
  (diffRot m))

(cl:ensure-generic-function 'mapChanges-val :lambda-list '(m))
(cl:defmethod mapChanges-val ((m <NavigationInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:mapChanges-val is deprecated.  Use stroll_bearnav-msg:mapChanges instead.")
  (mapChanges m))

(cl:ensure-generic-function 'updated-val :lambda-list '(m))
(cl:defmethod updated-val ((m <NavigationInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:updated-val is deprecated.  Use stroll_bearnav-msg:updated instead.")
  (updated m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <NavigationInfo>) ostream)
  "Serializes a message object of type '<NavigationInfo>"
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'ratio))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'map) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'view) ostream)
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
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'mapMatchIndex))))
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
   (cl:slot-value msg 'mapMatchIndex))
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'mapMatchEval))))
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
   (cl:slot-value msg 'mapMatchEval))
  (cl:let* ((signed (cl:slot-value msg 'correct)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
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
  (cl:let* ((signed (cl:slot-value msg 'mapChanges)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'updated) 1 0)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <NavigationInfo>) istream)
  "Deserializes a message object of type '<NavigationInfo>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'ratio) (roslisp-utils:decode-single-float-bits bits)))
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'map) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'view) istream)
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
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'mapMatchIndex) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'mapMatchIndex)))
    (cl:dotimes (i __ros_arr_len)
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:aref vals i) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296)))))))
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'mapMatchEval) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'mapMatchEval)))
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
      (cl:setf (cl:slot-value msg 'correct) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'matches) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
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
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'mapChanges) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:setf (cl:slot-value msg 'updated) (cl:not (cl:zerop (cl:read-byte istream))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<NavigationInfo>)))
  "Returns string type for a message object of type '<NavigationInfo>"
  "stroll_bearnav/NavigationInfo")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'NavigationInfo)))
  "Returns string type for a message object of type 'NavigationInfo"
  "stroll_bearnav/NavigationInfo")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<NavigationInfo>)))
  "Returns md5sum for a message object of type '<NavigationInfo>"
  "82d671060d2954f62b3d0cce462cfb6c")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'NavigationInfo)))
  "Returns md5sum for a message object of type 'NavigationInfo"
  "82d671060d2954f62b3d0cce462cfb6c")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<NavigationInfo>)))
  "Returns full string definition for message of type '<NavigationInfo>"
  (cl:format cl:nil "float32 ratio                           #matching ratio~%stroll_bearnav/FeatureArray map         #features from the current map~%stroll_bearnav/FeatureArray view	#features from the current view~%int32[] histogram    			#navigation histogram~%int32[] mapMatchIndex			#contains indices of 'view' features corresponding to a given mapFeature~%int32[] mapMatchEval			#contains information of mapFeatures - correct 1, incorrect -1, unmatched 0~%int32 correct                   	#number of correct matches~%int32 matches                   	#number of good matches~%float32 distance                	#traveled distance~%float32 diffRot                 	#difference between features~%int32 mapChanges                	#number of traversals~%bool updated                    	#map contains rating~%~%================================================================================~%MSG: stroll_bearnav/FeatureArray~%Header header~%float32 distance~%string id ~%stroll_bearnav/Feature[] feature~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%# 0: no frame~%# 1: global frame~%string frame_id~%~%================================================================================~%MSG: stroll_bearnav/Feature~%float32 x~%float32 y~%float32 size~%float32 angle~%float32 response~%int32 octave~%int32 class_id~%float32[] descriptor~%float32 rating~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'NavigationInfo)))
  "Returns full string definition for message of type 'NavigationInfo"
  (cl:format cl:nil "float32 ratio                           #matching ratio~%stroll_bearnav/FeatureArray map         #features from the current map~%stroll_bearnav/FeatureArray view	#features from the current view~%int32[] histogram    			#navigation histogram~%int32[] mapMatchIndex			#contains indices of 'view' features corresponding to a given mapFeature~%int32[] mapMatchEval			#contains information of mapFeatures - correct 1, incorrect -1, unmatched 0~%int32 correct                   	#number of correct matches~%int32 matches                   	#number of good matches~%float32 distance                	#traveled distance~%float32 diffRot                 	#difference between features~%int32 mapChanges                	#number of traversals~%bool updated                    	#map contains rating~%~%================================================================================~%MSG: stroll_bearnav/FeatureArray~%Header header~%float32 distance~%string id ~%stroll_bearnav/Feature[] feature~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%# 0: no frame~%# 1: global frame~%string frame_id~%~%================================================================================~%MSG: stroll_bearnav/Feature~%float32 x~%float32 y~%float32 size~%float32 angle~%float32 response~%int32 octave~%int32 class_id~%float32[] descriptor~%float32 rating~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <NavigationInfo>))
  (cl:+ 0
     4
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'map))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'view))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'histogram) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'mapMatchIndex) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'mapMatchEval) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
     4
     4
     4
     4
     4
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <NavigationInfo>))
  "Converts a ROS message object to a list"
  (cl:list 'NavigationInfo
    (cl:cons ':ratio (ratio msg))
    (cl:cons ':map (map msg))
    (cl:cons ':view (view msg))
    (cl:cons ':histogram (histogram msg))
    (cl:cons ':mapMatchIndex (mapMatchIndex msg))
    (cl:cons ':mapMatchEval (mapMatchEval msg))
    (cl:cons ':correct (correct msg))
    (cl:cons ':matches (matches msg))
    (cl:cons ':distance (distance msg))
    (cl:cons ':diffRot (diffRot msg))
    (cl:cons ':mapChanges (mapChanges msg))
    (cl:cons ':updated (updated msg))
))
