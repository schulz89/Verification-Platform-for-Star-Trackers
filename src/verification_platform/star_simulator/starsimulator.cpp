// Copyright 2019 Victor Hugo Schulz

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "starsimulator.h"

using namespace std;
using namespace cv;
using namespace boost::qvm;
using namespace std_str;

namespace ssim{

StarSimulator::StarSimulator()
{// Constructor
    load_config("config/common.yml");
    database = initialize_database();
    vstars = visible_stars(database);
}

void StarSimulator::reload()
{// Reload configuration and reset RNGs.
    database = initialize_database();
    vstars = visible_stars(database);
    rng_quaternion.seed();
    rng_noise.seed();
}

Sky StarSimulator::generate_sky()
{// Generate a sky configuration from a random quaternion;
    Sky output;
    quat<double> q_boost = random_quaternion();
    memcpy(&output.q,q_boost.a,sizeof(output.q));
    vector<DB> fov = projected_fov(inverse(q_boost));
    for(uint i=0;i<fov.size();i++){
        Star s;
        s.id = fov[i].id;
        s.mag = fov[i].mag;
        memcpy(static_cast<void*>(&s.uv_inertial), fov[i].uv_inertial.a, sizeof(s.uv_inertial));
        memcpy(static_cast<void*>(&s.uv_camera),   fov[i].uv_camera.a,   sizeof(s.uv_camera));
        memcpy(static_cast<void*>(&s.centroid),    fov[i].centroid.a,    sizeof(s.centroid));
        output.stars.push_back(s);
    }
    output.image = build_image(fov);
    return output;
}

Sky StarSimulator::generate_sky(Quaternion q)
{// Generate a sky configuration from a given quaternion;
    Sky output;
    output.q = q;
    quat<double> q_boost; memcpy(q_boost.a,&q,sizeof(q));
    vector<DB> fov = projected_fov(inverse(q_boost));
    for(uint i=0;i<fov.size();i++){
        Star s;
        s.id = fov[i].id;
        s.mag = fov[i].mag;
        memcpy(static_cast<void*>(&s.uv_inertial),fov[i].uv_inertial.a,sizeof(s.uv_inertial));
        memcpy(static_cast<void*>(&s.uv_camera),  fov[i].uv_camera.a,  sizeof(s.uv_camera));
        memcpy(static_cast<void*>(&s.centroid),   fov[i].centroid.a,   sizeof(s.centroid));
        output.stars.push_back(s);
    }
    output.image = build_image(fov);
    return output;
}

void StarSimulator::load_config(string filename)
{// Load configuration from file.
    FileStorage fs(filename, FileStorage::READ);
    Mat mtmp;
    double dtmp;
    fs["cp_hipparcos_filename"]  >> config.catalog_parameters.hipparcos;
    fs["cp_hipparcos2_filename"] >> config.catalog_parameters.hipparcos2;
    fs["cp_hipparcos_size"]      >> dtmp; config.catalog_parameters.hipparcos_size = (int)dtmp;
    fs["cp_epoch"]               >> dtmp; config.catalog_parameters.epoch = (int)dtmp;
    fs["op_resolution"]          >> mtmp; config.optical_parameters.resolution = (Point2i)mtmp;
    fs["op_pixel_size"]          >> mtmp; config.optical_parameters.pixel_size = (Point2d)mtmp;
    fs["op_sensor_size"]         >> mtmp; config.optical_parameters.sensor_size = (Point2d)mtmp;
    fs["op_focus"]               >> config.optical_parameters.focus;
    fs["op_center"]              >> mtmp; config.optical_parameters.center = (Point2i)mtmp;
    fs["op_max_mag"]             >> config.optical_parameters.max_mag;
    fs["sp_psf_B"]               >> config.simulator_parameters.psf_B;
    fs["sp_psf_C"]               >> config.simulator_parameters.psf_C;
    fs["sp_psf_sigma"]           >> config.simulator_parameters.psf_sigma;
    fs["sp_psf_roi"]             >> config.simulator_parameters.psf_roi;
    fs["sp_position_std_dev"]    >> config.simulator_parameters.position_std_dev;
    fs["sp_mag_std_dev"]         >> config.simulator_parameters.mag_std_dev;
    fs["sp_background_std_dev"]  >> config.simulator_parameters.background_std_dev;
    fs["sp_background_dc"]       >> config.simulator_parameters.background_dc;
    fs["sp_focus_std_dev"]       >> config.simulator_parameters.focus_std_dev;
    fs["sp_focus_dc"]            >> config.simulator_parameters.focus_dc;
    fs["sp_false_stars"]         >> dtmp; config.simulator_parameters.false_stars = (int)dtmp;
    fs["sp_generate_image"]      >> dtmp; config.simulator_parameters.build_image = (bool)dtmp;
    fs.release();
}

vector<DB> StarSimulator::initialize_database()
{// Read information from Hipparcos and Hipparcos 2 catalogs for initializing the database.
    Hip *hipparcos = new Hip [config.catalog_parameters.hipparcos_size];
    Hip initial = {0,'\0',0};
    for(int i=0;i<config.catalog_parameters.hipparcos_size;i++)
        hipparcos[i] = initial;

    double time_y = config.catalog_parameters.epoch - 1991.25;

    // Hipparcos
    string line;
    ifstream myfile (config.catalog_parameters.hipparcos);
    if (myfile.is_open()) {
        while ( getline (myfile,line) ) {
            Hip data;
            string sstr;
            int HIP      = stoi(line.substr(8,6));
            data.Proxy   = line.substr(15,1).compare(" ") ? 1:0;
            sstr = line.substr(41,5);
            if (!sstr.compare("     ")){
                //cerr << "Warning: Empty Vmag (HIP=" << HIP << ")" << endl;
                continue;
            }
            data.Vmag    = stod(sstr);
            sstr = line.substr(47,1);
            if (sstr.compare(" "))
                data.VarFlag = stoi(sstr);
            else
                data.VarFlag = 0;
            hipparcos[HIP] = data;
        }
        myfile.close();
    }
    else {
        cout << "Unable to open file \"" << config.catalog_parameters.hipparcos2 << "\"." << endl;
        exit(1);
    }

    // Hipparcos 2
    vector<Hip2> hipparcos2;
    ifstream myfile2 (config.catalog_parameters.hipparcos2);
    if (myfile2.is_open()) {
        while ( getline (myfile2,line) ) {
            Hip2 data;
            data.HIP   = stoi(line.substr(0,6));
            data.RArad = stod(line.substr(15,13));
            data.DErad = stod(line.substr(29,13));
            data.pmRA  = stod(line.substr(51,8));
            data.pmDE  = stod(line.substr(60,8));
            hipparcos2.push_back(data);
        }
        myfile2.close();
    }
    else {
        cout << "Unable to open file \"" << config.catalog_parameters.hipparcos2 << "\"." << endl;
        exit(1);
    }

    // Database
    vector<DB> database;
    for(uint i=0;i<hipparcos2.size();i++){
        DB entry;
        entry.id         = hipparcos2[i].HIP;
        entry.mag        = hipparcos[entry.id].Vmag;
        proper_motion_correction(hipparcos2[i],time_y,&entry);
        database.push_back(entry);
    }
    delete [] hipparcos;
    return database;
}

void StarSimulator::proper_motion_correction(Hip2 entry, int time_y, DB *output)
{// Proper motion conversion, with output as an unit vector.
    double mu_alpha_rad = (entry.pmRA * M_PI) / (3600 * 1000 * 180 * cos(entry.DErad));
    double mu_delta_rad = (entry.pmDE * M_PI) / (3600 * 1000 * 180);
    double alpha        = entry.RArad + mu_alpha_rad * time_y;
    double delta        = entry.DErad + mu_delta_rad * time_y;
    output->uv_inertial = {cos(alpha) * cos(delta),
                           sin(alpha) * cos(delta),
                           sin(delta)};
}

vector<DB> StarSimulator::visible_stars(vector<DB> &database)
{// Extract only the visible stars from the database.
    vector<DB> output;
    for(uint i=0;i<database.size();i++){
        if(database[i].mag <= config.optical_parameters.max_mag)
            output.push_back(database[i]);
    }
    return output;
}

double StarSimulator::vangle (vec<double,3> p1, vec<double,3> p2)
{// Angle between two vectors.
    return atan2(mag(cross(p1,p2)), dot(p1,p2));
}

quat<double> StarSimulator::rot_quat (vec<double,3> v1, vec<double,3> v2)
{// Rotation quaternion between two unit vectors. V1 must be different than V2.
    double theta = vangle(v1,v2);
    double angle = cos(theta/2);
    vec<double,3> v = cross(v1,v2);
    normalize(v);
    v *= sin(theta/2);
    quat<double> output;
    output = {angle,v.a[0],v.a[1],v.a[2]};
    return output;
}

vector<DB> StarSimulator::rotate3d(vector<DB> visible_stars, quat<double> q)
{// Rotates the database by the given quaternion. Returns a new and rotated structure.
    for(uint i = 0; i<visible_stars.size(); i++){
        vec<double,3> p_ = visible_stars[i].uv_inertial;
        quat<double> p = {0,p_.a[0],p_.a[1],p_.a[2]};
        quat<double> rot_uv = q * p * inverse(q);
        visible_stars[i].uv_camera = V(rot_uv);
    }
    return visible_stars;
}

void StarSimulator::to_pixel_coordinates (vector<DB> &star_list)
{// Transformation into pixel coordinates.
 // Equivalent to multiplication by the camera matrix and normalization.
    double f = (config.optical_parameters.focus + awgn(config.simulator_parameters.focus_std_dev) + config.simulator_parameters.focus_dc)
            / config.optical_parameters.pixel_size.y; // unit: px
    for(uint i=0; i<star_list.size(); i++){
        star_list[i].centroid.a[0] = f * star_list[i].uv_camera.a[0] / star_list[i].uv_camera.a[2]
                + config.optical_parameters.center.x;
        star_list[i].centroid.a[1] = f * star_list[i].uv_camera.a[1] / star_list[i].uv_camera.a[2]
                + config.optical_parameters.center.y;
        star_list[i].centroid.a[2] = copysign(1.0,star_list[i].uv_camera.a[2]);
    }
}

vector<DB> StarSimulator::relevant_fov(vector<DB> &lf_list)
{// Remove stars which are outside of the field of view.
    vector<DB> fov;
    for(uint i = 0; i<lf_list.size(); i++){
        if(     (lf_list[i].centroid.a[0] >= 0) &&
                (lf_list[i].centroid.a[1] >= 0) &&
                (lf_list[i].centroid.a[2] >= 0) &&
                (lf_list[i].centroid.a[0] < (config.optical_parameters.resolution.x - 1)) &&
                (lf_list[i].centroid.a[1] < (config.optical_parameters.resolution.y - 1)))
            fov.push_back(lf_list[i]);

    }
    return fov;
}

vector<DB> StarSimulator::projected_fov(quat<double> q)
{// Gets the projected star 2D coordinates for a given attitude quaternion.
    vector<DB> rotated = rotate3d(vstars,q);
    to_pixel_coordinates(rotated);
    vector<DB> fov = relevant_fov(rotated);
    return fov;
}

Mat StarSimulator::build_image(vector<DB> star_list)
{// Builds a grayscale image from the stars in the field of view.
    for(int i=0; i<config.simulator_parameters.false_stars; i++)
        star_list.push_back(fake_star());
    if(config.simulator_parameters.build_image){
        Mat image(config.optical_parameters.resolution.y,config.optical_parameters.resolution.x,CV_8U);
        double rem = (config.simulator_parameters.psf_C * M_PI_2 * config.simulator_parameters.psf_sigma * config.simulator_parameters.psf_sigma);
        double ram = M_SQRT2 * config.simulator_parameters.psf_sigma;
        for(auto &star : star_list){
            star.centroid.a[0] += awgn(config.simulator_parameters.position_std_dev);
            star.centroid.a[1] += awgn(config.simulator_parameters.position_std_dev);
        }
        #pragma omp parallel for
        for(int m=0;m<config.optical_parameters.resolution.x;m++){
            for(int n=0;n<config.optical_parameters.resolution.y;n++){
                double acc = 0.0;
                for(uint s=0;s<star_list.size();s++){
                    double X_i = star_list[s].centroid.a[0];
                    double Y_i = star_list[s].centroid.a[1];
                    if((fabs(m-X_i) + fabs(n-Y_i)) < config.simulator_parameters.psf_roi){
                        double mult = rem / pow( 2.512, star_list[s].mag + awgn(config.simulator_parameters.mag_std_dev) );
                        acc += config.simulator_parameters.psf_B + mult * ((erf((m-X_i)/ram) - erf((1+m-X_i)/ram)) *
                                                                           (erf((n-Y_i)/ram) - erf((1+n-Y_i)/ram)));
                    }
                }

                acc += config.simulator_parameters.background_dc
                        + awgn(config.simulator_parameters.background_std_dev);

                if(acc < 0)
                    image.at<uchar>(n,m) = 0x00;
                else if(acc >= UCHAR_MAX)
                    image.at<uchar>(n,m) = UCHAR_MAX;
                else
                    image.at<uchar>(n,m) = acc;
            }
        }
        return image;
    }
    else{
        Mat image;
        return image;
    }
}

vec<double,3> StarSimulator::random_unit_vector ()
{// Generates a random unit vector.
    uniform_real_distribution<double> distribution(-1.0,1.0);
    double x = distribution(rng_quaternion);
    double y = distribution(rng_quaternion);
    double z = distribution(rng_quaternion);
    double r = sqrt(x*x+y*y+z*z);
    vec<double,3> output = {x/r, y/r, z/r};
    return output;
}

double StarSimulator::random_angle ()
{// Generates an angle between -π and π
    uniform_real_distribution<double> distribution(-M_PI,M_PI);
    return distribution(rng_quaternion);
}

quat<double> StarSimulator::random_quaternion()
{// Generates a random rotation quaternion.
    double angle = random_angle();
    vec<double,3> v = random_unit_vector();
    double a = cos(angle/2);
    double b = sin(angle/2);
    quat<double> random_q = {a, b*v.a[0], b*v.a[1], b*v.a[2]};
    return random_q;
}

// quat<double> StarSimulator::random_quaternion()
// {// Generates a random rotation quaternion.
//     vec<double,3> a = random_unit_vector();
//     vec<double,3> b = random_unit_vector();
//     quat<double> output = rot_quat(a,b);
//     return output;
// }

double StarSimulator::awgn(double standard_deviation)
{// Additive white Gaussian noise.
    double output = 0.;
    if(standard_deviation != 0.) {
        normal_distribution<double> distribution(0.0,standard_deviation);
        output = distribution(rng_noise);
    }
    return output;
}

DB StarSimulator::fake_star()
{// Randomly generates a false star in camera frame
    DB star;
    uniform_real_distribution<double> distribution_mag(0,config.optical_parameters.max_mag);
    uniform_real_distribution<double> distribution_centroid_x(0,config.optical_parameters.resolution.x);
    uniform_real_distribution<double> distribution_centroid_y(0,config.optical_parameters.resolution.y);
    star.id = -1;
    star.mag = distribution_mag(rng_fake_star);
    star.centroid.a[0] = distribution_centroid_x(rng_fake_star);
    star.centroid.a[1] = distribution_centroid_y(rng_fake_star);
    star.centroid.a[2] = 0;
    star.uv_camera.a[0] = 0;   star.uv_camera.a[1] = 0;   star.uv_camera.a[2] = 0;
    star.uv_inertial.a[0] = 0; star.uv_inertial.a[1] = 0; star.uv_inertial.a[2] = 0;
    return star;
}

}
