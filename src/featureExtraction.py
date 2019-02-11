#!/usr/bin/env python
from __future__ import print_function
import numpy as np
import cv2
import os
import time
from copy import deepcopy
import tensorflow as tf
import rospy
# Ros Messages
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
from stroll_bearnav.msg import FeatureArray, Feature

from utils.helper import load_patches, get_ratio_scale, get_patch_size
from utils.dump import load_network, loadh5
from utils.kp_tools import XYZS2kpList, update_affine, get_XYZS_from_res_list
from utils.config import get_config

from modules.lift import Network
from six.moves import xrange


IDX_X, IDX_Y, IDX_SIZE, IDX_ANGLE, IDX_RESPONSE, IDX_OCTAVE = (0, 1, 2, 3, 4, 5)
KPS, ORI, DESC = (0, 1, 2)

floatX = 'float32'
READY = False
IMAGE_SIZE = (320, 752)


class image_feature:
    def __init__(self):
        """Initialize ros publisher, ros subscriber"""
        # topic where we publish
        self.feature_pub = rospy.Publisher("/features", FeatureArray, queue_size=10)
        self.bridge = CvBridge()
        # subscribed Topic
        self.subscriber = rospy.Subscriber("/image", Image, self.callback)
        self.image_pub_ = rospy.Publisher("/image_with_features", Image, queue_size=10)

    def prep(self):
        self.newsizes = []
        self.networks = []
        self.useOri = True
        self.config = get_config("")
        tf.set_random_seed(self.config.random_seed)
        # Open a tensorflow session
        tfconfig = tf.ConfigProto()
        tfconfig.gpu_options.allow_growth = True
        self.sess = [tf.Session(config=tfconfig)] * 3
        self.configs = [self.config, deepcopy(self.config), deepcopy(self.config)]

        # Standard nearby: nonmax will check the approx. the same area as descriptor support region.
        nearby = int(np.round(
            (0.5 * (self.config.kp_input_size - 1.0) *
             float(self.config.desc_input_size) /
             float(get_patch_size(self.config)))
        ))
        nearbyRatio = self.config.test_nearby_ratio*0.25
        self.nearby = max(int(np.round(nearby * nearbyRatio)), 1)
        self.nms_intv = self.config.test_nms_intv
        self.edge_th = self.config.test_edge_th
        self.best_val_loss = {}
        self.best_step = {}
        self.scl_intv = self.config.test_scl_intv

        # min_scale_log2 = 1  # min scale = 2
        # max_scale_log2 = 4  # max scale = 16
        min_scale_log2 = self.config.test_min_scale_log2
        max_scale_log2 = self.config.test_max_scale_log2
        # Test starting with double scale if small image
        min_hw = np.min(IMAGE_SIZE)
        if min_hw <= 1600 and min_scale_log2 != max_scale_log2:
            print("INFO: Testing double scale")
            min_scale_log2 -= 1
        # range of scales to check
        num_division = (max_scale_log2 - min_scale_log2) * (self.scl_intv + 1) + 1
        num_division /= 2
        self.scales_to_test = 2**np.linspace(min_scale_log2, max_scale_log2, num_division)

        # convert scale to image resizes
        self.resize_to_test = ((float(self.config.kp_input_size - 1) / 2.0) /
                               (get_ratio_scale(self.config) * self.scales_to_test))

        # check if resize is valid
        min_hw_after_resize = self.resize_to_test * min_hw
        is_resize_valid = min_hw_after_resize > self.config.kp_filter_size + 1

        # if there are invalid scales and resizes
        if not np.prod(is_resize_valid):
            # find first invalid
            first_invalid = np.where(~is_resize_valid)[0][0]
            # remove scales from testing
            self.scales_to_test = self.scales_to_test[:first_invalid]
            self.resize_to_test = self.resize_to_test[:first_invalid]

        for resize in self.resize_to_test:
            new_height = np.cast['int'](np.round(IMAGE_SIZE[0] * resize))
            new_width = np.cast['int'](np.round(IMAGE_SIZE[1] * resize))
            self.newsizes.append((new_height, new_width))

    def initNN(self):
        for i, subtask in enumerate(["kp", "ori", "desc"]):
            start_time = time.clock()
            # Retrieve mean/std
            logdir = os.path.join(self.config.logdir, subtask)
            if os.path.exists(os.path.join(logdir, "mean.h5")):
                training_mean = loadh5(os.path.join(logdir, "mean.h5"))
                training_std = loadh5(os.path.join(logdir, "std.h5"))
                # Create the model instance
                self.configs[i].subtask = subtask
                self.networks.append(Network(self.sess[i], self.configs[i], None, {
                    'mean': training_mean, 'std': training_std}))
            else:
                self.networks.append(Network(self.sess[i], self.configs[i], None))

            # We have everything ready. We finalize and initialie the network here.
            self.sess[i].run(tf.global_variables_initializer())

            # Load the network weights for the module of interest
            restore_res = load_network(self, subtask)
            if not restore_res:
                raise RuntimeError("Could not load network weights!")
            end_time = time.clock()
            compile_time = (end_time - start_time) * 1000.0
            rospy.logwarn("CREATED {} NETWORK! Time taken to compile is {} ms".format(
                subtask.upper(), compile_time))

    def callback(self, ros_data):
        """Callback function of subscribed topic. Here images get converted and features detected"""
        rospy.loginfo("get Ready")
        while not READY:
            pass
        start = time.clock()
        try:
            image_color = self.bridge.imgmsg_to_cv2(ros_data, "bgr8")
        except CvBridgeError as e:
            rospy.logwarn(e)
            return

        total_time = 0
        # Run learned network
        start_time = time.clock()
        image = cv2.cvtColor(image_color, cv2.COLOR_BGR2GRAY).astype(floatX)
        assert len(image.shape) == 2

        end_time = time.clock()
        load_prep_time = (end_time - start_time) * 1000.0
        # rospy.logwarn("Time taken to read and prepare the image is {} ms".format(load_prep_time))

        # Run for each scale
        test_res_list = []
        for new_height, new_width in self.newsizes:
            new_image = cv2.resize(image, (new_width, new_height))
            scoremap = self.networks[KPS].test("kp", new_image.reshape(1, new_height, new_width, 1))
            scoremap = np.squeeze(scoremap)
            end_time = time.clock()
            compute_time = (end_time - start_time) * 1000.0
            # print("Time taken for image size {} is {} ms".format(new_image.shape, compute_time))
            total_time += compute_time
            # pad invalid regions and add to list
            start_time = time.clock()
            test_res_list.append(np.pad(scoremap, int((self.config.kp_filter_size - 1) / 2),
                                        mode='constant', constant_values=-np.inf))

            end_time = time.clock()
            pad_time = (end_time - start_time) * 1000.0
            total_time += pad_time

        # Non-max suppression and draw.

        # rospy.logwarn("Performing NMS")
        start_time = time.clock()
        res_list = test_res_list
        XYZS = get_XYZS_from_res_list(res_list, self.resize_to_test, self.scales_to_test,
                                      self.nearby, self.edge_th, self.scl_intv, self.nms_intv,
                                      do_interpolation=True)
        end_time = time.clock()
        XYZS = XYZS[:self.config.test_num_keypoint]
        [cv2.circle(image_color, tuple(np.round(pos).astype(int)), np.round(rad).astype(int),
                    (0, 255, 0), 2, lineType=cv2.LINE_AA)
         for pos, rad in zip(XYZS[:, :2], XYZS[:, 2])]

        nms_time = (end_time - start_time) * 1000.0
        # rospy.logwarn("NMS time is {} ms".format(nms_time))
        total_time += nms_time
        # rospy.logwarn("Total time for detection is {} ms".format(total_time))

        # Save as keypoint file to be used by the oxford thing
        kp_list = XYZS2kpList(XYZS)  # note that this is already sorted
        # Read image
        start_time = time.clock()
        cur_data = self.load_data(image, kp_list)
        end_time = time.clock()
        load_time = (end_time - start_time) * 1000.0
        # print("Time taken to load patches is {} ms".format(load_time))
        total_time += load_time
        if self.useOri:
            # Test using the test function
            start_time = time.clock()
            oris = self.test_multibatch(cur_data, "ori")
            end_time = time.clock()
            compute_time = (end_time - start_time) * 1000.0
            # print("Time taken to compute is {} ms".format(compute_time))
            total_time += compute_time

            # update keypoints and save as new
            start_time = time.clock()
            kps = cur_data["kps"]
            for idxkp in xrange(len(kps)):
                kps[idxkp][IDX_ANGLE] = oris[idxkp] * 180.0 / np.pi % 360.0
                kps[idxkp] = update_affine(kps[idxkp])

            end_time = time.clock()
            update_time = (end_time - start_time) * 1000.0
            # rospy.logwarn("Time taken to update is {} ms".format(update_time))
            total_time += update_time
            # rospy.logwarn("Total time for orientation is {} ms".format(
            #     load_time+update_time+compute_time)
            # )

            start_time = time.clock()
            cur_data = self.load_data(image, kps)
            end_time = time.clock()
            load_time = (end_time - start_time) * 1000.0
            # rospy.logwarn("Time taken to load patches is {} ms".format(load_time))
            total_time += load_time

        start_time = time.clock()
        descs = self.test_multibatch(cur_data, "desc")
        end_time = time.clock()
        compute_time = (end_time - start_time) * 1000.0
        # rospy.logwarn("Time taken to compute is {} ms".format(compute_time))
        total_time += compute_time
        # rospy.logwarn("Total time for descriptor is {} ms".format(compute_time+load_time))

        # Overwrite angle
        kps = cur_data["kps"].copy()
        kps[:, 3] = cur_data["angle"][:, 0]
        features = FeatureArray()
        for i, kp in enumerate(kps):
            feature = Feature()
            feature.x = kp[IDX_X]
            feature.y = kp[IDX_Y]
            feature.size = kp[IDX_SIZE]*2
            feature.angle = kp[IDX_ANGLE]
            feature.response = kp[IDX_RESPONSE]
            feature.octave = kp[IDX_OCTAVE]
            feature.class_id = 4
            feature.descriptor = descs[i, :]
            features.feature.append(feature)
        self.feature_pub.publish(features)
        image_message = self.bridge.cv2_to_imgmsg(image_color, encoding="bgr8")
        self.image_pub_.publish(image_message)
        end = time.clock()
        rospy.logwarn("Summary time is: {} ms".format((end-start)*1000.0))

    def test_multibatch(self, cur_data, subtask):
        """A sub test routine."""

        batch_size = self.config.batch_size
        num_patch = len(cur_data["patch"])
        num_batch = int(np.ceil(float(num_patch) / float(batch_size)))
        # Initialize the batch items
        cur_batch = {}
        for _key in cur_data:
            cur_batch[_key] = np.zeros_like(cur_data[_key][:batch_size])

        # Do muiltiple times
        res = []
        for _idx_batch in xrange(num_batch):
            # start of the batch
            bs = _idx_batch * batch_size
            # end of the batch
            be = min(num_patch, (_idx_batch + 1) * batch_size)
            # number of elements in batch
            bn = be - bs
            for _key in cur_data:
                cur_batch[_key][:bn] = cur_data[_key][bs:be]
            if subtask == "ori":
                cur_res = self.networks[ORI].test(subtask, cur_batch).squeeze()[:bn]
            else:
                cur_res = self.networks[DESC].test(subtask, cur_batch).squeeze()[:bn]
            # Append
            res.append(cur_res)

        return np.concatenate(res, axis=0)

    def load_data(self, img, kp):
        in_dim = 1
        # Load keypoints
        kp = np.asarray(kp)
        # Assign dummy values to y, ID, angle
        y = np.zeros((len(kp),))
        ID = np.zeros((len(kp),), dtype='int64')
        angle = np.pi / 180.0 * kp[:, IDX_ANGLE]  # store angle in radians

        # load patches with id (drop out of boundary)
        bPerturb = False
        fPerturbInfo = np.zeros((3,))
        dataset = load_patches(img, kp, y, ID, angle,
                               get_ratio_scale(self.config), 1.0,
                               int(get_patch_size(self.config)),
                               int(self.config.desc_input_size), in_dim,
                               bPerturb, fPerturbInfo, bReturnCoords=True,
                               is_test=True)

        # Change old dataset return structure to necessary data
        x = dataset[0]
        pos = dataset[3]
        angle = dataset[4]
        coords = dataset[5]

        # Return the dictionary structure
        cur_data = {}
        cur_data["patch"] = np.transpose(x, (0, 2, 3, 1))  # In NHWC
        cur_data["kps"] = coords
        cur_data["xyz"] = pos
        # Make sure that angle is a Nx1 vector
        cur_data["angle"] = np.reshape(angle, (-1, 1))
        return cur_data


if __name__ == '__main__':
    '''Initializes and cleanup ros node'''
    READY = False
    ic = image_feature()
    rospy.init_node('PyNN', anonymous=True)
    rospy.logwarn("PyNN init!")
    ic.prep()
    ic.initNN()
    READY = True
    try:
        rospy.spin()
    except KeyboardInterrupt:
        rospy.logwarn("Shutting down ROS Image feature detector module")
