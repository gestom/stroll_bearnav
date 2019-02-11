# dump.py ---
#
# Filename: dump.py
# Description:
# Author: Kwang Moo Yi
# Maintainer:
# Created: Thu Jul  6 15:36:36 2017 (+0200)
# Version:
# Package-Requires: ()
# URL:
# Doc URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
#
#
#

# Change Log:
#
#
#
# Copyright (C), EPFL Computer Vision Lab.

# Code:

import os

import h5py
import tensorflow as tf
import tensorflow.contrib.slim as slim

# Some constant strings
best_val_loss_filename = "best_val_loss.h5"
best_step_filename = "step.h5"


def show_all_variables():
    # Adapted from original code at
    # https://github.com/carpedm20/simulated-unsupervised-tensorflow
    model_vars = tf.trainable_variables()
    slim.model_analyzer.analyze_vars(model_vars, print_info=False)


def image_summary_nhwc(name, img, max_outputs=1):
    """Image summary function for NHWC format"""

    return tf.summary.image(name, img, max_outputs)


def image_summary_nchw(name, img, max_outputs=1):
    """Image summary function for NCHW format"""

    return tf.summary.image(
        name, tf.transpose(img, (0, 2, 3, 1)), max_outputs)


def loadh5(dump_file_full_name):
    """ Loads a h5 file as dictionary """

    with h5py.File(dump_file_full_name, 'r') as h5file:
        dict_from_file = readh5(h5file)

    return dict_from_file


def readh5(h5node):
    """ Recursive function to read h5 nodes as dictionary """

    dict_from_file = {}
    for _key in h5node.keys():
        if isinstance(h5node[_key], h5py._hl.group.Group):
            dict_from_file[_key] = readh5(h5node[_key])
        else:
            dict_from_file[_key] = h5node[_key].value

    return dict_from_file


def load_network(super, subtask):
    """Load function for our new framework"""

    load_dir = os.path.join(super.config.logdir, subtask)
    latest_checkpoint = tf.train.latest_checkpoint(load_dir)
    if latest_checkpoint is None:
        print("[{}] No previous Tensorflow result".format(subtask))
        return False

    super.best_val_loss[subtask] = loadh5(os.path.join(load_dir, best_val_loss_filename))[subtask]
    print("[{}] Loaded best validation result = {}".format(subtask, super.best_val_loss[subtask]))
    # Load best validation result
    super.best_step[subtask] = loadh5(os.path.join(load_dir, best_step_filename))[subtask]
    print("[{}] Loaded best step = {}".format(subtask, super.best_step[subtask]))
    return True



#
# dump.py ends here
