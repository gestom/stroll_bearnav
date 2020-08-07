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

class PathProfile {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.distance = null;
      this.forwardSpeed = null;
      this.angularSpeed = null;
      this.flipper = null;
    }
    else {
      if (initObj.hasOwnProperty('distance')) {
        this.distance = initObj.distance
      }
      else {
        this.distance = [];
      }
      if (initObj.hasOwnProperty('forwardSpeed')) {
        this.forwardSpeed = initObj.forwardSpeed
      }
      else {
        this.forwardSpeed = [];
      }
      if (initObj.hasOwnProperty('angularSpeed')) {
        this.angularSpeed = initObj.angularSpeed
      }
      else {
        this.angularSpeed = [];
      }
      if (initObj.hasOwnProperty('flipper')) {
        this.flipper = initObj.flipper
      }
      else {
        this.flipper = [];
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type PathProfile
    // Serialize message field [distance]
    bufferOffset = _arraySerializer.float32(obj.distance, buffer, bufferOffset, null);
    // Serialize message field [forwardSpeed]
    bufferOffset = _arraySerializer.float32(obj.forwardSpeed, buffer, bufferOffset, null);
    // Serialize message field [angularSpeed]
    bufferOffset = _arraySerializer.float32(obj.angularSpeed, buffer, bufferOffset, null);
    // Serialize message field [flipper]
    bufferOffset = _arraySerializer.float32(obj.flipper, buffer, bufferOffset, null);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type PathProfile
    let len;
    let data = new PathProfile(null);
    // Deserialize message field [distance]
    data.distance = _arrayDeserializer.float32(buffer, bufferOffset, null)
    // Deserialize message field [forwardSpeed]
    data.forwardSpeed = _arrayDeserializer.float32(buffer, bufferOffset, null)
    // Deserialize message field [angularSpeed]
    data.angularSpeed = _arrayDeserializer.float32(buffer, bufferOffset, null)
    // Deserialize message field [flipper]
    data.flipper = _arrayDeserializer.float32(buffer, bufferOffset, null)
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += 4 * object.distance.length;
    length += 4 * object.forwardSpeed.length;
    length += 4 * object.angularSpeed.length;
    length += 4 * object.flipper.length;
    return length + 16;
  }

  static datatype() {
    // Returns string type for a message object
    return 'stroll_bearnav/PathProfile';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'b1915db84c0f5e0644fc19db32af4bed';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    float32[] distance
    float32[] forwardSpeed
    float32[] angularSpeed
    float32[] flipper
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new PathProfile(null);
    if (msg.distance !== undefined) {
      resolved.distance = msg.distance;
    }
    else {
      resolved.distance = []
    }

    if (msg.forwardSpeed !== undefined) {
      resolved.forwardSpeed = msg.forwardSpeed;
    }
    else {
      resolved.forwardSpeed = []
    }

    if (msg.angularSpeed !== undefined) {
      resolved.angularSpeed = msg.angularSpeed;
    }
    else {
      resolved.angularSpeed = []
    }

    if (msg.flipper !== undefined) {
      resolved.flipper = msg.flipper;
    }
    else {
      resolved.flipper = []
    }

    return resolved;
    }
};

module.exports = PathProfile;
