#!/usr/bin/env python3

# Copyright 2019 Victor Hugo Schulz

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#     http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os, subprocess, csv
import numpy as np
import matplotlib.pyplot as plt
import copy
from config_common import ConfigCommon
from config_dut import ConfigDut
from definitions import test_t, dut_t, grid_t

class SingleTest:  # Base class
    def __init__(self, common_list = [ConfigCommon()], dut_list = [ConfigDut()]):
        os.makedirs("config", exist_ok=True)
        self.dut_list = copy.deepcopy(dut_list)
        self.common_list = copy.deepcopy(common_list)
        for common, dut in zip(self.common_list, self.dut_list):
            self.common = common
            self.dut = dut
            self.parameters()
            if not self.exists(common, dut):
                self.configure(self.common, self.dut)
                self.run(self.common, self.dut)
                self.save(self.common, self.dut)
        self.plot()
    def parameters (self):
        pass
    def delta_update(self, value):
        pass
    def exists(self, common_config, dut_config):
        directory_name = "data/" + common_config.test_identifier
        filename = directory_name + "/" + dut_config.dut_identifier + ".npy"
        return os.path.isfile(filename) # If the file already exists, skip test
    def configure(self, common_config, dut_config):
        common_config.generate_yml("config/common.yml")
        dut_config.generate_yml("config/dut.yml")
        subprocess.run(["bin/grid_database"])
    def run(self, common_config, dut_config):
        self.test_sel = test_t[common_config.test_name]
        self.dut_sel  = dut_t[dut_config.dut_name]
        #if subprocess.call(["bin/verification_platform", str(self.dut_sel), str(self.test_sel)]) :
        if subprocess.call(["bin/verification_platform", str(self.dut_sel), str(self.test_sel)], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL) :
            exit()
        self.data = np.genfromtxt("data/" + common_config.output_filename, comments="%", delimiter="\t")
        # print("Data size:" + repr(np.size(self.data,axis=0)) + "x" + repr(np.size(self.data,axis=1)))
    def save(self, common_config, dut_config):
        directory_name = "data/" + common_config.test_identifier
        filename_base = directory_name + "/" + dut_config.dut_identifier
        os.makedirs(directory_name, exist_ok=True)
        np.save(filename_base + ".npy", self.data)    
    def plot(self):
        for common, dut in zip(self.common_list, self.dut_list):
            directory_name = "data/" + common.test_identifier
            filename_base = directory_name + "/" + dut.dut_identifier
            data = np.load(filename_base + ".npy")
            np.savetxt(filename_base + ".csv", data, delimiter=',')
            ratio = data[:,0] / data[:,2]
            plt.hist(ratio, bins = 20)
            # plt.hist(ratio, bins = 20, facecolor = "dimgrey")
            axes = plt.gca()
            axes.set_xlim([0,1])
            plt.xlabel(common.x_axis_name)
            plt.ylabel(common.y_axis_name)
            plt.grid()
            plt.savefig(filename_base + ".pdf")
            # plt.show()
            plt.clf()

class MultipleTests(SingleTest): # Base class
    def exists(self, common_config, dut_config):
        directory_name = "data/" + self.common.test_identifier
        filename_x = directory_name + "/" + dut_config.dut_identifier + "_x.npy"
        filename_y = directory_name + "/" + dut_config.dut_identifier + "_y.npy"
        return os.path.isfile(filename_x) and os.path.isfile(filename_y) # If the files already exists, skip test
    def run(self, common_config, dut_config):
        start = common_config.start
        end = common_config.end
        steps = common_config.steps
        self.x = np.linspace(start,end,steps)
        self.y = np.empty(steps)
        for i in range(0,steps) :
            self.delta_update(self.x[i])
            super().run(common_config, dut_config)
            total = np.size(self.data,axis=0)
            acc = np.sum(self.data,axis=0)
            self.y[i] = acc[4]/total
    def save(self, common_config, dut_config):
        directory_name = "data/" + common_config.test_identifier
        filename_base = directory_name + "/" + dut_config.dut_identifier
        os.makedirs(directory_name, exist_ok=True)
        np.save(filename_base + "_x.npy", self.x)
        np.save(filename_base + "_y.npy", self.y)
    def plot(self):
        directory_name = "data/" + self.common_list[0].test_identifier
        linestyles = ['-', '--', '-.', ':']
        i = 0
        for common, dut in zip(self.common_list, self.dut_list):
            filename_base = directory_name + "/" + dut.dut_identifier
            x = np.load(filename_base + "_x.npy")
            np.savetxt(filename_base + "_x.csv", x, delimiter=',')
            y = np.load(filename_base + "_y.npy")
            np.savetxt(filename_base + "_y.csv", y, delimiter=',')
            plt.plot(x, y, label=dut.dut_identifier, linestyle=linestyles[i%len(linestyles)])
            i = i + 1
        plt.xlabel(common.x_axis_name)
        plt.ylabel(common.y_axis_name)
        plt.grid()
        plt.legend()
        plt.savefig(directory_name + "/" + self.common_list[0].test_identifier + ".pdf")
        # plt.show()
        plt.clf()

class TestStarIdHistogram(SingleTest):
    def parameters(self):
        self.dut.dut_name = "STAR_ID_GRID"
        self.common.test_name = "STAR_ID"
        self.common.test_identifier = "Star ID Histogram"
        self.common.sp_generate_image = 0
        self.common.n_tests = 10000
        self.common.x_axis_name = "Fraction of correctly identified stars"
        self.common.y_axis_name = "Tests"

