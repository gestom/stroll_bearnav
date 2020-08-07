// Auto-generated. Do not edit!

// (in-package stroll_bearnav.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;

//-----------------------------------------------------------

class navigatorFeedback {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.distance = null;
      this.diffRot = null;
      this.histogram = null;
      this.max = null;
      this.correct = null;
      this.outliers = null;
      this.keypoints_avg = null;
      this.matches = null;
      this.stats = null;
    }
    else {
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
      if (initObj.hasOwnProperty('histogram')) {
        this.histogram = initObj.histogram
      }
      else {
        this.histogram = [];
      }
      if (initObj.hasOwnProperty('max')) {
        this.max = initObj.max
      }
      else {
        this.max = 0;
      }
      if (initObj.hasOwnProperty('correct')) {
        this.correct = initObj.correct
      }
      else {
        this.correct = 0;
      }
      if (initObj.hasOwnProperty('outliers')) {
        this.outliers = initObj.outliers
      }
      else {
        this.outliers = 0;
      }
      if (initObj.hasOwnProperty('keypoints_avg')) {
        this.keypoints_avg = initObj.keypoints_avg
      }
      else {
        this.keypoints_avg = 0;
      }
      if (initObj.hasOwnProperty('matches')) {
        this.matches = initObj.matches
      }
      else {
        this.matches = 0;
      }
      if (initObj.hasOwnProperty('stats')) {
        this.stats = initObj.stats
      }
      else {
        this.stats = [];
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type navigatorFeedback
    // Serialize message field [distance]
    bufferOffset = _serializer.float32(obj.distance, buffer, bufferOffset);
    // Serialize message field [diffRot]
    bufferOffset = _serializer.float32(obj.diffRot, buffer, bufferOffset);
    // Serialize message field [histogram]
    bufferOffset = _arraySerializer.int32(obj.histogram, buffer, bufferOffset, null);
    // Serialize message field [max]
    bufferOffset = _serializer.int32(obj.max, buffer, bufferOffset);
    // Serialize message field [correct]
    bufferOffset = _serializer.int32(obj.correct, buffer, bufferOffset);
    // Serialize message field [outliers]
    bufferOffset = _serializer.int32(obj.outliers, buffer, bufferOffset);
    // Serialize message field [keypoints_avg]
    bufferOffset = _serializer.int32(obj.keypoints_avg, buffer, bufferOffset);
    // Serialize message field [matches]
    bufferOffset = _serializer.int32(obj.matches, buffer, bufferOffset);
    // Serialize message field [stats]
    bufferOffset = _arraySerializer.int32(obj.stats, buffer, bufferOffset, null);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type navigatorFeedback
    let len;
    let data = new navigatorFeedback(null);
    // Deserialize message field [distance]
    data.distance = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [diffRot]
    data.diffRot = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [histogram]
    data.histogram = _arrayDeserializer.int32(buffer, bufferOffset, null)
    // Deserialize message field [max]
    data.max = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [correct]
    data.correct = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [outliers]
    data.outliers = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [keypoints_avg]
    data.keypoints_avg = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [matches]
    data.matches = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [stats]
    data.stats = _arrayDeserializer.int32(buffer, bufferOffset, null)
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += 4 * object.histogram.length;
    length += 4 * object.stats.length;
    return length + 36;
  }

  static datatype() {
    // Returns string type for a message object
    return 'stroll_bearnav/navigatorFeedback';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '7b676e19f8b4915c865123ddcb157234';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    # ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======
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
    
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new navigatorFeedback(null);
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

    if (msg.histogram !== undefined) {
      resolved.histogram = msg.histogram;
    }
    else {
      resolved.histogram = []
    }

    if (msg.max !== undefined) {
      resolved.max = msg.max;
    }
    else {
      resolved.max = 0
    }

    if (msg.correct !== undefined) {
      resolved.correct = msg.correct;
    }
    else {
      resolved.correct = 0
    }

    if (msg.outliers !== undefined) {
      resolved.outliers = msg.outliers;
    }
    else {
      resolved.outliers = 0
    }

    if (msg.keypoints_avg !== undefined) {
      resolved.keypoints_avg = msg.keypoints_avg;
    }
    else {
      resolved.keypoints_avg = 0
    }

    if (msg.matches !== undefined) {
      resolved.matches = msg.matches;
    }
    else {
      resolved.matches = 0
    }

    if (msg.stats !== undefined) {
      resolved.stats = msg.stats;
    }
    else {
      resolved.stats = []
    }

    return resolved;
    }
};

module.exports = navigatorFeedback;
