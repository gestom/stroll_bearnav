; Auto-generated. Do not edit!


(cl:in-package stroll_bearnav-msg)


;//! \htmlinclude loadMapResult.msg.html

(cl:defclass <loadMapResult> (roslisp-msg-protocol:ros-message)
  ((distance
    :reader distance
    :initarg :distance
    :type cl:float
    :initform 0.0)
   (numFeatures
    :reader numFeatures
    :initarg :numFeatures
    :type cl:integer
    :initform 0)
   (numMaps
    :reader numMaps
    :initarg :numMaps
    :type cl:integer
    :initform 0))
)

(cl:defclass loadMapResult (<loadMapResult>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <loadMapResult>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'loadMapResult)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name stroll_bearnav-msg:<loadMapResult> is deprecated: use stroll_bearnav-msg:loadMapResult instead.")))

(cl:ensure-generic-function 'distance-val :lambda-list '(m))
(cl:defmethod distance-val ((m <loadMapResult>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:distance-val is deprecated.  Use stroll_bearnav-msg:distance instead.")
  (distance m))

(cl:ensure-generic-function 'numFeatures-val :lambda-list '(m))
(cl:defmethod numFeatures-val ((m <loadMapResult>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:numFeatures-val is deprecated.  Use stroll_bearnav-msg:numFeatures instead.")
  (numFeatures m))

(cl:ensure-generic-function 'numMaps-val :lambda-list '(m))
(cl:defmethod numMaps-val ((m <loadMapResult>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-msg:numMaps-val is deprecated.  Use stroll_bearnav-msg:numMaps instead.")
  (numMaps m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <loadMapResult>) ostream)
  "Serializes a message object of type '<loadMapResult>"
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
  (cl:let* ((signed (cl:slot-value msg 'numMaps)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <loadMapResult>) istream)
  "Deserializes a message object of type '<loadMapResult>"
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
      (cl:setf (cl:slot-value msg 'numMaps) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<loadMapResult>)))
  "Returns string type for a message object of type '<loadMapResult>"
  "stroll_bearnav/loadMapResult")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'loadMapResult)))
  "Returns string type for a message object of type 'loadMapResult"
  "stroll_bearnav/loadMapResult")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<loadMapResult>)))
  "Returns md5sum for a message object of type '<loadMapResult>"
  "8612f9000e7ffda9ca4803adc8bf9cf9")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'loadMapResult)))
  "Returns md5sum for a message object of type 'loadMapResult"
  "8612f9000e7ffda9ca4803adc8bf9cf9")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<loadMapResult>)))
  "Returns full string definition for message of type '<loadMapResult>"
  (cl:format cl:nil "# ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======~%#result definition~%float32 distance~%int32 numFeatures~%int32 numMaps~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'loadMapResult)))
  "Returns full string definition for message of type 'loadMapResult"
  (cl:format cl:nil "# ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======~%#result definition~%float32 distance~%int32 numFeatures~%int32 numMaps~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <loadMapResult>))
  (cl:+ 0
     4
     4
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <loadMapResult>))
  "Converts a ROS message object to a list"
  (cl:list 'loadMapResult
    (cl:cons ':distance (distance msg))
    (cl:cons ':numFeatures (numFeatures msg))
    (cl:cons ':numMaps (numMaps msg))
))
