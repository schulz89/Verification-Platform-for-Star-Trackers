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
from config_common import ConfigCommon
from config_dut import ConfigDut
from definitions import test_t, dut_t

class Test:    
    def __init__(self, test_config):
        self.test_sel = test_t[test_config.test_name]
        self.data_filename = test_config.output_filename
        self.test_config = test_config
        os.makedirs("config", exist_ok=True)
        test_config.generate_yml("config/common.yml")

    def dut(self, dut_config):
        self.dut_sel  = dut_t[dut_config.dut_name]
        self.dut_config = dut_config
        self.dut_config.generate_yml("config/dut.yml")
        subprocess.run(["bin/grid_database"])
    
    def run(self):
        if subprocess.call(["bin/verification_platform", str(self.dut_sel), str(self.test_sel)]) :
            exit()

    def process(self):
        self.data = np.genfromtxt("data/" + self.data_filename, comments="%", delimiter="\t")
        print("Data size:" + repr(np.size(self.data,axis=0)) + "x" + repr(np.size(self.data,axis=1)))

class TestStarTracker(Test):
    def run(self,target,start,end,ntests):
        self.x = np.linspace(start,end,ntests)
        self.y = np.empty(ntests)
        for i in range(0,ntests) :
            target(self.x[i])
            super().run()
            super().process()
            total = np.size(self.data,axis=0)
            acc = np.sum(self.data,axis=0)
            self.y[i] = acc[4]/total

    def focus_std(self,value):
        self.test_config.sp_focus_std_dev = value
        self.test_config.generate_yml("config/common.yml")

    def focus_dc(self,value):
        self.test_config.sp_focus_dc = value
        self.test_config.generate_yml("config/common.yml")

    def position(self,value):
        self.test_config.sp_position_std_dev = value
        self.test_config.generate_yml("config/common.yml")

    def false_stars(self,value):
        self.test_config.sp_false_stars = value
        self.test_config.generate_yml("config/common.yml")

    def magnitude(self,value):
        self.test_config.sp_mag_std_dev = value
        self.test_config.generate_yml("config/common.yml")
    
    def grid_size(self,value):
        self.dut_config.gp_g = value
        self.dut_config.generate_yml("config/dut.yml")
        subprocess.run(["bin/grid_database"])


# ==========================================================================

def attitude_error_run():
    ntests = 10000
    test_config = ConfigCommon(test_name = "ATTITUDE", sp_generate_image = 0, output_filename = "test_attitude.csv", n_tests = ntests)
    dut_config  = ConfigDut(dut_name= "ATTITUDE_DETERMINATION")
    test = Test(test_config)
    test.dut(dut_config)
    test.run()

def star_id_histogram_run():
    ntests = 10000
    test_config = ConfigCommon(test_name = "STAR_ID", sp_generate_image = 0, output_filename = "test_star_id_grid.csv", op_vertical_fov = 15, n_tests = ntests)
    dut_config  = ConfigDut(dut_name= "STAR_ID_GRID", gp_alg_variation = "BINARY")
    test = Test(test_config)
    test.dut(dut_config)
    test.run()
    test.process()
    np.save("data/star_id_histogram.npy", test.data)
    
def star_id_histogram_plot():
    data = np.load("data/star_id_histogram.npy")
    ratio = data[:,0] / data[:,2]
    
    plt.hist(ratio, bins = 20)
    # plt.hist(ratio, bins = 20, facecolor = "dimgrey")
    axes = plt.gca()
    axes.set_xlim([0,1])
    plt.xlabel("Fraction of correctly identified stars")
    plt.ylabel("Tests")
    plt.grid()
    # plt.title("Histogram of fraction of correctly identified stars in FOV")
    plt.savefig("data/star_id_histogram.pdf")
    plt.show()

# ==========================================================================

