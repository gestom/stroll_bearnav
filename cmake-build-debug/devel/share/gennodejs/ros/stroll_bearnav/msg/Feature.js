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

class Feature {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.x = null;
      this.y = null;
      this.size = null;
      this.angle = null;
      this.response = null;
      this.octave = null;
      this.class_id = null;
      this.descriptor = null;
      this.rating = null;
    }
    else {
      if (initObj.hasOwnProperty('x')) {
        this.x = initObj.x
      }
      else {
        this.x = 0.0;
      }
      if (initObj.hasOwnProperty('y')) {
        this.y = initObj.y
      }
      else {
        this.y = 0.0;
      }
      if (initObj.hasOwnProperty('size')) {
        this.size = initObj.size
      }
      else {
        this.size = 0.0;
      }
      if (initObj.hasOwnProperty('angle')) {
        this.angle = initObj.angle
      }
      else {
        this.angle = 0.0;
      }
      if (initObj.hasOwnProperty('response')) {
        this.response = initObj.response
      }
      else {
        this.response = 0.0;
      }
      if (initObj.hasOwnProperty('octave')) {
        this.octave = initObj.octave
      }
      else {
        this.octave = 0;
      }
      if (initObj.hasOwnProperty('class_id')) {
        this.class_id = initObj.class_id
      }
      else {
        this.class_id = 0;
      }
      if (initObj.hasOwnProperty('descriptor')) {
        this.descriptor = initObj.descriptor
      }
      else {
        this.descriptor = [];
      }
      if (initObj.hasOwnProperty('rating')) {
        this.rating = initObj.rating
      }
      else {
        this.rating = 0.0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type Feature
    // Serialize message field [x]
    bufferOffset = _serializer.float32(obj.x, buffer, bufferOffset);
    // Serialize message field [y]
    bufferOffset = _serializer.float32(obj.y, buffer, bufferOffset);
    // Serialize message field [size]
    bufferOffset = _serializer.float32(obj.size, buffer, bufferOffset);
    // Serialize message field [angle]
    bufferOffset = _serializer.float32(obj.angle, buffer, bufferOffset);
    // Serialize message field [response]
    bufferOffset = _serializer.float32(obj.response, buffer, bufferOffset);
    // Serialize message field [octave]
    bufferOffset = _serializer.int32(obj.octave, buffer, bufferOffset);
    // Serialize message field [class_id]
    bufferOffset = _serializer.int32(obj.class_id, buffer, bufferOffset);
    // Serialize message field [descriptor]
    bufferOffset = _arraySerializer.float32(obj.descriptor, buffer, bufferOffset, null);
    // Serialize message field [rating]
    bufferOffset = _serializer.float32(obj.rating, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type Feature
    let len;
    let data = new Feature(null);
    // Deserialize message field [x]
    data.x = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [y]
    data.y = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [size]
    data.size = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [angle]
    data.angle = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [response]
    data.response = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [octave]
    data.octave = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [class_id]
    data.class_id = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [descriptor]
    data.descriptor = _arrayDeserializer.float32(buffer, bufferOffset, null)
    // Deserialize message field [rating]
    data.rating = _deserializer.float32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += 4 * object.descriptor.length;
    return length + 36;
  }

  static datatype() {
    // Returns string type for a message object
    return 'stroll_bearnav/Feature';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '4fee28b7b26e4a1bbd92f2b4f4726cd5';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
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
    const resolved = new Feature(null);
    if (msg.x !== undefined) {
      resolved.x = msg.x;
    }
    else {
      resolved.x = 0.0
    }

    if (msg.y !== undefined) {
      resolved.y = msg.y;
    }
    else {
      resolved.y = 0.0
    }

    if (msg.size !== undefined) {
      resolved.size = msg.size;
    }
    else {
      resolved.size = 0.0
    }

    if (msg.angle !== undefined) {
      resolved.angle = msg.angle;
    }
    else {
      resolved.angle = 0.0
    }

    if (msg.response !== undefined) {
      resolved.response = msg.response;
    }
    else {
      resolved.response = 0.0
    }

    if (msg.octave !== undefined) {
      resolved.octave = msg.octave;
    }
    else {
      resolved.octave = 0
    }

    if (msg.class_id !== undefined) {
      resolved.class_id = msg.class_id;
    }
    else {
      resolved.class_id = 0
    }

    if (msg.descriptor !== undefined) {
      resolved.descriptor = msg.descriptor;
    }
    else {
      resolved.descriptor = []
    }

    if (msg.rating !== undefined) {
      resolved.rating = msg.rating;
    }
    else {
      resolved.rating = 0.0
    }

    return resolved;
    }
};

module.exports = Feature;
