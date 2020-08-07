// Auto-generated. Do not edit!

// (in-package stroll_bearnav.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let FeatureArray = require('./FeatureArray.js');

//-----------------------------------------------------------

class NavigationInfo {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.ratio = null;
      this.map = null;
      this.view = null;
      this.histogram = null;
      this.mapMatchIndex = null;
      this.mapMatchEval = null;
      this.correct = null;
      this.matches = null;
      this.distance = null;
      this.diffRot = null;
      this.mapChanges = null;
      this.updated = null;
    }
    else {
      if (initObj.hasOwnProperty('ratio')) {
        this.ratio = initObj.ratio
      }
      else {
        this.ratio = 0.0;
      }
      if (initObj.hasOwnProperty('map')) {
        this.map = initObj.map
      }
      else {
        this.map = new FeatureArray();
      }
      if (initObj.hasOwnProperty('view')) {
        this.view = initObj.view
      }
      else {
        this.view = new FeatureArray();
      }
      if (initObj.hasOwnProperty('histogram')) {
        this.histogram = initObj.histogram
      }
      else {
        this.histogram = [];
      }
      if (initObj.hasOwnProperty('mapMatchIndex')) {
        this.mapMatchIndex = initObj.mapMatchIndex
      }
      else {
        this.mapMatchIndex = [];
      }
      if (initObj.hasOwnProperty('mapMatchEval')) {
        this.mapMatchEval = initObj.mapMatchEval
      }
      else {
        this.mapMatchEval = [];
      }
      if (initObj.hasOwnProperty('correct')) {
        this.correct = initObj.correct
      }
      else {
        this.correct = 0;
      }
      if (initObj.hasOwnProperty('matches')) {
        this.matches = initObj.matches
      }
      else {
        this.matches = 0;
      }
      if (initObj.hasOwnProperty('distance')) {
        this.distance = initObj.distance
      }
      else {
        this.distance = 0.0;
      }
      if (initObj.hasOwnProperty('diffRot')) {
        this.diffRot = initObj.diffRot
      }
      else {
        this.diffRot = 0.0;
      }
      if (initObj.hasOwnProperty('mapChanges')) {
        this.mapChanges = initObj.mapChanges
      }
      else {
        this.mapChanges = 0;
      }
      if (initObj.hasOwnProperty('updated')) {
        this.updated = initObj.updated
      }
      else {
        this.updated = false;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type NavigationInfo
    // Serialize message field [ratio]
    bufferOffset = _serializer.float32(obj.ratio, buffer, bufferOffset);
    // Serialize message field [map]
    bufferOffset = FeatureArray.serialize(obj.map, buffer, bufferOffset);
    // Serialize message field [view]
    bufferOffset = FeatureArray.serialize(obj.view, buffer, bufferOffset);
    // Serialize message field [histogram]
    bufferOffset = _arraySerializer.int32(obj.histogram, buffer, bufferOffset, null);
    // Serialize message field [mapMatchIndex]
    bufferOffset = _arraySerializer.int32(obj.mapMatchIndex, buffer, bufferOffset, null);
    // Serialize message field [mapMatchEval]
    bufferOffset = _arraySerializer.int32(obj.mapMatchEval, buffer, bufferOffset, null);
    // Serialize message field [correct]
    bufferOffset = _serializer.int32(obj.correct, buffer, bufferOffset);
    // Serialize message field [matches]
    bufferOffset = _serializer.int32(obj.matches, buffer, bufferOffset);
    // Serialize message field [distance]
    bufferOffset = _serializer.float32(obj.distance, buffer, bufferOffset);
    // Serialize message field [diffRot]
    bufferOffset = _serializer.float32(obj.diffRot, buffer, bufferOffset);
    // Serialize message field [mapChanges]
    bufferOffset = _serializer.int32(obj.mapChanges, buffer, bufferOffset);
    // Serialize message field [updated]
    bufferOffset = _serializer.bool(obj.updated, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type NavigationInfo
    let len;
    let data = new NavigationInfo(null);
    // Deserialize message field [ratio]
    data.ratio = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [map]
    data.map = FeatureArray.deserialize(buffer, bufferOffset);
    // Deserialize message field [view]
    data.view = FeatureArray.deserialize(buffer, bufferOffset);
    // Deserialize message field [histogram]
    data.histogram = _arrayDeserializer.int32(buffer, bufferOffset, null)
    // Deserialize message field [mapMatchIndex]
    data.mapMatchIndex = _arrayDeserializer.int32(buffer, bufferOffset, null)
    // Deserialize message field [mapMatchEval]
    data.mapMatchEval = _arrayDeserializer.int32(buffer, bufferOffset, null)
    // Deserialize message field [correct]
    data.correct = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [matches]
    data.matches = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [distance]
    data.distance = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [diffRot]
    data.diffRot = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [mapChanges]
    data.mapChanges = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [updated]
    data.updated = _deserializer.bool(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += FeatureArray.getMessageSize(object.map);
    length += FeatureArray.getMessageSize(object.view);
    length += 4 * object.histogram.length;
    length += 4 * object.mapMatchIndex.length;
    length += 4 * object.mapMatchEval.length;
    return length + 37;
  }

  static datatype() {
    // Returns string type for a message object
    return 'stroll_bearnav/NavigationInfo';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '82d671060d2954f62b3d0cce462cfb6c';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    float32 ratio                           #matching ratio
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
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new NavigationInfo(null);
    if (msg.ratio !== undefined) {
      resolved.ratio = msg.ratio;
    }
    else {
      resolved.ratio = 0.0
    }

    if (msg.map !== undefined) {
      resolved.map = FeatureArray.Resolve(msg.map)
    }
    else {
      resolved.map = new FeatureArray()
    }

    if (msg.view !== undefined) {
      resolved.view = FeatureArray.Resolve(msg.view)
    }
    else {
      resolved.view = new FeatureArray()
    }

    if (msg.histogram !== undefined) {
      resolved.histogram = msg.histogram;
    }
    else {
      resolved.histogram = []
    }

    if (msg.mapMatchIndex !== undefined) {
      resolved.mapMatchIndex = msg.mapMatchIndex;
    }
    else {
      resolved.mapMatchIndex = []
    }

    if (msg.mapMatchEval !== undefined) {
      resolved.mapMatchEval = msg.mapMatchEval;
    }
    else {
      resolved.mapMatchEval = []
    }

    if (msg.correct !== undefined) {
      resolved.correct = msg.correct;
    }
    else {
      resolved.correct = 0
    }

    if (msg.matches !== undefined) {
      resolved.matches = msg.matches;
    }
    else {
      resolved.matches = 0
    }

    if (msg.distance !== undefined) {
      resolved.distance = msg.distance;
    }
    else {
      resolved.distance = 0.0
    }

    if (msg.diffRot !== undefined) {
      resolved.diffRot = msg.diffRot;
    }
    else {
      resolved.diffRot = 0.0
    }

    if (msg.mapChanges !== undefined) {
      resolved.mapChanges = msg.mapChanges;
    }
    else {
      resolved.mapChanges = 0
    }

    if (msg.updated !== undefined) {
      resolved.updated = msg.updated;
    }
    else {
      resolved.updated = false
    }

    return resolved;
    }
};

module.exports = NavigationInfo;