def star_tracker_position_run():
    start  = 0 # pixels
    end    = 2 # pixels
    ntests = 100

    test = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "REFERENCE") )
    test.run(test.position, start, end, ntests)
    np.save("data/star_tracker_position_x.npy", test.x)
    np.save("data/star_tracker_position_y.npy", test.y)

def star_tracker_position_plot():
    x = np.load("data/star_tracker_position_x.npy")
    y = np.load("data/star_tracker_position_y.npy")
    
    plt.plot(x, y)
    plt.xlabel("Additive Gaussian noise on position (pixel)")
    plt.ylabel("Ratio of correct attitude determination")
    plt.grid()
    plt.savefig("data/star_tracker_position.pdf")
    plt.show()

# ==========================================================================

def star_tracker_magnitude_run():
    start  = 0 # mag
    end    = 2 # mag
    ntests = 100

    test = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "REFERENCE") )
    test.run(test.magnitude, start, end, ntests)
    np.save("data/star_tracker_magnitude_x.npy", test.x)
    np.save("data/star_tracker_magnitude_y.npy", test.y)

def star_tracker_magnitude_plot():
    x = np.load("data/star_tracker_magnitude_x.npy")
    y = np.load("data/star_tracker_magnitude_y.npy")
    
    plt.plot(x, y)
    plt.xlabel("Additive Gaussian noise on magnitude (mag)")
    plt.ylabel("Ratio of correct attitude determination")
    plt.grid()
    plt.savefig("data/star_tracker_magnitude.pdf")
    plt.show()

# ==========================================================================

def star_tracker_false_stars_run():
    start  = 0 # stars
    end    = 5 # stars
    ntests = end - start + 1

    test = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "REFERENCE") )
    test.run(test.false_stars, start, end, ntests)
    np.save("data/star_tracker_false_stars_x.npy", test.x)
    np.save("data/star_tracker_false_stars_y.npy", test.y)

def star_tracker_false_stars_plot():
    x = np.load("data/star_tracker_false_stars_x.npy")
    y = np.load("data/star_tracker_false_stars_y.npy")
    
    plt.plot(x, y)
    plt.xlabel("Number of false stars")
    plt.ylabel("Ratio of correct attitude determination")
    plt.grid()
    plt.savefig("data/star_tracker_false_stars.pdf")
    plt.show()

# ==========================================================================

def star_tracker_focus_std_run():
    start  = 0    # meters
    end    = 2e-3 # meters
    ntests = 100

    test = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "REFERENCE") )
    test.run(test.focus_std, start, end, ntests)
    np.save("data/star_tracker_focus_std_x.npy", test.x)
    np.save("data/star_tracker_focus_std_y.npy", test.y)

def star_tracker_focus_std_plot():
    x = np.load("data/star_tracker_focus_std_x.npy")
    y = np.load("data/star_tracker_focus_std_y.npy")
    
    plt.plot(x * 1e3 ,y)
    plt.xlabel("Additive Gaussian noise on focal length (mm)")
    plt.ylabel("Ratio of correct attitude determination")
    plt.grid()
    plt.savefig("data/star_tracker_focus_std.pdf")
    plt.show()

# ==========================================================================

def star_tracker_focus_dc_run():
    start  = -2e-3    # meters
    end    = 2e-3 # meters
    ntests = 100

    test = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "REFERENCE") )
    test.run(test.focus_dc, start, end, ntests)
    np.save("data/star_tracker_focus_dc_x.npy", test.x)
    np.save("data/star_tracker_focus_dc_y.npy", test.y)

def star_tracker_focus_dc_plot():
    x = np.load("data/star_tracker_focus_dc_x.npy")
    y = np.load("data/star_tracker_focus_dc_y.npy")
    
    plt.plot(x * 1e3 ,y)
    plt.xlabel("Change in focal length (mm)")
    plt.ylabel("Ratio of correct attitude determination")
    plt.grid()
    plt.savefig("data/star_tracker_focus_dc.pdf")
    plt.show()

