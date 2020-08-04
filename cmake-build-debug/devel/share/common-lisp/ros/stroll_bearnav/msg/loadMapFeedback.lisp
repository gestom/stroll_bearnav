; Auto-generated. Do not edit!


(cl:in-package stroll_bearnav-msg)


;//! \htmlinclude loadMapFeedback.msg.html

(cl:defclass <loadMapFeedback> (roslisp-msg-protocol:ros-message)
  ((fileName
    :reader fileName
    :initarg :fileName
    :type cl:string
    :initform "")
   (distance
    :reader distance
    :initarg :distance
    :type cl:float
    :initform 0.0)
   (numFeatures
    :reader numFeatures
    :initarg :numFeatures
    :type cl:integer
    :initform 0)
   (numberOfMaps
    :reader numberOfMaps
    :initarg :numberOfMaps
    :type cl:integer
    :initform 0)
   (mapIndex
    :reader mapIndex
    :initarg :mapIndex
    :type cl:integer
    :initform 0))
)

(cl:defclass loadMapFeedback (<loadMapFeedback>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <loadMapFeedback>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'loadMapFeedback)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name stroll_bearnav-msg:<loadMapFeedback> is deprecated: use stroll_bearnav-msg:loadMapFeedback instead.")))

(cl:ensure-generic-function 'fileName-val :lambda-list '(m))
(cl:defmethod fileName-val ((m <loadMapFeedback>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:fileName-val is deprecated.  Use stroll_bearnav-msg:fileName instead.")
  (fileName m))

(cl:ensure-generic-function 'distance-val :lambda-list '(m))
(cl:defmethod distance-val ((m <loadMapFeedback>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:distance-val is deprecated.  Use stroll_bearnav-msg:distance instead.")
  (distance m))

(cl:ensure-generic-function 'numFeatures-val :lambda-list '(m))
(cl:defmethod numFeatures-val ((m <loadMapFeedback>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:numFeatures-val is deprecated.  Use stroll_bearnav-msg:numFeatures instead.")
  (numFeatures m))

(cl:ensure-generic-function 'numberOfMaps-val :lambda-list '(m))
(cl:defmethod numberOfMaps-val ((m <loadMapFeedback>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:numberOfMaps-val is deprecated.  Use stroll_bearnav-msg:numberOfMaps instead.")
  (numberOfMaps m))

(cl:ensure-generic-function 'mapIndex-val :lambda-list '(m))
(cl:defmethod mapIndex-val ((m <loadMapFeedback>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:mapIndex-val is deprecated.  Use stroll_bearnav-msg:mapIndex instead.")
  (mapIndex m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <loadMapFeedback>) ostream)
  "Serializes a message object of type '<loadMapFeedback>"
  (cl:let ((__ros_str_len (cl:length (cl:slot-value msg 'fileName))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_str_len) ostream))
  (cl:map cl:nil #'(cl:lambda (c) (cl:write-byte (cl:char-code c) ostream)) (cl:slot-value msg 'fileName))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'distance))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let* ((signed (cl:slot-value msg 'numFeatures)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'numberOfMaps)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'mapIndex)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <loadMapFeedback>) istream)
  "Deserializes a message object of type '<loadMapFeedback>"
    (cl:let ((__ros_str_len 0))
      (cl:setf (cl:ldb (cl:byte 8 0) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'fileName) (cl:make-string __ros_str_len))
      (cl:dotimes (__ros_str_idx __ros_str_len msg)
        (cl:setf (cl:char (cl:slot-value msg 'fileName) __ros_str_idx) (cl:code-char (cl:read-byte istream)))))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'distance) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'numFeatures) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'numberOfMaps) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'mapIndex) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<loadMapFeedback>)))
  "Returns string type for a message object of type '<loadMapFeedback>"
  "stroll_bearnav/loadMapFeedback")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'loadMapFeedback)))
  "Returns string type for a message object of type 'loadMapFeedback"
  "stroll_bearnav/loadMapFeedback")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<loadMapFeedback>)))
  "Returns md5sum for a message object of type '<loadMapFeedback>"
  "ad02e9dc7b7886909443644d024fde29")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'loadMapFeedback)))
  "Returns md5sum for a message object of type 'loadMapFeedback"
  "ad02e9dc7b7886909443644d024fde29")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<loadMapFeedback>)))
  "Returns full string definition for message of type '<loadMapFeedback>"
  (cl:format cl:nil "# ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======~%#feedback~%string fileName~%float32 distance ~%int32 numFeatures~%int32 numberOfMaps~%int32 mapIndex~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'loadMapFeedback)))
  "Returns full string definition for message of type 'loadMapFeedback"
  (cl:format cl:nil "# ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======~%#feedback~%string fileName~%float32 distance ~%int32 numFeatures~%int32 numberOfMaps~%int32 mapIndex~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <loadMapFeedback>))
  (cl:+ 0
     4 (cl:length (cl:slot-value msg 'fileName))
     4
     4
     4
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <loadMapFeedback>))
  "Converts a ROS message object to a list"
  (cl:list 'loadMapFeedback
    (cl:cons ':fileName (fileName msg))
    (cl:cons ':distance (distance msg))
    (cl:cons ':numFeatures (numFeatures msg))
    (cl:cons ':numberOfMaps (numberOfMaps msg))
    (cl:cons ':mapIndex (mapIndex msg))
))
