; Auto-generated. Do not edit!


(cl:in-package stroll_bearnav-srv)


;//! \htmlinclude SetDistance-request.msg.html

(cl:defclass <SetDistance-request> (roslisp-msg-protocol:ros-message)
  ((distance
    :reader distance
    :initarg :distance
    :type cl:float
    :initform 0.0))
)

(cl:defclass SetDistance-request (<SetDistance-request>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <SetDistance-request>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'SetDistance-request)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name stroll_bearnav-srv:<SetDistance-request> is deprecated: use stroll_bearnav-srv:SetDistance-request instead.")))

(cl:ensure-generic-function 'distance-val :lambda-list '(m))
(cl:defmethod distance-val ((m <SetDistance-request>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-srv:distance-val is deprecated.  Use stroll_bearnav-srv:distance instead.")
  (distance m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <SetDistance-request>) ostream)
  "Serializes a message object of type '<SetDistance-request>"
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'distance))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <SetDistance-request>) istream)
  "Deserializes a message object of type '<SetDistance-request>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'distance) (roslisp-utils:decode-single-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<SetDistance-request>)))
  "Returns string type for a service object of type '<SetDistance-request>"
  "stroll_bearnav/SetDistanceRequest")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'SetDistance-request)))
  "Returns string type for a service object of type 'SetDistance-request"
  "stroll_bearnav/SetDistanceRequest")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<SetDistance-request>)))
  "Returns md5sum for a message object of type '<SetDistance-request>"
  "3ff5ee2fe3eb1f4b34a6e4483099be55")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'SetDistance-request)))
  "Returns md5sum for a message object of type 'SetDistance-request"
  "3ff5ee2fe3eb1f4b34a6e4483099be55")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<SetDistance-request>)))
  "Returns full string definition for message of type '<SetDistance-request>"
  (cl:format cl:nil "float32 distance~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'SetDistance-request)))
  "Returns full string definition for message of type 'SetDistance-request"
  (cl:format cl:nil "float32 distance~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <SetDistance-request>))
  (cl:+ 0
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <SetDistance-request>))
  "Converts a ROS message object to a list"
  (cl:list 'SetDistance-request
    (cl:cons ':distance (distance msg))
))
;//! \htmlinclude SetDistance-response.msg.html

(cl:defclass <SetDistance-response> (roslisp-msg-protocol:ros-message)
  ((distance
    :reader distance
    :initarg :distance
    :type cl:float
    :initform 0.0))
)

(cl:defclass SetDistance-response (<SetDistance-response>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <SetDistance-response>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'SetDistance-response)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name stroll_bearnav-srv:<SetDistance-response> is deprecated: use stroll_bearnav-srv:SetDistance-response instead.")))

(cl:ensure-generic-function 'distance-val :lambda-list '(m))
(cl:defmethod distance-val ((m <SetDistance-response>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader stroll_bearnav-srv:distance-val is deprecated.  Use stroll_bearnav-srv:distance instead.")
  (distance m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <SetDistance-response>) ostream)
  "Serializes a message object of type '<SetDistance-response>"
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'distance))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <SetDistance-response>) istream)
  "Deserializes a message object of type '<SetDistance-response>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'distance) (roslisp-utils:decode-single-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<SetDistance-response>)))
  "Returns string type for a service object of type '<SetDistance-response>"
  "stroll_bearnav/SetDistanceResponse")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'SetDistance-response)))
  "Returns string type for a service object of type 'SetDistance-response"
  "stroll_bearnav/SetDistanceResponse")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<SetDistance-response>)))
  "Returns md5sum for a message object of type '<SetDistance-response>"
  "3ff5ee2fe3eb1f4b34a6e4483099be55")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'SetDistance-response)))
  "Returns md5sum for a message object of type 'SetDistance-response"
  "3ff5ee2fe3eb1f4b34a6e4483099be55")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<SetDistance-response>)))
  "Returns full string definition for message of type '<SetDistance-response>"
  (cl:format cl:nil "float32 distance~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'SetDistance-response)))
  "Returns full string definition for message of type 'SetDistance-response"
  (cl:format cl:nil "float32 distance~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <SetDistance-response>))
  (cl:+ 0
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <SetDistance-response>))
  "Converts a ROS message object to a list"
  (cl:list 'SetDistance-response
    (cl:cons ':distance (distance msg))
))
(cl:defmethod roslisp-msg-protocol:service-request-type ((msg (cl:eql 'SetDistance)))
  'SetDistance-request)
(cl:defmethod roslisp-msg-protocol:service-response-type ((msg (cl:eql 'SetDistance)))
  'SetDistance-response)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'SetDistance)))
  "Returns string type for a service object of type '<SetDistance>"
  "stroll_bearnav/SetDistance")