# ==========================================================================

def star_tracker_grid_size_run():
    start  = 1
    end    = 64
    ntests = end - start + 1

    test = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "BINARY_INDEX") )
    test.run(test.grid_size, start, end, ntests)
    np.save("data/star_tracker_grid_size_x.npy", test.x)
    np.save("data/star_tracker_grid_size_y.npy", test.y)

def star_tracker_grid_size_plot():
    x = np.load("data/star_tracker_grid_size_x.npy")
    y = np.load("data/star_tracker_grid_size_y.npy")
    
    plt.plot(x, y)
    plt.xlabel("Grid size g")
    plt.ylabel("Ratio of correct attitude determination")
    plt.grid()
    plt.savefig("data/star_tracker_grid_size.pdf")
    plt.show()

# ==========================================================================

def star_tracker_position_triple_run():
    start  = 0 # pixels
    end    = 2 # pixels
    ntests = 100

    test_ref = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_ref.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "REFERENCE") )
    test_ref.run(test_ref.position, start, end, ntests)
    np.save("data/star_tracker_position_triple_ref_x.npy", test_ref.x)
    np.save("data/star_tracker_position_triple_ref_y.npy", test_ref.y)

    test_mod0 = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_mod0.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "BINARY", gp_nn_error_angle = 0.5e-3) )
    test_mod0.run(test_mod0.position, start, end, ntests)
    np.save("data/star_tracker_position_triple_mod0_x.npy", test_mod0.x)
    np.save("data/star_tracker_position_triple_mod0_y.npy", test_mod0.y)

    test_mod1 = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_mod1.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "BINARY", gp_nn_error_angle = 1.0e-3) )
    test_mod1.run(test_mod1.position, start, end, ntests)
    np.save("data/star_tracker_position_triple_mod1_x.npy", test_mod1.x)
    np.save("data/star_tracker_position_triple_mod1_y.npy", test_mod1.y)

def star_tracker_position_triple_plot():
    ref_x = np.load("data/star_tracker_position_triple_ref_x.npy")
    ref_y = np.load("data/star_tracker_position_triple_ref_y.npy")
    mod0_x = np.load("data/star_tracker_position_triple_mod0_x.npy")
    mod0_y = np.load("data/star_tracker_position_triple_mod0_y.npy")
    mod1_x = np.load("data/star_tracker_position_triple_mod1_x.npy")
    mod1_y = np.load("data/star_tracker_position_triple_mod1_y.npy")

    plt.plot(ref_x, ref_y,  label="Reference")
    plt.plot(mod0_x, mod0_y, label="Modified (e=0.5mrad)")
    plt.plot(mod1_x, mod1_y, label="Modified (e=1.0mrad)")
    plt.xlabel("Additive Gaussian noise on position (pixel)")
    plt.ylabel("Ratio of correct attitude determination")
    plt.legend()
    plt.grid()
    plt.savefig("data/star_tracker_position_triple.pdf")
    plt.show()

# ==========================================================================

def star_tracker_magnitude_triple_run():
    start  = 0 # mag
    end    = 2 # mag
    ntests = 100

    test_ref = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_ref.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "REFERENCE") )
    test_ref.run(test_ref.magnitude, start, end, ntests)

    test_mod0 = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_mod0.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "BINARY", gp_nn_error_angle = 0.5e-3) )
    test_mod0.run(test_mod0.magnitude, start, end, ntests)

    test_mod1 = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_mod1.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "BINARY", gp_nn_error_angle = 1.0e-3) )
    test_mod1.run(test_mod1.magnitude, start, end, ntests)
    
    np.save("data/star_tracker_magnitude_triple_ref_x.npy", test_ref.x)
    np.save("data/star_tracker_magnitude_triple_ref_y.npy", test_ref.y)
    np.save("data/star_tracker_magnitude_triple_mod0_x.npy", test_mod0.x)
    np.save("data/star_tracker_magnitude_triple_mod0_y.npy", test_mod0.y)
    np.save("data/star_tracker_magnitude_triple_mod1_x.npy", test_mod1.x)
    np.save("data/star_tracker_magnitude_triple_mod1_y.npy", test_mod1.y)
    