class TestStarTrackerPosition(MultipleTests):
    def parameters(self):
        self.common.test_identifier = "Star Tracker Position"
        self.common.start = 0 # pixels
        self.common.end = 2   # pixels
        self.common.steps = 100
        self.common.x_axis_name = "Additive Gaussian noise on position (pixel)"
        self.common.y_axis_name = "Ratio of correct attitude determination"
    def delta_update(self,value):
        self.common.sp_position_std_dev = value
        self.common.generate_yml("config/common.yml")

class TestStarTrackerPositionRPR(MultipleTests):
    def parameters(self):
        self.common.test_identifier = "Star Tracker Position Reproduction"
        self.common.start = 0 # pixels
        self.common.end = 1   # pixels
        self.common.steps = 6
        self.common.x_axis_name = "Additive Gaussian noise on position (pixel)"
        self.common.y_axis_name = "Ratio of correct attitude determination"
    def delta_update(self,value):
        self.common.sp_position_std_dev = value
        self.common.generate_yml("config/common.yml")
    def plot(self):
        directory_name = "data/" + self.common_list[0].test_identifier
        linestyles = ['-', '--', '-.', ':']
        i = 0
        for common, dut in zip(self.common_list, self.dut_list):
            filename_base = directory_name + "/" + dut.dut_identifier
            x = np.load(filename_base + "_x.npy")
            np.savetxt(filename_base + "_x.csv", x, delimiter=',')
            y = np.load(filename_base + "_y.npy")
            np.savetxt(filename_base + "_y.csv", y, delimiter=',')
            plt.plot(x, y, label=dut.dut_identifier, linestyle=linestyles[i%len(linestyles)])
            i = i + 1
        plt.xlabel(common.x_axis_name)
        plt.ylabel(common.y_axis_name)
        axes = plt.gca()
        axes.set_xlim([0,1])
        axes.set_ylim([0.795,1.005])
        plt.grid()
        plt.legend()
        plt.savefig(directory_name + "/" + self.common_list[0].test_identifier + ".pdf")
        plt.show()
        plt.clf()


class TestStarTrackerMagnitude(MultipleTests):
    def parameters(self):
        self.common.test_identifier = "Star Tracker Magnitude"
        self.common.start = 0 # mag
        self.common.end   = 2 # mag
        self.common.steps = 100
        self.common.x_axis_name = "Additive Gaussian noise on magnitude (mag)"
        self.common.y_axis_name = "Ratio of correct attitude determination"
    def delta_update(self,value):
        self.common.sp_mag_std_dev = value
        self.common.generate_yml("config/common.yml")

class TestStarTrackerFalseStars(MultipleTests):
    def parameters(self):
        self.common.test_identifier = "Star Tracker False Stars"
        self.common.start = 0 # stars
        self.common.end   = 5 # stars
        self.common.steps = self.common.end - self.common.start + 1
        self.common.x_axis_name = "Number of false stars"
        self.common.y_axis_name = "Ratio of correct attitude determination"
    def delta_update(self,value):
        self.common.sp_false_stars = value
        self.common.generate_yml("config/common.yml")

class TestStarTrackerGridSize(MultipleTests):
    def parameters(self):
        self.common.test_identifier = "Star Tracker Grid Size"
        self.common.start = 1
        self.common.end   = 64
        self.common.steps = self.common.end - self.common.start + 1
        self.common.x_axis_name = "Grid size g"
        self.common.y_axis_name = "Ratio of correct attitude determination"
    def delta_update(self,value):
        self.dut.gp_g = value
        self.dut.generate_yml("config/dut.yml")
        subprocess.run(["bin/grid_database"])

class TestStarTrackerFocusDC(MultipleTests):
    def parameters(self):
        self.common.test_identifier = "Star Tracker Focus DC"
        self.common.start = -2 # mm
        self.common.end   = +2 # mm
        self.common.steps = 100
        self.common.x_axis_name = "Change in focal length (mm)"
        self.common.y_axis_name = "Ratio of correct attitude determination"
    def delta_update(self,value):
        self.common.sp_focus_dc = value
        self.common.generate_yml("config/common.yml")

class TestStarTrackerRuntime(SingleTest):
    def parameters(self):
        # self.common.test_identifier = "Star Tracker Runtime"
        self.common.n_tests = 10000
    def plot(self):
        print("{:>20} {:>8} {:>8} {:>8} {:>8}".format("DUT","C", "C_noth", "S_ID", "Att"))
        for dut in self.dut_list:
            directory_name = "data/" + self.common.test_identifier
            filename_base = directory_name + "/" + dut.dut_identifier
            data = np.load(filename_base + ".npy")
            np.savetxt(filename_base + ".csv", data, delimiter=',')
            acc = data.sum(0)
            print("{:>20} {:>8.4f} {:>8.4f} {:>8.4f} {:>8.4f}".format(dut.dut_identifier, acc[0], acc[1], acc[2], acc[3]))

# ==========================================================================

plt.rcParams['lines.linewidth'] = 2
plt.rcParams["font.family"] = "STIXGeneral"
plt.rcParams["mathtext.fontset"] = "stix"
plt.rcParams["font.size"] = 12
plt.rcParams['legend.fontsize'] = 'medium'
plt.rcParams['figure.titlesize'] = 'large'
plt.rcParams['axes.labelsize'] = 'x-large'
plt.rcParams['xtick.labelsize'] = 'large'
plt.rcParams['ytick.labelsize'] = 'large'
plt.rcParams['figure.autolayout'] = True

# ==========================================================================