def star_tracker_magnitude_triple_plot():
    ref_x = np.load("data/star_tracker_magnitude_triple_ref_x.npy")
    ref_y = np.load("data/star_tracker_magnitude_triple_ref_y.npy")
    mod0_x = np.load("data/star_tracker_magnitude_triple_mod0_x.npy")
    mod0_y = np.load("data/star_tracker_magnitude_triple_mod0_y.npy")
    mod1_x = np.load("data/star_tracker_magnitude_triple_mod1_x.npy")
    mod1_y = np.load("data/star_tracker_magnitude_triple_mod1_y.npy")
    
    plt.plot(ref_x, ref_y,  label="Reference")
    plt.plot(mod0_x, mod0_y, label="Modified (e=0.5mrad)")
    plt.plot(mod1_x, mod1_y, label="Modified (e=1.0mrad)")
    plt.xlabel("Additive Gaussian noise on magnitude (mag)")
    plt.ylabel("Ratio of correct attitude determination")
    plt.legend()
    plt.grid()
    plt.savefig("data/star_tracker_magnitude_triple.pdf")
    plt.show()

# ==========================================================================

def star_tracker_false_stars_triple_run():
    start  = 0 # stars
    end    = 5 # stars
    ntests = end - start + 1

    test_ref = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_ref.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "REFERENCE") )
    test_ref.run(test_ref.false_stars, start, end, ntests)

    test_mod0 = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_mod0.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "BINARY", gp_nn_error_angle = 0.5e-3) )
    test_mod0.run(test_mod0.false_stars, start, end, ntests)

    test_mod1 = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_mod1.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "BINARY", gp_nn_error_angle = 1.0e-3) )
    test_mod1.run(test_mod1.false_stars, start, end, ntests)
    
    np.save("data/star_tracker_false_stars_triple_ref_x.npy", test_ref.x)
    np.save("data/star_tracker_false_stars_triple_ref_y.npy", test_ref.y)
    np.save("data/star_tracker_false_stars_triple_mod0_x.npy", test_mod0.x)
    np.save("data/star_tracker_false_stars_triple_mod0_y.npy", test_mod0.y)
    np.save("data/star_tracker_false_stars_triple_mod1_x.npy", test_mod1.x)
    np.save("data/star_tracker_false_stars_triple_mod1_y.npy", test_mod1.y)

def star_tracker_false_stars_triple_plot():
    ref_x = np.load("data/star_tracker_false_stars_triple_ref_x.npy")
    ref_y = np.load("data/star_tracker_false_stars_triple_ref_y.npy")
    mod0_x = np.load("data/star_tracker_false_stars_triple_mod0_x.npy")
    mod0_y = np.load("data/star_tracker_false_stars_triple_mod0_y.npy")
    mod1_x = np.load("data/star_tracker_false_stars_triple_mod1_x.npy")
    mod1_y = np.load("data/star_tracker_false_stars_triple_mod1_y.npy")
    
    plt.plot(ref_x, ref_y,  label="Reference")
    plt.plot(mod0_x, mod0_y, label="Modified (e=0.5mrad)")
    plt.plot(mod1_x, mod1_y, label="Modified (e=1.0mrad)")
    plt.xlabel("Number of false stars")
    plt.ylabel("Ratio of correct attitude determination")
    plt.grid()
    plt.legend()
    plt.savefig("data/star_tracker_false_stars_triple.pdf")
    plt.show()

# ==========================================================================

def star_tracker_focus_std_triple_run():
    start  = 0    # meters
    end    = 2e-3 # meters
    ntests = 100

    test_reference = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_reference.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "REFERENCE") )
    test_reference.run(test_reference.focus_std, start, end, ntests)
    np.save("data/star_tracker_focus_std_triple_ref_x.npy", test_reference.x)
    np.save("data/star_tracker_focus_std_triple_ref_y.npy", test_reference.y)

    test_modified_0 = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_modified_0.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "BINARY", gp_nn_error_angle = 0.5e-3) )
    test_modified_0.run(test_modified_0.focus_std, start, end, ntests)
    np.save("data/star_tracker_focus_std_triple_mod0_x.npy", test_modified_0.x)
    np.save("data/star_tracker_focus_std_triple_mod0_y.npy", test_modified_0.y)

    test_modified_1 = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_modified_1.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "BINARY", gp_nn_error_angle = 1.0e-3) )
    test_modified_1.run(test_modified_1.focus_std, start, end, ntests)
    np.save("data/star_tracker_focus_std_triple_mod1_x.npy", test_modified_1.x)
    np.save("data/star_tracker_focus_std_triple_mod1_y.npy", test_modified_1.y)

def star_tracker_focus_std_triple_plot():
    ref_x = np.load("data/star_tracker_focus_std_triple_ref_x.npy")
    ref_y = np.load("data/star_tracker_focus_std_triple_ref_y.npy")
    mod0_x = np.load("data/star_tracker_focus_std_triple_mod0_x.npy")
    mod0_y = np.load("data/star_tracker_focus_std_triple_mod0_y.npy")
    mod1_x = np.load("data/star_tracker_focus_std_triple_mod1_x.npy")
    mod1_y = np.load("data/star_tracker_focus_std_triple_mod1_y.npy")

    plt.plot(ref_x  * 1e3 ,ref_y,  label="Reference")
    plt.plot(mod0_x * 1e3 ,mod0_y, label="Modified (e=0.5mrad)")
    plt.plot(mod1_x * 1e3 ,mod1_y, label="Modified (e=1.0mrad)")
    plt.xlabel("Additive Gaussian noise on focal length (mm)")
    plt.ylabel("Ratio of correct attitude determination")
    plt.legend()
    plt.grid()
    plt.savefig("data/star_tracker_focus_std_triple.pdf")
    plt.show()

# ==========================================================================

def star_tracker_focus_dc_triple_run():
    start  = -2e-3 # meters
    end    = +2e-3 # meters
    ntests = 100

    test_reference = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv", op_resolution = (1600,1200)) )
    test_reference.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "REFERENCE") )
    test_reference.run(test_reference.focus_dc, start, end, ntests)
    np.save("data/star_tracker_focus_dc_triple_ref_x.npy", test_reference.x)
    np.save("data/star_tracker_focus_dc_triple_ref_y.npy", test_reference.y)

    test_modified_0 = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_modified_0.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "BINARY", gp_nn_error_angle = 0.5e-3) )
    test_modified_0.run(test_modified_0.focus_dc, start, end, ntests)
    np.save("data/star_tracker_focus_dc_triple_mod0_x.npy", test_modified_0.x)
    np.save("data/star_tracker_focus_dc_triple_mod0_y.npy", test_modified_0.y)

    test_modified_1 = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_modified_1.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "BINARY", gp_nn_error_angle = 1.0e-3) )
    test_modified_1.run(test_modified_1.focus_dc, start, end, ntests)
    np.save("data/star_tracker_focus_dc_triple_mod1_x.npy", test_modified_1.x)
    np.save("data/star_tracker_focus_dc_triple_mod1_y.npy", test_modified_1.y)

def star_tracker_focus_dc_triple_plot():
    ref_x = np.load("data/star_tracker_focus_dc_triple_ref_x.npy")
    ref_y = np.load("data/star_tracker_focus_dc_triple_ref_y.npy")
    mod0_x = np.load("data/star_tracker_focus_dc_triple_mod0_x.npy")
    mod0_y = np.load("data/star_tracker_focus_dc_triple_mod0_y.npy")
    mod1_x = np.load("data/star_tracker_focus_dc_triple_mod1_x.npy")
    mod1_y = np.load("data/star_tracker_focus_dc_triple_mod1_y.npy")

    plt.plot(ref_x  * 1e3 ,ref_y,  label="Reference")
    plt.plot(mod0_x * 1e3 ,mod0_y, label="Modified (e=0.5mrad)")
    plt.plot(mod1_x * 1e3 ,mod1_y, label="Modified (e=1.0mrad)")
    plt.xlabel("Change in focal length (mm)")
    plt.ylabel("Ratio of correct attitude determination")
    plt.legend()
    plt.grid()
    plt.savefig("data/star_tracker_focus_dc_triple.pdf")
    plt.show()

# ==========================================================================

def star_tracker_grid_size_triple_run():
    start  = 1
    end    = 64
    ntests = end - start + 1

    test_reference = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_reference.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "REFERENCE") )
    test_reference.run(test_reference.grid_size, start, end, ntests)
    np.save("data/star_tracker_grid_size_triple_ref_x.npy", test_reference.x)
    np.save("data/star_tracker_grid_size_triple_ref_y.npy", test_reference.y)

    test_modified_0 = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_modified_0.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "BINARY_INDEX", gp_nn_error_angle = 0.5e-3) )
    test_modified_0.run(test_modified_0.grid_size, start, end, ntests)
    np.save("data/star_tracker_grid_size_triple_mod0_x.npy", test_modified_0.x)
    np.save("data/star_tracker_grid_size_triple_mod0_y.npy", test_modified_0.y)

    test_modified_1 = TestStarTracker( ConfigCommon(test_name = "STAR_TRACKER", output_filename = "test_star_tracker.csv") )
    test_modified_1.dut( ConfigDut(dut_name= "STAR_TRACKER", gp_alg_variation = "BINARY_INDEX", gp_nn_error_angle = 1.0e-3) )
    test_modified_1.run(test_modified_1.grid_size, start, end, ntests)
    np.save("data/star_tracker_grid_size_triple_mod1_x.npy", test_modified_1.x)
    np.save("data/star_tracker_grid_size_triple_mod1_y.npy", test_modified_1.y)

def star_tracker_grid_size_triple_plot():
    ref_x = np.load("data/star_tracker_grid_size_triple_ref_x.npy")
    ref_y = np.load("data/star_tracker_grid_size_triple_ref_y.npy")
    mod0_x = np.load("data/star_tracker_grid_size_triple_mod0_x.npy")
    mod0_y = np.load("data/star_tracker_grid_size_triple_mod0_y.npy")
    mod1_x = np.load("data/star_tracker_grid_size_triple_mod1_x.npy")
    mod1_y = np.load("data/star_tracker_grid_size_triple_mod1_y.npy")
   
    plt.plot(ref_x, ref_y,  label="Reference")
    plt.plot(mod0_x, mod0_y, label="Modified (e=0.5mrad)")
    plt.plot(mod1_x, mod1_y, label="Modified (e=1.0mrad)")
    plt.xlabel("Grid size g")
    plt.ylabel("Ratio of correct attitude determination")
    plt.legend()
    plt.grid()
    plt.savefig("data/star_tracker_grid_size_triple.pdf")
    plt.show()

# ==========================================================================

plt.rcParams["font.family"] = "STIXGeneral"
plt.rcParams["mathtext.fontset"] = "stix"
plt.rcParams["font.size"] = 12
plt.rcParams['legend.fontsize'] = 'medium'
plt.rcParams['figure.titlesize'] = 'large'
plt.rcParams['axes.labelsize'] = 'x-large'
plt.rcParams['xtick.labelsize'] = 'large'
plt.rcParams['ytick.labelsize'] = 'large'
plt.rcParams['figure.autolayout'] = True
